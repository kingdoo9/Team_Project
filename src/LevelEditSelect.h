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

#ifndef LEVELEDITSELECT_H
#define LEVELEDITSELECT_H

#include "LevelSelect.h"
#include "GameState.h"
#include "GameObjects.h"
#include "Player.h"
#include "GUIObject.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include <string>

//이 파일은 LevelEditSelect 상태 파일입니다. 여기서 levelpacks과 level을 선택할 수 있습니다.
class LevelEditSelect :public LevelSelect{
private:
	//levelselect 메인 Gui의 GUIObjectRoot의 포인터
	GUIObject* levelEditGUIObjectRoot;

	//새로운 levelpack 텍스트 필드의 포인터
	GUIObject* levelpackName;

	//levelpack button을 지우기 위한 포인터
	GUIObject* propertiesPack;
	//levelpack button을 지우기 위한 포인터
	GUIObject* removePack;

	//map button을 움직이기 위한 포인터
	GUIObject* move;
	//map button을 지우기 위한 포인터
	GUIObject* remove;
	//map button을 편집하기 위한 포인터
	GUIObject* edit;

	//현재 levelpack의 이름을 포함하고 있는 String
	std::string packName;

	//GUI elements를 생성할 함수
	//initial: 최초로 GUI가 생성되는지 아닌지의 Boolean 변수
	void createGUI(bool initial);

	//비정상적인 방법에서 현재 levelpack을 바꿀 때 불러오는 함수
	void changePack();

	//levelpack의 구체적인 설정을 popup으로 보여줄 함수
	//newPack: 새로운 levelpack인지 아닌지의 Boolean 변수
	void packProperties(bool newPack);

	//추가 level dialog를 보여줄 함수
	void addLevel();

	//level dialog의 변화를 보여줄 함수
	void moveLevel();
public:
	//생성자
	LevelEditSelect();
	//소멸자
	~LevelEditSelect();

	//LevelSelect에서 가져옴
	//change: levelpack이 바뀌었는지 아닌지의 Boolean 변수, 만약 아니라면 수를 재배열 해야 한다.
	void refresh(bool change=true);
	void selectNumber(unsigned int number,bool selected);

	//GameState에서 가져옴
	void render();

	//GameState에서 가져옴
	void resize();

	//LevelSelect에서 가져옴
	void renderTooltip(unsigned int number,int dy);

	//GUI 이벤트는 여기서 다룰 것이다.
	void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType);
};

#endif
