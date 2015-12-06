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

#include "GameState.h"
#include "Functions.h"
#include "FileManager.h"
#include "Globals.h"
#include "Objects.h"
#include "LevelSelect.h"
#include "GUIObject.h"
#include "GUIListBox.h"
#include "GUIScrollBar.h"
#include "InputManager.h"
#include "Game.h"
#include <SDL/SDL_ttf.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>

#include "libs/tinyformat/tinyformat.h"

using namespace std;

////////////////////NUMBER////////////////////////
Number::Number(){
	image=NULL;
	number=0;
	medal=0;
	selected=false;
	locked=false;

	//초기 화면(demention)을 설정 한다.
	box.x=0;
	box.y=0;
	box.h=50;
	box.w=50;

	//medals의 이미지를 불러온다.
	background=loadImage(getDataPath()+"gfx/level.png");
	backgroundLocked=loadImage(getDataPath()+"gfx/levellocked.png");
	medals=loadImage(getDataPath()+"gfx/medals.png");
}

Number::~Number(){
	//SDLSurface free 해야한다.
	if(image) SDL_FreeSurface(image);
}

void Number::init(int number,SDL_Rect box){
	//number를 처음으로 설정하고 status를 업데이트 한다.
	this->number=number;

	//text를 작성 0으로 시작하는게 아니라 1부터 세기 때문에 number+1을 한다.
	std::stringstream text;
	number++;
	text<<number;

	//text 이미지를 생성
	SDL_Color black={0,0,0};
	if(image) SDL_FreeSurface(image);
	//text 이미지를 생성
	//사용할 폰트를 체크한다. 만약 수가 100 이상이라면 작은 font사이즈를 사용
	image=TTF_RenderUTF8_Blended(fontGUI,text.str().c_str(),black);

	//number의 새로운 위치를 지정
	this->box.x=box.x;
	this->box.y=box.y;
}

void Number::init(std::string text,SDL_Rect box){
	//number를 처음으로 설정하고 status를 업데이트 한다.
	this->number=-1;

	//text 이미지를 설정함.
	SDL_Color black={0,0,0};
	if(image) SDL_FreeSurface(image);
	image=TTF_RenderUTF8_Blended(fontGUI,text.c_str(),black);

	//number의 새로운 위치를 지정
	this->box.x=box.x;
	this->box.y=box.y;
}

void Number::show(int dy){
	//background를 그리고 yoffset을 적용한다.
	if(!locked)
		applySurface(box.x,box.y-dy,background,screen,NULL);
	else
		applySurface(box.x,box.y-dy,backgroundLocked,screen,NULL);
	//background위에 text이미지를 그린다.
	//box안 가운데 위치하도록 한다.
	applySurface((box.x+25-(image->w/2)),box.y-dy,image,screen,NULL);

	//selection mark를 draw
	if(selected){
		drawGUIBox(box.x,box.y-dy,50,50,screen,0xFFFFFF23);
	}

	//medal을 draw
	if(medal>0){
		SDL_Rect r={(medal-1)*30,0,30,30};
		applySurface(box.x+30,(box.y+30)-dy,medals,screen,&r);
	}
}

void Number::setLocked(bool locked){
	this->locked=locked;
}

void Number::setMedal(int medal){
	this->medal=medal;
}


