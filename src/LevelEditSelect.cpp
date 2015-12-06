// 60142234 강승덕  소스 분석
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

#include "LevelEditSelect.h"
#include "GameState.h"
#include "Functions.h"
#include "FileManager.h"
#include "Globals.h"
#include "Objects.h"
#include "GUIObject.h"
#include "GUIListBox.h"
#include "GUIScrollBar.h"
#include "InputManager.h"
#include "Game.h"
#include "GUIOverlay.h"
#include <SDL/SDL_ttf.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <iostream>

#include "libs/tinyformat/tinyformat.h"

using namespace std;

LevelEditSelect::LevelEditSelect():LevelSelect(_("Map Editor"),LevelPackManager::CUSTOM_PACKS){
	//gui 생성
	createGUI(true);

	//levelEditGUIObjectRoot를 설정
	levelEditGUIObjectRoot=GUIObjectRoot;

	//레벨리스트를 출력
	changePack();
	refresh();
}

LevelEditSelect::~LevelEditSelect(){
	selectedNumber=NULL;
}

void LevelEditSelect::createGUI(bool initial){
	if(initial){
		//레벨 이름 텍스트 필드.
		levelpackName=new GUIObject(280,104,240,32,GUIObjectTextBox);
		levelpackName->eventCallback=this;
		levelpackName->visible=false;
		GUIObjectRoot->childControls.push_back(levelpackName);
	}

	if(!initial){
		//이전 bottons을 지움
		for(int i=0;i<GUIObjectRoot->childControls.size();i++){
			if(GUIObjectRoot->childControls[i]->type==GUIObjectButton && GUIObjectRoot->childControls[i]->caption!=_("Back")){
				delete GUIObjectRoot->childControls[i];
				GUIObjectRoot->childControls.erase(GUIObjectRoot->childControls.begin()+i);
				i--;
			}
		}
	}

	//화면 아래쪽에 여섯개의 bottons를 만듦
	GUIObject* obj=new GUIObject(SCREEN_WIDTH*0.02,SCREEN_HEIGHT-120,-1,32,GUIObjectButton,_("New Levelpack"));
	obj->name="cmdNewLvlpack";
	obj->eventCallback=this;
	GUIObjectRoot->childControls.push_back(obj);

	propertiesPack=new GUIObject(SCREEN_WIDTH*0.5,SCREEN_HEIGHT-120,-1,32,GUIObjectButton,_("Pack Properties"),0,true,true,GUIGravityCenter);
	propertiesPack->name="cmdLvlpackProp";
	propertiesPack->eventCallback=this;
	GUIObjectRoot->childControls.push_back(propertiesPack);

	removePack=new GUIObject(SCREEN_WIDTH*0.98,SCREEN_HEIGHT-120,-1,32,GUIObjectButton,_("Remove Pack"),0,true,true,GUIGravityRight);
	removePack->name="cmdRmLvlpack";
	removePack->eventCallback=this;
	GUIObjectRoot->childControls.push_back(removePack);

	move=new GUIObject(SCREEN_WIDTH*0.02,SCREEN_HEIGHT-60,-1,32,GUIObjectButton,_("Move Map"));
	move->name="cmdMoveMap";
	move->eventCallback=this;
	//NOTE: Set enabled equal to the inverse of initial.
	//윈도우창의 크기를 변경했을 때 initial변수는 false가 되고 그대신에 move button은 활성화를 유지할 수 있다.
	move->enabled=false;
	GUIObjectRoot->childControls.push_back(move);

	remove=new GUIObject(SCREEN_WIDTH*0.5,SCREEN_HEIGHT-60,-1,32,GUIObjectButton,_("Remove Map"),0,false,true,GUIGravityCenter);
	remove->name="cmdRmMap";
	remove->eventCallback=this;
	GUIObjectRoot->childControls.push_back(remove);

	edit=new GUIObject(SCREEN_WIDTH*0.98,SCREEN_HEIGHT-60,-1,32,GUIObjectButton,_("Edit Map"),0,false,true,GUIGravityRight);
	edit->name="cmdEdit";
	edit->eventCallback=this;
	GUIObjectRoot->childControls.push_back(edit);

	//widgets 업데이트 하고 겹치는지 체크한다.
	GUIObjectRoot->render(0,0,false);
	if(propertiesPack->left-propertiesPack->gravityX < obj->left+obj->width ||
	   propertiesPack->left-propertiesPack->gravityX+propertiesPack->width > removePack->left-removePack->gravityX){
		obj->smallFont=true;
		obj->width=-1;

		propertiesPack->smallFont=true;
		propertiesPack->width=-1;

		removePack->smallFont=true;
		removePack->width=-1;

		move->smallFont=true;
		move->width=-1;

		remove->smallFont=true;
		remove->width=-1;

		edit->smallFont=true;
		edit->width=-1;
	}

	//Check again
	GUIObjectRoot->render(0,0,false);
	if(propertiesPack->left-propertiesPack->gravityX < obj->left+obj->width ||
	   propertiesPack->left-propertiesPack->gravityX+propertiesPack->width > removePack->left-removePack->gravityX){
		obj->left = SCREEN_WIDTH*0.02;
		obj->top = SCREEN_HEIGHT-140;
		obj->smallFont=false;
		obj->width=-1;
		obj->gravity = GUIGravityLeft;

		propertiesPack->left = SCREEN_WIDTH*0.02;
		propertiesPack->top = SCREEN_HEIGHT-100;
		propertiesPack->smallFont=false;
		propertiesPack->width=-1;
		propertiesPack->gravity = GUIGravityLeft;

		removePack->left = SCREEN_WIDTH*0.02;
		removePack->top = SCREEN_HEIGHT-60;
		removePack->smallFont=false;
		removePack->width=-1;
		removePack->gravity = GUIGravityLeft;

		move->left = SCREEN_WIDTH*0.98;
		move->top = SCREEN_HEIGHT-140;
		move->smallFont=false;
		move->width=-1;
		move->gravity = GUIGravityRight;

		remove->left = SCREEN_WIDTH*0.98;
		remove->top = SCREEN_HEIGHT-100;
		remove->smallFont=false;
		remove->width=-1;
		remove->gravity = GUIGravityRight;

		edit->left = SCREEN_WIDTH*0.98;
		edit->top = SCREEN_HEIGHT-60;
		edit->smallFont=false;
		edit->width=-1;
		edit->gravity = GUIGravityRight;
	}
}

