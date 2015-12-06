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

#include "LevelPlaySelect.h"
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
using namespace std;

/////////////////////LEVEL SELECT/////////////////////
static string levelDescription,levelMedal2,levelMedal3,levelTime,levelRecs;
static string bestTimeFilePath,bestRecordingFilePath;

LevelPlaySelect::LevelPlaySelect():LevelSelect(_("Select Level")){
	//필요하다면 play bottons를 로드시킴
	playButtonImage=loadImage(getDataPath()+"gfx/playbutton.png");
	timeIcon=loadImage(getDataPath()+"gfx/time.png");
	recordingsIcon=loadImage(getDataPath()+"gfx/recordings.png");

	//gui를 생성시킴
	createGUI(true);

	//level list를 보여줌
	refresh();
}

LevelPlaySelect::~LevelPlaySelect(){
	play=NULL;
	recordingsIcon=NULL;
	timeIcon=NULL;
}

void LevelPlaySelect::createGUI(bool initial){
	//play button을 생성
	if(initial){
		play=new GUIObject(SCREEN_WIDTH-240,SCREEN_HEIGHT-60,240,32,GUIObjectButton,_("Play"));
	}else{
		play->left=SCREEN_WIDTH-240;
		play->top=SCREEN_HEIGHT-60;
	}
	play->name="cmdPlay";
	play->eventCallback=this;
	play->enabled=false;
	if(initial)
		GUIObjectRoot->childControls.push_back(play);
}

