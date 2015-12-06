//강승덕 소스분석
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
#include "Globals.h"
#include "Functions.h"
#include "FileManager.h"
#include "GameObjects.h"
#include "ThemeManager.h"
#include "Objects.h"
#include "LevelPack.h"
#include "LevelEditor.h"
#include "TreeStorageNode.h"
#include "POASerializer.h"
#include "GUIListBox.h"
#include "GUITextArea.h"
#include "GUIOverlay.h"
#include "InputManager.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include "libs/tinyformat/tinyformat.h"

using namespace std;

static int levelTime,levelRecordings;
static GUIObject *levelTimeProperty,*levelRecordingsProperty;

//블럭들의 이름을 가지고 있는 배열 변수
static const char* blockNames[TYPE_MAX]={
	__("Block"),__("Player Start"),__("Shadow Start"),
	__("Exit"),__("Shadow Block"),__("Spikes"),
	__("Checkpoint"),__("Swap"),__("Fragile"),
	__("Moving Block"),__("Moving Shadow Block"),__("Moving Spikes"),
	__("Teleporter"),__("Button"),__("Switch"),
	__("Conveyor Belt"),__("Shadow Conveyor Belt"),__("Notification Block"),__("Collectable")
};

 //블럭이 상태 변경이 가능한지 알려주는 배열 변수
static const bool isConfigurable[TYPE_MAX]={
	false,false,false,
	false,false,false,
	false,false,true,
	true,true,true,
	true,true,true,
	true,true,true,false
};

//블럭이 링크 가능한지 알려주는 배열 변수
static const bool isLinkable[TYPE_MAX]={
	false,false,false,
	false,false,false,
	false,false,false,
	true,true,true,
	true,true,true,
	false,false,false,false
};

/////////////////LevelEditorToolbox////////////////////////

class LevelEditorToolbox{
private:
	//부모 오브젝트
	LevelEditor* parent;

	//윈도우 창의 위치
	SDL_Rect rect;

	//배경화면
	SDL_Surface *background;

	//GUI 이미지
	SDL_Surface *bmGUI;

public:
	int startRow,maxRow;
	bool visible;
	bool dragging;

public:
	SDL_Rect getRect(){
		return rect;
	}
	int width(){
		return 320;
	}
	int height(){
		return 180;
	}
	LevelEditorToolbox(LevelEditor* parent){  // 생성자
		this->parent=parent;

		visible=false;
		dragging=false;

		//열(ROW)을 계산
		startRow=0;
		maxRow=(LevelEditor::EDITOR_ORDER_MAX+4)/5;

		//사이즈를 정함
		rect.w=width();
		rect.h=height();

		//GUI 이미지를 로드
		bmGUI=loadImage(getDataPath()+"gfx/gui.png");

		//배경을 생성하고 거기에 무언가라도 DRAW
		background=SDL_CreateRGBSurface(SDL_HWSURFACE,
			rect.w,rect.h,screen->format->BitsPerPixel,
			screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,0);

		//background
		drawGUIBox(0,0,rect.w,rect.h,background,0xFFFFFFFFU);

		//caption
		{
			SDL_Rect captionRect={6,8,width()-16,32};
			//SDL_FillRect(background,&captionRect,0xCCCCCCU);

			SDL_Color fg={0,0,0};

			SDL_Surface *caption=TTF_RenderUTF8_Blended(fontGUI,_("Toolbox"),fg);

			applySurface(captionRect.x+(captionRect.w-caption->w)/2,
				captionRect.y+(captionRect.h-caption->h)/2,caption,background,NULL);

			SDL_FreeSurface(caption);
		}
	}
	~LevelEditorToolbox(){
		SDL_FreeSurface(background);
	}
	void move(int x,int y){
		if(x>SCREEN_WIDTH-rect.w) x=SCREEN_WIDTH-rect.w;
		else if(x<0) x=0;
		if(y>SCREEN_HEIGHT-rect.h) y=SCREEN_HEIGHT-rect.h;
		else if(y<0) y=0;
		rect.x=x;
		rect.y=y;
	}
	void render(){
		if(visible){
			applySurface(rect.x,rect.y,background,screen,NULL);

			//get mouse position
			int x,y;
			SDL_GetMouseState(&x,&y);
			SDL_Rect mouse={x,y,0,0};

			//닫기 버튼을 그림
			{
				//check highlight
				SDL_Rect r={rect.x+rect.w-36,rect.y+12,24,24};

				if(checkCollision(mouse,r)){
					drawGUIBox(r.x,r.y,r.w,r.h,screen,0x999999FFU);
				}

				SDL_Rect r1={112,0,16,16};
				applySurface(rect.x+rect.w-32,rect.y+16,bmGUI,screen,&r1);
			}

			//아이템 TOOL TIP
			SDL_Rect tooltipRect;
			string tooltip;

			//사용가능한 아이템 DRAW
			for(int i=0;i<2;i++){
				int j=startRow+i;
				if(j>=maxRow) j-=maxRow;

				for(int k=0;k<5;k++){
					int idx=j*5+k;
					if(idx<0 || idx>=LevelEditor::EDITOR_ORDER_MAX) break;

					SDL_Rect r={rect.x+k*60+10,rect.y+i*60+50,60,60};

					//check highlight
					if(checkCollision(mouse,r)){
						tooltipRect=r;
						tooltip=_(blockNames[LevelEditor::editorTileOrder[idx]]);

						if(parent->currentType==idx){
							drawGUIBox(r.x,r.y,r.w,r.h,screen,0x999999FFU);
						}else{
							SDL_FillRect(screen,&r,0xCCCCCC);
						}
					}else if(parent->currentType==idx){
						drawGUIBox(r.x,r.y,r.w,r.h,screen,0xCCCCCCFFU);
					}

					ThemeBlock* obj=objThemes.getBlock(LevelEditor::editorTileOrder[idx]);
					if(obj){
						obj->editorPicture.draw(screen,r.x+5,r.y+5);
					}
				}
			}

			//TOOL TIP DRAW
			if(!tooltip.empty()){
				//백그라운드 포그라운드 컬러
				SDL_Color fg={0,0,0};

				//Tool specific text.
				SDL_Surface* tip=TTF_RenderUTF8_Blended(fontText,tooltip.c_str(),fg);

				//사용가능한 TOOL TIP만 DRAW 해줌
				if(tip!=NULL){
					if(tooltipRect.y+tooltipRect.h+tip->h>SCREEN_HEIGHT-20)
						tooltipRect.y-=tip->h;
					else
						tooltipRect.y+=tooltipRect.h;

					if(tooltipRect.x+tip->w>SCREEN_WIDTH-20)
						tooltipRect.x=SCREEN_WIDTH-20-tip->w;

					//텍스트 주위에 경계선 DRAW
					Uint32 color=0xFFFFFF00|230;
					drawGUIBox(tooltipRect.x-2,tooltipRect.y-2,tip->w+4,tip->h+4,screen,color);

					//TOOL TIP 텍스트 DRAW
					SDL_BlitSurface(tip,NULL,screen,&tooltipRect);
					SDL_FreeSurface(tip);
				}
			}
		}
	}
	void handleEvents(){
		if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
			SDL_Rect mouse={event.button.x,event.button.y,0,0};

			//item이 클릭되었는지 체크
			for(int i=0;i<2;i++){
				int j=startRow+i;
				if(j>=maxRow) j-=maxRow;

				for(int k=0;k<5;k++){
					int idx=j*5+k;
					if(idx<0 || idx>=LevelEditor::EDITOR_ORDER_MAX) break;

					SDL_Rect r={rect.x+k*60+10,rect.y+i*60+50,60,60};

					//check highlight
					if(checkCollision(mouse,r)){ //마우스와 충돌하는지 체크함.
						parent->currentType=idx;
						return;
					}
				}
			}

			//이제 TOOL BOX를 그리기 시작
			dragging=true;
		}
		else if(event.type==SDL_MOUSEBUTTONUP && event.button.button==SDL_BUTTON_LEFT){
			//드래그 멈춤
			dragging=false;

			SDL_Rect mouse={event.button.x,event.button.y,0,0};

			//닫기 버튼이 클릭되었는지 체크
			{
				SDL_Rect r={rect.x+rect.w-36,rect.y+12,24,24};
				if(checkCollision(mouse,r)){
					visible=false;
					return;
				}
			}
		}
		else if(event.type==SDL_MOUSEMOTION){
			if((event.motion.state & SDL_BUTTON_LMASK)==0){
				dragging=false;
			}else if(dragging){
				move(rect.x+event.motion.xrel,rect.y+event.motion.yrel);
			}
		}
	}
};

/////////////////LevelEditorSelectionPopup/////////////////

class LevelEditorSelectionPopup{
private:
	//부모 오브젝트
	LevelEditor* parent;

	//The position of window
	SDL_Rect rect;

	//GUI image
	SDL_Surface *bmGUI;

	//선택된 게임오브젝트
	std::vector<GameObject*> selection;

	//The scrollbar
	GUIScrollBar *scrollBar;

	//Highlighted object
	GameObject* highlightedObj;

	//Highlighted button index. 0=none 1=select/deselect 2=configure 3=link 4=delete
	int highlightedBtn;
public:
	int startRow,showedRow;
	bool dragging;

