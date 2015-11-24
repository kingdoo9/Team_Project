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
	//Integer containing the relative time used to store in the level.
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

	//Surface used for drawing transparent selection/dragging.
	SDL_Surface* placement;

	//A circle at the location of moving positions in configure mode.
	SDL_Surface* movingMark;


	//The current type of block to place in Add mode.
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
	//SDL_Rect used to store the camera's location when entering playMode.
	SDL_Rect cameraSave;

	//선택된 것이 드래그 되었는지 확인하는 bool 함수
	bool selectionDrag;
	//Pointer to the gameobject that's the center of the drag.
	GameObject* dragCenter;

	//Integer containing a unique id.
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

	//Pointer to a GUIObject for a property of the object.
	//Only used in the configure tool.
	GUIObject* objectProperty;
	//Pointer to a GUIObject for a property of the object.
	//Only used in the configure tool.
	GUIObject* secondObjectProperty;
	//Pointer to the object that is being configured.
	GameObject* configuredObject;

	//String containing the levelTheme.
	std::string levelTheme;

	//Integer containing the button of which a tool tip should be shown.
	int tooltip;

	//GUI event handling is done here.
	void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType);

	//Method that will let you configure the levelSettings.
	void levelSettings();

	//Method used to save the level.
	//fileName: Thge filename to write the level to.
	void saveLevel(string fileName);

	//Method used to convert a given x and y to snap to grid.
	//x: Pointer to the x location.
	//y: Pointer to the y location.
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
	//Inherits the function loadLevelFromNode from Game class.
	virtual void loadLevelFromNode(TreeStorageNode* obj, const std::string& fileName);

public:
	//Constructor.
	LevelEditor();
	//Destructor.
	~LevelEditor();

	//Method that will reset some default values.
	void reset();

	//Inherited from Game(State).
	void handleEvents();
	void logic();
	void render();
	void resize();

	//Method used to draw the currentType on the placement surface.
	//This will only be called when the tool is ADD.
	void showCurrentObject();
	//Method used to draw the selection that's being dragged.
	void showSelectionDrag();
	//Method used to draw configure tool specific things like moving positions, teleport lines.
	void showConfigure();

	//Method that will render the HUD.
	//It will be rendered after the placement suface but before the toolbar.
	void renderHUD();

	//Method called after loading a level.
	//It will fill the triggers vector.
	void postLoad();

	//Event that is invoked when there's a mouse click on an object.
	//obj: Pointer to the GameObject clicked on.
	//selected: Boolean if the GameObject that has been clicked on was selected.
	void onClickObject(GameObject* obj,bool selected);
	//Event that is invoked when there's a right mouse button click on an object.
	//obj: Pointer to the GameObject clicked on.
	//selected: Boolean if the GameObject that has been clicked on was selected.
	void onRightClickObject(GameObject* obj,bool selected);
	//Event that is invoked when there's a mouse click but not on any object.
	//x: The x location of the click on the game field (+= camera.x).
	//y: The y location of the click on the game field (+= camera.y).
	void onClickVoid(int x,int y);
	//Event that is invoked when the dragging starts.
	//x: The x location the drag started. (mouse.x+camera.x)
	//y: The y location the drag started. (mouse.y+camera.y)
	void onDragStart(int x,int y);
	//Event that is invoked when the mouse is dragged.
	//dx: The relative x distance the mouse dragged.
	//dy: The relative y distance the mouse dragged.
	void onDrag(int dx,int dy);
	//Event that is invoked when the mouse stopped dragging.
	//x: The x location the drag stopped. (mouse.x+camera.x)
	//y: The y location the drag stopped. (mouse.y+camera.y)
	void onDrop(int x,int y);
	//Event that is invoked when the camera is moved.
	//dx: The relative x distance the camera moved.
	//dy: The relative y distance the camera moved.
	void onCameraMove(int dx,int dy);
	//Event that is invoked when enter is pressed above an object.
	//obj: Pointer to the GameObject entered above.
	void onEnterObject(GameObject* obj);

	//Method used to add a GameObject to the level.
	//obj: Pointer to the gameobject to add.
	void addObject(GameObject* obj);
	//Method used to move a GameObject from the level.
	//obj: Pointer to the gameobject to move.
	//x: The new x location of the GameObject.
	//y: The new y location of the GameObject.
	void moveObject(GameObject* obj,int x,int y);
	//Method used to remove a GameObject from the level.
	//obj: Pointer to the gameobject to remove.
	void removeObject(GameObject* obj);
};
#endif