void LevelPlaySelect::refresh(bool change){
	int m=levels->getLevelCount();
	numbers.clear();

	//selected level을 clear함
	if(selectedNumber!=NULL){
		delete selectedNumber;
		selectedNumber=NULL;
	}
	//선택되지 않은 number을 재생성함.
	selectedNumber=new Number();
	SDL_Rect box={40,SCREEN_HEIGHT-130,50,50};
	selectedNumber->init(" ",box);
	selectedNumber->setLocked(true);

	levelDescription=_("Choose a level");
	levelMedal2=string(_("Time:"));
	levelMedal3=string(_("Recordings:"));
	levelTime=string("- / -");
	levelRecs=string("- / -");

	bestTimeFilePath.clear();
	bestRecordingFilePath.clear();

	//play button 비활성화
	play->enabled=false;

	for(int n=0; n<m; n++){
		numbers.push_back(Number());
	}

	for(int n=0; n<m; n++){
		SDL_Rect box={(n%LEVELS_PER_ROW)*64+(SCREEN_WIDTH*0.2)/2,(n/LEVELS_PER_ROW)*64+184,0,0};
		numbers[n].init(n,box);
		numbers[n].setLocked(levels->getLocked(n));
		int medal=levels->getLevel(n)->won;
		if(medal){
			if(levels->getLevel(n)->targetTime<0 || levels->getLevel(n)->time<=levels->getLevel(n)->targetTime)
				medal++;
			if(levels->getLevel(n)->targetRecordings<0 || levels->getLevel(n)->recordings<=levels->getLevel(n)->targetRecordings)
				medal++;
		}
		numbers[n].setMedal(medal);
	}

	if(m>LEVELS_DISPLAYED_IN_SCREEN){
		levelScrollBar->maxValue=(m-LEVELS_DISPLAYED_IN_SCREEN+(LEVELS_PER_ROW-1))/LEVELS_PER_ROW;
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

void LevelPlaySelect::selectNumber(unsigned int number,bool selected){
	if(selected){
		levels->setCurrentLevel(number);
		setNextState(STATE_GAME);

		//현재 music list 에서 music을 고른다.
		getMusicManager()->pickMusic();
	}else{
		displayLevelInfo(number);
	}
}

void LevelPlaySelect::checkMouse(){
	int x,y;

	//현재 마우스 좌표를 얻음
	SDL_GetMouseState(&x,&y);

	//record를 replay하는지 체크함.
	if(selectedNumber!=NULL){
		SDL_Rect mouse={x,y,0,0};
		if(!bestTimeFilePath.empty()){
			SDL_Rect box={SCREEN_WIDTH-420,SCREEN_HEIGHT-130,372,32};
			if(checkCollision(box,mouse)){
				Game::recordFile=bestTimeFilePath;
				levels->setCurrentLevel(selectedNumber->getNumber());
				setNextState(STATE_GAME);

				//현재 music list 에서 music을 고른다.
				getMusicManager()->pickMusic();
				return;
			}
		}
		if(!bestRecordingFilePath.empty()){
			SDL_Rect box={SCREEN_WIDTH-420,SCREEN_HEIGHT-98,372,32};
			if(checkCollision(box,mouse)){
				Game::recordFile=bestRecordingFilePath;
				levels->setCurrentLevel(selectedNumber->getNumber());
				setNextState(STATE_GAME);

				//현재 music list 에서 music을 고른다.
				getMusicManager()->pickMusic();
				return;
			}
		}
	}

	//super class로부터 온 기본함수를 qnfma
	LevelSelect::checkMouse();
}

void LevelPlaySelect::displayLevelInfo(int number){
	//현재 선택된 level을 update
	if(selectedNumber==NULL){
		selectedNumber=new Number();
	}
	SDL_Rect box={40,SCREEN_HEIGHT-130,50,50};
	selectedNumber->init(number,box);
	selectedNumber->setLocked(false);

	//level description을 보여준다.
	levelDescription=levels->getLevelName(number);

	//level medal을 보여준다.
	int medal=levels->getLevel(number)->won;
	int time=levels->getLevel(number)->time;
	int targetTime=levels->getLevel(number)->targetTime;
	int recordings=levels->getLevel(number)->recordings;
	int targetRecordings=levels->getLevel(number)->targetRecordings;

	if(medal){
		if(targetTime<0){
			medal=-1;
		}else{
			if(targetTime<0 || time<=targetTime)
				medal++;
			if(targetRecordings<0 || recordings<=targetRecordings)
				medal++;
		}
	}
	selectedNumber->setMedal(medal);

	//최고의 time 과 recordings을 보여줌.
	if(medal){
		char s[64];

		if(time>0)
			if(targetTime>0)
				sprintf(s,"%-.2fs / %-.2fs",time/40.0f,targetTime/40.0f);
			else
				sprintf(s,"%-.2fs / -",time/40.0f);
		else
			s[0]='\0';
		levelTime=string(s);

		if(recordings>=0)
			if(targetRecordings>=0)
				sprintf(s,"%5d / %d",recordings,targetRecordings);
			else
				sprintf(s,"%5d / -",recordings);
		else
			s[0]='\0';
		levelRecs=string(s);
	}else{
		levelTime=string("- / -");
		levelRecs=string("- / -");
	}

	//play button을 보여줌
	play->enabled=true;

	//자동적으로 file이 기록되었는지 확인.
	levels->getLevelAutoSaveRecordPath(number,bestTimeFilePath,bestRecordingFilePath,false);
	if(!bestTimeFilePath.empty()){
		FILE *f;
		f=fopen(bestTimeFilePath.c_str(),"rb");
		if(f==NULL){
			bestTimeFilePath.clear();
		}else{
			fclose(f);
		}
	}
	if(!bestRecordingFilePath.empty()){
		FILE *f;
		f=fopen(bestRecordingFilePath.c_str(),"rb");
		if(f==NULL){
			bestRecordingFilePath.clear();
		}else{
			fclose(f);
		}
	}
}

void LevelPlaySelect::render(){
	//levelselect 를 render 시킴
	LevelSelect::render();

	int x,y,dy=0,m=levels->getLevelCount();

	//현재 마우스 좌표를 얻음
	SDL_GetMouseState(&x,&y);

	if(levelScrollBar)
		dy=levelScrollBar->value;
	//Upper bound of levels we'd like to display.
	if(m>dy*LEVELS_PER_ROW+LEVELS_DISPLAYED_IN_SCREEN)
		m=dy*LEVELS_PER_ROW+LEVELS_DISPLAYED_IN_SCREEN;
	y+=dy*64;

	SDL_Rect mouse={x,y,0,0};

	//현재 선택된 level을 보여줌 (어떤 것이라도)
	if(selectedNumber!=NULL){
		selectedNumber->show(0);

		SDL_Color fg={0,0,0};
		SDL_Surface* bm;

		if(!levelDescription.empty()){
			bm=TTF_RenderUTF8_Blended(fontText,_C(levels->getDictionaryManager(),levelDescription.c_str()),fg);
			applySurface(100,SCREEN_HEIGHT-130+(50-bm->h)/2,bm,screen,NULL);
			SDL_FreeSurface(bm);
		}

		//오직 level이 완료(이김)되야 repaly를 보여줌
		if(selectedNumber->getNumber()>=0 && selectedNumber->getNumber()<levels->getLevelCount()) {
			if(levels->getLevel(selectedNumber->getNumber())->won){
				if(!bestTimeFilePath.empty()){
					SDL_Rect r={0,0,32,32};
					SDL_Rect box={SCREEN_WIDTH-420,SCREEN_HEIGHT-130,372,32};

					if(checkCollision(box,mouse)){
						r.x=32;
						SDL_FillRect(screen,&box,0xFFCCCCCC);
					}

					applySurface(SCREEN_WIDTH-80,SCREEN_HEIGHT-130,playButtonImage,screen,&r);
				}

				if(!bestRecordingFilePath.empty()){
					SDL_Rect r={0,0,32,32};
					SDL_Rect box={SCREEN_WIDTH-420,SCREEN_HEIGHT-98,372,32};

					if(checkCollision(box,mouse)){
						r.x=32;
						SDL_FillRect(screen,&box,0xFFCCCCCC);
					}

					applySurface(SCREEN_WIDTH-80,SCREEN_HEIGHT-98,playButtonImage,screen,&r);
				}
			}
		}

		if(!levelMedal2.empty()){
			//icon을 그림
			applySurface(SCREEN_WIDTH-405,SCREEN_HEIGHT-130+3,timeIcon,screen,NULL);

			//text (title)을 draw 시킴
			bm=TTF_RenderUTF8_Blended(fontText,levelMedal2.c_str(),fg);
			applySurface(SCREEN_WIDTH-380,SCREEN_HEIGHT-130+3,bm,screen,NULL);
			SDL_FreeSurface(bm);

			//두번째 text (value)을 draw 시킴
			bm=TTF_RenderUTF8_Blended(fontText,levelTime.c_str(),fg);
			applySurface(SCREEN_WIDTH-bm->w-80,SCREEN_HEIGHT-130+3,bm,screen,NULL);
			SDL_FreeSurface(bm);
		}

		if(!levelMedal3.empty()){
			//icon을 그림
			applySurface(SCREEN_WIDTH-405,SCREEN_HEIGHT-98+(6)/2,recordingsIcon,screen,NULL);

			//text (title)을 draw 시킴
			bm=TTF_RenderUTF8_Blended(fontText,levelMedal3.c_str(),fg);
			applySurface(SCREEN_WIDTH-380,SCREEN_HEIGHT-98+(32-bm->h)/2,bm,screen,NULL);
			SDL_FreeSurface(bm);

			//두번째 text (value)을 draw 시킴
			bm=TTF_RenderUTF8_Blended(fontText,levelRecs.c_str(),fg);
			applySurface(SCREEN_WIDTH-bm->w-80,SCREEN_HEIGHT-98+(32-bm->h)/2,bm,screen,NULL);
			SDL_FreeSurface(bm);
		}
	}
}

void LevelPlaySelect::renderTooltip(unsigned int number,int dy){
	SDL_Color fg={0,0,0};
	char s[64];

	// level의 name을 render 시킴
	SDL_Surface* name=TTF_RenderUTF8_Blended(fontText,_C(levels->getDictionaryManager(),levels->getLevelName(number)),fg);
	SDL_Surface* time=NULL;
	SDL_Surface* recordings=NULL;

	//걸린 시간.
	if(levels->getLevel(number)->time>0){
		sprintf(s,"%-.2fs",levels->getLevel(number)->time/40.0f);
		time=TTF_RenderUTF8_Blended(fontText,s,fg);
	}

	//사용한 recordings키의 개수
	if(levels->getLevel(number)->recordings>=0){
		sprintf(s,"%d",levels->getLevel(number)->recordings);
		recordings=TTF_RenderUTF8_Blended(fontText,s,fg);
	}


	//세개의  texts를 조함한 크기만큼 사각형을 draw한다.
	SDL_Rect r=numbers[number].box;
	r.y-=dy*64;
	if(time!=NULL && recordings!=NULL){
		r.w=(name->w)>(25+time->w+40+recordings->w)?(name->w):(25+time->w+40+recordings->w);
		r.h=name->h+5+20;
	}else{
		r.w=name->w;
		r.h=name->h;
	}

	//tooltip이 윈도우 창 밖으로 가지 않음을 확실히 한다.
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

	//그릴 위치를 계산 한다.
	SDL_Rect r2=r;

	//Now we render the name if the surface isn't null.
	//surface가 null이 아니라면 name을 render한다.
	if(name!=NULL){
		//name을 그린다.
		SDL_BlitSurface(name,NULL,screen,&r2);
	}
	//hight를 증가시킨다 name과 stats 사이의 간격을 줄이기 위해서
	r2.y+=30;
	if(time!=NULL){
		//Now draw the time.
		applySurface(r2.x,r2.y,timeIcon,screen,NULL);
		r2.x+=25;
		SDL_BlitSurface(time,NULL,screen,&r2);
		r2.x+=time->w+15;
	}
	if(recordings!=NULL){
		//recordings을 draw 시킴
		applySurface(r2.x,r2.y,recordingsIcon,screen,NULL);
		r2.x+=25;
		SDL_BlitSurface(recordings,NULL,screen,&r2);
	}

	//surfaces를 free시켜줌
	SDL_FreeSurface(name);
	SDL_FreeSurface(time);
	SDL_FreeSurface(recordings);
}

void LevelPlaySelect::resize(){
	//Let the LevelSelect do his stuff.
	LevelSelect::resize();

	//gui를 다시 그림
	createGUI(false);
}

void LevelPlaySelect::GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType){
	//level select 에 관련된 gui 이벤트를 다룸.
	LevelSelect::GUIEventCallback_OnEvent(name,obj,eventType);

	//playbutton을 체크함.
	if(name=="cmdPlay"){
		if(selectedNumber!=NULL){
			levels->setCurrentLevel(selectedNumber->getNumber());
			setNextState(STATE_GAME);

			//현재 music list에서 music을 고름.
			getMusicManager()->pickMusic();
		}
	}
}