	//선택에 뭐라도 남아있는지의 Boolean 변수
	bool dirty;

public:
	SDL_Rect getRect(){
		return rect;
	}
	int width(){
		return rect.w;
	}
	int height(){
		return rect.h;
	}
	void updateScrollBar(){
		int m=selection.size()-showedRow;
		if(m>0){
			if(startRow<0) startRow=0;
			else if(startRow>m) startRow=m;

			if(scrollBar==NULL){
				scrollBar=new GUIScrollBar(0,0,16,rect.h-16,ScrollBarVertical,startRow,0,m,1,showedRow);
			}

			scrollBar->visible=true;
			scrollBar->maxValue=m;
			scrollBar->value=startRow;
		}else{
			startRow=0;
			if(scrollBar){
				scrollBar->visible=false;
				scrollBar->value=0;
			}
		}
	}
	void updateSelection(){
		if(parent!=NULL){
			std::vector<GameObject*>& v=parent->levelObjects;

			for(int i=selection.size()-1;i>=0;i--){
				if(find(v.begin(),v.end(),selection[i])==v.end()){
					selection.erase(selection.begin()+i);
				}
			}

			updateScrollBar();
		}
	}
	void dismiss(){
		if(parent!=NULL && parent->selectionPopup==this){
			parent->selectionPopup=NULL;
		}
		delete this;
	}
	LevelEditorSelectionPopup(LevelEditor* parent, std::vector<GameObject*>& selection, int x=0, int y=0){
		this->parent=parent;
		this->selection=selection;

		dirty=false;
		dragging=false;
		scrollBar=NULL;
		highlightedObj=NULL;
		highlightedBtn=0;

		//윈도우 크기를 계산
		startRow=0;
		showedRow=selection.size();
		int m=SCREEN_HEIGHT/64-1;
		if(showedRow>m) showedRow=m;

		rect.w=320;
		rect.h=showedRow*64+16;

		if(x>SCREEN_WIDTH-rect.w) x=SCREEN_WIDTH-rect.w;
		else if(x<0) x=0;
		if(y>SCREEN_HEIGHT-rect.h) y=SCREEN_HEIGHT-rect.h;
		else if(y<0) y=0;
		rect.x=x;
		rect.y=y;

		updateScrollBar();

		//gui image 로드
		bmGUI=loadImage(getDataPath()+"gfx/gui.png");
	}
	~LevelEditorSelectionPopup(){
		if(scrollBar) delete scrollBar;
	}
	void move(int x,int y){
		if(x>SCREEN_WIDTH-rect.w) x=SCREEN_WIDTH-rect.w;
		else if(x<0) x=0;
		if(y>SCREEN_HEIGHT-rect.h) y=SCREEN_HEIGHT-rect.h;
		else if(y<0) y=0;
		rect.x=x;
		rect.y=y;
	}
	void render(){
		//Check dirty
		if(dirty){
			updateSelection();
			if(selection.empty()){
				dismiss();
				return;
			}
			dirty=false;
		}

		//background
		drawGUIBox(rect.x,rect.y,rect.w,rect.h,screen,0xFFFFFFFFU);

		//마우스 위치를 얻어옴.
		int x,y;
		SDL_GetMouseState(&x,&y);
		SDL_Rect mouse={x,y,0,0};

		//item의 tooltip
		SDL_Rect tooltipRect;
		string tooltip;

		if(scrollBar && scrollBar->visible){
			startRow=scrollBar->value;
		}

		highlightedObj=NULL;
		highlightedBtn=0;

		//사용 가능한 아이템 draw
		for(int i=0;i<showedRow;i++){
			int j=startRow+i;
			if(j>=(int)selection.size()) break;

			SDL_Rect r={rect.x+8,rect.y+i*64+8,rect.w-16,64};
			if(scrollBar && scrollBar->visible) r.w-=24;

			//check highlight
			if(checkCollision(mouse,r)){
				highlightedObj=selection[j];
				SDL_FillRect(screen,&r,0xCCCCCC);
			}

			int type=selection[j]->type;

			//TILE 그림을 그려준다.
			ThemeBlock* obj=objThemes.getBlock(type);
			if(obj){
				obj->editorPicture.draw(screen,r.x+7,r.y+7);
			}

			//이름을 DRAW
			SDL_Color fg={0,0,0};
			SDL_Surface* txt=TTF_RenderUTF8_Blended(fontText,_(blockNames[type]),fg);
			if(txt!=NULL){
				SDL_Rect r2={r.x+64,r.y+(64-txt->h)/2,0,0};
				SDL_BlitSurface(txt,NULL,screen,&r2);
				SDL_FreeSurface(txt);
			}

			if(parent!=NULL){
				//선택 된걸 DRAW
				{
					std::vector<GameObject*> &v=parent->selection;
					bool isSelected=find(v.begin(),v.end(),selection[j])!=v.end();

					SDL_Rect r1={isSelected?16:0,0,16,16};
					SDL_Rect r2={r.x+r.w-96,r.y+20,24,24};
					if(checkCollision(mouse,r2)){
						drawGUIBox(r2.x,r2.y,r2.w,r2.h,screen,0x999999FFU);
						tooltipRect=r2;
						tooltip=_("Select");
						highlightedBtn=1;
					}
					r2.x+=4;
					r2.y+=4;
					SDL_BlitSurface(bmGUI,&r1,screen,&r2);
				}

				//draw configure
				if(isConfigurable[type]){
					SDL_Rect r1={112,16,16,16};
					SDL_Rect r2={r.x+r.w-72,r.y+20,24,24};
					if(checkCollision(mouse,r2)){
						drawGUIBox(r2.x,r2.y,r2.w,r2.h,screen,0x999999FFU);
						tooltipRect=r2;
						tooltip=_("Configure");
						highlightedBtn=2;
					}
					r2.x+=4;
					r2.y+=4;
					SDL_BlitSurface(bmGUI,&r1,screen,&r2);
				}

				//링크 DRAW
				if(isLinkable[type]){
					SDL_Rect r1={112,32,16,16};
					SDL_Rect r2={r.x+r.w-48,r.y+20,24,24};
					if(checkCollision(mouse,r2)){
						drawGUIBox(r2.x,r2.y,r2.w,r2.h,screen,0x999999FFU);
						tooltipRect=r2;
						tooltip=_("Link");
						highlightedBtn=3;
					}
					r2.x+=4;
					r2.y+=4;
					SDL_BlitSurface(bmGUI,&r1,screen,&r2);
				}

				//draw delete
				{
					SDL_Rect r1={112,0,16,16};
					SDL_Rect r2={r.x+r.w-24,r.y+20,24,24};
					if(checkCollision(mouse,r2)){
						drawGUIBox(r2.x,r2.y,r2.w,r2.h,screen,0x999999FFU);
						tooltipRect=r2;
						tooltip=_("Delete");
						highlightedBtn=4;
					}
					r2.x+=4;
					r2.y+=4;
					SDL_BlitSurface(bmGUI,&r1,screen,&r2);
				}
			}
		}

		//draw scrollbar
		if(scrollBar && scrollBar->visible){
			scrollBar->render(rect.x+rect.w-24,rect.y+8);
		}

		//draw tooltip
		if(!tooltip.empty()){
			//The back and foreground colors.
			SDL_Color fg={0,0,0};

			//Tool specific text.
			SDL_Surface* tip=TTF_RenderUTF8_Blended(fontText,tooltip.c_str(),fg);

			//사용가능한 TOOL TIP 이 있다면 DRAW
			if(tip!=NULL){
				tooltipRect.y-=4;
				tooltipRect.h+=8;
				if(tooltipRect.y+tooltipRect.h+tip->h>SCREEN_HEIGHT-20)
					tooltipRect.y-=tip->h;
				else
					tooltipRect.y+=tooltipRect.h;

				if(tooltipRect.x+tip->w>SCREEN_WIDTH-20)
					tooltipRect.x=SCREEN_WIDTH-20-tip->w;

				//Draw borders around text
				Uint32 color=0xFFFFFF00|230;
				drawGUIBox(tooltipRect.x-2,tooltipRect.y-2,tip->w+4,tip->h+4,screen,color);

				//Draw tooltip's text
				SDL_BlitSurface(tip,NULL,screen,&tooltipRect);
				SDL_FreeSurface(tip);
			}
		}
	}
	void handleEvents(){
		//Check dirty
		if(dirty){
			updateSelection();
			if(selection.empty()){
				dismiss();
				return;
			}
			dirty=false;
		}

		//Check scrollbar event
		if(scrollBar && scrollBar->visible){
			if(scrollBar->handleEvents(rect.x+rect.w-24,rect.y+8)) return;
		}

		if(event.type==SDL_MOUSEBUTTONDOWN){
			//mousewheel 체크
			if(event.button.button==SDL_BUTTON_WHEELUP){
				startRow-=2;
				updateScrollBar();
				return;
			}else if(event.button.button==SDL_BUTTON_WHEELDOWN){
				startRow+=2;
				updateScrollBar();
				return;
			}

			//begin drag
			if(event.button.button==SDL_BUTTON_LEFT) dragging=true;
		}
		else if(event.type==SDL_MOUSEBUTTONUP && event.button.button==SDL_BUTTON_LEFT){
			//end drag
			dragging=false;

			SDL_Rect mouse={event.button.x,event.button.y,0,0};

			//Check if close it
			if(!checkCollision(mouse,rect)){
				dismiss();
				return;
			}

			//ITEM이 클릭됬는지 체크
			if(highlightedObj!=NULL && highlightedBtn>0 && parent!=NULL){
				std::vector<GameObject*>& v=parent->levelObjects;

				if(find(v.begin(),v.end(),highlightedObj)!=v.end()){
					switch(highlightedBtn){
					case 1:
						{
							std::vector<GameObject*>& v2=parent->selection;
							std::vector<GameObject*>::iterator it=find(v2.begin(),v2.end(),highlightedObj);

							if(it==v2.end()){
								v2.push_back(highlightedObj);
							}else{
								v2.erase(it);
							}
						}
						break;
					case 2:
						parent->tool=LevelEditor::CONFIGURE;
						parent->onEnterObject(highlightedObj);
						break;
					case 3:
						{
							std::vector<GameObject*>& v2=parent->selection;

							parent->tool=LevelEditor::CONFIGURE;
							parent->onRightClickObject(highlightedObj,find(v2.begin(),v2.end(),highlightedObj)!=v2.end());

							dismiss();
						}
						break;
					case 4:
						parent->removeObject(highlightedObj);
						break;
					}
				}
			}
		}
		else if(event.type==SDL_MOUSEMOTION){
			if((event.motion.state & SDL_BUTTON_LMASK)==0){
				dragging=false;
			}else if(dragging){
				move(rect.x+event.motion.xrel,rect.y+event.motion.yrel);
			}
		}
	}
};

/////////////////MovingPosition////////////////////////////
MovingPosition::MovingPosition(int x,int y,int time){
	this->x=x;
	this->y=y;
	this->time=time;
}

MovingPosition::~MovingPosition(){}

void MovingPosition::updatePosition(int x,int y){
	this->x=x;
	this->y=y;
}


/////////////////LEVEL EDITOR//////////////////////////////
LevelEditor::LevelEditor():Game(true){
	//target time 과 recordings. 을 얻는다.
	levelTime=levels->getLevel()->targetTime;
	levelRecordings=levels->getLevel()->targetRecordings;

	//몇가지를 초기값으로 설정함.
	reset();

	//게임을 로드한 후 실행됨., so do postLoad.
	postLoad();

	//TOOLBAR 로드
	toolbar=loadImage(getDataPath()+"gfx/menu/toolbar.png");
	SDL_Rect tmp={(SCREEN_WIDTH-460)/2,SCREEN_HEIGHT-50,460,50};
	toolbarRect=tmp;

	toolbox=NULL;
	selectionPopup=NULL;

	movingSpeedWidth=-1;

	//선택 마크 로드
	selectionMark=loadImage(getDataPath()+"gfx/menu/selection.png");

	//움직임
	movingMark=loadImage(getDataPath()+"gfx/menu/moving.png");

	//반투명한 화면을 만든다.
	placement=SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,SCREEN_WIDTH,SCREEN_HEIGHT,32,0x000000FF,0x0000FF00,0x00FF0000,0);
	SDL_SetColorKey(placement,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(placement->format,255,0,255));
	SDL_SetAlpha(placement,SDL_SRCALPHA,125);
}

LevelEditor::~LevelEditor(){
	//LEVELOBJECTS를 LOOP돌고 DELETE도 같이 함.
	for(unsigned int i=0;i<levelObjects.size();i++)
		delete levelObjects[i];
	levelObjects.clear();
	selection.clear();

	//Free the placement surface.
	SDL_FreeSurface(placement);

	//TOOLBOX가 있다면 삭제
	if(toolbox){
		delete toolbox;
		toolbox=NULL;
	}

	//POPUP되있다면 삭제
	if(selectionPopup){
		delete selectionPopup;
		selectionPopup=NULL;
	}

	//카메라 리셋
	camera.x=0;
	camera.y=0;
}

void LevelEditor::reset(){
	//Set some default values.
	playMode=false;
	tool=ADD;
	currentType=0;
	pressedShift=false;
	pressedLeftMouse=false;
	dragging=false;
	selectionDrag=false;
	dragCenter=NULL;
	if(LEVEL_WIDTH<SCREEN_WIDTH)
		camera.x=-(SCREEN_WIDTH-LEVEL_WIDTH)/2;
	else
		camera.x=0;
	if(LEVEL_HEIGHT<SCREEN_HEIGHT)
		camera.y=-(SCREEN_HEIGHT-LEVEL_HEIGHT)/2;
	else
		camera.y=0;
	cameraXvel=0;
	cameraYvel=0;
	objectProperty=NULL;
	secondObjectProperty=NULL;
	configuredObject=NULL;
	linking=false;
	linkingTrigger=NULL;
	currentId=0;
	movingBlock=NULL;
	moving=false;
	movingSpeed=10;
	tooltip=-1;

	//플레이어와 그림자를 시작위치에 위치시킴.
	player.setPosition(player.fx,player.fy);
	shadow.setPosition(shadow.fx,shadow.fy);

	selection.clear();
	clipboard.clear();
	triggers.clear();
	movingBlocks.clear();
}

void LevelEditor::loadLevelFromNode(TreeStorageNode* obj, const std::string& fileName){
	//기본 클래스에서 함수들을 불러온다.
	Game::loadLevelFromNode(obj,fileName);

	//now do our own stuff.
	string s=editorData["time"];
	if(s.empty() || !(s[0]>='0' && s[0]<='9')){
		levelTime=-1;
	}else{
		levelTime=atoi(s.c_str());
	}

	s=editorData["recordings"];
	if(s.empty() || !(s[0]>='0' && s[0]<='9')){
		levelRecordings=-1;
	}else{
		levelRecordings=atoi(s.c_str());
	}
}

void LevelEditor::saveLevel(string fileName){
	//출력 STREAM을 생성하고 그것이 시작됬는지 확인한다.
	std::ofstream save(fileName.c_str());
	if(!save) return;

	//The dimensions of the level.
	int maxX=0;
	int maxY=0;

	//STORAGENODE를 데이터안에 넣는다. 파일화 되기 전에.
	TreeStorageNode node;
	char s[64];

	//LEVEL의 이름
	if(!levelName.empty()){
		node.attributes["name"].push_back(levelName);

		//LEVELPACK의 LEVEL 이름을 업데이트 시킨다.
		levels->getLevel()->name=levelName;
	}

	//LEVELTHEME 부분.
	if(!levelTheme.empty())
		node.attributes["theme"].push_back(levelTheme);

	//목적시간과 녹화.
	{
		char c[32];
		if(levelTime>=0){
			sprintf(c,"%d",levelTime);
			node.attributes["time"].push_back(c);
		}
		if(levelRecordings>=0){
			sprintf(c,"%d",levelRecordings);
			node.attributes["recordings"].push_back(c);
		}
	}

	//LEVEL의 가로넓이
	maxX=LEVEL_WIDTH;
	sprintf(s,"%d",maxX);
	node.attributes["size"].push_back(s);

	//LEVEL의 세로높이
	maxY=LEVEL_HEIGHT;
	sprintf(s,"%d",maxY);
	node.attributes["size"].push_back(s);

	//GAMEOBJECT 들을 LOOP돌고 저장함.
	for(int o=0;o<(signed)levelObjects.size();o++){
		int objectType=levelObjects[o]->type;

		//GAMEOBJECT가 맞는 타입인가 확인함.
		if(objectType>=0 && objectType<TYPE_MAX){
			TreeStorageNode* obj1=new TreeStorageNode;
			node.subNodes.push_back(obj1);

			//It's a tile so name the node tile.
			obj1->name="tile";

			//Write away the type of the gameObject.
			sprintf(s,"%d",objectType);
			obj1->value.push_back(blockName[objectType]);

			//GAMEOBJECT 위치를 위해 BOX를 불러온다.
			SDL_Rect box=levelObjects[o]->getBox(BoxType_Base);
			//Put the location in the storageNode.
			sprintf(s,"%d",box.x);
			obj1->value.push_back(s);
			sprintf(s,"%d",box.y);
			obj1->value.push_back(s);

			//EditorData를 loop돌면서 저장.
			vector<pair<string,string> > obj;
			levelObjects[o]->getEditorData(obj);
			for(unsigned int i=0;i<obj.size();i++){
				if((!obj[i].first.empty()) && (!obj[i].second.empty())){
					obj1->attributes[obj[i].first].push_back(obj[i].second);
				}
			}
		}
	}

	//POASerializer을 만들어 LEVEL NODE를 쓴다.
	POASerializer objSerializer;
	objSerializer.writeNode(&node,save,true,true);
}


