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

#ifndef TITLE_MENU_H
#define TITLE_MENU_H

#include <SDL/SDL.h>
#include "GameState.h"

// option메뉴를 포함한다.
#include "GUIObject.h"
#include "GUIListBox.h"
#include "GUISlider.h"


// 메인메뉴
class Menu : public GameState{
private:
	// 메인메뉴의 티이틀
	SDL_Surface* title;

	// 포인터가 가르킨다섯 메인메뉴인자를 포함하는 배열
	// 마지막 두개는 '>'  '<' 문자
	SDL_Surface* entries[7];

	// 생성 메뉴의 아이콘
	SDL_Surface* creditsIcon;

	// 정수형은 애니메이션을 위해 사용딘다.
	int animation;
public:
	// 생성자
	Menu();
	// 생성자 파괴
	~Menu();

	// GameState 로부터 상속받는다.
	void handleEvents();
	void logic();
	void render();
	void resize();
};

// 옵션메뉴
class Options : public GameState, private GUIEventCallback{
private:
	// 옵션메뉴의 제목
	SDL_Surface* title;

	// Slider는 음악볼륨을 설정하는데 사용된다.
	GUISlider* musicSlider;
	// Slider는 소리크기를 설정하는데 사용된다.
	GUISlider* soundSlider;


	// configure 소리 크리고 점프소리가참조에  사용된다.
	Mix_Chunk* jumpSound;
	// 정수형은 시간의 트랙을 보존한다. 넘긴다. 이후로 테스트 사운드의 마지막 플레이를
	int lastJumpSound;

	// ListBox가 포함한 테마를 사용자는 골라낼 수 있다.
	GUISingleLineListBox* theme;

	// Map은 테마가 저장된 위치를 포함한다.
	// 키는 테마의 이름이고, path의 값이다.
	std::map<std::string,std::string> themeLocations;

	// 이용가능한 언어
	GUISingleLineListBox* langs;
	std::vector<std::string> langValues;

	// 해결 목록
	GUISingleLineListBox* resolutions;

	// GUI 이벤트를 여기서 조정할 수 있다.
	// NAME : 구성요소의 이름이 사건을 포함한다.
	// OBJ : event를 포함하는 객체를 가르킨다.
	// eventType : 사건을 포함하는 정수형
	void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType);

public:
	// 생성자
	Options();
	// 생성자 파괴
	~Options();

	// option메뉴를 위해 gui를 생성하는 메소드
	void createGUI();

	// gamestate를 상속받는다.
	void handleEvents();
	void logic();
	void render();
	void resize();
};

// 해결의 가장 심플한 구조체
struct _res{
	int w,h;
};

// 메뉴생성
class Credits : public GameState, private GUIEventCallback{
private:
	// 메뉴생성의 제목
	SDL_Surface* title;
	SDL_Surface* creditsText;

	// 스크롤바
	GUIScrollBar* scrollbarH;
	GUIScrollBar* scrollbarV;

	// GUI 이벤트를 여기서 조정한다.
	// name : 이벤트가 깨어난 구성요서의 이름
	// OBJ : 사건을 포함하는 객체의 포인터
	// eventType : EVENT타입을 포함하는 정수형
	void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType);

public:
	// 생성자
	Credits();
	// 파괴자
	~Credits();

	// 옵션메뉴의 GUI를 생성하는 메소드
	void createGUI();

	// GameState로부터 상속받는다.
	void handleEvents();
	void logic();
	void render();
	void resize();
};

#endif
