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

#ifndef LEVELPLAYSELECT_H
#define LEVELPLAYSELECT_H

#include "GameState.h"
#include "LevelSelect.h"
#include "GameObjects.h"
#include "Player.h"
#include "GUIObject.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include <string>

//이 파일을 LevelSelect 상태파일이다. levels와 levelpacks 을 선택 할 수 있다
class LevelPlaySelect : public LevelSelect{
private:
	//level이 선택되었을때만 play button의 포인터를 보여준다.
	GUIObject* play;

	//repalys를 시작할 버튼으로 사용할 play icon이미지
	SDL_Surface* playButtonImage;

	//time icon을 포함하는 이미지
	SDL_Surface* timeIcon;
	//recordings icon을 포함하는 이미지
	SDL_Surface* recordingsIcon;

	//GUI leements를 생성할 함수
	//initial: 최초로 gui가 생성되었는지 확인하는 Boolean
	void createGUI(bool initial);

	//level info를 보여줌
	void displayLevelInfo(int number);

	//마우스가 어느 숫자위를 클릭하는지 체크함
	//만약 그렇다면 level을 시작함
	void checkMouse();
public:
	//생성자.
	LevelPlaySelect();
	//소멸자.
	~LevelPlaySelect();

	//LevelSelect에서 가져옴
	void refresh(bool change=true);
	void selectNumber(unsigned int number,bool selected);

	//GameState에서 가져옴
	void render();

	//GameState에서 가져옴
	void resize();

	//LevelSelect에서 가져옴
	void renderTooltip(unsigned int number,int dy);

	//GUI 이벤트는 여기서 다룸.
	void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType);
};

#endif