///////////////EVENT///////////////////
void LevelEditor::handleEvents(){
	//종료가 필요하다면 체크하고, 만약 그렇다면 종료 STATE 로 들어간다.
	if(event.type==SDL_QUIT){
		setNextState(STATE_EXIT);
	}

	//게임이 PLAY중인지 TEST중인지 확인하고 그에 따른 이벤트를 발생시킨다.
	if(playMode){
		Game::handleEvents();

		//PLAY모드를 종료시킨다면 그에 따른 SET과정.
		if(inputMgr.isKeyDownEvent(INPUTMGR_ESCAPE)){
			//play모드가 종료되고 게임을 reset
			Game::reset(true);
			playMode=false;
			camera.x=cameraSave.x;
			camera.y=cameraSave.y;

			//NOTE: To prevent the mouse to still "be pressed" we set it to false.
			pressedLeftMouse=false;
		}
	}else{
		//editor를 종료시킨지 확인.
		if(inputMgr.isKeyDownEvent(INPUTMGR_ESCAPE)){
			//종료전 최종 확인문 출력
			if(msgBox(_("Are you sure you want to quit?"),MsgBoxYesNo,_("Quit prompt"))==MsgBoxYes){
				//editor 종료.
				if(GUIObjectRoot){
					delete GUIObjectRoot;
					GUIObjectRoot=NULL;
				}
				setNextState(STATE_LEVEL_EDIT_SELECT);

				//menu 음악을 다시 틀어준다.
				getMusicManager()->playMusic("menu");
			}
		}

		//선택한 것이 popup이 되는 이벤트를 재 연결했는지 체크
		if(selectionPopup!=NULL){
			if(event.type==SDL_MOUSEBUTTONDOWN
				|| event.type==SDL_MOUSEBUTTONUP
				|| event.type==SDL_MOUSEMOTION)
			{
				selectionPopup->handleEvents();
				return;
			}
		}

		//toolbar가 클릭 되었는지 체크
		if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT && tooltip>=0){
			int t=tooltip;

			if(t<NUMBER_TOOLS){
				tool=(Tools)t;

				//toolbox를 보여줄지 말지 체크
				if(tool==ADD){
					//toolbox을 보여줌
					if(toolbox==NULL){
						toolbox=new LevelEditorToolbox(this);
						toolbox->move(event.button.x,event.button.y-toolbox->height()-20);
					}
					if(!toolbox->visible){
						toolbox->visible=true;
					}
				}
			}else{
				//선택된 버튼은 tool이 아닙니다.
				//어떤 버튼이 어떤 것인지 체크
				if(t==NUMBER_TOOLS){
					playMode=true;
					cameraSave.x=camera.x;
					cameraSave.y=camera.y;

					if(tool==CONFIGURE){
						//연결과 움직임을 멈춤.
						if(linking){
							linking=false;
							linkingTrigger=NULL;
						}

						if(moving){
							//움직이는 블럭의 경로를 적음.
							std::map<std::string,std::string> editorData;
							char s[64], s0[64];

							sprintf(s,"%d",int(movingBlocks[movingBlock].size()));
							editorData["MovingPosCount"]=s;
							//위치를 계속  loop 돈다.
							for(unsigned int o=0;o<movingBlocks[movingBlock].size();o++){
								sprintf(s0+1,"%d",o);
								sprintf(s,"%d",movingBlocks[movingBlock][o].x);
								s0[0]='x';
								editorData[s0]=s;
								sprintf(s,"%d",movingBlocks[movingBlock][o].y);
								s0[0]='y';
								editorData[s0]=s;
								sprintf(s,"%d",movingBlocks[movingBlock][o].time);
								s0[0]='t';
								editorData[s0]=s;
							}
							movingBlock->setEditorData(editorData);

							moving=false;
							movingBlock=NULL;
						}
					}
				}
				if(t==NUMBER_TOOLS+2){
					//Open up level settings dialog
					levelSettings();
				}
				if(t==NUMBER_TOOLS+4){
					//level에디터의 레벨 선택화면으로 돌아간다.
					setNextState(STATE_LEVEL_EDIT_SELECT);
					//음악은 menu 음악으로 바꾼다.
					getMusicManager()->playMusic("menu");
				}
				if(t==NUMBER_TOOLS+3){
					//현재 레벨을 저장
					saveLevel(levelFile);
					//그리고 feedback을 유저에게 준다.
					if(levelName.empty())
						msgBox(tfm::format(_("Level \"%s\" saved"),fileNameFromPath(levelFile)),MsgBoxOKOnly,_("Saved"));
					else
						msgBox(tfm::format(_("Level \"%s\" saved"),levelName),MsgBoxOKOnly,_("Saved"));
				}
			}

			return;
		}

		//shift 키가 눌렸는지 체크한다.
		if(inputMgr.isKeyDownEvent(INPUTMGR_SHIFT)){
			pressedShift=true;
		}
		if(inputMgr.isKeyUpEvent(INPUTMGR_SHIFT)){
			pressedShift=false;
		}

		//delete키가 눌렸는지 체크한다.
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_DELETE){
			if(!selection.empty()){
				//선택된 game object를 loop를 돈다.
				 while(!selection.empty()){
					//선택된 object 를 지운다.
					removeObject(selection[0]);
				}

				//vector 선택(select) 변수를 지운다.
				selection.clear();
				dragCenter=NULL;
				selectionDrag=false;
			}
		}

		//복사키 Ctrl+c 또는 Ctrl+x 가 눌렸는지 체크
		if(event.type==SDL_KEYDOWN && (event.key.keysym.sym==SDLK_c || event.key.keysym.sym==SDLK_x) && (event.key.keysym.mod & KMOD_CTRL)){
			//현재 clipboard를 지운다.
			clipboard.clear();

			//선택이 비었는지 아닌지 체크한다.
			if(!selection.empty()){
				//왼쪽 위 블럭을 찾기위해 loop를 돈다.
				int x=selection[0]->getBox().x;
				int y=selection[0]->getBox().y;
				for(unsigned int o=1; o<selection.size(); o++){
					if(selection[o]->getBox().x<x || selection[o]->getBox().y<y){
						x=selection[o]->getBox().x;
						y=selection[o]->getBox().y;
					}
				}

				//실제로 복사를 하기 위해  selection을 loop 돈다.
				for(unsigned int o=0; o<selection.size(); o++){
					//object의 editor data를 얻는다.
					vector<pair<string,string> > obj;
					selection[o]->getEditorData(obj);

					//editor data를 loop돌면서 그것을 변환시킨다.
					map<string,string> objMap;
					for(unsigned int i=0;i<obj.size();i++){
						objMap[obj[i].first]=obj[i].second;
					}
					//모델 들을 map에 추가시킨다.
					char s[64];
					sprintf(s,"%d",selection[o]->getBox().x-x);
					objMap["x"]=s;
					sprintf(s,"%d",selection[o]->getBox().y-y);
					objMap["y"]=s;
					sprintf(s,"%d",selection[o]->type);
					objMap["type"]=s;

					//triggers, buttons, mobingblocks, etc등을 고장으로부터 막기위해 id를 덮어씌운다.
					//적당하고 인식불가능한 id로서 빈 string 을 준다.
					objMap["id"]="";
					//타입이 portal이면 같은 방법으로 destination을 준다.
					if(selection[o]->type==TYPE_PORTAL){
						objMap["destination"]="";
					}

					//clipboard vector를 map에 추가한다.
					clipboard.push_back(objMap);

					if(event.key.keysym.sym==SDLK_x){
						//Ctrl + x 는 game object 를 삭제한다는 의미이다.
						removeObject(selection[o]);
						o--;
					}
				}

				//Ctrl + x를 했을때만 selection은 clear 한다.
				if(event.key.keysym.sym==SDLK_x){
					selection.clear();
					dragCenter=NULL;
					selectionDrag=false;
				}
			}
		}

		//Ctrl+v를 눌렀는지 체크
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_v && (event.key.keysym.mod & KMOD_CTRL)){
			//clipboard가 비었는지 첫번째 확인
			if(!clipboard.empty()){
				//현재 선택을 clear
				selection.clear();

				//현재 마우스 위치를 얻는다.
				int x,y;
				SDL_GetMouseState(&x,&y);
				x+=camera.x;
				y+=camera.y;

				//Apply snap to grid.
				if(!pressedShift){
					snapToGrid(&x,&y);
				}else{
					x-=25;
					y-=25;
				}

				//Integers containing the diff of the x that occurs when placing a block outside the level size on the top or left.
				// 맵의 size를 넘은 블럭이 있을 때  그 x y 값을 저장.
				//We use it to compensate the corrupted x and y locations of the other clipboard blocks.
				int diffX=0;
				int diffY=0;


				//clipboard 를 loop돈다.
				for(unsigned int o=0;o<clipboard.size();o++){
					Block* block=new Block(0,0,atoi(clipboard[o]["type"].c_str()),this);
					block->setPosition(atoi(clipboard[o]["x"].c_str())+x+diffX,atoi(clipboard[o]["y"].c_str())+y+diffY);
					block->setEditorData(clipboard[o]);

					if(block->getBox().x<0){
						//A block on the left side of the level, meaning we need to shift everything.
						//첫번째 차이를 계산
						diffX+=(0-(block->getBox().x));
					}
					if(block->getBox().y<0){
						//A block on the left side of the level, meaning we need to shift everything.
						//First calc the difference.
						diffY+=(0-(block->getBox().y));
					}

					//addObject 함수를 이용하여 object 추가
					addObject(block);

					//선택에  block을 추가.
					selection.push_back(block);
				}
			}
		}

		//return 버튼이 눌렸는지 체크
		// 만약 그렇다면 configure tool을 실행
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_RETURN){
			//Get the current mouse location.
			int x,y;
			SDL_GetMouseState(&x,&y);
			//사각형을 생성
			SDL_Rect mouse={x+camera.x,y+camera.y,0,0};

			//선택된 gameobject를 loop
			for(unsigned int o=0; o<levelObjects.size(); o++){
				//충돌 체크
				if(checkCollision(mouse,levelObjects[o]->getBox())){
					tool=CONFIGURE;
					//onEnterObject를 알려준다.
					onEnterObject(levelObjects[o]);
					//loop 를 빠져나간다.
					break;
				}
			}
		}

		//playMode=false일때 카메라 움직임을 위해서 방향키를 체크.
		cameraXvel=0;
		cameraYvel=0;
		if(inputMgr.isKeyDown(INPUTMGR_RIGHT)){
			if(pressedShift){
				cameraXvel+=10;
			}else{
				cameraXvel+=5;
			}
		}
		if(inputMgr.isKeyDown(INPUTMGR_LEFT)){
			if(pressedShift){
				cameraXvel-=10;
			}else{
				cameraXvel-=5;
			}
		}
		if(inputMgr.isKeyDown(INPUTMGR_UP)){
			if(pressedShift){
				cameraYvel-=10;
			}else{
				cameraYvel-=5;
			}
		}
		if(inputMgr.isKeyDown(INPUTMGR_DOWN)){
			if(pressedShift){
				cameraYvel+=10;
			}else{
				cameraYvel+=5;
			}
		}

		//마우스 왼쪽 버튼이 pressed 되었는지 holding 되었는지 체크.
		if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
			pressedLeftMouse=true;
		}
		if(event.type==SDL_MOUSEBUTTONUP && event.button.button==SDL_BUTTON_LEFT){
			pressedLeftMouse=false;

			//dragging 도 활성화 되었는지 확인한다.
			if(dragging){
				//dragging 을 false로 설정하고 onDrop 이벤트를 부른다.
				dragging=false;
				int x,y;
				SDL_GetMouseState(&x,&y);
				//drop 이벤트를 부른다.
				onDrop(x+camera.x,y+camera.y);
			}
		}

		//마우스가 dragging 되었는지 체크한다.
		if(pressedLeftMouse && event.type==SDL_MOUSEMOTION){
			if(abs(event.motion.xrel)+abs(event.motion.yrel)>=2){
				//dragging이 시작되었는지 체크
				if(!dragging){
					//The mouse is moved enough so let's set dragging true.
					dragging=true;
					//현재 마우스 좌표를 얻는다.
					int x,y;
					SDL_GetMouseState(&x,&y);
					//dragstart 이벤트를 부른다.
					onDragStart(x+camera.x,y+camera.y);
				}else{
					//Dragging 이 이미 true라는 말은 onDrag함수를 onDragStart함수 대신에 불렀다는 의미이다.
					onDrag(event.motion.xrel,event.motion.yrel);
				}
			}
		}

		//현재 마우스 위치를 받는다.
		int x,y;
		SDL_GetMouseState(&x,&y);
		//Create the rectangle.
		SDL_Rect mouse={x,y,0,0};

		//마우스가 툴박스 안에 있는지 체크한다.
		bool mouseInToolbox=(toolbox!=NULL && !playMode && tool==ADD && toolbox->visible
			&& (toolbox->dragging || checkCollision(mouse,toolbox->getRect())));

		//scroll 을 위로 올린것을 체크하고 이것은 currentType++을 의미한다.
		if((event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_WHEELUP) || inputMgr.isKeyDownEvent(INPUTMGR_NEXT)){
			switch(tool){
			case ADD:
				//오직 add툴을 사용할때만 현재 타입을 바꾼다.
				if(mouseInToolbox){
					if((--toolbox->startRow)<0){
						toolbox->startRow=toolbox->maxRow-1;
					}
				}else{
					currentType++;
					if(currentType>=EDITOR_ORDER_MAX){
						currentType=0;
					}
				}
				break;
			case CONFIGURE:
				//configure 모드 일때
				movingSpeed++;
				//움직임은 100으로 고정한다.
				if(movingSpeed>100){
					movingSpeed=100;
				}
				break;
			default:
				//다른 모드일때 맵을 scrolling한다.
				if(pressedShift) camera.x-=200;
				else camera.y-=200;
				break;
			}
		}
		//스크롤을 아래로 내리는걸 체크하고, 이건 currentType--를 의미한다.
		if((event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_WHEELDOWN) || inputMgr.isKeyDownEvent(INPUTMGR_PREVIOUS)){
			switch(tool){
			case ADD:
			//오직 add툴을 사용할때만 현재 타입을 바꾼다.
				if(mouseInToolbox){
					if((++toolbox->startRow)>=toolbox->maxRow){
						toolbox->startRow=0;
					}
				}else{
					currentType--;
					if(currentType<0){
						currentType=EDITOR_ORDER_MAX-1;
					}
				}
				break;
			case CONFIGURE:
				//configure mode일 때
				movingSpeed--;
				if(movingSpeed<=0){
					movingSpeed=1;
				}
				break;
			default:
			//다른 모드일때 맵을 scrolling한다.
				if(pressedShift) camera.x+=200;
				else camera.y+=200;
				break;
			}
		}

		//playMode로 들어가는것을 체크한다.
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_p){
			playMode=true;
			cameraSave.x=camera.x;
			cameraSave.y=camera.y;
		}
		//shortcuts 툴을 위한 체크
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_a){
			tool=ADD;
		}
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_s){
			tool=SELECT;
		}
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_d){
			//deletion tool에서 사용할 수 없게끔 하기 위해 selection을 clear한다.
			selection.clear();
			tool=REMOVE;
		}
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_w){
			tool=CONFIGURE;
		}

		//Check for certain events. => 확실한 이벤트를 체크한다.(?)

		//마우스가 toolbar위에 있지 않는것을 첫번째로 확인한다.
		if(checkCollision(mouse,toolbarRect)==false){
			//마우스가 툴바 안에 있는지 확인한다.
			if(mouseInToolbox){
				toolbox->handleEvents();
			}else{
				//We didn't hit the toolbar so convert the mouse location to ingame location.
				mouse.x+=camera.x;
				mouse.y+=camera.y;

				//클릭 이벤트가 끝났는지 아닌지 bool대수
				bool clickEvent=false;
				//마우스 버튼이 눌렸는지 체크한다.
				if(event.type==SDL_MOUSEBUTTONUP){
					std::vector<GameObject*> clickObjects;

					//object의 충돌에 대한  loop를 돈다.
					for(unsigned int o=0; o<levelObjects.size(); o++){
						if(checkCollision(levelObjects[o]->getBox(),mouse)==true){
							clickObjects.push_back(levelObjects[o]);
						}
					}

					if(clickObjects.size()==1){
						//여기서 충돌이란(collision) 마우스가 object 위에 있는 것을 의미한다.
						std::vector<GameObject*>::iterator it;
						it=find(selection.begin(),selection.end(),clickObjects[0]);

						//click 이벤트가 있을때 clickEvent를 true 시킨다.
						clickEvent=true;

						//클릭된 object가 selection된건지 아닌지 체크한다.
						bool isSelected=(it!=selection.end());
						if(event.button.button==SDL_BUTTON_LEFT){
							onClickObject(clickObjects[0],isSelected);
						}else if(event.button.button==SDL_BUTTON_RIGHT){
							onRightClickObject(clickObjects[0],isSelected);
						}
					}else if(clickObjects.size()>1){
						//There are more than one object under the mouse
						clickEvent=true;

						SDL_Rect r=clickObjects[0]->getBox();

						if(selectionPopup!=NULL) delete selectionPopup;
						selectionPopup=new LevelEditorSelectionPopup(this,clickObjects,
							r.x-camera.x,r.y-camera.y);
					}
				}

				//void를 클릭했다면 이벤트를 false시킴
				if(!clickEvent){
					if(event.type==SDL_MOUSEBUTTONUP){
						if(event.button.button==SDL_BUTTON_LEFT){
							//void 위에서 왼쪽 마우스 버튼
							onClickVoid(mouse.x,mouse.y);
						}else if(event.button.button==SDL_BUTTON_RIGHT && tool==CONFIGURE){
							//linking 을 멈춤
							linking=false;
							linkingTrigger=NULL;

							//움직이는 블럭의 경로를 설정
							if(moving){
								std::map<std::string,std::string> editorData;
								char s[64], s0[64];

								sprintf(s,"%d",int(movingBlocks[movingBlock].size()));
								editorData["MovingPosCount"]=s;
								//지정된 좌표를 loop
								for(unsigned int o=0;o<movingBlocks[movingBlock].size();o++){
									sprintf(s0+1,"%d",o);
									sprintf(s,"%d",movingBlocks[movingBlock][o].x);
									s0[0]='x';
									editorData[s0]=s;
									sprintf(s,"%d",movingBlocks[movingBlock][o].y);
									s0[0]='y';
									editorData[s0]=s;
									sprintf(s,"%d",movingBlocks[movingBlock][o].time);
									s0[0]='t';
									editorData[s0]=s;
								}
								movingBlock->setEditorData(editorData);

								//움직임 멈춤
								moving=false;
								movingBlock=NULL;
							}
						}
					}
				}
			}
		}

		//움직이고 있을때 movingposition을 지우기 위해 backspace키가 눌렸는지 체크
		if(moving && event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_BACKSPACE){
			if(movingBlocks[movingBlock].size()>0){
				movingBlocks[movingBlock].pop_back();
			}
		}

		//tap키가 눌렸는지 체크, level settings.
		if(inputMgr.isKeyDownEvent(INPUTMGR_TAB)){
			//levelSettings을 보여준다
			levelSettings();
		}

		//새로운 level을 체크한다. (Ctrl+n)
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_n && (event.key.keysym.mod & KMOD_CTRL)){
			reset();
			//NOTE: We don't have anything to load from so we create an empty TreeStorageNode.
			Game::loadLevelFromNode(new TreeStorageNode,"");

			//선택된것이 popup된것을 숨긴다. (어떠한 것이라도.)
			if(selectionPopup!=NULL){
				delete selectionPopup;
				selectionPopup=NULL;
			}
		}
		//Ctrl+s로 level을 저장하거나 Ctrl+Shift+s 키로 레벡팩을 저장하는지 체크
		if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_s && (event.key.keysym.mod & KMOD_CTRL)){
			saveLevel(levelFile);
			//feedback 을 유저에게 준다.
			if(levelName.empty())
				msgBox(tfm::format(_("Level \"%s\" saved"),fileNameFromPath(levelFile)),MsgBoxOKOnly,_("Saved"));
			else
				msgBox(tfm::format(_("Level \"%s\" saved"),levelName),MsgBoxOKOnly,_("Saved"));
		}
	}
}

