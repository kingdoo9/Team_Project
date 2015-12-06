// 강승덕 소스 분석

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

#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include "GameState.h"
#include "GameObjects.h"
#include "Player.h"
#include "Game.h"
#include "GUIObject.h"
#include <vector>
#include <map>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

//움직이는 블럭을 위한 클래스
class MovingPosition{
public:
	//level에서 저장하는 상대적 time을 가지고 있는 Integer변수
	//level에 저장될
	int time;
	// x 위치
	int x;
	// y 위치
	int y;

	//생성자
	//x: 움직이는 블럭의 위치에 관련된 x포지션
	//y: 움직이는 블럭의 위치에 관련된 y포지션
	//time: 이전 포지션에서 현 포지션까지 걸린 시간
	MovingPosition(int x,int y,int time);
	//소멸자
	~MovingPosition();

	//움직인 위치를 업데이트 시켜주는 함수
	//x: 움직이는 블럭의 위치에 관련된 x포지션
	//y: 움직이는 블럭의 위치에 관련된 y포지션
	void updatePosition(int x,int y); // 포지션 업데이트
};

//툴 박스 클래스 LevelEditor.cpp 파일에 자세한 내용 있음.
class LevelEditorToolbox;

//선택시 팝업 클래스 LevelEditor.cpp 파일에 자세한 내용 있음.
class LevelEditorSelectionPopup;

//The LevelEditor state, it's based on the Game state.
class LevelEditor: public Game{
	friend class LevelEditorToolbox; //LevelEditorToolbox의 private변수까지 사용가능.
	friend class LevelEditorSelectionPopup; //LevelEditorSelectionPopup의 private변수까지 사용가능.
private:
	//사용자가 플레이중인지 테스트 중인지 알아보는 bool 변수
	bool playMode;

	//tools 안에있는 Enum 함수
	//SELECT: 블럭의 선택 및 드래그를 위한 변수
	//ADD: 블럭의 추가를 위한 변수
	//REMOVE: 블럭의 삭제를 위한 변수
	//CONFIGURE: 특수 블럭의 상태를 바꾸는 변수
	enum Tools{
		SELECT,
		ADD,
		REMOVE,
		CONFIGURE,

		NUMBER_TOOLS
	};
	//사용자가 선택한 툴
	Tools tool;
	//툴바 화면
	SDL_Surface* toolbar;
	//화면 툴바의 위치나 사이즈의 사각형 함수
	SDL_Rect toolbarRect;

	//툴박스 클래스
	LevelEditorToolbox* toolbox;

	//선택시 팝업 클래스
	LevelEditorSelectionPopup* selectionPopup;

	//선택된 GameObject의 포인터를 가지고 있는 vector
	vector<GameObject*> selection;
	//네모난 선택 마크
	SDL_Surface* selectionMark;

	//selection/dragging 을 그리기위해 사용하는 Surface
	SDL_Surface* placement;

	//configure 모드에서 움직이는 위치의 좌표에 원을 그림.
	SDL_Surface* movingMark;


	//Add 모드에서 블럭울 두기 위한 현재 블럭 type
	int currentType;

	//shift 키가 눌렸는지 확인하는 bool변수
	bool pressedShift;
	//왼쪽 클릭이 눌렸는지 확인하는 bool변수
	bool pressedLeftMouse;
	//드래그인지 확인하는 bool변수 (Left button pressed and moved)
	bool dragging;

	//카메라 x,y축 속도
	int cameraXvel;
	int cameraYvel;
	//playMode에 들어갈 때 카메라의 좌표를 저장하는 변수
	SDL_Rect cameraSave;

	//선택된 것이 드래그 되었는지 확인하는 bool 함수
	bool selectionDrag;
	//drag의 가운데 있는 gameobject의 포인터
	GameObject* dragCenter;

	//특별한 id를 가지고있는 변수
	//Everytime a new id is needed it will increase by one.
	unsigned int currentId;

	//Vector containing the trigger GameObjects.
	map<GameObject*,vector<GameObject*> > triggers;
	//설정 모드에서 트리거와 타켓이 연결되었을때 사용하는 bool 변수
	bool linking;
	//Pointer to the trigger that's is being linked.
	GameObject* linkingTrigger;

	//움직이는 게임오브젝트를 포함하는 변수
	map<GameObject*,vector<MovingPosition> > movingBlocks;
	//Integer containing the speed the block is moving for newly added blocks.
	//움직임 속도. 100으로 고정
	int movingSpeed;
	//Boolean used in configure mode when configuring moving blocks.
	bool moving;
	//Pointer to the moving block that's is being configured.
	GameObject* movingBlock;

	//마우스 가로 스피드
	int movingSpeedWidth;

	//The clipboard.
	vector<map<string,string> > clipboard;

	//object의 속성을 위한 GUIObject의 포인터
	//Only used in the configure tool.
	GUIObject* objectProperty;
	//object의 속성을 위한 GUIObject의 포인터
	//Only used in the configure tool.
	GUIObject* secondObjectProperty;
	//configured된 object의 포인터
	GameObject* configuredObject;