/////////////////////LEVEL SELECT/////////////////////
LevelSelect::LevelSelect(string titleText,LevelPackManager::LevelPackLists packType){
	//선택된 level을 clear
	selectedNumber=NULL;

	//이미 계산되어있지 않다면 LEVELS_PER_ROW and LEVEL_ROWS 를 계산함.
	calcRows();

	//Render the title.
	SDL_Color black={0,0,0};
	title=TTF_RenderUTF8_Blended(fontTitle,titleText.c_str(),black);

	//create GUI (test only)
	GUIObject* obj;
	if(GUIObjectRoot){
		delete GUIObjectRoot;
		GUIObjectRoot=NULL;
	}

	GUIObjectRoot=new GUIObject(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

	//the level select scroll bar
	levelScrollBar=new GUIScrollBar(SCREEN_WIDTH*0.9,184,16,SCREEN_HEIGHT-344,ScrollBarVertical,0,0,0,1,4,true,false);
	GUIObjectRoot->childControls.push_back(levelScrollBar);

	//level pack 묘사
	levelpackDescription=new GUIObject(0,140,SCREEN_WIDTH,32,GUIObjectLabel,"",0,true,true,GUIGravityCenter);
	GUIObjectRoot->childControls.push_back(levelpackDescription);

	levelpacks=new GUISingleLineListBox((SCREEN_WIDTH-500)/2,104,500,32);
	levelpacks->name="cmdLvlPack";
	levelpacks->eventCallback=this;
	vector<string> v=getLevelPackManager()->enumLevelPacks(packType);
	levelpacks->item=v;
	levelpacks->value=0;

	//lastlevelpack을 찾을 수 있는지 체크
	for(vector<string>::iterator i=v.begin(); i!=v.end(); ++i){
		if(*i==getSettings()->getValue("lastlevelpack")){
			levelpacks->value=i-v.begin();
		}
	}

	//선택된 levelpack의 name을 얻음
	string levelpackName=levelpacks->item[levelpacks->value];
	string s1=getUserPath(USER_DATA)+"progress/"+levelpackName+".progress";

	//progress를 로드함.
	levels=getLevelPackManager()->getLevelPack(v[levelpacks->value]);
	levels->loadProgress(s1);

	//And add the levelpack single line listbox to the GUIObjectRoot.
	GUIObjectRoot->childControls.push_back(levelpacks);

	obj=new GUIObject(20,20,-1,32,GUIObjectButton,_("Back"));
	obj->name="cmdBack";
	obj->eventCallback=this;
	GUIObjectRoot->childControls.push_back(obj);

	section=1;
}

LevelSelect::~LevelSelect(){
	if(GUIObjectRoot){
		delete GUIObjectRoot;
		GUIObjectRoot=NULL;
	}
	levelScrollBar=NULL;
	levelpackDescription=NULL;

	selectedNumber=NULL;

	//Free the rendered title surface.
	SDL_FreeSurface(title);
}

void LevelSelect::calcRows(){
	//열(row)와 한열 속에 들어가는 levels의 수를 계산한다.
	LEVELS_PER_ROW=(SCREEN_WIDTH*0.8)/64;
	int LEVEL_ROWS=(SCREEN_HEIGHT-344)/64;
	LEVELS_DISPLAYED_IN_SCREEN=LEVELS_PER_ROW*LEVEL_ROWS;
}

void LevelSelect::selectNumberKeyboard(int x,int y){
	if(section==2){
		//selection을 움직임
		int realNumber=0;
		if(selectedNumber)
			realNumber=selectedNumber->getNumber()+x+(y*LEVELS_PER_ROW);

		//만약 selection이 map grid밖에 있다면 section을 바꾼다.
		if(realNumber<0 || realNumber>numbers.size()-1){
			section=1;
			for(int i=0;i<levels->getLevelCount();i++){
				numbers[i].selected=false;
				refresh();
			}
		}else{
			//그렇지 않다면 selection을 움직인다.
			if(!numbers[realNumber].getLocked()){
				for(int i=0;i<levels->getLevelCount();i++){
					numbers[i].selected=(i==realNumber);
				}
				selectNumber(realNumber,false);
			}
		}
	}else if(section==1){
		//levelpacks를 loop 돌고 GUI를 업데이트 함.
		levelpacks->value+=x;

		if(levelpacks->value<0){
			levelpacks->value=levelpacks->item.size()-1;
		}else if(levelpacks->value>levelpacks->item.size()-1){
			levelpacks->value=0;
		}

		GUIEventCallback_OnEvent("cmdLvlPack",static_cast<GUIObject*>(levelpacks),0);

		//up키가 눌렸다면 section을 바꿈.
		if(y==1){
			section=2;
			selectNumber(0,false);
			numbers[0].selected=true;
		}
	}else{
		section=clamp(section+y,0,2);
	}
}

void LevelSelect::handleEvents(){
	//SDL_QUIT 이벤트를 체크함.
	if(event.type==SDL_QUIT){
		setNextState(STATE_EXIT);
	}

	//mouse click을 체크함
	if(event.type==SDL_MOUSEBUTTONUP && event.button.button==SDL_BUTTON_LEFT){
		checkMouse();
	}

	//focus된 움직임을 체크한다.
	if(inputMgr.isKeyDownEvent(INPUTMGR_RIGHT)){
		selectNumberKeyboard(1,0);
	}else if(inputMgr.isKeyDownEvent(INPUTMGR_LEFT)){
		selectNumberKeyboard(-1,0);
	}else if(inputMgr.isKeyDownEvent(INPUTMGR_UP)){
		selectNumberKeyboard(0,-1);
	}else if(inputMgr.isKeyDownEvent(INPUTMGR_DOWN)){
		selectNumberKeyboard(0,1);
	}

	//enter키가 눌렸는지 체크한다.
	if(section==2 && inputMgr.isKeyUpEvent(INPUTMGR_SELECT)){
		selectNumber(selectedNumber->getNumber(),true);
	}

	//escape키가 눌렸는지 체크한다.
	if(inputMgr.isKeyUpEvent(INPUTMGR_ESCAPE)){
		setNextState(STATE_MENU);
	}

	//scrolling 아래인지 위인지 체크한다.
	if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_WHEELDOWN && levelScrollBar){
		if(levelScrollBar->value<levelScrollBar->maxValue) levelScrollBar->value++;
		return;
	}else if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_WHEELUP && levelScrollBar){
		if(levelScrollBar->value>0) levelScrollBar->value--;
		return;
	}
}

