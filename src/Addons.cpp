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

#include "Addons.h"
#include "GameState.h"
#include "Functions.h"
#include "FileManager.h"
#include "Globals.h"
#include "Objects.h"
#include "GUIObject.h"
#include "GUIScrollBar.h"
#include "GUIListBox.h"
#include "POASerializer.h"
#include "InputManager.h"
#include <string>
#include <sstream>
#include <iostream>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL.h>
using namespace std;

Addons::Addons(){
	//타이틀을 만듬
	SDL_Color black={0,0,0};
	title=TTF_RenderUTF8_Blended(fontTitle,_("Addons"),black);
	
	FILE* addon=fopen((getUserPath(USER_CACHE)+"addons").c_str(),"wb");
	action=NONE;

	addons=NULL;
	
	//GUI가 있으면 GUI를 삭제
	if(GUIObjectRoot){
		delete GUIObjectRoot;
		GUIObjectRoot=NULL;
	}
	
	//addonsList를 다운 시도
	if(getAddonsList(addon)==false) {
		//실패하면 에러메시지가 뜸
		GUIObjectRoot=new GUIObject(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
		GUIObject* obj=new GUIObject(90,96,200,32,GUIObjectLabel,_("Unable to initialize addon menu:"));
		obj->name="lbl";
		GUIObjectRoot->childControls.push_back(obj);
		
		obj=new GUIObject(120,130,200,32,GUIObjectLabel,error.c_str());
		obj->name="lbl";
		GUIObjectRoot->childControls.push_back(obj);
		
		obj=new GUIObject(90,550,200,32,GUIObjectButton,_("Back"));
		obj->name="cmdBack";
		obj->eventCallback=this;
		GUIObjectRoot->childControls.push_back(obj);
		return;
	}
	
	//GUI생성
	createGUI();
}

Addons::~Addons(){
	delete addons;
	

	SDL_FreeSurface(title);
	
	//GUIObjectRoot가 존재하면 그걸 삭제함
	if(GUIObjectRoot){
		delete GUIObjectRoot;
		GUIObjectRoot=NULL;
	}
}

void Addons::createGUI(){	
	//addons file이 다운되어짐, GUI를 만들수 있음
	GUIObjectRoot=new GUIObject(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	
	//카테고리 리스트 생성 레벨, 레벨팩,테마...
	GUISingleLineListBox *listTabs=new GUISingleLineListBox((SCREEN_WIDTH-360)/2,100,360,36);
	listTabs->name="lstTabs";
	listTabs->item.push_back(_("Levels"));
	listTabs->item.push_back(_("Level Packs"));
	listTabs->item.push_back(_("Themes"));
	listTabs->value=0;
	listTabs->eventCallback=this;
	GUIObjectRoot->childControls.push_back(listTabs);

	//addons 리스트를 생성하고
	//By default levels will be selected.
	list=new GUIListBox(SCREEN_WIDTH*0.1,160,SCREEN_WIDTH*0.8,SCREEN_HEIGHT-220);
	list->item=addonsToList("levels");
	list->name="lstAddons";
	list->eventCallback=this;
	list->value=-1;
	GUIObjectRoot->childControls.push_back(list);
	type="levels";
	
	//화면 아래쪽 버튼들
	GUIObject* obj=new GUIObject(SCREEN_WIDTH*0.3,SCREEN_HEIGHT-50,-1,32,GUIObjectButton,_("Back"),0,true,true,GUIGravityCenter);
	obj->name="cmdBack";
	obj->eventCallback=this;
	GUIObjectRoot->childControls.push_back(obj);
	actionButton=new GUIObject(SCREEN_WIDTH*0.7,SCREEN_HEIGHT-50,-1,32,GUIObjectButton,_("Install"),0,false,true,GUIGravityCenter);
	actionButton->name="cmdInstall";
	actionButton->eventCallback=this;
	GUIObjectRoot->childControls.push_back(actionButton);
	updateButton=new GUIObject(SCREEN_WIDTH*0.5,SCREEN_HEIGHT-50,-1,32,GUIObjectButton,_("Update"),0,false,false,GUIGravityCenter);
	updateButton->name="cmdUpdate";
	updateButton->eventCallback=this;
	GUIObjectRoot->childControls.push_back(updateButton);
}

bool Addons::getAddonsList(FILE* file){
	//파일 다운로드
	if(downloadFile("http://meandmyshadow.git.sourceforge.net/git/gitweb.cgi?p=meandmyshadow/meandmyshadow;a=blob_plain;f=addons04",file)==false){
		//NOTE: We keep the console output English so we put the string literal here twice.
		cerr<<"ERROR: unable to download addons file!"<<endl;
		error=_("ERROR: unable to download addons file!");
		return false;
	}
	fclose(file);
	
	//다운로드 파일 로드부분
	ifstream addonFile;
	addonFile.open((getUserPath(USER_CACHE)+"addons").c_str());
	
	if(!addonFile.good()) {
		//참고 : 두 번 리터럴 문자열을 넣어 콘솔 출력을 유지합니다.
		cerr<<"ERROR: unable to load addon_list file!"<<endl;
		error=_("ERROR: unable to load addon_list file!");
		return false;
	}
	
	//Parse the addonsfile.
	TreeStorageNode obj;
	{
		POASerializer objSerializer;
		if(!objSerializer.readNode(addonFile,&obj,true)){
			//참고 : 두 번 리터럴 문자열을 넣어 콘솔 출력을 유지합니다.
			cerr<<"ERROR: Invalid file format of addons file!"<<endl;
			error=_("ERROR: Invalid file format of addons file!");
			return false;
		}
	}
	
	//installed_addons 파일을 로드합니다.
	ifstream iaddonFile;
	iaddonFile.open((getUserPath(USER_CONFIG)+"installed_addons").c_str());
	
	if(!iaddonFile) {
		//installed_addons 파일이 존재하지 않기 때문에, 만들 수 있다.
		ofstream iaddons;
		iaddons.open((getUserPath(USER_CONFIG)+"installed_addons").c_str());
		iaddons<<" "<<endl;
		iaddons.close();
		
		//또한 installed_addons 파일을 로드합니다.
		iaddonFile.open((getUserPath(USER_CONFIG)+"installed_addons").c_str());
		if(!iaddonFile) {
			//참고 : 두 번 리터럴 문자열을 넣어 콘솔 출력을 유지합니다.
			cerr<<"ERROR: Unable to create the installed_addons file."<<endl;
			error=_("ERROR: Unable to create the installed_addons file.");
			return false;
		}
	}
	
	//그리고 installed_addons 파일을 분석합니다.
	TreeStorageNode obj1;
	{
		POASerializer objSerializer;
		if(!objSerializer.readNode(iaddonFile,&obj1,true)){
			//참고 : 두 번 리터럴 문자열을 넣어 콘솔 출력을 유지합니다.
			cerr<<"ERROR: Invalid file format of the installed_addons!"<<endl;
			error=_("ERROR: Invalid file format of the installed_addons!");
			return false;
		}
	}
	
	
	//벡터를 입력합니다.
	addons = new std::vector<Addon>;
	fillAddonList(*addons,obj,obj1);
		
	//파일을 닫습니다.
	iaddonFile.close();
	addonFile.close();
	return true;
}

void Addons::fillAddonList(std::vector<Addons::Addon> &list, TreeStorageNode &addons, TreeStorageNode &installed_addons){
	// 애드온 파일의 블록을 통해 반복.
 // 레벨, levelpacks, 테마를 포함해야 한다.
	for(unsigned int i=0;i<addons.subNodes.size();i++){
		TreeStorageNode* block=addons.subNodes[i];
		if(block==NULL) continue;
		
		string type;
		type=block->name;
		//이제 루프 블록의 항목 (하위 노드)부분.
		for(unsigned int i=0;i<block->subNodes.size();i++){
			TreeStorageNode* entry=block->subNodes[i];
			if(entry==NULL) continue;
			if(entry->name=="entry" && entry->value.size()==1){
				//항목은 새로운 애드온을 만들기에 유효함
				Addon addon = *(new Addon);
				addon.type=type;
				addon.name=entry->value[0];
				addon.file=entry->attributes["file"][0];
				if(!entry->attributes["folder"].empty()){
					addon.folder=entry->attributes["folder"][0];
				}
				addon.author=entry->attributes["author"][0];
				addon.version=atoi(entry->attributes["version"][0].c_str());
				addon.upToDate=false;
				addon.installed=false;
				
				//애드온이 이미 설치되어 있는지 확인합니다.
				for(unsigned int i=0;i<installed_addons.subNodes.size();i++){
					TreeStorageNode* installed=installed_addons.subNodes[i];
					if(installed==NULL) continue;
					if(installed->name=="entry" && installed->value.size()==3){
						if(addon.type.compare(installed->value[0])==0 && addon.name.compare(installed->value[1])==0) {
							addon.installed=true;
							addon.installedVersion=atoi(installed->value[2].c_str());
							if(addon.installedVersion>=addon.version) {
								addon.upToDate=true;
							}

						}
					}
				}
				
				//마지막으로 목록에 집어 넣음
				list.push_back(addon);
			}
		}
	}
}

std::vector<std::string> Addons::addonsToList(const std::string &type){
	std::vector<std::string> result;
	
	for(unsigned int i=0;i<addons->size();i++) {
		//애드온 오른쪽 유형에 되어 있는지 확인합니다.
		if((*addons)[i].type==type) {
			string entry = (*addons)[i].name + " by " + (*addons)[i].author;
			if((*addons)[i].installed) {
				if((*addons)[i].upToDate) {
					entry += " *";
				} else {
					entry += " +";
				}
			}
			result.push_back(entry);
		}
	}
	return result;
}

bool Addons::saveInstalledAddons(){
	if(!addons) return false;

	//파일을 연다
	ofstream iaddons;
	iaddons.open((getUserPath(USER_CONFIG)+"installed_addons").c_str());
	if(!iaddons) return false;
	
	//모든 레벨을 루프
	TreeStorageNode installed;
	for(unsigned int i=0;i<addons->size();i++) {
		//레벨이 설치되었는지 아닌지 확인
		if((*addons)[i].installed) {
			TreeStorageNode *entry=new TreeStorageNode;
			entry->name="entry";
			entry->value.push_back((*addons)[i].type);
			entry->value.push_back((*addons)[i].name);
			char version[64];
			sprintf(version,"%d",(*addons)[i].installedVersion);
			entry->value.push_back(version);
			
			installed.subNodes.push_back(entry);
		}
	}
	
	
	//And write away the file.
	POASerializer objSerializer;
	objSerializer.writeNode(&installed,iaddons,true,true);
	
	return true;
}

void Addons::handleEvents(){
	//종료할건지 확인합니다.
	if(event.type==SDL_QUIT){
		//종료하기 전에 설치된 애드온을 저장합니다.
		saveInstalledAddons();
		setNextState(STATE_EXIT);
	}

	//탈출을 눌렀을때 주 메뉴로 돌아가 있는지 확인합니다.
	if(inputMgr.isKeyUpEvent(INPUTMGR_ESCAPE)){
		setNextState(STATE_MENU);
	}
}

void Addons::logic(){}

void Addons::render(){
	//메뉴 배경 그림
	applySurface(0,0,menuBackground,screen,NULL);
	
	//타이틀 그리기
	applySurface((SCREEN_WIDTH-title->w)/2,40-TITLE_FONT_RAISE,title,screen,NULL);
}

void Addons::resize(){
	//GUI (있는 경우)를 삭제합니다.
	if(GUIObjectRoot){
		delete GUIObjectRoot;
		GUIObjectRoot=NULL;
	}
	
	//이제 새로 만듭니다.
	createGUI();
}

void Addons::GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType){
	if(name=="lstTabs"){
		if(obj->value==0){
			list->item=addonsToList("levels");
			type="levels";
		}else if(obj->value==1){
			list->item=addonsToList("levelpacks");
			type="levelpacks";
		}else{
			list->item=addonsToList("themes");
			type="themes";
		}
		list->value=0;
		GUIEventCallback_OnEvent("lstAddons",list,GUIEventChange);
	}else if(name=="lstAddons"){
		//여기에 속하는 애드온 구조체를 가져옵니다.
		Addon *addon=NULL;
		if(!list->item.empty()) {
			string entry = list->item[list->value];
			if(type.compare("levels")==0) {
				for(unsigned int i=0;i<addons->size();i++) {
					std::string prefix=(*addons)[i].name;
					if(!entry.compare(0, prefix.size(), prefix)) {
						addon=&(*addons)[i];
					}
				}
			} else if(type.compare("levelpacks")==0) {
				for(unsigned int i=0;i<addons->size();i++) {
					std::string prefix=(*addons)[i].name;
					if(!entry.compare(0, prefix.size(), prefix)) {
						addon=&(*addons)[i];
					}
				} 
			} else if(type.compare("themes")==0) {
				for(unsigned int i=0;i<addons->size();i++) {
					std::string prefix=(*addons)[i].name;
					if(!entry.compare(0, prefix.size(), prefix)) {
						addon=&(*addons)[i];
					}
				}
			}
		}
		
		selected=addon;
		updateActionButton();
		updateUpdateButton();
	}else if(name=="cmdBack"){
		saveInstalledAddons();
		setNextState(STATE_MENU);
	}else if(name=="cmdUpdate"){

		//처음에 에드온을 제거한 다음 다시 설치합니다.
		if(type.compare("levels")==0) {	
			if(downloadFile(selected->file,(getUserPath(USER_DATA)+"/levels/"))!=false){
				selected->upToDate=true;
				selected->installedVersion=selected->version;
				list->item=addonsToList("levels");
				updateActionButton();
				updateUpdateButton();
			}else{
				cerr<<"ERROR: Unable to download addon!"<<endl;
				msgBox(_("ERROR: Unable to download addon!"),MsgBoxOKOnly,_("ERROR:"));
				return;
			}
		}else if(type.compare("levelpacks")==0) {
			if(!removeDirectory((getUserPath(USER_DATA)+"levelpacks/"+selected->folder+"/").c_str())){
				cerr<<"ERROR: Unable to remove the directory "<<(getUserPath(USER_DATA)+"levelpacks/"+selected->folder+"/")<<"."<<endl;
				return;
			}	
			if(downloadFile(selected->file,(getUserPath(USER_CACHE)+"/tmp/"))!=false){
				extractFile(getUserPath(USER_CACHE)+"/tmp/"+fileNameFromPath(selected->file,true),getUserPath(USER_DATA)+"/levelpacks/"+selected->folder+"/");
				selected->upToDate=true;
				selected->installedVersion=selected->version;
				list->item=addonsToList("levelpacks");
				updateActionButton();
				updateUpdateButton();
			}else{
				cerr<<"ERROR: Unable to download addon!"<<endl;
				msgBox(_("ERROR: Unable to download addon!"),MsgBoxOKOnly,_("ERROR:"));
				return;
			}
		}else if(type.compare("themes")==0) {
			if(!removeDirectory((getUserPath(USER_DATA)+"themes/"+selected->folder+"/").c_str())){
				cerr<<"ERROR: Unable to remove the directory "<<(getUserPath(USER_DATA)+"themes/"+selected->folder+"/")<<"."<<endl;
				return;
			}		
			if(downloadFile(selected->file,(getUserPath(USER_CACHE)+"/tmp/"))!=false){
				extractFile((getUserPath(USER_CACHE)+"/tmp/"+fileNameFromPath(selected->file,true)),(getUserPath(USER_DATA)+"/themes/"+selected->folder+"/"));
				selected->upToDate=true;
				selected->installedVersion=selected->version;
				list->item=addonsToList("themes");
				updateActionButton();
				updateUpdateButton();
			}else{
				cerr<<"ERROR: Unable to download addon!"<<endl;
				msgBox(_("ERROR: Unable to download addon!"),MsgBoxOKOnly,_("ERROR:"));
				return;
			}
		}
		    
	
	}else if(name=="cmdInstall"){
		switch(action) {
		  case NONE:
		    break;
		  case INSTALL:
			//에드온 다운
			if(type.compare("levels")==0) {
				if(downloadFile(selected->file,getUserPath(USER_DATA)+"/levels/")!=false){
					selected->upToDate=true;
					selected->installed=true;
					selected->installedVersion=selected->version;
					list->item=addonsToList("levels");
					updateActionButton();
					;
					
					//levelpack에 레벨을 추가 할 수 있습니다.
					LevelPack* levelsPack=getLevelPackManager()->getLevelPack("Levels");
					levelsPack->addLevel(getUserPath(USER_DATA)+"/levels/"+fileNameFromPath(selected->file));
					levelsPack->setLocked(levelsPack->getLevelCount()-1);
				}else{
					cerr<<"ERROR: Unable to download addon!"<<endl;
					msgBox(_("ERROR: Unable to download addon!"),MsgBoxOKOnly,_("ERROR:"));
					return;
				}
			}else if(type.compare("levelpacks")==0) {
				if(downloadFile(selected->file,getUserPath(USER_CACHE)+"/tmp/")!=false){
					extractFile(getUserPath(USER_CACHE)+"/tmp/"+fileNameFromPath(selected->file,true),getUserPath(USER_DATA)+"/levelpacks/"+selected->folder+"/");
					selected->upToDate=true;
					selected->installed=true;
					selected->installedVersion=selected->version;
					list->item=addonsToList("levelpacks");
					updateActionButton();
					updateUpdateButton();
					
					//그리고 levelpackManager에 levelpack를 추가합니다.
					getLevelPackManager()->loadLevelPack(getUserPath(USER_DATA)+"/levelpacks/"+selected->folder);
				}else{
					cerr<<"ERROR: Unable to download addon!"<<endl;
					msgBox(_("ERROR: Unable to download addon!"),MsgBoxOKOnly,_("ERROR:"));
					return;
				}
			}else if(type.compare("themes")==0) {
				if(downloadFile(selected->file,getUserPath(USER_CACHE)+"/tmp/")!=false){
					extractFile(getUserPath(USER_CACHE)+"/tmp/"+fileNameFromPath(selected->file,true),getUserPath(USER_DATA)+"/themes/"+selected->folder+"/");
					selected->upToDate=true;
					selected->installed=true;
					selected->installedVersion=selected->version;
					list->item=addonsToList("themes");
					updateActionButton();
					updateUpdateButton();
				}else{
					cerr<<"ERROR: Unable to download addon!"<<endl;
					msgBox(_("ERROR: Unable to download addon!"),MsgBoxOKOnly,_("ERROR:"));
					return;
				}
			}
		    break;
		  case UNINSTALL:
			//에드온 제거
			if(type.compare("levels")==0) {
				if(remove((getUserPath(USER_DATA)+"levels/"+fileNameFromPath(selected->file)).c_str())){
					cerr<<"ERROR: Unable to remove the file "<<(getUserPath(USER_DATA) + "levels/" + fileNameFromPath(selected->file))<<"."<<endl;
					return;
				}
				
				selected->upToDate=false;
				selected->installed=false;
				list->item=addonsToList("levels");
				updateActionButton();
				updateUpdateButton();
				
				//그리고 레벨 levelpack에서 레벨을 제거합니다.
				LevelPack* levelsPack=getLevelPackManager()->getLevelPack("Levels");
				for(int i=0;i<levelsPack->getLevelCount();i++){
					if(levelsPack->getLevelFile(i)==(getUserPath(USER_DATA)+"levels/"+fileNameFromPath(selected->file))){
						//레벨제거, 루프탈출 
						levelsPack->removeLevel(i);
						break;
					}
				}
			}else if(type.compare("levelpacks")==0) {
				if(!removeDirectory((getUserPath(USER_DATA)+"levelpacks/"+selected->folder+"/").c_str())){
					cerr<<"ERROR: Unable to remove the directory "<<(getUserPath(USER_DATA)+"levelpacks/"+selected->folder+"/")<<"."<<endl;
					return;
				}
				  
				selected->upToDate=false;
				selected->installed=false;
				list->item=addonsToList("levelpacks");
				updateActionButton();
				updateUpdateButton();
				
				//그리고 levelpack 관리자에서 levelpack를 제거합니다.
				getLevelPackManager()->removeLevelPack(selected->folder);
			}else if(type.compare("themes")==0) {
				if(!removeDirectory((getUserPath(USER_DATA)+"themes/"+selected->folder+"/").c_str())){
					cerr<<"ERROR: Unable to remove the directory "<<(getUserPath(USER_DATA)+"themes/"+selected->folder+"/")<<"."<<endl;
					return;
				}
				  
				selected->upToDate=false;
				selected->installed=false;
				list->item=addonsToList("themes");
				updateActionButton();
				updateUpdateButton();
			}
		    break;
		}
	}
}

void Addons::updateUpdateButton(){
	//some sanity check
	if(selected==NULL){
		updateButton->enabled=false;
		return;
	}

	//선택한 애드온이 설치되어 있는지 확인합니다.
	if(selected->installed){
		//설치되어 있지만 최신 상태인지 알아보는 부분
		if(selected->upToDate){
			//애드온가 설치되고 버튼을 표시 할 필요가 없다.
			updateButton->enabled=false;
			updateButton->visible=false;
		}else{
			//그렇지 않으면 버튼을 보이기
			updateButton->enabled=true;
			updateButton->visible=true;
		}
	}else{
		
		updateButton->enabled=false;
	}
}



void Addons::updateActionButton(){
	//some sanity check
	if(selected==NULL){
		actionButton->enabled=false;
		action = NONE;
		return;
	}

	//선택한 애드온이 설치되어 있는지 확인합니다.
	if(selected->installed){
		//설치되어 있지만을 최신 상태인지 알아보는 부분
		actionButton->enabled=true;
		actionButton->caption=_("Uninstall");
		action = UNINSTALL;
	}else{
		//The addon isn't installed so we can only install it.
		actionButton->enabled=true;
		actionButton->caption=_("Install");
		action = INSTALL;
	}
}
