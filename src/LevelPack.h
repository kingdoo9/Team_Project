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

#ifndef LEVELPACK_H
#define LEVELPACK_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include <string>
#include "GameObjects.h"
#include "Player.h"

#include "libs/tinygettext/tinygettext.hpp"


class LevelPack{
public:
	//A level entry structure.
	struct Level{
		//level의 이름
		string name;
		//level의 파일이름
		string file;

		//level이 잠겼는지 확인하는 Boolean
		bool locked;
		//level을 이겼는지 확인하는 Boolean
		bool won;

		//level을 끝내기위해 걸리는 몇번의 째깍거림을 가지고있는 정수 (40 = 1S)
		//만약 time이 없다면 값은 -1
		int time;
		//medal을 얻는데까지 목적 시간을 가지고 있다.
		int targetTime;

		//level을 끝내기 위해 사용한 녹화의 개수를 가지고있다.
		//이기지 못했다면 값은 -1
		int recordings;
		//medal을 얻기 위한 목적 녹화의 개수를 가지고있다.
		int targetRecordings;

		//level 노드의 MD5
		unsigned char md5Digest[16];
	};
private:
	//현재 level의 index
	int currentLevel;

	//level이 로드 되었는지의 Boolean
	bool loaded;

	//Vector는 level의 파일이름을 가지고 있다.
	std::vector<Level> levels;

	//level progress의 파일 이름
	std::string levelProgressFile;

public:
	//levelpack의 이름
	std::string levelpackName;
	//Tlevelpack이 저장되어있는 위치
	std::string levelpackPath;
	//levelpack의 서술
	std::string levelpackDescription;

	//levels이 끝났을 때 text를 보여줌
	std::string congratulationText;

	//문장을 번역하는데 사용되는 levelpack의 dictionaryManager
	tinygettext::DictionaryManager* dictionaryManager;

	//생성자.
	LevelPack();
	//소멸자.
	~LevelPack();

	//gettext 함수
	inline tinygettext::DictionaryManager* getDictionaryManager() const{
		return dictionaryManager;
	}

	//Method for updating the language to the configured one.
	//NOTE: This is called when changing the translation in the Options menu.
	void updateLanguage();

	//levels에 level을 추가
	//levelFileName: 추가를 위한 level의 파일 이름
	//level: 추가를 위한 level의 index
	void addLevel(const std::string& levelFileName,int levelno=-1);
	//levels에 있는 level을 삭제
	//level: 삭제하기 위한 level의 index
	void removeLevel(unsigned int level);
	//주어진 index로 level을 옮김
	//level1: 옮겨질 level
	//level2: 목적지
	void moveLevel(unsigned int level1,unsigned int level2);
	//레벨을 바꿈
	//level1: 바꾸기위한 첫번째 level
	//level2: 바꾸기위한 두번째 level
	void swapLevel(unsigned int level1,unsigned int level2);

	//주어진 level을 위해 levelFile을 얻음
	//levelfileName으로부터 level의 인덱스를 얻는다.
	//Returns: levelFileName을 포함한 String
	const std::string& getLevelFile(int level=-1);
	//levels의 levelpackPath를 얻음
	//Returns: levelpackPath를 포함한 String
	const std::string& getLevelpackPath();
	//주어진 level을 위한 levelName을 얻음
	//levelName으로부터 얻는 level index
	//Returns: levelName를 포함한 String
	const std::string& getLevelName(int level=-1);
	//주어진 level을 위해 levelName을 설정
	//level: levelName으로부터 얻는 level index
	//name: level의 새로운 이름
	void setLevelName(unsigned int level,const std::string& name);
	//주어진 level을 위한 MD5를 얻음
	//level: The level index.
	//Returns: digest를 포함한 상수형 unsigned char[16]
	const unsigned char* getLevelMD5(int level=-1);

	//level의 자동 저장 기록 경로를 얻음
	//level의 MD5, 파일 이름 그리고 다른 정보를 사용
	void getLevelAutoSaveRecordPath(int level,std::string &bestTimeFilePath,std::string &bestRecordingFilePath,bool createPath);

	//currentLevel 설정
	//level: 새로운 current level.
	void setCurrentLevel(unsigned int level);
	//currentLevel을 얻음
	//Returns: The currentLevel.
	inline int getCurrentLevel(){return currentLevel;}
	//levelCount를 얻음
	//Returns: The level count.
	inline int getLevelCount(){return levels.size();}

	//요청된 level을 return시킬 함수
	//level: level의 index  default값은 currentLevel
	//Returns: 요청된 level 구조체의 Pointer
	struct Level* getLevel(int level=-1);

	//주어진 level에 대한 모든 progress/statistics를 reset시킬 함수
	//level: level의 index  default값은 currentLevel
	void resetLevel(int level=-1);

	//level이 잠겨있는지 체크
	//level: 체크를 위한 level의 index
	//Returns: 만약 level이 잠겨있다면 true
	bool getLocked(unsigned int level);
	//level이 잠긴지 아닌지 설정
	//level: The level to (un)lock.
	//locked: level의 새로운 잠금 상태, default값은 잠금
	void setLocked(unsigned int level,bool locked=false);

	//levels를 clear
	void clear();


	bool loadLevels(const std::string& levelListFile);
	void loadProgress(const std::string& levelProgressFile);
	void saveLevels(const std::string& levelListFile);
	void saveLevelProgress();

	void nextLevel();

};
#endif
