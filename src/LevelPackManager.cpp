// 60142234 강승덕 소스 분석
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

#include "LevelPackManager.h"
#include "LevelPack.h"
#include "FileManager.h"
#include <stdio.h>

void LevelPackManager::loadLevelPack(std::string path){
	//levelpack을 로드
	LevelPack* levelpack=new LevelPack();
	levelpack->loadLevels(path+"/levels.lst");

	//entry가 이미 존재하진 않는지 체크
	if(levelpacks.find(levelpack->levelpackName)!=levelpacks.end()){
		cerr<<"WARNING: Levelpack entry \""+levelpack->levelpackName+"\" already exist."<<endl;
		return;
	}

	//존재하지 않으면 추가한다.
	levelpacks[levelpack->levelpackName]=levelpack;
}

void LevelPackManager::addLevelPack(LevelPack* levelpack){
	//entry가 이미 존재하진 않는지 체크
	if(levelpacks.find(levelpack->levelpackName)!=levelpacks.end()){
		cerr<<"WARNING: Levelpack entry \""+levelpack->levelpackName+"\" already exist."<<endl;
		return;
	}

	//존재하지 않으면 추가한다.
	levelpacks[levelpack->levelpackName]=levelpack;
}

void LevelPackManager::removeLevelPack(std::string name){
	std::map<std::string,LevelPack*>::iterator it=levelpacks.find(name);

	//entry가 있는지 확인
	if(it!=levelpacks.end()){
		levelpacks.erase(it);
	}else{
		cerr<<"WARNING: Levelpack entry \""+name+"\" doesn't exist."<<endl;
	}
}

LevelPack* LevelPackManager::getLevelPack(std::string name){
	return levelpacks[name];
}

vector<string> LevelPackManager::enumLevelPacks(int type){
	//vector가 return 될 것이다.
	vector<string> v;

	//Now do the type dependent adding.
	switch(type){
		case ALL_PACKS:
		{
			std::map<std::string,LevelPack*>::iterator i;
			for(i=levelpacks.begin();i!=levelpacks.end();++i){
				//levels안에있는 "Custom Levels" pack 을 제외한 모든것을 추가
				if(i->first!="Custom Levels")
					v.push_back(i->first);
			}
			break;
		}
		case CUSTOM_PACKS:
		{
			std::map<std::string,LevelPack*>::iterator i;
			for(i=levelpacks.begin();i!=levelpacks.end();++i){
				//"Custom Levels" levelpack.일 경우에 custom 폴더 안에 있는 levelpacks를 추가한다.
				if(i->second->levelpackPath.find(getUserPath(USER_DATA)+"custom/")==0 || i->first=="Custom Levels"){
					v.push_back(i->first);
				}
			}
			break;
		}
	}

	//vector를 return
	return v;
}

void LevelPackManager::updateLanguage(){
	std::map<std::string,LevelPack*>::iterator i;
	for(i=levelpacks.begin();i!=levelpacks.end();++i){
		i->second->updateLanguage();
	}
}

LevelPackManager::~LevelPackManager(){
	//destroy() 함수를 부른다.
	destroy();
}

void LevelPackManager::destroy(){
	//levelpacks를 loop돌면서 delete한다.
	std::map<std::string,LevelPack*>::iterator i;
	for(i=levelpacks.begin();i!=levelpacks.end();++i){
		delete i->second;
	}
	levelpacks.clear();
}
