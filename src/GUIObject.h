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
//안지호 수정
#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include "Globals.h"
#include "Functions.h"
#include "FileManager.h"
#include <string>
#include <vector>
#include <list>

//Ids for the different GUIObject types.
//None is a special type, it has no visual form.(보여주는 형태가 아니다.)
const int GUIObjectNone=0;
//A label used to dispaly text. (텍스트를 보여줄때 쓰인다.)
const int GUIObjectLabel=1;
//Button which will invoke an event when pressed.(눌렸을때 이벤트를 발생시킨다)
const int GUIObjectButton=2;
//Checkbox which represents a boolean value and can be toggled.(체크박스 참 거짓을 확인)
const int GUIObjectCheckBox=3;
//A text box used to enter text.(텍스트를 입력하는 박스)
const int GUIObjectTextBox=5;
//Frame which is like a container.
const int GUIObjectFrame=6;

//Widget gravity properties(중력 특성 위젯)
const int GUIGravityLeft=0;
const int GUIGravityCenter=1;
const int GUIGravityRight=2;

//The event id's.
//A click event used for e.g. buttons.(GUI 가 클릭되었을때)
const int GUIEventClick=0;
//A change event used for e.g. textboxes.(GUI 이벤트가 change 되었을때)
const int GUIEventChange=1;


class GUIObject;

//Class that is used as event callback.(GUI 이벤트 콜백함수)
class GUIEventCallback{
public:
	//This method is called when an event is fired.
	//name: The name of the event.
	//obj: Pointer to the GUIObject which caused this event.
	//eventType: The type of event as defined above.
	virtual void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType)=0;
};

//Class containing the (GUI Object 클래스)
class GUIObject{
public:
	//The relative x location of the GUIObject.(가로축)
	int left;
	//The relative y location of the GUIObject.(세로축)
	int top;
	//The width of the GUIObject.(너비)
	int width;
	//The height of the GUIObject.(높이)
	int height;

	//The type of the GUIObject.(타입)
	int type;
	//The value of the GUIObject.
	//It depends on the type of GUIObject what it means.(값)
	int value;

	//The name of the GUIObject.
	std::string name;
	//The caption of the GUIObject.
	//It depends on the type of GUIObject what it is.
	std::string caption;

	//Boolean if the GUIObject is enabled.
	bool enabled; //사용가능한지
	//Boolean if the GUIObject is visible.
	bool visible;//보이는지

	//Vector containing the children of the GUIObject.
	std::vector<GUIObject*> childControls;

	//Event callback used to invoke events.
	GUIEventCallback* eventCallback;

	//Widget's gravity to centering (중력 관련 변수)
	int gravity;
	int gravityX;
	bool autoWidth;

	//Use small font
	bool smallFont;
protected:
	//The state of the GUIObject.
	//It depends on the type of GUIObject where it's used for.
	int state;

	//Surface containing some gui images.
	SDL_Surface* bmGUI;

	//Surface that can be used to cache rendered text.
	SDL_Surface* cache;
	//String containing the old caption to detect if it changed.
	std::string cachedCaption;
	//Boolean containing the previous enabled state.
	bool cachedEnabled;
public:
	//Constructor.
	//left: The relative x location of the GUIObject.
	//top: The relative y location of the GUIObject.
	//witdh: The width of the GUIObject.
	//height: The height of the GUIObject.
	//type: The type of the GUIObject.
	//caption: The text on the GUIObject.
	//value: The value of the GUIObject.
	//enabled: Boolean if the GUIObject is enabled or not.
	//visible: Boolean if the GUIObject is visisble or not.
	//gravity: The way the GUIObject needs to be aligned.
	GUIObject(int left=0,int top=0,int width=0,int height=0,int type=0,
		const char* caption=NULL,int value=0,
		bool enabled=true,bool visible=true,int gravity=0):
		left(left),top(top),width(width),height(height),
		type(type),gravity(gravity),value(value),
		enabled(enabled),visible(visible),
		eventCallback(NULL),state(0),
		cache(NULL),cachedEnabled(enabled),gravityX(0),smallFont(false)
	{
		//Make sure that caption isn't NULL before setting it.(세팅전 기본값)
		if(caption){
			GUIObject::caption=caption;
			//And set the cached caption.
			cachedCaption=caption;
		}

		if(width<=0)
			autoWidth=true;
		else
			autoWidth=false;

		//Load the gui images.
		bmGUI=loadImage(getDataPath()+"gfx/gui.png");
	}
	//Destructor.
	virtual ~GUIObject();

	//Method used to handle mouse and/or key events.(마우스나 키이벤트를 받는 함수)
	//x: The x mouse location.
	//y: The y mouse location.
	//enabled: Boolean if the parent is enabled or not.
	//visible: Boolean if the parent is visible or not.
	//processed: Boolean if the event has been processed (by the parent) or not.
	//Returns: Boolean if the event is processed by the child.
	virtual bool handleEvents(int x=0,int y=0,bool enabled=true,bool visible=true,bool processed=false);
	//Method that will render the GUIObject.(GUI Object를 보내주는 함수)
	//x: The x location to draw the GUIObject. (x+left)
	//y: The y location to draw the GUIObject. (y+top)
	//draw: Draw widget or just update it without drawing
	virtual void render(int x=0,int y=0,bool draw=true);
};

//Method used to handle the GUIEvents from the GUIEventQueue.(GUI 이벤트목록에서 이벤트를 가져와 처리하는 함수)
//kill: Boolean if an SDL_QUIT event may kill the GUIObjectRoot.
void GUIObjectHandleEvents(bool kill=false);

//A structure containing the needed variables to call an event. (GUI 이벤트 구조체)
struct GUIEvent{
	//Event callback used to invoke the event.
	GUIEventCallback* eventCallback;
	//The name of the event.
	std::string name;
	//Pointer to the object which invoked the event.
	GUIObject* obj;
	//The type of event.
	int eventType;
};

//List used to queue the gui events.
extern std::list<GUIEvent> GUIEventQueue;

#endif
