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

#ifndef LEVELPACKMANAGER_H
#define LEVELPACKMANAGER_H

#include "LevelPack.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string>
#include <map>
#include <vector>

//levelpacks를 관리하고 불러오는 클래스
class LevelPackManager{
public:
	//생성자.
	LevelPackManager(){}
	//소멸자.
	~LevelPackManager();

	//levelpack을 불러오고 map에 추가시킨다.
	//name: levelpack 파일의 name
	void loadLevelPack(std::string name);

	//levelpack을 levelpackManager에 넣는다.
	//levelpack: levelpack을 추가하는 pointer
	void addLevelPack(LevelPack* levelpack);

	//levelPackManager으로부터 온 levelpack을 지운다.
	//name: 지울 levelpack의 이름
	void removeLevelPack(std::string name);

	//levelpack을 반환시킬 함수
	//name: levelpack의 이름
	//Returns: 요청된 levelpack의 이름
	LevelPack* getLevelPack(std::string name);

	//levelpacks의 모든것을 포함하는 vector를 return시키는 함수
	//type: list type이다 기본값은 ALL_Packs다.
	std::vector<std::string> enumLevelPacks(int type=ALL_PACKS);

	//levelpacks의 변화를 update 시켜줄 함수
	//NOTE:Options menu의 언어를 바꿀때 나타나는 변화를 업데이트 시켜준다.
	void updateLanguage();

	//levelpacks을 destroy해줌.
	void destroy();

	//levelpack lists의 다른타입을 포함하는 enum 변수
	enum LevelPackLists
	{
		//모든 levelpack을 포함하는 lists
		ALL_PACKS,
		//모든 custom levelpacks(또는 level)를 포함하는 lists
		CUSTOM_PACKS

	};
private:
	//levelpack을 포함하는 map.
	//key는 levelpack의 name을 포함하고 levelpack의 포인터 값을 가진다.
	std::map<std::string,LevelPack*> levelpacks;
};

#endif
