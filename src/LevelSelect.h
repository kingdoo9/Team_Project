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

#ifndef LEVELSELECT_H
#define LEVELSELECT_H

#include "GameState.h"
#include "GameObjects.h"
#include "Player.h"
#include "GUIObject.h"
#include "GUIScrollBar.h"
#include "GUIListBox.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include <string>

//levelselect menu안에 level을 대표하는 클래스
class Number{
private:
	//배경화면의 수
	SDL_Surface* background;
	//배경화면의 개수 그것이 잠기었을 때
	SDL_Surface* backgroundLocked;
	//text이미지의 개수
	SDL_Surface* image;

	//player가 얻을 수 있는 세개의 별을 포함하는 이미지
	SDL_Surface* medals;

	//숫자나 텍스트
	int number;
	//player가 얻을 수 있는 medal을 포함하는 integer
	//0 = none, 1 = bronze, 2 = silver, 3 = gold
	int medal;

	//숫자가 잠겼는지 아닌지 체크하는 Boolean
	bool locked;
public:
	//숫자의 크기와 위치
	SDL_Rect box;

	//선택된 수가 있다면 그것을 가리키는 무언가를 draw한다.
	bool selected;

	//생성자.
	Number();
	//소멸자.
	~Number();

	//수를 초기화 하는 함수
	//number: The number.
	//box: 숫자의 크기와 위치
	void init(int number,SDL_Rect box);

	//수를 초기화 하는 함수
	//text: The caption of the number.
	//box: 숫자의 크기와 위치
	void init(std::string text,SDL_Rect box);

	//현재 수를 얻음.
	inline int getNumber(){return number;}

	//수의 상태가 잠겼는지 설정할 때 사용하는 함수
	//locked: 잠겼는지 아닌지 확인하는 Boolean함수
	void setLocked(bool locked=true);
	//수의 잠금 상태를 되돌려주는 함수
	//Returns: 수가 잠겼으면 true를 반환
	inline bool getLocked(){return locked;}

	//이 수의 메달을 설정할 때 사용하는 함수
	//medal: The new medal for this number.
	void setMedal(int medal);

	//수를 그리는데 사용되는 함수
	//dy: The y offset.
	void show(int dy);
};

//이 파일을 LevelSelect 상태파일이다. levels와 levelpacks 을 선택 할 수 있다
class LevelSelect : public GameState,public GUIEventCallback{
protected:
	//title을 포함하는 surface
	SDL_Surface* title;

	//numbers를 포함하는 vector
	std::vector<Number> numbers;

	//선택된 level number를 포함한다.(좌측 하단 코너에서 보여짐)
	//만약 null이라면 아무것도 선택되지 않음.
	Number* selectedNumber;

	//scrollbar의 pointer
	GUIScrollBar* levelScrollBar;
	//description의 pointer
	GUIObject* levelpackDescription;

	//levelpack list의 pointer
	GUISingleLineListBox* levelpacks;

	//number위에서 마우스가 어디를 선택했는지 체크
	//만약그렇다면 number를 선택함.
	virtual void checkMouse();

	//keyboard/gamepad control 의 부분을 선택
	int section;

	//열(row)의 블럭 개수.
	int LEVELS_PER_ROW;
	//한번에 스크린에 보여지는 levels의 개수
	int LEVELS_DISPLAYED_IN_SCREEN;
public:
	//생성자.
	//titleText: 스크린 위쪽에 보여지는 title
	//packType: list에 있는 levelpack의 type들. (LevelPackManager.h안에 있다.).
	LevelSelect(std::string titleText,LevelPackManager::LevelPackLists packType=LevelPackManager::ALL_PACKS);
	//소멸자.
	virtual ~LevelSelect();

	//열(row)의 개수와 한 열(row)에 있는 levels의 개수를 계산 할 함수
	void calcRows();

	//scrollbar와 수를 update시켜주는 함수.
	//change: Boolean if the levelpack changed, if not only the numbers need to be replaced.
	virtual void refresh(bool change=true)=0;

	//number이 선택되었을 때 불러오는 함수.
	//number: The selected number.
	//selected: number이 이미 선택되었는지 아닌지의 Boolean 함수.
	virtual void selectNumber(unsigned int number,bool selected)=0;

	//Used for keyboard/gamepad navigation
	void selectNumberKeyboard(int x,int y);

	//GameState에서 불러옴.
	void handleEvents();
	void logic();
	void render();
	void resize();

	//tooltip을 render하기 위해 불러오는 함수.
	//number: The number that the tooltip should be drawn for.
	//dy: The y offset of the number, used to draw the tooltip in the right place.
	virtual void renderTooltip(unsigned int number,int dy)=0;

	//GUI이벤트는 여기서 다룬다.
	virtual void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType)=0;
};

#endif
