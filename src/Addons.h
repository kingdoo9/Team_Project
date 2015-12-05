/*60142233 강민경
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

#ifndef ADDONS_H
#define ADDONS_H

#include "GameState.h"
#include "GameObjects.h"
#include "GUIObject.h"
#include "GUIListBox.h"
#include <vector>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

//addons menu.
class Addons: public GameState,public GUIEventCallback{
private:
	//An addon entry.
	struct Addon{
		//addon 이름
		string name;
		//addon 타입(Level, Levelpack, Theme)
		string type;
		//Taddon file 링크
		string file;
		//폴더는 테마와 levelpacks를 들면, 애드온을 배치합니다.
		string folder;
		//저자의 이름
		string author;
		
		//애드온의 최신 버전.
		int version;
		//설치 한 경우 사용자가 설치 한 버전입니다.
		int installedVersion;
		
		//애드온이 설치되어있는 경우 부울.
		bool installed;
		//애드온이 최신 상태 인 경우 부울. (installedVersion == 버전)
		bool upToDate;
	};

	//The title.
	SDL_Surface* title;
	
	//모든 애드온을 포함하는 벡터.
	std::vector<Addon>* addons;
	
	//userpath에 애드온 파일을 가리키는 파일.
	FILE* addon;
	
	//무언가가 실패 할 때 오류를 포함해야 문자열.
	string error;
	
	//현재 선택된 애드온의 유형
	string type;
	//선택 애드온의 포인터.
	Addon* selected;
	
	//에드온 목록에 사용되는 목록입니다.
	GUIListBox* list;
	//install/uninstall 버튼
	GUIObject* actionButton;
	//업데이트에 사용되는 버튼
	GUIObject* updateButton;
	
	//The possible actions for the action button.
	//주 : 업데이트는 별도의 버튼으로 이동했다.
	enum Action{
		NONE, INSTALL, UNINSTALL
	};
	//현재의 액션.
	Action action;
public:
	//Constructor.
	Addons();
	//Destructor.
	~Addons();
	
	//GUI를 만드는 방법.
	void createGUI();
	
	// 애드온 목록을 다운로드 방법.
// 파일 : 포인터 파일로 목록을 다운로드합니다.
// 반환 값 : 파일이 성공적으로 다운되드되면 트루
	bool getAddonsList(FILE* file);
	//
	void fillAddonList(std::vector<Addons::Addon> &list,TreeStorageNode &addons,TreeStorageNode &installed);
	// 벡터에 주어진 유형의 모든 애드온을 넣습니다.
	// 타입 : 애드온이 있어야하는 타입
	// 반환 값 : 애드온을 포함하는 벡터.
	std::vector<std::string> addonsToList(const string &type);
	
	// installed_addons 파일에 설치된 애드온을 절약 할 수 방법.
 // 반환 값 : 파일이 성공적으로 저장되면 트루
	bool saveInstalledAddons();
	
	//GameState에서 상속됩니다.
	void handleEvents();
	void logic();
	void render();
	void resize();
	
	// GUI 이벤트가 처리하기 위해 사용되는 방법.
// 이름 : 콜백의 이름
// OBJ : 이벤트를 발생시킨 GUIObject 포인터.
// EVENTTYPE : 이벤트 유형 : 클릭, 변경, 등
	void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType);
	
	//현재의 액션에 액션 버튼의 텍스트를 업데이트합니다.
	void updateActionButton();
	//업데이트 버튼이 활성화 스위치 / 비활성화
	void updateUpdateButton();
};
#endif
