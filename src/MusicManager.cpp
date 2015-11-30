/*
 * Copyright (C) 2012 Me and My Shadow
 *
 * This file is part of Me and My Shadow.
 *
 * Me and My Shadow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Me and My Shadow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Me and My Shadow.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MusicManager.h"
#include "TreeStorageNode.h"
#include "POASerializer.h"
#include "FileManager.h"
#include "Functions.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

MusicManager::MusicManager(){
	Mix_HookMusicFinished(musicStoppedHook);
	Mix_VolumeMusic(MIX_MAX_VOLUME);
	enabled=false;
	currentList="default";
	lastTime=0;
	playing=NULL;
}

MusicManager::~MusicManager(){
	// destroy()를 불러온다.
	destroy();
}

void MusicManager::destroy(){
	// 이미지 모음들을 순환하고 그것들을 자유로이한다.
	std::map<std::string,Music*>::iterator i;
	for(i=musicCollection.begin();i!=musicCollection.end();++i){
		if(i->second!=NULL){
			Mix_FreeMusic(i->second->music);
			if(i->second->loop)
				Mix_FreeMusic(i->second->loop);
			delete i->second;
		}
	}
	playing=NULL;

	// 모아놓은 것들과 목록들을 클리어한다.
	musicCollection.clear();
	musicLists.clear();
}

void MusicManager::setEnabled(bool enable){
	// 새로운 상태로 세팅한다.
	if(enabled!=enable)
		enabled=enable;
	else
		return;

	if(enable){
		// 켜지면 메뉴의 음악을 시작한다.
		playMusic("menu",false);
	}else{
		// 현재의 음악을 멈춘다.
		Mix_HaltMusic();
		Mix_VolumeMusic(atoi(getSettings()->getValue("music").c_str()));
	}
}

void MusicManager::setVolume(int volume){
	Mix_VolumeMusic(volume);
}

string MusicManager::loadMusic(const std::string &file){
	// .음악파일을 연다.
	ifstream musicFile;
	musicFile.open(file.c_str());
	string returnString="";

	// 파일이 존재하면 체크한다.
	if(musicFile){
		// 파일을 넘겨준다.
		TreeStorageNode obj;
		{
			POASerializer objSerializer;
			if(!objSerializer.readNode(musicFile,&obj,true)){
				cerr<<"ERROR: Invalid file format of music description file."<<endl;
			}
		}

		// 입장을 통해 순환한다.
		for(unsigned int i=0;i<obj.subNodes.size();i++){
			TreeStorageNode* obj1=obj.subNodes[i];
			if(obj1==NULL)
				continue;
			if(obj1->value.size()>=1 && obj1->name=="music"){
				// 이 음악이 이미 불러오지 않았는지 확인한다.
				map<string,Music*>::iterator it=musicCollection.find(obj1->value[0]);
				if(it==musicCollection.end()){
					// 음악 파일의 입구를 찾는다.
					Music* music=new Music;
					music->music=NULL;
					music->loop=NULL;

					// 데이터를 불러온다
					for(map<string,vector<string> >::iterator i=obj1->attributes.begin();i!=obj1->attributes.end();++i){
						if(i->first=="file"){
							// 음악 파일을 불러온다.
							music->music=Mix_LoadMUS((getDataPath()+"music/"+i->second[0]).c_str());
						}
						if(i->first=="loopfile"){
							// 순환 파일을 불러온다.
							music->loop=Mix_LoadMUS((getDataPath()+"music/"+i->second[0]).c_str());
						}
						if(i->first=="trackname"){
							music->trackName=i->second[0];
						}
						if(i->first=="author"){
							music->author=i->second[0];
						}
						if(i->first=="license"){
							music->license=i->second[0];
						}
						if(i->first=="start"){
							music->start=(atoi(i->second[0].c_str()));
						}
						if(i->first=="volume"){
							music->volume=(atoi(i->second[0].c_str()));
						}
						if(i->first=="loopstart"){
							music->loopStart=(atoi(i->second[0].c_str()));
						}
						if(i->first=="loopend"){
							music->loopEnd=(atoi(i->second[0].c_str()));
						}
					}

					// 마지막시간의 default 값을 설정한다.
					music->lastTime=-1;
					music->name=obj1->value[0];

					// 모음집에 추가한다.
					musicCollection[obj1->value[0]]=music;
				}

				// 음악의 이름을 반환 string에 추가한다. 이미 그걸 불러왔더라도
				// 이것은 음악 목록에 곱하는것을 허락한다..?
				//This is to allow music to be in multiple music lists.
				if(!returnString.empty())
					returnString+=',';
				returnString+=obj1->value[0];
			}
		}
	}

	// 반환 string을 반환한다.
	return returnString;
}

bool MusicManager::loadMusicList(const std::string &file){
	// .list 파일을 연다
	ifstream musicFile;
	musicFile.open(file.c_str());

	// 만약 파일이 존재하면 체크한다.
	if(musicFile){
		// 그 파일을 전달한다.
		TreeStorageNode obj;
		{
			POASerializer objSerializer;
			if(!objSerializer.readNode(musicFile,&obj,true)){
				cerr<<"ERROR: Invalid file format of music list file."<<endl;
				return false;
			}
		}

		// 목록들의 이름을 받는다.
		std::string name;
		{
			map<string,vector<string> >::iterator it=obj.attributes.find("name");
			if(it!=obj.attributes.end()){
				name=obj.attributes["name"][0];
			}else{
				cerr<<"ERROR: No name for music list "<<file<<endl;
				return false;
			}
		}

		// 만약 리스트들을 이미 불러오지 않았으면 체크한다.
		std::map<std::string,std::vector<std::string> >::iterator it=musicLists.find(name);
		if(it!=musicLists.end())
			return true;

		// entry들을 통해 순환한다.
		for(unsigned int i=0;i<obj.subNodes.size();i++){
			TreeStorageNode* obj1=obj.subNodes[i];
			if(obj1==NULL)
				continue;
			if(obj1->value.size()>=1 && obj1->name=="musicfile"){
				// 음악 파일을 불러온다.
				string result=loadMusic(getDataPath()+"music/"+obj1->value[0]);
				if(!result.empty()){
					if(result.find(',')!=string::npos){
						size_t pos=result.find(',');
						while(pos!=string::npos){
							musicLists[name].push_back(result.substr(pos,result.find(',',pos+1)));
						}
					}else{
						musicLists[name].push_back(result);
					}
				}
			}
		}
	}else{
		cerr<<"ERROR: Unable to open music list file "<<file<<endl;
		return false;
	}

	// 아무것도 틀린게 없어서 참인 값을 반환한다.
	return true;
}

void MusicManager::playMusic(const std::string &name,bool fade){
	// 음악이 실행되는데 당연하다.
	if(!enabled)
		return;

	// 만약 음아이 모음집에 있으면 체크한다.
	Music* music=musicCollection[name];
	if(music==NULL){
		cerr<<"ERROR: Unable to play music "<<name<<endl;
		return;
	}

	// 만약 우리가 앞에 하나가 희미해지면 체크한다.
	//Now check if we should fade the previous one out.
	if(fade){
	  	Mix_FadeOutMusic(375);
		// 다음 음악을 세팅한다.
		nextMusic=name;
	}else{
		if(music->loopStart<=0 && music->loop==NULL){
			Mix_FadeInMusicPos(music->music,-1,0,music->start);
		}else{
			Mix_FadeInMusicPos(music->music,0,0,music->start);
		}
		Mix_VolumeMusic(music->volume*float(atoi(getSettings()->getValue("music").c_str())/float(MIX_MAX_VOLUME)));

		// playing 포인터를 설정한다.
		playing=music;
	}
}

void MusicManager::pickMusic(){
	// 현재목록이 존재하는게 당연하다
	vector<std::string> list=musicLists[currentList];
	if(currentList.empty()){
		cerr<<"ERROR: Unkown music list "<<currentList<<endl;
		return;
	}

	//Shuffle the list.
	random_shuffle(list.begin(),list.end());

	//Now loop through the music and search the oldest.
	Music* oldest=NULL;
	for(unsigned int i=0;i<list.size();i++){
		//Check if oldest is set.
		if(oldest==NULL){
			//It isn't so pick the first music.
			oldest=musicCollection[list[i]];
			continue;
		}

		//Check if this song is null.
		if(musicCollection[list[i]]==NULL)
			continue;

		//Check if this music is never played.
		if(musicCollection[list[i]]->lastTime==-1){
			oldest=musicCollection[list[i]];
			//And break out.
			break;
		}

		//Check if this music is older.
		if(musicCollection[list[i]]->lastTime<oldest->lastTime){
			oldest=musicCollection[list[i]];
		}
	}

	//Check if oldest ins't null.
	if(oldest!=NULL){
		playMusic(oldest->name);
		//Set the lastTime and increase it.
		oldest->lastTime=lastTime;
		lastTime++;
	}
}

void MusicManager::musicStopped(){
	//Check if there's a music to play.
	if(!nextMusic.empty()){
		//Check if the music is in the collection.
		Music* music=musicCollection[nextMusic];
		if(music==NULL){
			cerr<<"ERROR: Unable to play music "<<nextMusic<<endl;
			return;
		}

		if(music->loopStart<=0){
			Mix_FadeInMusicPos(music->music,-1,375,music->start);
		}else{
			Mix_FadeInMusicPos(music->music,0,375,music->start);
		}

		Mix_VolumeMusic(music->volume*float(atoi(getSettings()->getValue("music").c_str())/float(MIX_MAX_VOLUME)));

		//Set playing.
		playing=music;

		//Now reset nextMusic.
		nextMusic.clear();
	}else{
		//Check what kind of loop.
		if(playing->loop!=NULL){
			Mix_FadeInMusicPos(playing->loop,-1,0,playing->loopStart);
		}else{
			//This is for looping the end of music.
			Mix_FadeInMusicPos(playing->music,0,0,playing->loopStart);
		}
	}
}

void MusicManager::setMusicList(const string &list){
	//Check if the list exists.

}

vector<string> MusicManager::createCredits(){
	//Vector that will be returned.
	vector<string> result;

	//Loop through the music tracks.
	std::map<std::string,Music*>::iterator it;
	for(it=musicCollection.begin();it!=musicCollection.end();it++){
		result.push_back("    - "+it->second->trackName);
		result.push_back("        License: "+it->second->license);
		result.push_back("        Attribution: "+it->second->author);
	}

	//And return the result.
	return result;
}
