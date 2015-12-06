// 60142234 강승덕 소스 분석
/*
 * Copyright (C) 2011-2012 Me and My Shadow
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

#include "LevelPack.h"
#include "Functions.h"
#include "FileManager.h"
#include "TreeStorageNode.h"
#include "POASerializer.h"
#include "MD5.h"
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
using namespace std;

LevelPack::LevelPack():currentLevel(0),loaded(false),levels(){
	//Pointer dictionaryManager을 null로 설정한다.
	dictionaryManager=NULL;
}

LevelPack::~LevelPack(){
	//dictionaryManager을 포함하여 이미 삭제를 관리하고있어서 celar함수를 불러온다.
	clear();
}

void LevelPack::clear(){
	currentLevel=0;
	loaded=false;
	levels.clear();
	levelpackDescription.clear();
	levelpackPath.clear();
	levelProgressFile.clear();
	congratulationText.clear();

	//만약 null이 아니라면 dictionaryManager를 삭제함
	if(dictionaryManager){
		delete dictionaryManager;
		dictionaryManager=NULL;
	}
}

bool LevelPack::loadLevels(const std::string& levelListFile){
	//우선 먼저 있었던 levels를 모두 없애고 새로운 levellist를 불러올 것이다.
	clear();

	//levelListFile이 비어있다면 아무것도 불러오지 않은것이고 false를 return한다.
	if(levelListFile.empty()){
		cerr<<"ERROR: No levellist file given."<<endl;
		return false;
	}

	//LevelListFile의 과정, LevelListFile이 문장이기때문에 새로운 String을 만든다.
	string levelListNew=levelListFile;
	levelpackPath=pathFromFileName(levelListNew);

	//두개의 stream을 받아온다. 하나는 Levellistfile에 쓰일 것이고 다른 하나는 levelProgress에 쓰인다.
	ifstream level(levelListNew.c_str());

	if(!level){
		cerr<<"ERROR: Can't load level list "<<levelListNew<<endl;
		return false;
	}

	//level list 을 불러온다.
	TreeStorageNode obj;
	{
		POASerializer objSerializer;
		if(!objSerializer.readNode(level,&obj,true)){
			cerr<<"ERROR: Invalid file format of level list "<<levelListNew<<endl;
			return false;
		}
	}

	//levels에 대한 변화가 있는지 체크
	{
		//levelpack이 변화가 있는지 감지하기 위해 local 폴더에 있는 levelpack을 열어본다.
		vector<string> v;
		v=enumAllDirs(pathFromFileName(levelListNew),false);
		if(std::find(v.begin(),v.end(),"locale")!=v.end()){
			//폴더는 levelDictionaryManager를 나타낸다.
			dictionaryManager=new tinygettext::DictionaryManager();
			dictionaryManager->add_directory(pathFromFileName(levelListNew)+"locale/");
			dictionaryManager->set_charset("UTF-8");
			dictionaryManager->set_language(tinygettext::Language::from_name(language));
		}else{
			dictionaryManager=NULL;
		}
	}

	//name을 본다.
	{
		vector<string> &v=obj.attributes["name"];
		if(!v.empty()){
			levelpackName=v[0];
		}else{
			//Name이 정의되지 않아서 폴더 name을 가져온다.
			levelpackName=pathFromFileName(levelListFile);
			//마지막 문자 '/'를 지운다.
			levelpackName=levelpackName.substr(0,levelpackName.size()-1);
			levelpackName=fileNameFromPath(levelpackName);
		}
	}

	//description변수를 살펴보자.
	{
		vector<string> &v=obj.attributes["description"];
		if(!v.empty())
			levelpackDescription=v[0];
	}

	//congratulation text를 살펴보자.
	{
		vector<string> &v=obj.attributes["congratulations"];
		if(!v.empty())
			congratulationText=v[0];
	}

	//level list 속을 Loop 돈다.
	for(unsigned int i=0;i<obj.subNodes.size();i++){
		TreeStorageNode* obj1=obj.subNodes[i];
		if(obj1==NULL)
			continue;
		if(obj1->value.size()>=1 && obj1->name=="levelfile"){
			Level level;
			level.file=obj1->value[0];
			level.targetTime=0;
			level.targetRecordings=0;

			//name과 목적 time/recordings를 불러오기 위해 levelfile을 염
			TreeStorageNode obj;
			POASerializer objSerializer;
			if(objSerializer.loadNodeFromFile((levelpackPath+level.file).c_str(),&obj,true)){
				//obj.attributes라는 질의문이 내부 구조를 바꿀것이므로 MD5를 계산한다.
				obj.name.clear();
				obj.calcMD5(level.md5Digest);

				//level의 name을 얻는다.
				vector<string>& v=obj.attributes["name"];
				if(!v.empty())
					level.name=v[0];
				//만약 name이 비어있다면 filename을 설정해준다.
				if(level.name.empty())
					level.name=fileNameFromPath(level.file);

				//level의 목적 time을 얻는다.
				v=obj.attributes["time"];
				if(!v.empty())
					level.targetTime=atoi(v[0].c_str());
				else
					level.targetTime=-1;
				//level의 목적 recordings를 얻는다.
				v=obj.attributes["recordings"];
				if(!v.empty())
					level.targetRecordings=atoi(v[0].c_str());
				else
					level.targetRecordings=-1;
			}

			//그것이 최초가 아니라면 Locked을 true로 설정.
			level.locked=!levels.empty();
			level.won=false;
			level.time=-1;
			level.recordings=-1;

			//levels에 level을 추가한다.
			levels.push_back(level);
		}
	}

	loaded=true;
	return true;
}

void LevelPack::loadProgress(const std::string& levelProgressFile){
	//levelProgress 파일을 연다.
	ifstream levelProgress;
	if(!levelProgressFile.empty()){
		this->levelProgressFile=levelProgressFile;
		levelProgress.open(processFileName(this->levelProgressFile).c_str());
	}

	//file이 존재하는지 체크한다.
	if(levelProgress){
		//progress/statistics을 불러온다.
		TreeStorageNode obj;
		{
			POASerializer objSerializer;
			if(!objSerializer.readNode(levelProgress,&obj,true)){
				cerr<<"ERROR: Invalid file format of level progress file."<<endl;
			}
		}

		//entries를 Loop 돈다.
		for(unsigned int i=0;i<obj.subNodes.size();i++){
			TreeStorageNode* obj1=obj.subNodes[i];
			if(obj1==NULL)
				continue;
			if(obj1->value.size()>=1 && obj1->name=="level"){
				//level을 위한 entry를 찾고 적합한 level을 검색한다.
				Level* level=NULL;
				for(unsigned int o=0;o<levels.size();o++){
					if(obj1->value[0]==levels[o].file){
						level=&levels[o];
						break;
					}
				}

				//level을 찾았는지 체크한다.
				if(!level)
					continue;

				//progress/statistics를 얻는다.
				for(map<string,vector<string> >::iterator i=obj1->attributes.begin();i!=obj1->attributes.end();++i){
					if(i->first=="locked"){
					level->locked=(i->second[0]=="1");
					}
					if(i->first=="won"){
						level->won=(i->second[0]=="1");
					}
					if(i->first=="time"){
						level->time=(atoi(i->second[0].c_str()));
					}
					if(i->first=="recordings"){
						level->recordings=(atoi(i->second[0].c_str()));
					}
				}
			}
		}
	}
}

void LevelPack::saveLevels(const std::string& levelListFile){
	//fileName을 얻는다.
	string levelListNew=processFileName(levelListFile);
	//output stream을 연다.
	ofstream level(levelListNew.c_str());

	//사용 할 수 있는 file인지 체크한다.
	if(!level){
		cerr<<"ERROR: Can't save level list "<<levelListNew<<endl;
		return;
	}

	//쓰여진 data를 포함할 node를 저장한다.
	TreeStorageNode obj;

	//description을 확실하게함.
	if(!levelpackDescription.empty())
		obj.attributes["description"].push_back(levelpackDescription);

	//congratulation text을 다시 설정.
	if(!congratulationText.empty())
		obj.attributes["congratulations"].push_back(congratulationText);

	//file의 level을 추가한다.
	for(unsigned int i=0;i<levels.size();i++){
		TreeStorageNode* obj1=new TreeStorageNode;
		obj1->name="levelfile";
		obj1->value.push_back(fileNameFromPath(levels[i].file));
		obj1->value.push_back(levels[i].name);
		obj.subNodes.push_back(obj1);
	}

	//Write the it away.
	POASerializer objSerializer;
	objSerializer.writeNode(&obj,level,false,true);
}

void LevelPack::updateLanguage() {
	if(dictionaryManager!=NULL)
		dictionaryManager->set_language(tinygettext::Language::from_name(language));
}

void LevelPack::addLevel(const string& levelFileName,int levelno){
	//세부 사항을 채운다.
	Level level;
	if(!levelpackPath.empty() && levelFileName.compare(0,levelpackPath.length(),levelpackPath)==0){
		level.file=fileNameFromPath(levelFileName);
	}else{
		level.file=levelFileName;
	}
	level.targetTime=0;
	level.targetRecordings=0;

	//level의 name을 얻음
	TreeStorageNode obj;
	POASerializer objSerializer;
	if(objSerializer.loadNodeFromFile(levelFileName.c_str(),&obj,true)){
		//obj.attributes라는 질의문이 내부 구조를 바꿀것이므로 MD5를 계산한다.
		obj.name.clear();
		obj.calcMD5(level.md5Digest);

		//level의 이름을 얻는다.
		vector<string>& v=obj.attributes["name"];
		if(!v.empty())
			level.name=v[0];
		//If the name is empty then we set it to the file name.
		//name이 비어있다면 file name에 그것을 설정한다.
		if(level.name.empty())
			level.name=fileNameFromPath(levelFileName);

		//level의 목적 time을 얻어옴
		v=obj.attributes["time"];
		if(!v.empty())
			level.targetTime=atoi(v[0].c_str());
		else
			level.targetTime=-1;
		//level의 목적 recordings을 얻어옴
		v=obj.attributes["recordings"];
		if(!v.empty())
			level.targetRecordings=atoi(v[0].c_str());
		else
			level.targetRecordings=-1;
	}
	//Locked인지 아닌지를 설정한다.
	level.won=false;
	level.time=-1;
	level.recordings=-1;
	level.locked=levels.empty()?false:true;

	//level이 끝에 있거나 중간 어딘가에 있는지 체크한다.
	if(levelno<0 || levelno>=int(levels.size())){
		levels.push_back(level);
	}else{
		levels.insert(levels.begin()+levelno,level);
	}

	//NOTE: We set loaded to true.
	loaded=true;
}

void LevelPack::moveLevel(unsigned int level1,unsigned int level2){
	if(level1<0 || level1>=levels.size())
		return;
	if(level2<0 || level2>=levels.size())
		return;
	if(level1==level2)
		return;

	levels.insert(levels.begin()+level2,levels[level1]);
	if(level2<=level1)
		levels.erase(levels.begin()+level1+1);
	else
		levels.erase(levels.begin()+level1);
}

void LevelPack::saveLevelProgress(){
	//level이 로드되었고 progress file이 주어졌는지 체크
	if(!loaded || levelProgressFile.empty())
		return;

	//progress file을 연다.
	ofstream levelProgress(processFileName(levelProgressFile).c_str());
	if(!levelProgress)
		return;

	//output stream을 연다.
	TreeStorageNode node;

	//levels을 Loop
	for(unsigned int o=0;o<levels.size();o++){
		TreeStorageNode* obj=new TreeStorageNode;
		node.subNodes.push_back(obj);

		char s[64];

		//node의 이름을 설정
		obj->name="level";
		obj->value.push_back(levels[o].file);

		//값을 설정
		obj->attributes["locked"].push_back(levels[o].locked?"1":"0");
		obj->attributes["won"].push_back(levels[o].won?"1":"0");
		sprintf(s,"%d",levels[o].time);
		obj->attributes["time"].push_back(s);
		sprintf(s,"%d",levels[o].recordings);
		obj->attributes["recordings"].push_back(s);
	}


	//POASerializer을 생성하고 level node를 적는다.
	POASerializer objSerializer;
	objSerializer.writeNode(&node,levelProgress,true,true);
}

const string& LevelPack::getLevelName(int level){
	if(level<0)
		level=currentLevel;
	return levels[level].name;
}

const unsigned char* LevelPack::getLevelMD5(int level){
	if(level<0)
		level=currentLevel;
	return levels[level].md5Digest;
}

void LevelPack::getLevelAutoSaveRecordPath(int level,std::string &bestTimeFilePath,std::string &bestRecordingFilePath,bool createPath){
	if(level<0)
		level=currentLevel;

	bestTimeFilePath.clear();
	bestRecordingFilePath.clear();

	//level pack 경로를 얻는다.
	string levelpackPath=LevelPack::levelpackPath;
	string s=levels[level].file;

	//process level pack name
	for(;;){
		string::size_type lps=levelpackPath.find_last_of("/\\");
		if(lps==string::npos){
			break;
		}else if(lps==levelpackPath.size()-1){
			levelpackPath.resize(lps);
		}else{
			levelpackPath=levelpackPath.substr(lps+1);
			break;
		}
	}

	//profess file name
	{
		string::size_type lps=s.find_last_of("/\\");
		if(lps!=string::npos) s=s.substr(lps+1);
	}

	//custom level인지 아닌지 체크
	{
		string path="%USER%/records/autosave/";
		if(!levelpackPath.empty()){
			path+=levelpackPath;
			path+='/';
		}
		path=processFileName(path);
		if(createPath) createDirectory(path.c_str());
		s=path+s;
	}

	//MD5 계산
	s+='-';
	s+=Md5::toString(levels[level].md5Digest);

	//끝
	bestTimeFilePath=s+"-best-time.mnmsrec";
	bestRecordingFilePath=s+"-best-recordings.mnmsrec";
}

void LevelPack::setLevelName(unsigned int level,const std::string& name){
	if(level<levels.size())
		levels[level].name=name;
}

const string& LevelPack::getLevelFile(int level){
	if(level<0)
		level=currentLevel;
	return levels[level].file;
}

const string& LevelPack::getLevelpackPath(){
	return levelpackPath;
}

struct LevelPack::Level* LevelPack::getLevel(int level){
	if(level<0)
		return &levels[currentLevel];
	return &levels[level];
}

void LevelPack::resetLevel(int level){
	if(level<0)
		level=currentLevel;

	//default값으로 다시 설정.
	levels[level].locked=(level!=0);
	levels[level].won=false;
	levels[level].time=-1;
	levels[level].recordings=-1;
}

void LevelPack::nextLevel(){
	currentLevel++;
}

bool LevelPack::getLocked(unsigned int level){
	return levels[level].locked;
}

void LevelPack::setCurrentLevel(unsigned int level){
	currentLevel=level;
}

void LevelPack::setLocked(unsigned int level,bool locked){
	levels[level].locked=locked;
}

void LevelPack::swapLevel(unsigned int level1,unsigned int level2){
	if(level1<levels.size()&&level2<levels.size()){
		swap(levels[level1],levels[level2]);
	}
}

void LevelPack::removeLevel(unsigned int level){
	if(level<levels.size()){
		levels.erase(levels.begin()+level);
	}
}