	//레벨테마를 포함하는 스트링
	std::string levelTheme;

	//보여준 tool tip 버튼을 포함하는 integer 변수
	int tooltip;

	//GUI 이벤트 핸들링이 이루어지는 장소
	void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType);

	//levelSettings를 알려주는 함수.
	void levelSettings();

	//level을 저장하는 함수 filename은 저장할 이름.
	void saveLevel(string fileName);

	//격자에 맞게 주어진 x와 y를 변환시키는데 사용되는 함수
	//x: x위치의 포인터
	//y: y위치의 포인터
	void snapToGrid(int* x,int* y);

public:
	//Array containing the ids of different block types in a wanted order
	//Maybe also useful to disable deprecated block types in the editor
	//PLEASE NOTE: Must be updated for new block types
	//Ordered for Edward Liis proposal:

	//Normal->Shadow->Spikes->Fragile
	//Normal moving->Shadow moving->Moving spikes
	//Conveyor belt->Shadow conveyor belt
	//Button->Switch->Portal->Swap->Checkpoint->Notification block
	//Player start->Shadow start->Exit

	static const int EDITOR_ORDER_MAX=19;
	static const int editorTileOrder[EDITOR_ORDER_MAX];

protected:
	//Game 클래스로부터 loadLevelFromNode 기능을 가져옴.
	virtual void loadLevelFromNode(TreeStorageNode* obj, const std::string& fileName);

public:
	//생성자
	LevelEditor();
	//소멸자
	~LevelEditor();

	//기본값들로 리셋
	void reset();

	//Game에서 다음 함수를 가져옴
	void handleEvents();
	void logic();
	void render();
	void resize();

	//현재 블럭 타입을 둘 수 있는 곳에 draw 시키는 함수.
	//Tool에 무언가가 들어있을 경우에만 불러오는 함수

	void showCurrentObject();
	//드래그를 시작할때 현재 블럭 타입을 draw시킴
	void showSelectionDrag();
	//움직이는 블록이나 텔레포트 같은 특정 블럭의 옵션 toolbar를 그리는 함수.
	void showConfigure();

	//HUD를 랜더링 해준다.
	//It will be rendered after the placement suface but before the toolbar.
	void renderHUD();

	//레벨을 로딩한 후 호출되는 함수
	//It will fill the triggers vector.
	void postLoad();

	//오브젝트가 클릭됬는지 알려주고 이벤트를 처리하는 함수
	//obj: 클릭된 게임오브젝트의 포인터
	//selected: 클릭된 게임오브젝트가 선택 되었는지 확인해주는 bool 대수
	void onClickObject(GameObject* obj,bool selected);
	//오브젝트가 마우스 오른쪽 클릭이 됬는지 알려주는 함수
	//obj: 클릭된 게임오브젝트의 포인터
	//selected: 클릭된 게임오브젝트가 선택 되었는지 확인해주는 bool 대수
	void onRightClickObject(GameObject* obj,bool selected);
	//마우스를 클릭하는데 아무 오브젝트가 클릭되지 않았을때 이벤트 처리 함수
	//x: The x location of the click on the game field (+= camera.x).
	//y: The y location of the click on the game field (+= camera.y).
	void onClickVoid(int x,int y);
	//마우스가 드래그를 시작할때 알려주는 함수
	//x: The x location the drag started. (mouse.x+camera.x)
	//y: The y location the drag started. (mouse.y+camera.y)
	void onDragStart(int x,int y);
	//마우스가 드래그 중일때 알려주는 함수
	//dx: The relative x distance the mouse dragged.
	//dy: The relative y distance the mouse dragged.
	void onDrag(int dx,int dy);
	//마우스의 드래그를 멈추었을때를 알려주는 함수
	//x: The x location the drag stopped. (mouse.x+camera.x)
	//y: The y location the drag stopped. (mouse.y+camera.y)
	void onDrop(int x,int y);
	//카메라가 움직일때 알려주는 함수
	//dx: The relative x distance the camera moved.
	//dy: The relative y distance the camera moved.
	void onCameraMove(int dx,int dy);
	//물체위에서 enter 키가 눌렸을때 알려주는 함수.
	//obj: 엔터키가 눌려진 GameObject의 포인터
	void onEnterObject(GameObject* obj);

	//레벨에 오브직트를 추가할때 알려주는 함수
	//obj: Pointer to the gameobject to add.
	void addObject(GameObject* obj);
	//레벨에 게임 오브젝트를 추가할때 나오는 함수
	//obj: 움직인 게임 오브젝트의 포인터
	//x: The new x location of the GameObject.
	//y: The new y location of the GameObject.
	void moveObject(GameObject* obj,int x,int y);
	//게임 오브젝트를 삭제할때 나오는 함수
	//obj: Pointer to the gameobject to remove.
	void removeObject(GameObject* obj);
};
#endif