void LevelEditor::levelSettings(){
	//이름을 물어보는 popup창을 띄우지 않는다.
	//모든 존재하는 gui를 최초로 전부 지운다.
	if(GUIObjectRoot){
		delete GUIObjectRoot;
		GUIObjectRoot=NULL;
	}

	GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-300)/2,600,300,GUIObjectFrame,_("Level settings"));
	GUIObject* obj;

	//NOTE: We reuse the objectProperty and secondProperty.
	obj=new GUIObject(40,50,240,36,GUIObjectLabel,_("Name:"));
	root->childControls.push_back(obj);
	obj=new GUIObject(140,50,410,36,GUIObjectTextBox,levelName.c_str());
	objectProperty=obj;
	root->childControls.push_back(obj);

	obj=new GUIObject(40,100,240,36,GUIObjectLabel,_("Theme:"));
	root->childControls.push_back(obj);
	obj=new GUIObject(140,100,410,36,GUIObjectTextBox,levelTheme.c_str());
	secondObjectProperty=obj;
	root->childControls.push_back(obj);

	//목적 시간과 녹화
	{
		char c[32];

		if(levelTime>=0){
			sprintf(c,"%-.2f",levelTime/40.0f);
		}else{
			c[0]='\0';
		}
		obj=new GUIObject(40,150,240,36,GUIObjectLabel,_("Target time (s):"));
		root->childControls.push_back(obj);
		obj=new GUIObject(290,150,260,36,GUIObjectTextBox,c);
		levelTimeProperty=obj;
		root->childControls.push_back(obj);

		if(levelRecordings>=0){
			sprintf(c,"%d",levelRecordings);
		}else{
			c[0]='\0';
		}
		obj=new GUIObject(40,200,240,36,GUIObjectLabel,_("Target recordings:"));
		root->childControls.push_back(obj);
		obj=new GUIObject(290,200,260,36,GUIObjectTextBox,c);
		levelRecordingsProperty=obj;
		root->childControls.push_back(obj);
	}


	//확인과 취소 버튼
	obj=new GUIObject(root->width*0.3,300-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
	obj->name="lvlSettingsOK";
	obj->eventCallback=this;
	root->childControls.push_back(obj);
	obj=new GUIObject(root->width*0.7,300-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
	obj->name="lvlSettingsCancel";
	obj->eventCallback=this;
	root->childControls.push_back(obj);

	GUIOverlay* overlay=new GUIOverlay(root);
}

void LevelEditor::postLoad(){
	// triggers 를 찾는다.
	for(unsigned int o=0;o<levelObjects.size();o++){
		//editor data를 얻는다.
		vector<pair<string,string> > objMap;
		levelObjects[o]->getEditorData(objMap);

		//highest id.를 체크한다.
		for(unsigned int i=0;i<objMap.size();i++){
			if(objMap[i].first=="id"){
				unsigned int id=atoi(objMap[i].second.c_str());
				if(id>=currentId){
					currentId=id+1;
				}
			}
		}

		switch(levelObjects[o]->type){
			case TYPE_BUTTON:
			case TYPE_SWITCH:
			{
				//triggers vector 에 object를 추가한다.
				vector<GameObject*> linked;
				triggers[levelObjects[o]]=linked;
				//같은 id의 object를 찾기 위해 levelObjects를 loop돈다.
				for(unsigned int oo=0;oo<levelObjects.size();oo++){
					//같은 object가 아니지만 같은 id를 갖는 것을 체크한다.
					if(o!=oo && (dynamic_cast<Block*>(levelObjects[o]))->id==(dynamic_cast<Block*>(levelObjects[oo]))->id){
						//trigger의 vector 링크에 object를 추가 한다.
						triggers[levelObjects[o]].push_back(levelObjects[oo]);
					}
				}
				break;
			}
			case TYPE_PORTAL:
			{
				//triggers vector에 object를 추가한다.
				vector<GameObject*> linked;
				triggers[levelObjects[o]]=linked;

				//destination이 비었으면 return해준다.
				if((dynamic_cast<Block*>(levelObjects[o]))->destination.empty()){
					break;
				}

				//목적지로서 같은 id의 object를 찾기 위해 levelObjects를 loop돈다.
				for(unsigned int oo=0;oo<levelObjects.size();oo++){
					//같은 object가 아니지만 같은 id를 갖는 것을 체크한다.
					if(o!=oo && (dynamic_cast<Block*>(levelObjects[o]))->destination==(dynamic_cast<Block*>(levelObjects[oo]))->id){
						//trigger의 vector 링크에 object를 추가 한다.
						triggers[levelObjects[o]].push_back(levelObjects[oo]);
					}
				}
				break;
			}
			case TYPE_MOVING_BLOCK:
			case TYPE_MOVING_SHADOW_BLOCK:
			case TYPE_MOVING_SPIKES:
			{
				//movingBlocks vector에 object를 추가한다.
				vector<MovingPosition> positions;
				movingBlocks[levelObjects[o]]=positions;

				//the editor data 모델의 개수를 얻는다.
				int m=objMap.size();

				//editor data가 비어있지 않는지 체크한다.
				if(m>0){
					//Integer 는 위치를 포함하고있다.
					int pos=0;
					int currentPos=0;

					//movingpositions의 개수를 얻는다.
					pos=atoi(objMap[1].second.c_str());

					while(currentPos<pos){
						int x=atoi(objMap[currentPos*3+4].second.c_str());
						int y=atoi(objMap[currentPos*3+5].second.c_str());
						int t=atoi(objMap[currentPos*3+6].second.c_str());

						//새로운 movingPosition을 만든다.
						MovingPosition position(x,y,t);
						movingBlocks[levelObjects[o]].push_back(position);

						//currentPos를 하나씩 증가시킨다.
						currentPos++;
					}
				}

				break;
			}
			default:
			  break;
		}
	}
}

void LevelEditor::snapToGrid(int* x,int* y){
	//x좌표가 음수값인지 체크한다.
	if(*x<0){
		*x=-((abs(*x-50)/50)*50);
	}else{
		*x=(*x/50)*50;
	}

	//y좌표도 체크한다.
	if(*y<0){
		*y=-((abs(*y-50)/50)*50);
	}else{
		*y=(*y/50)*50;
	}
}

void LevelEditor::onClickObject(GameObject* obj,bool selected){
	switch(tool){
	  //NOTE:같은 selection함수를 사용하기 위해서 ADD와 SELECT에 CONFIGURE을 두었다
	  //CONFIGURE의 끝에 break가 없다는 것을 의미한다.
	  case CONFIGURE:
	  {
	    //링크 되었는지 체크한다.
	    if(linking){
			//obj 가 link된 값인지 체크한다.
			switch(obj->type){
				case TYPE_CONVEYOR_BELT:
				case TYPE_SHADOW_CONVEYOR_BELT:
				case TYPE_MOVING_BLOCK:
				case TYPE_MOVING_SHADOW_BLOCK:
				case TYPE_MOVING_SPIKES:
				{
					//portal 에 링크 되어있지 않을때 유효한 값을 가짐.
					if(linkingTrigger->type==TYPE_PORTAL){
						//portal을 움직이는 블럭이나 다른 것들에 링크 할 수 없다.
						//링크를 멈추고 반환.
						linkingTrigger=NULL;
						linking=false;
						return;
					}
					break;
				}
				case TYPE_PORTAL:
				{
					//linkingTrigger 또한 portal 이어야 한다.
					if(linkingTrigger->type!=TYPE_PORTAL){
						//The linkingTrigger가 portal이 아니여서 링크를 멈추고 리턴.
						linkingTrigger=NULL;
						linking=false;
						return;
					}
					break;
				}
				default:
					//값이 유효하지 않아서 링크를 멈추고 리턴
					linkingTrigger=NULL;
					linking=false;
					return;
				break;
			}

			//linkingTrigger가 여러개의 링크를 다루고 있는지 또는 오직 하나만 가지고있는지 체크
			switch(linkingTrigger->type){
				case TYPE_PORTAL:
				{
					//Portals 은 오직 하나의 링크만 가질 수 있어서 모든 존재하는 링크를 지움
					triggers[linkingTrigger].clear();
					triggers[linkingTrigger].push_back(obj);
					break;
				}
				default:
				{
					//대부분은 다중링크를 가질 수 있다.
					triggers[linkingTrigger].push_back(obj);
					break;
				}
			}

			//portal인지 확인.
			if(linkingTrigger->type==TYPE_PORTAL){
				//portals는 자기만의 id를 주는대신 다른것의 아이디를 필요로한다.
				vector<pair<string,string> > objMap;
				obj->getEditorData(objMap);
				int m=objMap.size();
				if(m>0){
					std::map<std::string,std::string> editorData;
					char s[64];
					sprintf(s,"%d",atoi(objMap[0].second.c_str()));
					editorData["destination"]=s;
					linkingTrigger->setEditorData(editorData);
				}
			}else{
				//trigger로서 같은아이디를 object에 준다.
				vector<pair<string,string> > objMap;
				linkingTrigger->getEditorData(objMap);
				int m=objMap.size();
				if(m>0){
					std::map<std::string,std::string> editorData;
					char s[64];
					sprintf(s,"%d",atoi(objMap[0].second.c_str()));
					editorData["id"]=s;
					obj->setEditorData(editorData);
				}
			}


			//We return to prevent configuring stuff like conveyor belts, etc...
			linking=false;
			linkingTrigger=NULL;
			return;
	    }

	    //If we're moving add a movingposition.
	    if(moving){
			//현재 마우스 좌표를 얻는다.
			int x,y;
			SDL_GetMouseState(&x,&y);
			x+=camera.x;
			y+=camera.y;

			//Apply snap to grid.
			if(!pressedShift){
				snapToGrid(&x,&y);
			}else{
				x-=25;
				y-=25;
			}

			x-=movingBlock->getBox().x;
			y-=movingBlock->getBox().y;

			//길이를 계산
			//delta x 와 y 처음으로 얻음.
			int dx,dy;
			if(movingBlocks[movingBlock].empty()){
				dx=x;
				dy=y;
			}else{
				dx=x-movingBlocks[movingBlock].back().x;
				dy=y-movingBlocks[movingBlock].back().y;
			}

			double length=sqrt(double(dx*dx+dy*dy));
			movingBlocks[movingBlock].push_back(MovingPosition(x,y,(int)(length*(10/(double)movingSpeed))));
			return;
	    }

			//그리고 user가 enter키를 누름으로서 다룬다. (블록의 속성을 보여준다.)
	    onEnterObject(obj);
	  }
	  case SELECT:
	  case ADD:
	  {
		//object가 이미 선택되었는지 체크
		if(!selected){
			//shift키가 눌렸는지 아닌지 처음으로 체크
			if(!pressedShift){
				//selection을 clear
				selection.clear();
			}

			//object를 selection에 추가
			selection.push_back(obj);
		}
	    break;
	  }
	  case REMOVE:
	  {
	    //Remove the object.
	    removeObject(obj);
	    break;
	  }
	  default:
	    break;
	}
}

void LevelEditor::onRightClickObject(GameObject* obj,bool selected){
	switch(tool){
	  case CONFIGURE:
	  {
		//Make sure we aren't doing anything special.
		if(moving || linking)
			break;

		//그것이 trigger 인지 확인
		if(obj->type==TYPE_PORTAL || obj->type==TYPE_BUTTON || obj->type==TYPE_SWITCH){
			//링크가 true인지 확인
			linking=true;
			linkingTrigger=obj;
		}

		//움직이는 블럭인지 확인
		if(obj->type==TYPE_MOVING_BLOCK || obj->type==TYPE_MOVING_SHADOW_BLOCK || obj->type==TYPE_MOVING_SPIKES){
			//움직임을 true로 설정
			moving=true;
			movingBlock=obj;
		}
		break;
	  }
	  case SELECT:
	  case ADD:
	  {
		//We deselect the object if it's selected.
		if(selected){
			std::vector<GameObject*>::iterator it;
			it=find(selection.begin(),selection.end(),obj);

			//selection으로부터 object를 지움
			if(it!=selection.end()){
				selection.erase(it);
			}
		}else{
			//그것이 선택된 object가 아니여서 configure 모드로 전환
			//object가 맞는 타입인지 아닌지 체크
			if(obj->type==TYPE_MOVING_BLOCK || obj->type==TYPE_MOVING_SHADOW_BLOCK || obj->type==TYPE_MOVING_SPIKES ||
				obj->type==TYPE_PORTAL || obj->type==TYPE_BUTTON || obj->type==TYPE_SWITCH){
				tool=CONFIGURE;
				onRightClickObject(obj,selected);
			}

		}
		break;
	  }
	  default:
	    break;
	}
}

void LevelEditor::onClickVoid(int x,int y){
	switch(tool){
	  case SELECT:
	  {
	    //selection 을 clear
	    selection.clear();
	    break;
	  }
	  case ADD:
	  {
				//selection 을 clear
	      selection.clear();

	      //object를 둔다.
	      //Apply snap to grid.
	      if(!pressedShift){
			snapToGrid(&x,&y);
	      }else{
			x-=25;
			y-=25;
	      }
	      addObject(new Block(x,y,editorTileOrder[currentType],this));
	      break;
	  }
	  case CONFIGURE:
	  {
				//selection 을 clear
	      selection.clear();

	      //If we're linking we should stop, user abort.
	      if(linking){
			linking=false;
			linkingTrigger=NULL;
			//And return.
			return;
	      }

	      //움직이고 있다면 point를 추가한다.
	      if(moving){
			//Apply snap to grid.
			if(!pressedShift){
				snapToGrid(&x,&y);
			}else{
				x-=25;
				y-=25;
			}

			x-=movingBlock->getBox().x;
			y-=movingBlock->getBox().y;

			//길이를 계산한다.
			//delta x 와 y 처음으로 얻음.
			int dx,dy;
			if(movingBlocks[movingBlock].empty()){
				dx=x;
				dy=y;
			}else{
				dx=x-movingBlocks[movingBlock].back().x;
				dy=y-movingBlocks[movingBlock].back().y;
			}

			double length=sqrt(double(dx*dx+dy*dy));
			movingBlocks[movingBlock].push_back(MovingPosition(x,y,(int)(length*(10/(double)movingSpeed))));

			//And return.
			return;
	      }
	      break;
	  }
	  default:
	    break;
	}
}

void LevelEditor::onDragStart(int x,int y){
	switch(tool){
	  case SELECT:
	  case ADD:
	  case CONFIGURE:
	  {
			//selection을 drag하고 그 selection이 비어있지 않은지 체크한다.
	    if(!selection.empty()){
		//selection이 비어있지 않아서 dragCenter를 검색한다.
		//mouse rectangle을 만든다.
		SDL_Rect mouse={x,y,0,0};

		//충돌(collision)을 체크하기 위해 object를 loop돈다.
		for(unsigned int o=0; o<selection.size(); o++){
			if(checkCollision(selection[o]->getBox(),mouse)==true){
				//충돌이 나서 dragCenter을 설정
				dragCenter=selection[o];
				selectionDrag=true;
			}
		}
	    }
	    break;
	  }
	  default:
	    break;
	}
}

void LevelEditor::onDrag(int dx,int dy){
	switch(tool){
	  case REMOVE:
	  {
		//item이 삭제 될지라도 마우스는 그 위에 있다.
		//현재 마우스 좌표를 얻는다.
		int x,y;
		SDL_GetMouseState(&x,&y);
		//사각형을 그린다.
		SDL_Rect mouse={x+camera.x,y+camera.y,0,0};

		//충돌(collision)을 체크하기 위해 object를 loop돈다.
		for(unsigned int o=0; o<levelObjects.size(); o++){
			if(checkCollision(levelObjects[o]->getBox(),mouse)==true){
				//object를 삭제한다.
				removeObject(levelObjects[o]);
			}
		}
	    break;
	  }
	  default:
	    break;
	}
}

void LevelEditor::onDrop(int x,int y){
	switch(tool){
	  case SELECT:
	  case ADD:
	  case CONFIGURE:
	  {
	      //drag center가 null이 아닌지 체크
	      if(dragCenter==NULL) return;
	      //dragCenter의 좌표
	      SDL_Rect r=dragCenter->getBox();
	      //Apply snap to grid.
	      if(!pressedShift){
			snapToGrid(&x,&y);
	      }else{
			x-=25;
			y-=25;
	      }

	      //selection을 loop돈다.
	      for(unsigned int o=0; o<selection.size(); o++){
			SDL_Rect r1=selection[o]->getBox();
			//object 를  drop 장소에 둔다.
			moveObject(selection[o],(r1.x-r.x)+x,(r1.y-r.y)+y);
	      }

	      //dragCenter 가 null이고 selectionDrag가 false로 설정을 한다.
	      dragCenter=NULL;
	      selectionDrag=false;
	      break;
	  }
	  default:
	    break;
	}
}

void LevelEditor::onCameraMove(int dx,int dy){
	switch(tool){
	  case REMOVE:
	  {
		//오직 왼쪽 마우스 버튼이 눌렸을 때만 삭제를 한다.
		if(pressedLeftMouse){
			//현재 마우스 좌표를 얻는다.
			int x,y;
			SDL_GetMouseState(&x,&y);
			//사각형을 그린다.
			SDL_Rect mouse={x+camera.x,y+camera.y,0,0};

			//충돌(collision)을 체크하기 위해 object를 loop돈다.
			for(unsigned int o=0; o<levelObjects.size(); o++){
				if(checkCollision(levelObjects[o]->getBox(),mouse)==true){
					//object를 삭제한다.
					removeObject(levelObjects[o]);
				}
			}
		}
	    break;
	  }
	  default:
	    break;
	}
}

void LevelEditor::onEnterObject(GameObject* obj){
	switch(tool){
	  case CONFIGURE:
	  {
	    //thpe이 움직이는 블럭인지 체크한다.
	    if(obj->type==TYPE_MOVING_BLOCK || obj->type==TYPE_MOVING_SHADOW_BLOCK || obj->type==TYPE_MOVING_SPIKES){
			//메시지를 popup한다.
			//존재하는 모든 gui를 삭제
			if(GUIObjectRoot){
				delete GUIObjectRoot;
				GUIObjectRoot=NULL;
			}

			//속성들을 얻는다.
			vector<pair<string,string> > objMap;
			obj->getEditorData(objMap);
			int m=objMap.size();
			if(m>0){
				//Set the object we configure.
				configuredObject=obj;

				//moving block이 경로를 가지고있는지 체크한다.
				string s1;
				bool path=false;
				if(!movingBlocks[obj].empty()){
					s1=_("Defined");
					path=true;
				}else{
					s1=_("None");
				}

				//gui를 생성한다.
				string s;
				switch(obj->type){
				  case TYPE_MOVING_BLOCK:
					s=_("Moving block");
				    break;
				  case TYPE_MOVING_SHADOW_BLOCK:
					s=_("Moving shadow block");
				    break;
				  case TYPE_MOVING_SPIKES:
					s=_("Moving spikes");
				    break;

				}
				GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-200)/2,600,200,GUIObjectFrame,s.c_str());
				GUIObject* obj;

				obj=new GUIObject(70,50,280,36,GUIObjectCheckBox,_("Enabled"),(objMap[2].second!="1"));
				obj->name="cfgMovingBlockEnabled";
				obj->eventCallback=this;
				objectProperty=obj;
				root->childControls.push_back(obj);

				obj=new GUIObject(70,80,280,36,GUIObjectCheckBox,_("Loop"),(objMap[3].second!="0"));
				obj->name="cfgMovingBlockLoop";
				obj->eventCallback=this;
				secondObjectProperty=obj;
				root->childControls.push_back(obj);

				obj=new GUIObject(70,110,280,36,GUIObjectLabel,_("Path"));
				root->childControls.push_back(obj);
				GUIObject* label=new GUIObject(330,110,-1,36,GUIObjectLabel,s1.c_str());
				root->childControls.push_back(label);
				label->render(0,0,false);

				if(path){
					obj=new GUIObject(label->left+label->width,110,36,36,GUIObjectButton,"x");
					obj->name="cfgMovingBlockClrPath";
					obj->eventCallback=this;
					root->childControls.push_back(obj);
				}else{
					//NOTE: The '+' is translated 5 pixels down to align with the 'x'.
					obj=new GUIObject(label->left+label->width,115,36,36,GUIObjectButton,"+");
					obj->name="cfgMovingBlockMakePath";
					obj->eventCallback=this;
					root->childControls.push_back(obj);
				}

				obj=new GUIObject(root->width*0.3,200-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
				obj->name="cfgMovingBlockOK";
				obj->eventCallback=this;
				root->childControls.push_back(obj);
				obj=new GUIObject(root->width*0.7,200-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
				obj->name="cfgCancel";
				obj->eventCallback=this;
				root->childControls.push_back(obj);

				//Create the GUI overlay.
				GUIOverlay* overlay=new GUIOverlay(root);
			}
	    }

	    //object가 무슨 type인지 체크한다.
	    if(obj->type==TYPE_NOTIFICATION_BLOCK){
			//Open a message popup.
			//존재하는 모든 gui를 삭제
			if(GUIObjectRoot){
				delete GUIObjectRoot;
				GUIObjectRoot=NULL;
			}

			//속성을 얻음
			vector<pair<string,string> > objMap;
			obj->getEditorData(objMap);
			int m=objMap.size();
			if(m>0){
				//Set the object we configure.
				configuredObject=obj;

				//GUI를 생성
				GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-250)/2,600,250,GUIObjectFrame,_("Notification block"));
				GUIObject* obj;

				obj=new GUIObject(40,50,240,36,GUIObjectLabel,_("Enter message here:"));
				root->childControls.push_back(obj);
				obj=new GUITextArea(50,90,500,100);
				string tmp=objMap[1].second.c_str();
				//Change \n with the characters '\n'.
				while(tmp.find("\\n")!=string::npos){
					tmp=tmp.replace(tmp.find("\\n"),2,"\n");
				}
				obj->caption=tmp.c_str();
				//textField를 설정
				objectProperty=obj;
				root->childControls.push_back(obj);

				obj=new GUIObject(root->width*0.3,250-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
				obj->name="cfgNotificationBlockOK";
				obj->eventCallback=this;
				root->childControls.push_back(obj);
				obj=new GUIObject(root->width*0.7,250-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
				obj->name="cfgCancel";
				obj->eventCallback=this;
				root->childControls.push_back(obj);

				//Create the GUI overlay.
				GUIOverlay* overlay=new GUIOverlay(root);
			}
	    }
	    if(obj->type==TYPE_CONVEYOR_BELT || obj->type==TYPE_SHADOW_CONVEYOR_BELT){
			//Open a message popup.
			//존재하는 모든 gui를 삭제
			if(GUIObjectRoot){
				delete GUIObjectRoot;
				GUIObjectRoot=NULL;
			}

			//속성들을 얻고 체크한다.
			vector<pair<string,string> > objMap;
			obj->getEditorData(objMap);
			int m=objMap.size();
			if(m>0){
				//Set the object we configure.
				configuredObject=obj;

				//Now create the GUI.
				string s;
				if(obj->type==TYPE_CONVEYOR_BELT){
					s=_("Shadow Conveyor belt");
				}else{
				  	s=_("Conveyor belt");
				}

				GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-200)/2,600,200,GUIObjectFrame,s.c_str());
				GUIObject* obj;

				obj=new GUIObject(40,60,220,36,GUIObjectCheckBox,_("Enabled"),(objMap[1].second!="1"));
				obj->name="cfgConveyorBlockEnabled";
				obj->eventCallback=this;
				objectProperty=obj;
				root->childControls.push_back(obj);

				obj=new GUIObject(40,100,240,36,GUIObjectLabel,_("Enter speed here:"));
				root->childControls.push_back(obj);
				obj=new GUIObject(240,100,320,36,GUIObjectTextBox,objMap[2].second.c_str());
				//Set the textField.
				secondObjectProperty=obj;
				root->childControls.push_back(obj);


				obj=new GUIObject(root->width*0.3,200-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
				obj->name="cfgConveyorBlockOK";
				obj->eventCallback=this;
				root->childControls.push_back(obj);
				obj=new GUIObject(root->width*0.7,200-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
				obj->name="cfgCancel";
				obj->eventCallback=this;
				root->childControls.push_back(obj);

				//Create the GUI overlay.
				GUIOverlay* overlay=new GUIOverlay(root);
			}
	    }

	    if(obj->type==TYPE_PORTAL){
			//Open a message popup.
			//존재하는 모든 gui 삭제
			if(GUIObjectRoot){
				delete GUIObjectRoot;
				GUIObjectRoot=NULL;
			}

			//Get the properties and check if
			vector<pair<string,string> > objMap;
			obj->getEditorData(objMap);
			int m=objMap.size();
			if(m>0){
				//Set the object we configure.
				configuredObject=obj;

				//Check how many targets there are for this object.
				string s1;
				bool target=false;
				if(!triggers[obj].empty()){
					s1=_("Defined");
					target=true;
				}else{
					s1=_("None");
				}

				//Now create the GUI.
				GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-200)/2,600,200,GUIObjectFrame,_("Portal"));
				GUIObject* obj;

				obj=new GUIObject(70,60,310,36,GUIObjectCheckBox,_("Activate on touch"),(objMap[1].second=="1"));
				obj->name="cfgPortalAutomatic";
				obj->eventCallback=this;
				objectProperty=obj;
				root->childControls.push_back(obj);

				obj=new GUIObject(70,100,240,36,GUIObjectLabel,_("Targets:"));
				root->childControls.push_back(obj);

				GUIObject* label=new GUIObject(360,100,-1,36,GUIObjectLabel,s1.c_str());
				root->childControls.push_back(label);
				label->render(0,0,false);

				//targets이 정의되어있는지 체크
				if(target){
					obj=new GUIObject(label->left+label->width,100,36,36,GUIObjectButton,"x");
					obj->name="cfgPortalUnlink";
					obj->eventCallback=this;
					root->childControls.push_back(obj);
				}else{
					//NOTE: The '+' is translated 5 pixels down to align with the 'x'.
					obj=new GUIObject(label->left+label->width,105,36,36,GUIObjectButton,"+");
					obj->name="cfgPortalLink";
					obj->eventCallback=this;
					root->childControls.push_back(obj);
				}

				obj=new GUIObject(root->width*0.3,200-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
				obj->name="cfgPortalOK";
				obj->eventCallback=this;
				root->childControls.push_back(obj);
				obj=new GUIObject(root->width*0.7,200-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
				obj->name="cfgCancel";
				obj->eventCallback=this;
				root->childControls.push_back(obj);

				//Create the GUI overlay.
				GUIOverlay* overlay=new GUIOverlay(root);
			}
	    }

	    if(obj->type==TYPE_BUTTON || obj->type==TYPE_SWITCH){
			//Open a message popup.
			//존재하는 모든 gui 삭제
			if(GUIObjectRoot){
				delete GUIObjectRoot;
				GUIObjectRoot=NULL;
			}

			//속성들을 얻어오고 체크함.
			vector<pair<string,string> > objMap;
			obj->getEditorData(objMap);
			int m=objMap.size();
			if(m>0){
				//Set the object we configure.
				configuredObject=obj;

				//Check how many targets there are for this object.
				string s1;
				bool targets=false;
				if(!triggers[obj].empty()){
					s1=tfm::format(_("%d Defined"),(int)triggers[obj].size());
					targets=true;
				}else{
					s1=_("None");
				}

				//Now create the GUI.
				string s;
				if(obj->type==TYPE_BUTTON){
					s=_("Button");
				}else{
					s=_("Switch");
				}
				GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-200)/2,600,200,GUIObjectFrame,s.c_str());
				GUIObject* obj;

				obj=new GUIObject(70,60,240,36,GUIObjectLabel,_("Behaviour:"));
				root->childControls.push_back(obj);

				obj=new GUISingleLineListBox(250,60,300,36);
				obj->name="lstBehaviour";
				vector<string> v;
				v.push_back(_("On"));
				v.push_back(_("Off"));
				v.push_back(_("Toggle"));
				(dynamic_cast<GUISingleLineListBox*>(obj))->item=v;

				//현재 행동을 얻음
				if(objMap[1].second=="on"){
					obj->value=0;
				}else if(objMap[1].second=="off"){
					obj->value=1;
				}else{
					//There's no need to check for the last one, since it's also the default.
					obj->value=2;
				}
				objectProperty=obj;
				root->childControls.push_back(obj);

				obj=new GUIObject(70,100,240,36,GUIObjectLabel,_("Targets:"));
				root->childControls.push_back(obj);

				GUIObject* label=new GUIObject(250,100,-1,36,GUIObjectLabel,s1.c_str());
				root->childControls.push_back(label);
				label->render(0,0,false);

				//NOTE: The '+' is translated 5 pixels down to align with the 'x'.
				obj=new GUIObject(label->left+label->width,105,36,36,GUIObjectButton,"+");
				obj->name="cfgTriggerLink";
				obj->eventCallback=this;
				root->childControls.push_back(obj);

				//targets이 정의되어있는지 체크
				if(targets){
					obj=new GUIObject(label->left+label->width+40,100,36,36,GUIObjectButton,"x");
					obj->name="cfgTriggerUnlink";
					obj->eventCallback=this;
					root->childControls.push_back(obj);
				}


				obj=new GUIObject(root->width*0.3,200-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
				obj->name="cfgTriggerOK";
				obj->eventCallback=this;
				root->childControls.push_back(obj);
				obj=new GUIObject(root->width*0.7,200-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
				obj->name="cfgCancel";
				obj->eventCallback=this;
				root->childControls.push_back(obj);

				//Create the GUI overlay.
				GUIOverlay* overlay=new GUIOverlay(root);
			}
	    }
	    if(obj->type==TYPE_FRAGILE){
			//존재하는 모든 gui 삭제
			if(GUIObjectRoot){
				delete GUIObjectRoot;
				GUIObjectRoot=NULL;
			}

			//속성들을 얻고 state data를 포함하고 있는지 체크
			vector<pair<string,string> > objMap;
			obj->getEditorData(objMap);
			int m=objMap.size();
			if(m>0){
				//Set the object we configure.
				configuredObject=obj;

				//Create the GUI.
				GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-200)/2,600,200,GUIObjectFrame,_("Fragile"));
				GUIObject* obj;

				obj=new GUIObject(70,60,240,36,GUIObjectLabel,_("State:"));
				root->childControls.push_back(obj);

				obj=new GUISingleLineListBox(250,60,300,36);
				obj->name="lstBehaviour";
				vector<string> v;
				v.push_back(_("Complete"));
				v.push_back(_("One step"));
				v.push_back(_("Two steps"));
				v.push_back(_("Gone"));
				(dynamic_cast<GUISingleLineListBox*>(obj))->item=v;

				//현재 state를 얻음
				obj->value=atoi(objMap[1].second.c_str());
				objectProperty=obj;
				root->childControls.push_back(obj);

				obj=new GUIObject(root->width*0.3,200-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
				obj->name="cfgFragileOK";
				obj->eventCallback=this;
				root->childControls.push_back(obj);
				obj=new GUIObject(root->width*0.7,200-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
				obj->name="cfgCancel";
				obj->eventCallback=this;
				root->childControls.push_back(obj);

				//Create the GUI overlay.
				GUIOverlay* overlay=new GUIOverlay(root);
			}
	    }

	    break;
	  }
	  default:
	    break;
	}
}

void LevelEditor::addObject(GameObject* obj){
    //새로운 collectable type을 추가할 때마다 totalCollectables 을 1 증가시킴
	if (obj->type==TYPE_COLLECTABLE) {
		totalCollectables++;
	}

	//player or shadow 가 시작했다면 이전 것을 삭제한다.
	if(obj->type==TYPE_START_PLAYER || obj->type==TYPE_START_SHADOW){
		//levelObjects를 Loop
		for(unsigned int o=0; o<levelObjects.size(); o++){
			//type이 같은지 체크
			if(levelObjects[o]->type==obj->type){
				removeObject(levelObjects[o]);
			}
		}
	}

	//levelObjects에 추가
	levelObjects.push_back(obj);

	//level dimensions나 다른것들에 object가 들어있는지 체크
	//이것을 수행하기 위해 moveObject()함수를 부름
	moveObject(obj,obj->getBox().x,obj->getBox().y);

	//GameObject type specific stuff.
	switch(obj->type){
		case TYPE_BUTTON:
		case TYPE_SWITCH:
		case TYPE_PORTAL:
		{
			//triggers에 object를 추가
			vector<GameObject*> linked;
			triggers[obj]=linked;

			//자신만의 id를 줌
			std::map<std::string,std::string> editorData;
			char s[64];
			sprintf(s,"%d",currentId);
			currentId++;
			editorData["id"]=s;
			obj->setEditorData(editorData);
			break;
		}
		case TYPE_MOVING_BLOCK:
		case TYPE_MOVING_SHADOW_BLOCK:
		case TYPE_MOVING_SPIKES:
		{
			//moving blocks에 object 추가.
			vector<MovingPosition> positions;
			movingBlocks[obj]=positions;

			//editor data를 얻어옴
			vector<pair<string,string> > objMap;
			obj->getEditorData(objMap);

			//editor data의 모델 개수를 얻음
			int m=objMap.size();

			//editor data가 비어있지 않은지 체크
			if(m>0){
				//Integer는 좌표를 가지고 있다.
				int pos=0;
				int currentPos=0;

				//movingpositions의 개수를 얻음
				pos=atoi(objMap[1].second.c_str());

				while(currentPos<pos){
					int x=atoi(objMap[currentPos*3+4].second.c_str());
					int y=atoi(objMap[currentPos*3+5].second.c_str());
					int t=atoi(objMap[currentPos*3+6].second.c_str());

					//새로움 movingPosition을 만듦
					MovingPosition position(x,y,t);
					movingBlocks[obj].push_back(position);

					//currentPos을 하나씩 증가시킴
					currentPos++;
				}
			}

			//자신만의 id를 줌
			std::map<std::string,std::string> editorData;
			char s[64];
			sprintf(s,"%d",currentId);
			currentId++;
			editorData["id"]=s;
			obj->setEditorData(editorData);
			break;
		}
		default:
		  break;
	}
}

void LevelEditor::moveObject(GameObject* obj,int x,int y){
	//새로운 좌표에 object 설정
	obj->setPosition(x,y);

	//level 속에 object가 있는지 체크
	//만약 그렇지 않다면 레벨을 올림
	if(obj->getBox().x+50>LEVEL_WIDTH){
		LEVEL_WIDTH=obj->getBox().x+50;
	}
	if(obj->getBox().y+50>LEVEL_HEIGHT){
		LEVEL_HEIGHT=obj->getBox().y+50;
	}
	if(obj->getBox().x<0 || obj->getBox().y<0){
		//A block on the left (or top) side of the level, meaning we need to shift everything.
		//=> 레벨의 왼쪽 또는 위에 있는 블럭은 모든것을 움직여야할 필요가 있음을 의미한다.
		//차이를 계산
		int diffx=(0-(obj->getBox().x));
		int diffy=(0-(obj->getBox().y));

		if(diffx<0) diffx=0;
		if(diffy<0) diffy=0;

		//level size 를 변환시킴
		//차이점 만큼 레벨의 크기을 올림, 0-(x+50).
		LEVEL_WIDTH+=diffx;
		LEVEL_HEIGHT+=diffy;
		//cout<<"x:"<<diffx<<",y:"<<diffy<<endl; //debug
		camera.x+=diffx;
		camera.y+=diffy;

		// player and shadow 좌표 설정
		//(although it's unnecessary if there is player and shadow start)
		player.setPosition(player.getBox().x+diffx,player.getBox().y+diffy);
		shadow.setPosition(shadow.getBox().x+diffx,shadow.getBox().y+diffy);

		for(unsigned int o=0; o<levelObjects.size(); o++){
			//FIXME: shouldn't recuesive call me (to prevent stack overflow bugs)
			moveObject(levelObjects[o],levelObjects[o]->getBox().x+diffx,levelObjects[o]->getBox().y+diffy);
		}
	}

	//object가 player또는 shadow 라면 시작한후 player와 shadow의 시작점을 바꾼다.
	if(obj->type==TYPE_START_PLAYER){
		//Center the player horizontally.
  		player.fx=obj->getBox().x+(50-23)/2;
		player.fy=obj->getBox().y;
		//새로운 시작점에 player을 두기 위해 reset
		player.reset(true);
	}
	if(obj->type==TYPE_START_SHADOW){
		//Center the shadow horizontally.
  		shadow.fx=obj->getBox().x+(50-23)/2;
		shadow.fy=obj->getBox().y;
		//새로운 시작점에 shadow를 두기 위해 reset
		shadow.reset(true);
	}
}

void LevelEditor::removeObject(GameObject* obj){
	std::vector<GameObject*>::iterator it;
	std::map<GameObject*,vector<GameObject*> >::iterator mapIt;

	//새로운 collectable을 추가할 때 마다 totalCollectables을 증가시킴
	if (obj->type==TYPE_COLLECTABLE) {
		totalCollectables--;
	}

	//selection안에 object가 있는지 체크
	it=find(selection.begin(),selection.end(),obj);
	if(it!=selection.end()){
		//만약 그렇다면 삭제시킴
		selection.erase(it);
	}

	//triggers안에 object가 있는지 체크
	mapIt=triggers.find(obj);
	if(mapIt!=triggers.end()){
		//만약 그렇다면 삭제시킴
		triggers.erase(mapIt);
	}

	//Boolean if it could be a target.
	if(obj->type==TYPE_MOVING_BLOCK || obj->type==TYPE_MOVING_SHADOW_BLOCK || obj->type==TYPE_MOVING_SPIKES
		|| obj->type==TYPE_CONVEYOR_BELT || obj->type==TYPE_SHADOW_CONVEYOR_BELT || obj->type==TYPE_PORTAL){
		for(mapIt=triggers.begin();mapIt!=triggers.end();++mapIt){
			//target vector를 loop시킴
			for(unsigned int o=0;o<(*mapIt).second.size();o++){
				//target vector안에 object가 있는지 체크
				if((*mapIt).second[o]==obj){
					(*mapIt).second.erase(find((*mapIt).second.begin(),(*mapIt).second.end(),obj));
					o--;
				}
			}
		}
	}

	//movingObjects안에 object가 있는지 체크
	std::map<GameObject*,vector<MovingPosition> >::iterator movIt;
	movIt=movingBlocks.find(obj);
	if(movIt!=movingBlocks.end()){
		//만약 그렇다면 삭제시킴
		movingBlocks.erase(movIt);
	}

	//levelObjects에 있는 object를 삭제시킴
	it=find(levelObjects.begin(),levelObjects.end(),obj);
	if(it!=levelObjects.end()){
		levelObjects.erase(it);
	}
	delete obj;
	obj=NULL;

	//Set dirty of selection popup
	if(selectionPopup!=NULL) selectionPopup->dirty=true;
}

void LevelEditor::GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType){
	//GUI이벤트를 체크함.
	//Notificationblock 이벤트 설정
	if(name=="cfgNotificationBlockOK"){
		if(GUIObjectRoot){
			//notification block의 메시지 설정
			std::map<std::string,std::string> editorData;
			editorData["message"]=objectProperty->caption;
			configuredObject->setEditorData(editorData);

			//GUI를 삭제
			objectProperty=NULL;
			secondObjectProperty=NULL;
			configuredObject=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
	//Conveyor belt block 설정 이벤트
	if(name=="cfgConveyorBlockOK"){
		if(GUIObjectRoot){
			//notification block의 메시지 설정
			std::map<std::string,std::string> editorData;
			editorData["speed"]=secondObjectProperty->caption;
			editorData["disabled"]=(objectProperty->value==0)?"1":"0";
			configuredObject->setEditorData(editorData);

			//GUI를 삭제
			objectProperty=NULL;
			secondObjectProperty=NULL;
			configuredObject=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
	//Moving block 설정 이벤트
	if(name=="cfgMovingBlockOK"){
		if(GUIObjectRoot){
			//moving block 활성화 또는 비활성화 되었는지 설정
			std::map<std::string,std::string> editorData;
			editorData["disabled"]=(objectProperty->value==0)?"1":"0";
			editorData["loop"]=(secondObjectProperty->value==1)?"1":"0";
			configuredObject->setEditorData(editorData);

			//GUI를 삭제
			objectProperty=NULL;
			secondObjectProperty=NULL;
			configuredObject=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
	if(name=="cfgMovingBlockClrPath"){
		if(GUIObjectRoot){
			//notification block의 메시지 설정
			std::map<std::string,std::string> editorData;
			editorData["MovingPosCount"]="0";
			configuredObject->setEditorData(editorData);

			std::map<GameObject*,vector<MovingPosition> >::iterator it;
			it=movingBlocks.find(configuredObject);
			if(it!=movingBlocks.end()){
				(*it).second.clear();
			}

			//GUI를 삭제
			objectProperty=NULL;
			secondObjectProperty=NULL;
			configuredObject=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
	if(name=="cfgMovingBlockMakePath"){
		if(GUIObjectRoot){
			//움직임 설정
			moving=true;
			movingBlock=configuredObject;

			//GUI를 삭제
			objectProperty=NULL;
			secondObjectProperty=NULL;
			configuredObject=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
	//Portal block 설정 이벤트
	if(name=="cfgPortalOK"){
		if(GUIObjectRoot){
			//notification block의 메시지 설정
			std::map<std::string,std::string> editorData;
			editorData["automatic"]=(objectProperty->value==1)?"1":"0";
			configuredObject->setEditorData(editorData);

			//GUI를 삭제
			objectProperty=NULL;
			secondObjectProperty=NULL;
			configuredObject=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
	if(name=="cfgPortalLink"){
		//linking 을 true로 설정
		linking=true;
		linkingTrigger=configuredObject;

		//GUI를 삭제
		objectProperty=NULL;
		secondObjectProperty=NULL;
		configuredObject=NULL;
		if(GUIObjectRoot){
			delete GUIObjectRoot;
		}
		GUIObjectRoot=NULL;
	}
	if(name=="cfgPortalUnlink"){
		std::map<GameObject*,vector<GameObject*> >::iterator it;
		it=triggers.find(configuredObject);
		if(it!=triggers.end()){
			//targets을 지움
			(*it).second.clear();
		}

		//destination을 reset시킴
		std::map<std::string,std::string> editorData;
		editorData["destination"]="";
		configuredObject->setEditorData(editorData);

		//GUI를 삭제
		objectProperty=NULL;
		secondObjectProperty=NULL;
		configuredObject=NULL;
		if(GUIObjectRoot){
			delete GUIObjectRoot;
		}
		GUIObjectRoot=NULL;
	}
	//Trigger block 설정 이벤트
	if(name=="cfgTriggerOK"){
		if(GUIObjectRoot){
			//notification block의 메시지 설정
			std::map<std::string,std::string> editorData;
			editorData["behaviour"]=(dynamic_cast<GUISingleLineListBox*>(objectProperty))->item[objectProperty->value];
			configuredObject->setEditorData(editorData);

			//GUI를 삭제
			objectProperty=NULL;
			secondObjectProperty=NULL;
			configuredObject=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
	if(name=="cfgTriggerLink"){
		//linking를 true로 설정
		linking=true;
		linkingTrigger=configuredObject;

		//GUI를 삭제
		objectProperty=NULL;
		secondObjectProperty=NULL;
		configuredObject=NULL;
		if(GUIObjectRoot){
			delete GUIObjectRoot;
		}
		GUIObjectRoot=NULL;
	}
	if(name=="cfgTriggerUnlink"){
		std::map<GameObject*,vector<GameObject*> >::iterator it;
		it=triggers.find(configuredObject);
		if(it!=triggers.end()){
			//targets을 지움
			(*it).second.clear();
		}

		//연결 되어있지 않은 targets의 활성화를 막기 위해 새로운 아이디를 trigger에 줌
		std::map<std::string,std::string> editorData;
		char s[64];
		sprintf(s,"%d",currentId);
		currentId++;
		editorData["id"]=s;
		configuredObject->setEditorData(editorData);

		//GUI를 삭제
		objectProperty=NULL;
		secondObjectProperty=NULL;
		configuredObject=NULL;
		if(GUIObjectRoot){
			delete GUIObjectRoot;
		}
		GUIObjectRoot=NULL;
	}

	//Fragile 설정
	if(name=="cfgFragileOK"){
		std::map<std::string,std::string> editorData;
		char s[64];
		sprintf(s,"%d",objectProperty->value);
		editorData["state"]=s;
		configuredObject->setEditorData(editorData);

		//GUI를 삭제
		objectProperty=NULL;
		secondObjectProperty=NULL;
		configuredObject=NULL;
		if(GUIObjectRoot){
			delete GUIObjectRoot;
		}
		GUIObjectRoot=NULL;
	}

	//취소
	if(name=="cfgCancel"){
		if(GUIObjectRoot){
			//GUI를 삭제
			objectProperty=NULL;
			secondObjectProperty=NULL;
			configuredObject=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}

	//LevelSetting 이벤트
	if(name=="lvlSettingsOK"){
		levelName=objectProperty->caption;
		levelTheme=secondObjectProperty->caption;

		//target 시간과 녹화
		string s=levelTimeProperty->caption;
		if(s.empty() || !(s[0]>='0' && s[0]<='9')){
			levelTime=-1;
		}else{
			levelTime=int(atof(s.c_str())*40.0+0.5);
		}

		s=levelRecordingsProperty->caption;
		if(s.empty() || !(s[0]>='0' && s[0]<='9')){
			levelRecordings=-1;
		}else{
			levelRecordings=atoi(s.c_str());
		}

		//GUI를 삭제
		if(GUIObjectRoot){
			objectProperty=NULL;
			secondObjectProperty=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
	if(name=="lvlSettingsCancel"){
		if(GUIObjectRoot){
			//Delete the GUI.
			objectProperty=NULL;
			secondObjectProperty=NULL;
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
}

////////////////LOGIC////////////////////
void LevelEditor::logic(){
	if(playMode){
		//PlayMode 는 게임을 logic대로 하도록 만든다.
		Game::logic();
	}else{
		//카메라 움직임
		if(cameraXvel!=0 || cameraYvel!=0){
			camera.x+=cameraXvel;
			camera.y+=cameraYvel;
			//onCameraMove 이벤트를 부름
			onCameraMove(cameraXvel,cameraYvel);
		}
		//마우스와 함께 카메라가 움직임
		{
			SDL_Rect r[3]={toolbarRect};
			int m=1;
			if(toolbox!=NULL && tool==ADD && toolbox->visible)
				r[m++]=toolbox->getRect();
			if(selectionPopup!=NULL)
				r[m++]=selectionPopup->getRect();
			setCamera(r,m);
		}

		//PlayMode가 아니여서 마우스가 체크됨
		tooltip=-1;
		//마우스 좌표를 얻음
		int x,y;
		SDL_GetMouseState(&x,&y);
		SDL_Rect mouse={x,y,0,0};

		//buttons의 개수 + tool의 개수 만큼 Loop를 돎
		for(int t=0; t<NUMBER_TOOLS+6; t++){
			SDL_Rect toolRect={(SCREEN_WIDTH-460)/2+(t*40)+((t+1)*10),SCREEN_HEIGHT-45,40,40};

			//충돌을 체크함
			if(checkCollision(mouse,toolRect)==true){
				//tooltip tool을 설정
				tooltip=t;
			}
		}
	}
}

/////////////////RENDER//////////////////////
void LevelEditor::render(){
	//Always let the game render the game.
	Game::render();

	//오직 playMode가 아닐때 toolbar, selection, 다른 등등같은 물건들을 render 한다.
	if(!playMode){
		//selectionmarks를 render
		//TODO: block이 시야 안에 있는지 체크
		for(unsigned int o=0; o<selection.size(); o++){
			//그리기 위해 좌표를 얻음
			SDL_Rect r=selection[o]->getBox();
			r.x-=camera.x;
			r.y-=camera.y;

			//selectionMarks를 그림
			applySurface(r.x,r.y,selectionMark,screen,NULL);
			applySurface(r.x+r.w-5,r.y,selectionMark,screen,NULL);
			applySurface(r.x,r.y+r.h-5,selectionMark,screen,NULL);
			applySurface(r.x+r.w-5,r.y+r.h-5,selectionMark,screen,NULL);
		}

		//placement 표면을 clear
		SDL_FillRect(placement,NULL,0x00FF00FF);

		//level의 바깥부분을 어둡게 marking 함
		SDL_Rect r;
		if(camera.x<0){
			//왼쪽 면을 그림
			r.x=0;
			r.y=0;
			r.w=0-camera.x;
			r.h=SCREEN_HEIGHT;
			SDL_FillRect(placement,&r,0);
		}
		if(camera.x>LEVEL_WIDTH-SCREEN_WIDTH){
			//오른쪽 면을 그림
			r.x=LEVEL_WIDTH-camera.x;
			r.y=0;
			r.w=SCREEN_WIDTH-(LEVEL_WIDTH-camera.x);
			r.h=SCREEN_HEIGHT;
			SDL_FillRect(placement,&r,0);
		}
		if(camera.y<0){
			//위를 그림
			r.x=0;
			r.y=0;
			r.w=SCREEN_WIDTH;
			r.h=0-camera.y;
			SDL_FillRect(placement,&r,0);
		}
		if(camera.y>LEVEL_HEIGHT-SCREEN_HEIGHT){
			//아래를 그림
			r.x=0;
			r.y=LEVEL_HEIGHT-camera.y;
			r.w=SCREEN_WIDTH;
			r.h=SCREEN_HEIGHT-(LEVEL_HEIGHT-camera.y);
			SDL_FillRect(placement,&r,0);
		}

		//placement surface위를 그렸는지 체크함
		if(selectionDrag){
			showSelectionDrag();
		}else{
			if(tool==ADD){
				showCurrentObject();
			}
			if(tool==CONFIGURE){
				showConfigure();
			}
		}

		//level 경계선을 그림
		drawRect(-camera.x,-camera.y,LEVEL_WIDTH,LEVEL_HEIGHT,screen);

		//placement surface를 render
		applySurface(0,0,placement,screen,NULL);

		//hud layer을 render
		renderHUD();

		//tool box를 render
		if(toolbox!=NULL && tool==ADD && toolbox->visible){
			toolbox->render();
		}

		//On top of all render the toolbar.
		applySurface((SCREEN_WIDTH-460)/2,SCREEN_HEIGHT-50,toolbar,screen,NULL);
		//tooltip을 render
		if(tooltip>=0){
			//The back and foreground colors.
			SDL_Color fg={0,0,0};

			//Tool specific text.
			SDL_Surface* tip=NULL;
			switch(tooltip){
				case 0:
					tip=TTF_RenderUTF8_Blended(fontText,_("Select"),fg);
					break;
				case 1:
					tip=TTF_RenderUTF8_Blended(fontText,_("Add"),fg);
					break;
				case 2:
					tip=TTF_RenderUTF8_Blended(fontText,_("Delete"),fg);
					break;
				case 3:
					tip=TTF_RenderUTF8_Blended(fontText,_("Configure"),fg);
					break;
				case 4:
					tip=TTF_RenderUTF8_Blended(fontText,_("Play"),fg);
					break;
				case 6:
					tip=TTF_RenderUTF8_Blended(fontText,_("Level settings"),fg);
					break;
				case 7:
					tip=TTF_RenderUTF8_Blended(fontText,_("Save level"),fg);
					break;
				case 8:
					tip=TTF_RenderUTF8_Blended(fontText,_("Back to menu"),fg);
					break;
				default:
					break;
			}

			//사용가능한 tooltip만을 그림
			if(tip!=NULL){
				SDL_Rect r={(SCREEN_WIDTH-440)/2+(tooltip*40)+(tooltip*10),SCREEN_HEIGHT-45,40,40};
				r.y=SCREEN_HEIGHT-50-tip->h;
				if(r.x+tip->w>SCREEN_WIDTH-50)
					r.x=SCREEN_WIDTH-50-tip->w;

				//text주위의 경계선을 그림
				Uint32 color=0xFFFFFF00|230;
				drawGUIBox(r.x-2,r.y-2,tip->w+4,tip->h+4,screen,color);

				//Draw tooltip's text
				SDL_BlitSurface(tip,NULL,screen,&r);
				SDL_FreeSurface(tip);
			}
		}

		//현재 tool 주위의 사각형을 그림
		Uint32 color=0xFFFFFF00;
		drawGUIBox((SCREEN_WIDTH-440)/2+(tool*40)+(tool*10),SCREEN_HEIGHT-46,42,42,screen,color);

		//Render selection popup (if any)
		if(selectionPopup!=NULL){
			if(linking){
				//linking모드로 바꾸었다면 삭제시킴
				delete selectionPopup;
				selectionPopup=NULL;
			}else{
				selectionPopup->render();
			}
		}
	}
}

void LevelEditor::renderHUD(){
	//Switch the tool.
	switch(tool){
	case CONFIGURE:
		//moving 변수는 오른쪽 위 코너부분에서 moving speed를 보여주는가?
		if(moving){
			//Calculate width of text "Movespeed: 100" to keep the same position with every value
			if (movingSpeedWidth==-1){
				int w;
				TTF_SizeUTF8(fontText,tfm::format(_("Movespeed: %s"),100).c_str(),&w,NULL);
				movingSpeedWidth=w+4;
			}

			//Now render the text.
			SDL_Color black={0,0,0,0};
			SDL_Surface* bm=TTF_RenderUTF8_Blended(fontText,tfm::format(_("Movespeed: %s"),movingSpeed).c_str(),black);

			//box안에 text를 rend하고 surface 를 free시킴
			drawGUIBox(SCREEN_WIDTH-movingSpeedWidth-2,-2,movingSpeedWidth+8,bm->h+6,screen,0xDDDDDDDD);
			applySurface(SCREEN_WIDTH-movingSpeedWidth,2,bm,screen,NULL);
			SDL_FreeSurface(bm);
		}
		break;
	default:
		break;
	}
}

void LevelEditor::showCurrentObject(){
	//현재 마우스 좌표를 얻음
	int x,y;
	SDL_GetMouseState(&x,&y);
	x+=camera.x;
	y+=camera.y;

	//Check if we should snap the block to grid or not.
	if(!pressedShift){
		snapToGrid(&x,&y);
	}else{
		x-=25;
		y-=25;
	}

	//currentType이 적합한지 체크함
	if(currentType>=0 && currentType<EDITOR_ORDER_MAX){
		ThemeBlock* obj=objThemes.getBlock(editorTileOrder[currentType]);
		if(obj){
			obj->editorPicture.draw(placement,x-camera.x,y-camera.y);
		}
	}
}

void LevelEditor::showSelectionDrag(){
	//현재 마우스 좌표를 얻음
	int x,y;
	SDL_GetMouseState(&x,&y);
	//사각형을 생성
	x+=camera.x;
	y+=camera.y;

	//Check if we should snap the block to grid or not.
	if(!pressedShift){
		snapToGrid(&x,&y);
	}else{
		x-=25;
		y-=25;
	}

	//drag center가 null이 아닌지 체크
	if(dragCenter==NULL) return;
	//dragCenter의 좌표
	SDL_Rect r=dragCenter->getBox();

	//selection을 loop
	//TODO: Check if block is in sight.
	for(unsigned int o=0; o<selection.size(); o++){
		ThemeBlock* obj=objThemes.getBlock(selection[o]->type);
		if(obj){
			SDL_Rect r1=selection[o]->getBox();
			obj->editorPicture.draw(placement,(r1.x-r.x)+x-camera.x,(r1.y-r.y)+y-camera.y);
		}
	}
}

void LevelEditor::showConfigure(){
	//arrow animation value. go through 0-65535 and loops.
	static unsigned short arrowAnimation=0;
	arrowAnimation++;

	//trigger lines을 그림
	{
		map<GameObject*,vector<GameObject*> >::iterator it;
		for(it=triggers.begin();it!=triggers.end();++it){
			//trigger가 targets과 연결되었는지(linking) 체크
			if(!(*it).second.empty()){
				//trigger의 좌표
				SDL_Rect r=(*it).first->getBox();

				//Loop through the targets.
				for(unsigned int o=0;o<(*it).second.size();o++){
					//target의 좌표를 얻음
					SDL_Rect r1=(*it).second[o]->getBox();

					//Draw the line from the center of the trigger to the center of the target.
					drawLineWithArrow(r.x-camera.x+25,r.y-camera.y+25,r1.x-camera.x+25,r1.y-camera.y+25,placement,0,32,arrowAnimation%32);

					//두가지 selection marks를 그림
					applySurface(r.x-camera.x+25-2,r.y-camera.y+25-2,selectionMark,screen,NULL);
					applySurface(r1.x-camera.x+25-2,r1.y-camera.y+25-2,selectionMark,screen,NULL);
				}
			}
		}

		//linkingTrigger 가 연결되었을때 mouse의 선을 그림
		if(linking){
			//현재 마우스 좌표를 얻음
			int x,y;
			SDL_GetMouseState(&x,&y);

			//trigger의 가운데부터 마우스까지의 선을 그림
			drawLineWithArrow(linkingTrigger->getBox().x-camera.x+25,linkingTrigger->getBox().y-camera.y+25,x,y,placement,0,32,arrowAnimation%32);
		}
	}

	//moving 좌표를 그림
	map<GameObject*,vector<MovingPosition> >::iterator it;
	for(it=movingBlocks.begin();it!=movingBlocks.end();++it){
		//block 이 가지고있는 좌표를 체크
		if(!(*it).second.empty()){
			//moving block의 좌표
			SDL_Rect block=(*it).first->getBox();
			block.x+=25-camera.x;
			block.y+=25-camera.y;

			//이전위치의 좌표
			//The first time it's the moving block's position self.
			SDL_Rect r=block;

			//positions을 loop
			for(unsigned int o=0;o<(*it).second.size();o++){
				//이전 위치의 가운데 부터 현재 위치의 가운데까지 선을 그림
				//x and y 를 현재 위치를 위해 조정
				int x=block.x+(*it).second[o].x;
				int y=block.y+(*it).second[o].y;

				//Check if we need to draw line
				double dx=r.x-x;
				double dy=r.y-y;
				double d=sqrt(dx*dx+dy*dy);
				if(d>0.001f){
					if(it->second[o].time>0){
						//moving speed를 포함하기 위해 offset을 계산
						int offset=int(d*arrowAnimation/it->second[o].time)%32;
						drawLineWithArrow(r.x,r.y,x,y,placement,0,32,offset);
					}else{
						//time==0 ???? so don't draw arrow at all
						drawLine(r.x,r.y,x,y,placement);
					}
				}

				//끝부분의 마크를 그림
				applySurface(x-13,y-13,movingMark,screen,NULL);

				//이전 위치의 box를 얻음
				SDL_Rect tmp={x,y,0,0};
				r=tmp;
			}
		}
	}

	//이전 moving 위치로부터 마우스까지 선을 그림
	if(moving){
		//현재 마우스 위치를 얻음
		int x,y;
		SDL_GetMouseState(&x,&y);

		//Check if we should snap the block to grid or not.
		if(!pressedShift){
			x+=camera.x;
			y+=camera.y;
			snapToGrid(&x,&y);
			x-=camera.x;
			y-=camera.y;
		}else{
			x-=25;
			y-=25;
		}

		int posX,posY;

		//moving block을 위해 moving 위치가 있는지 체크
		if(!movingBlocks[movingBlock].empty()){
			//이전 moving 위치로부터 마우스까지 선을 그림
			posX=movingBlocks[movingBlock].back().x;
			posY=movingBlocks[movingBlock].back().y;

			posX-=camera.x;
			posY-=camera.y;

			posX+=movingBlock->getBox().x;
			posY+=movingBlock->getBox().y;
		}else{
			//movingBlock의 가운데로부터 마우스까지 선을 그림
			posX=movingBlock->getBox().x-camera.x;
			posY=movingBlock->getBox().y-camera.y;
		}

		//moving speed를 포함하기 위해 offset을 계산
		int offset=int(double(arrowAnimation)*movingSpeed/10.0)%32;

		drawLineWithArrow(posX+25,posY+25,x+25,y+25,placement,0,32,offset);
		applySurface(x+12,y+12,movingMark,screen,NULL);
	}

}

void LevelEditor::resize(){
	//Game의 resize 함수를 불러옴
	Game::resize();

	//placement surface를 업데이트 시킴
	if(placement)
		SDL_FreeSurface(placement);
	placement=SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,SCREEN_WIDTH,SCREEN_HEIGHT,32,0x000000FF,0x0000FF00,0x00FF0000,0);
	SDL_SetColorKey(placement,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(placement->format,255,0,255));
	SDL_SetAlpha(placement,SDL_SRCALPHA,125);

}

//Filling the order array
const int LevelEditor::editorTileOrder[EDITOR_ORDER_MAX]={
	TYPE_BLOCK,
	TYPE_SHADOW_BLOCK,
	TYPE_SPIKES,
	TYPE_FRAGILE,
	TYPE_MOVING_BLOCK,
	TYPE_MOVING_SHADOW_BLOCK,
	TYPE_MOVING_SPIKES,
	TYPE_CONVEYOR_BELT,
	TYPE_SHADOW_CONVEYOR_BELT,
	TYPE_BUTTON,
	TYPE_SWITCH,
	TYPE_PORTAL,
	TYPE_SWAP,
	TYPE_CHECKPOINT,
	TYPE_NOTIFICATION_BLOCK,
	TYPE_START_PLAYER,
	TYPE_START_SHADOW,
	TYPE_EXIT,
	TYPE_COLLECTABLE
};