void LevelSelect::checkMouse(){
	int x,y,dy=0,m=numbers.size();

	//현재 마우스 위치를 얻음
	SDL_GetMouseState(&x,&y);

	//scrollbar가 있는지 체크하고 만약 그렇다면 값을 얻어온다.
	if(levelScrollBar)
		dy=levelScrollBar->value;
	//Upper bound of levels we'd like to display.
	if(m>dy*LEVELS_PER_ROW+LEVELS_DISPLAYED_IN_SCREEN)
		m=dy*LEVELS_PER_ROW+LEVELS_DISPLAYED_IN_SCREEN;
	y+=dy*64;

	SDL_Rect mouse={x,y,0,0};

	for(int n=dy*LEVELS_PER_ROW; n<m; n++){
		if(!numbers[n].getLocked()){
			if(checkCollision(mouse,numbers[n].box)==true){
				if(numbers[n].selected){
					selectNumber(n,true);
				}else{
					//현재 level을 선택한다.
					for(int i=0;i<levels->getLevelCount();i++){
						numbers[i].selected=(i==n);
					}
					selectNumber(n,false);
				}
				section=2;
				break;
			}
		}
	}
}

void LevelSelect::logic(){}

void LevelSelect::render(){
	int x,y,dy=0,m=numbers.size();
	int idx=-1;

	//현재 마우스 좌표를 얻어온다.
	SDL_GetMouseState(&x,&y);

	if(levelScrollBar)
		dy=levelScrollBar->value;
	//Upper bound of levels we'd like to display.
	if(m>dy*LEVELS_PER_ROW+LEVELS_DISPLAYED_IN_SCREEN)
		m=dy*LEVELS_PER_ROW+LEVELS_DISPLAYED_IN_SCREEN;
	y+=dy*64;

	SDL_Rect mouse={x,y,0,0};

	//menu background를 그린다.
	applySurface(0,0,menuBackground,screen,NULL);
	//title을 그린다.
	applySurface((SCREEN_WIDTH-title->w)/2,40-TITLE_FONT_RAISE,title,screen,NULL);

	//level blocks를 loop돌면서 그것들을 그린다.
	for(int n=dy*LEVELS_PER_ROW;n<m;n++){
		numbers[n].show(dy*64);
		if(numbers[n].getLocked()==false && checkCollision(mouse,numbers[n].box)==true)
			idx=n;
	}

	//tool tip text를 보여준다.
	if(idx>=0){
		renderTooltip(idx,dy);
	}
}

void LevelSelect::resize(){
	calcRows();
	refresh(false);

	//NOTE: We don't need to recreate the listbox and the back button, only resize the list.
	levelpacks->left=(SCREEN_WIDTH-500)/2;
	levelpackDescription->width = SCREEN_WIDTH;
}

void LevelSelect::GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType){
	string s;
	if(name=="cmdLvlPack"){
		getSettings()->setValue("lastlevelpack",static_cast<GUISingleLineListBox*>(obj)->item[obj->value]);
	}else if(name=="cmdBack"){
		setNextState(STATE_MENU);
		return;
	}else{
		return;
	}

	//new: reset the level list scroll bar
	if(levelScrollBar)
		levelScrollBar->value=0;

	string s1=getUserPath(USER_DATA)+"progress/"+static_cast<GUISingleLineListBox*>(obj)->item[obj->value]+".progress";
	levels=getLevelPackManager()->getLevelPack(static_cast<GUISingleLineListBox*>(obj)->item[obj->value]);
	//progress file을 로드
	levels->loadProgress(s1);

	//numbers변수 새로고침.
	refresh();
}