void LevelEditSelect::changePack(){
	packName=levelpacks->item[levelpacks->value];
	if(packName=="Custom Levels"){
		//몇가지 levelpack buttons을 비활성화
		propertiesPack->enabled=false;
		removePack->enabled=false;
	}else{
		//몇가지 levelpack buttons을 활성화
		propertiesPack->enabled=true;
		removePack->enabled=true;
	}

	//마지막 levelpack을 설정
	getSettings()->setValue("lastlevelpack",packName);

	//levels이 오른쪽 pack을 가리키도록 한다.
	levels=getLevelPackManager()->getLevelPack(packName);
}

void LevelEditSelect::packProperties(bool newPack){
	//Open a message popup.
	GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-320)/2,600,320,GUIObjectFrame,_("Properties"));
	GUIObject* obj;

	obj=new GUIObject(40,50,240,36,GUIObjectLabel,_("Name:"));
	root->childControls.push_back(obj);

	obj=new GUIObject(60,80,480,36,GUIObjectTextBox,packName.c_str());
	if(newPack)
		obj->caption="";
	obj->name="LvlpackName";
	root->childControls.push_back(obj);

	obj=new GUIObject(40,120,240,36,GUIObjectLabel,_("Description:"));
	root->childControls.push_back(obj);

	obj=new GUIObject(60,150,480,36,GUIObjectTextBox,levels->levelpackDescription.c_str());
	if(newPack)
		obj->caption="";
	obj->name="LvlpackDescription";
	root->childControls.push_back(obj);

	obj=new GUIObject(40,190,240,36,GUIObjectLabel,_("Congratulation text:"));
	root->childControls.push_back(obj);

	obj=new GUIObject(60,220,480,36,GUIObjectTextBox,levels->congratulationText.c_str());
	if(newPack)
		obj->caption="";
	obj->name="LvlpackCongratulation";
	root->childControls.push_back(obj);

	obj=new GUIObject(root->width*0.3,320-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
	obj->name="cfgOK";
	obj->eventCallback=this;
	root->childControls.push_back(obj);
	obj=new GUIObject(root->width*0.7,320-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
	obj->name="cfgCancel";
	obj->eventCallback=this;
	root->childControls.push_back(obj);

	// gui overlay를 생성
	GUIOverlay* overlay=new GUIOverlay(root);

	if(newPack){
		packName.clear();
	}
}

void LevelEditSelect::addLevel(){
	//Open a message popup.
	GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-200)/2,600,200,GUIObjectFrame,_("Add level"));
	GUIObject* obj;

	obj=new GUIObject(40,80,240,36,GUIObjectLabel,_("File name:"));
	root->childControls.push_back(obj);

	char s[64];
	sprintf(s,"map%02d.map",levels->getLevelCount()+1);
	obj=new GUIObject(300,80,240,36,GUIObjectTextBox,s);
	obj->name="LvlFile";
	root->childControls.push_back(obj);

	obj=new GUIObject(root->width*0.3,200-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
	obj->name="cfgAddOK";
	obj->eventCallback=this;
	root->childControls.push_back(obj);
	obj=new GUIObject(root->width*0.7,200-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
	obj->name="cfgAddCancel";
	obj->eventCallback=this;
	root->childControls.push_back(obj);

	//화면의 어두운 부분을 임시장소로(tempSurface) 사용
	GUIOverlay* overlay=new GUIOverlay(root);
}

void LevelEditSelect::moveLevel(){
	//Open a message popup.
	GUIObject* root=new GUIObject((SCREEN_WIDTH-600)/2,(SCREEN_HEIGHT-200)/2,600,200,GUIObjectFrame,_("Move level"));
	GUIObject* obj;

	obj=new GUIObject(40,60,240,36,GUIObjectLabel,_("Level: "));
	root->childControls.push_back(obj);

	obj=new GUIObject(300,60,240,36,GUIObjectTextBox,"1");
	obj->name="MoveLevel";
	root->childControls.push_back(obj);

	obj=new GUISingleLineListBox(root->width*0.5,110,240,36,true,true,GUIGravityCenter);
	obj->name="lstPlacement";
	vector<string> v;
	v.push_back(_("Before"));
	v.push_back(_("After"));
	v.push_back(_("Swap"));
	(dynamic_cast<GUISingleLineListBox*>(obj))->item=v;
	obj->value=0;
	root->childControls.push_back(obj);

	obj=new GUIObject(root->width*0.3,200-44,-1,36,GUIObjectButton,_("OK"),0,true,true,GUIGravityCenter);
	obj->name="cfgMoveOK";
	obj->eventCallback=this;
	root->childControls.push_back(obj);
	obj=new GUIObject(root->width*0.7,200-44,-1,36,GUIObjectButton,_("Cancel"),0,true,true,GUIGravityCenter);
	obj->name="cfgMoveCancel";
	obj->eventCallback=this;
	root->childControls.push_back(obj);

	// gui overlay를 생성
	GUIOverlay* overlay=new GUIOverlay(root);
}

void LevelEditSelect::refresh(bool change){
	int m=levels->getLevelCount();

	if(change){
		numbers.clear();

		//선택된 level을 clear
		if(selectedNumber!=NULL){
			selectedNumber=NULL;
		}

		//level specific buttons을 비활성화
		move->enabled=false;
		remove->enabled=false;
		edit->enabled=false;

		for(int n=0;n<=m;n++){
			numbers.push_back(Number());
		}
	}

	for(int n=0;n<m;n++){
		SDL_Rect box={(n%LEVELS_PER_ROW)*64+80,(n/LEVELS_PER_ROW)*64+184,0,0};
		numbers[n].init(n,box);
	}
	SDL_Rect box={(m%LEVELS_PER_ROW)*64+80,(m/LEVELS_PER_ROW)*64+184,0,0};
	numbers[m].init("+",box);

	m++; //including the "+" button
	if(m>LEVELS_DISPLAYED_IN_SCREEN){
		levelScrollBar->maxValue=(m-LEVELS_DISPLAYED_IN_SCREEN+LEVELS_PER_ROW-1)/LEVELS_PER_ROW;
		levelScrollBar->visible=true;
	}else{
		levelScrollBar->maxValue=0;
		levelScrollBar->visible=false;
	}
	if(!levels->levelpackDescription.empty())
		levelpackDescription->caption=_C(levels->getDictionaryManager(),levels->levelpackDescription);
	else
		levelpackDescription->caption="";
}

void LevelEditSelect::selectNumber(unsigned int number,bool selected){
	if(selected){
		levels->setCurrentLevel(number);
		setNextState(STATE_LEVEL_EDITOR);

		//현재 music list에서 music을 고른다.
		getMusicManager()->pickMusic();
	}else{
		if(number==numbers.size()-1){
			addLevel();
		}else if(number>=0 && number<numbers.size()){
			selectedNumber=&numbers[number];

			//level specific buttons을 활성화
			//NOTE: 'remove levelpack' 이 활성화 되었는지 체크한다. if not then it's the Levels levelpack.
			if(removePack->enabled)
				move->enabled=true;
			remove->enabled=true;
			edit->enabled=true;
		}
	}
}

void LevelEditSelect::render(){
	//levelselect 를 render한다.
	LevelSelect::render();
}

void LevelEditSelect::resize(){
	//levelselect 창을 크기 조절한다.
	LevelSelect::resize();

	//GUI 생성
	createGUI(false);

	//NOTE: This is a workaround for buttons failing when resizing.
	if(packName=="Custom Levels"){
		removePack->enabled=false;
		propertiesPack->enabled=false;
	}
	if(selectedNumber)
		selectNumber(selectedNumber->getNumber(),false);

}

void LevelEditSelect::renderTooltip(unsigned int number,int dy){
	SDL_Color fg={0,0,0};
	SDL_Surface* name;

	if(number==(unsigned)levels->getLevelCount()){
		//level의 이름을 render
		name=TTF_RenderUTF8_Blended(fontText,_("Add level"),fg);
	}else{
		//level의 이름을 render
		name=TTF_RenderUTF8_Blended(fontText,_C(levels->getDictionaryManager(),levels->getLevelName(number)),fg);
	}

	//name이 null이 아님을 체크
	if(name==NULL)
		return;

	//세가지 texts를 조합한 크기 만큼 정사각형을 그린다.
	SDL_Rect r=numbers[number].box;
	r.y-=dy*64;
	r.w=name->w;
	r.h=name->h;

	//tooltip이 윈도우창 밖으로 나가지 않음을 확실히 해둔다.
	if(r.y>SCREEN_HEIGHT-200){
		r.y-=name->h+4;
	}else{
		r.y+=numbers[number].box.h+2;
	}
	if(r.x+r.w>SCREEN_WIDTH-50)
		r.x=SCREEN_WIDTH-50-r.w;

	//사각형을 그린다.
	Uint32 color=0xFFFFFF00|240;
	drawGUIBox(r.x-5,r.y-5,r.w+10,r.h+10,screen,color);

	//그리기 위해 위치를 계산한다.
	SDL_Rect r2=r;

	//그리고 surface 가 null이 아니면 name을 render한다.
	if(name!=NULL){
		//Draw the name.
		SDL_BlitSurface(name,NULL,screen,&r2);
	}

	//And free the surfaces.
	SDL_FreeSurface(name);
}

void LevelEditSelect::GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType){
	//NOTE: We check for the levelpack change to enable/disable some levelpack buttons.
	if(name=="cmdLvlPack"){
		//levelSelect가 했던것을 되돌리는 것을 막기위해 changepack을 호출하고 return시킨다.
		changePack();
		refresh();
		return;
	}

	//level select 에서 자신과 관련된 GUI 이벤트를 다룸
	LevelSelect::GUIEventCallback_OnEvent(name,obj,eventType);

	//edit button을 체크
	if(name=="cmdNewLvlpack"){
		//새로운 pack을 만듦
		packProperties(true);
	}else if(name=="cmdLvlpackProp"){
		//pack 속성을 보여줌
		packProperties(false);
	}else if(name=="cmdRmLvlpack"){
		//"are you sure" 이란 메시지를 보여줌
		if(msgBox(_("Are you sure?"),MsgBoxYesNo,_("Remove prompt"))==MsgBoxYes){
			//directory를 삭제함
			if(!removeDirectory(levels->levelpackPath.c_str())){
				cerr<<"ERROR: Unable to remove levelpack directory "<<levels->levelpackPath<<endl;
			}

			//vector(levelpack list) 에서 삭제함
			vector<string>::iterator it;
			it=find(levelpacks->item.begin(),levelpacks->item.end(),packName);
			if(it!=levelpacks->item.end()){
				levelpacks->item.erase(it);
			}

			//levelpackManager에서 삭제함
			getLevelPackManager()->removeLevelPack(packName);

			//changePack함수를 부름
			levelpacks->value=levelpacks->item.size()-1;
			changePack();
			refresh();
		}
	}else if(name=="cmdMoveMap"){
		if(selectedNumber!=NULL){
			moveLevel();
		}
	}else if(name=="cmdRmMap"){
		if(selectedNumber!=NULL){
			if(packName!="Custom Levels"){
				if(!removeFile((levels->levelpackPath+"/"+levels->getLevel(selectedNumber->getNumber())->file).c_str())){
					cerr<<"ERROR: Unable to remove level "<<(levels->levelpackPath+"/"+levels->getLevel(selectedNumber->getNumber())->file).c_str()<<endl;
				}
				levels->removeLevel(selectedNumber->getNumber());
				levels->saveLevels(levels->levelpackPath+"/levels.lst");
			}else{
				//This is the levels levelpack so we just remove the file.
				if(!removeFile(levels->getLevel(selectedNumber->getNumber())->file.c_str())){
					cerr<<"ERROR: Unable to remove level "<<levels->getLevel(selectedNumber->getNumber())->file<<endl;
				}
				levels->removeLevel(selectedNumber->getNumber());
			}

			//selection 창을 새로고침
			refresh();
		}
	}else if(name=="cmdEdit"){
		if(selectedNumber!=NULL){
			levels->setCurrentLevel(selectedNumber->getNumber());
			setNextState(STATE_LEVEL_EDITOR);

			//현재 music list에서 music을 고른다.
			getMusicManager()->pickMusic();
		}
	}

	//levelpack 속성 이벤트를 체크한다.
	if(name=="cfgOK"){
		//필드를 찾기 위해 GUIObjectRoot의 자식들을 Loop 돈다.
		for(unsigned int i=0;i<GUIObjectRoot->childControls.size();i++){
			if(GUIObjectRoot->childControls[i]->name=="LvlpackName"){
				//name이 바뀌었는지 체크한다.
				if(packName!=GUIObjectRoot->childControls[i]->caption){
					//오래된것을 지운다.
					if(!packName.empty()){
						if(!renameDirectory((getUserPath(USER_DATA)+"custom/levelpacks/"+packName).c_str(),(getUserPath(USER_DATA)+"custom/levelpacks/"+GUIObjectRoot->childControls[i]->caption).c_str())){
							cerr<<"ERROR: Unable to move levelpack directory "<<(getUserPath(USER_DATA)+"custom/levelpacks/"+packName)<<" to "<<(getUserPath(USER_DATA)+"custom/levelpacks/"+GUIObjectRoot->childControls[i]->caption)<<endl;
						}

						//Rlevelpack manager에서 오래된 것을 지운다.
						getLevelPackManager()->removeLevelPack(packName);

						//And the levelpack list.
						vector<string>::iterator it1;
						it1=find(levelpacks->item.begin(),levelpacks->item.end(),packName);
							if(it1!=levelpacks->item.end()){
								levelpacks->item.erase(it1);
							if((unsigned)levelpacks->value>levelpacks->item.size())
								levelpacks->value=levelpacks->item.size()-1;
						}
					}else{
						//새로운 levelpack이기 때문에 levels의 배열을 바꾸어야 한다.
						LevelPack* pack=new LevelPack;
						levels=pack;

						//그리고 dirs을 만든다.
						if(!createDirectory((getUserPath(USER_DATA)+"custom/levelpacks/"+GUIObjectRoot->childControls[i]->caption).c_str())){
							cerr<<"ERROR: Unable to create levelpack directory "<<(getUserPath(USER_DATA)+"custom/levelpacks/"+GUIObjectRoot->childControls[i]->caption)<<endl;
						}
						if(!createFile((getUserPath(USER_DATA)+"custom/levelpacks/"+GUIObjectRoot->childControls[i]->caption+"/levels.lst").c_str())){
							cerr<<"ERROR: Unable to create levelpack file "<<(getUserPath(USER_DATA)+"custom/levelpacks/"+GUIObjectRoot->childControls[i]->caption+"/levels.lst")<<endl;
						}
					}
					//새로운 name을 설정한다.
					packName=GUIObjectRoot->childControls[i]->caption;
					levels->levelpackName=packName;
					levels->levelpackPath=(getUserPath(USER_DATA)+"custom/levelpacks/"+packName+"/");

					//levelpack의 위치를 추가한다.
					getLevelPackManager()->addLevelPack(levels);
					levelpacks->item.push_back(GUIObjectRoot->childControls[i]->caption);
					levelpacks->value=levelpacks->item.size()-1;

					//changePack함수를 부른다.
					changePack();
				}
			}
			if(GUIObjectRoot->childControls[i]->name=="LvlpackDescription"){
				levels->levelpackDescription=GUIObjectRoot->childControls[i]->caption;
			}
			if(GUIObjectRoot->childControls[i]->name=="LvlpackCongratulation"){
				levels->congratulationText=GUIObjectRoot->childControls[i]->caption;
			}
		}
		//올바른 정보를 보여주기 위해 leveleditselect를 새로고침
		refresh();

		//설정(configuration) 저장
		levels->saveLevels(getUserPath(USER_DATA)+"custom/levelpacks/"+packName+"/levels.lst");
		getSettings()->setValue("lastlevelpack",packName);

		//gui를 clear함
		if(GUIObjectRoot){
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}else if(name=="cfgCancel"){
		//packName이 비어있는지 체크하고 만약 그렇다면 이것은 새로운 levelpack이었고 이미 있었던 것으로 되돌아가야 한다.
		if(packName.empty()){
			packName=levelpacks->item[levelpacks->value];
			changePack();
		}

		//gui를 clear함
		if(GUIObjectRoot){
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}

	//추가 level이벤트를 체크한다.
	if(name=="cfgAddOK"){
		//file name 이 null이 아님을 체크한다.
		//fields를 찾기 위해서 GUIObjectRoot의 자식들을 Loop돈다.
		for(unsigned int i=0;i<GUIObjectRoot->childControls.size();i++){
			if(GUIObjectRoot->childControls[i]->name=="LvlFile"){
				if(GUIObjectRoot->childControls[i]->caption.empty()){
					msgBox(_("No file name given for the new level."),MsgBoxOKOnly,_("Missing file name"));
					return;
				}else{
					string tmp_caption = GUIObjectRoot->childControls[i]->caption;

					//모든 공간을 _(underLine)으로 대체한다.
					size_t j;
					for(;(j=tmp_caption.find(" "))!=string::npos;){
						tmp_caption.replace(j,1,"_");
					}

					//".map"이 없다면 추가한다.
					size_t found=tmp_caption.find_first_of(".");
					if(found!=string::npos)
						tmp_caption.replace(tmp_caption.begin()+found+1,tmp_caption.end(),"map");
					else if (tmp_caption.substr(found+1)!="map")
						tmp_caption.append(".map");

					/* 경로를 만들고 파일을 그 안에 둔다.*/
					string path=(levels->levelpackPath+"/"+tmp_caption);
					if(packName=="Custom Levels"){
						path=(getUserPath(USER_DATA)+"/custom/levels/"+tmp_caption);
					}

					//First check if the file doesn't exist already.
					FILE* f;
					f=fopen(path.c_str(),"rb");

					//있는지 체크한다.
					if(f){
						//file을 닫는다.
						fclose(f);

						//화면이 검게 변하거나 여러 GUI가 겹치는것을 막기위해 한번만 currentState를 render한다.
						currentState->render();
						levelEditGUIObjectRoot->render();

						//user에게 알린다.
						msgBox(("The file "+tmp_caption+" already exists.").c_str(),MsgBoxOKOnly,"Error");
						return;
					}

					if(!createFile(path.c_str())){
						cerr<<"ERROR: Unable to create level file "<<path<<endl;
					}
					levels->addLevel(path);
					if(packName!="Custom Levels")
						levels->saveLevels(getUserPath(USER_DATA)+"custom/levelpacks/"+packName+"/levels.lst");
					refresh();

					//gui를 clear함
					if(GUIObjectRoot){
						delete GUIObjectRoot;
						GUIObjectRoot=NULL;
						return;
					}
				}
			}
		}
	}else if(name=="cfgAddCancel"){
		//gui를 clear함
		if(GUIObjectRoot){
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}

	//move level 이벤트를 체크함
	if(name=="cfgMoveOK"){
		//들어가는 level의 숫자가 유효한지 체크한다.
		//fields를 찾기 위해서 GUIObjectRoot의 자식들을 Loop돈다.
		int level=0;
		int placement=0;
		for(unsigned int i=0;i<GUIObjectRoot->childControls.size();i++){
			if(GUIObjectRoot->childControls[i]->name=="MoveLevel"){
				level=atoi(GUIObjectRoot->childControls[i]->caption.c_str());
				if(level<=0 || level>levels->getLevelCount()){
					msgBox(_("The entered level number isn't valid!"),MsgBoxOKOnly,_("Illegal number"));
					return;
				}
			}
			if(GUIObjectRoot->childControls[i]->name=="lstPlacement"){
				placement=GUIObjectRoot->childControls[i]->value;
			}
		}

		//그리고 swap/move를 실행한다.
		//이전에 두었는지 체크한다.
		if(placement==0){
			//level에 들어가기 전에 선택된 level을 둔다
			levels->moveLevel(selectedNumber->getNumber(),level-1);
		}else if(placement==1){
			//level에 들어간 후 선택된 level을 둔다.
			if(level<selectedNumber->getNumber())
				levels->moveLevel(selectedNumber->getNumber(),level);
			else
				levels->moveLevel(selectedNumber->getNumber(),level+1);
		}else if(placement==2){
			//들어가진 level과 선택된 level을 바꾼다.
			levels->swapLevel(selectedNumber->getNumber(),level-1);
		}

		//변화를 저장한다.
		if(packName!="Custom Levels")
			levels->saveLevels(getUserPath(USER_DATA)+"custom/levelpacks/"+packName+"/levels.lst");

		refresh();

		//gui를 clear함
		if(GUIObjectRoot){
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}else if(name=="cfgMoveCancel"){
		//gui를 clear함
		if(GUIObjectRoot){
			delete GUIObjectRoot;
			GUIObjectRoot=NULL;
		}
	}
}
