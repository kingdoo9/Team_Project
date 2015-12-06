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

#include "Functions.h"
#include "Timer.h"
#include "Objects.h"
#include "Globals.h"
#include "TitleMenu.h"
#include "GUIObject.h"
#include "InputManager.h"
#include "MD5.h"
#include <SDL/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#ifdef HARDWARE_ACCELERATION
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// 녹화를 위한 변수
//#define RECORD_PICUTRE_SEQUENCE
#ifdef RECORD_PICUTRE_SEQUENCE
bool recordPictureSequence=false;
int recordPictureIndex=0;
#endif

int main(int argc, char** argv) {
#ifdef _MSC_VER
	// 비쥬얼 스트디오 아래에있는 non-latin파일(이름이bug)을 고정시킨다.
	setlocale(LC_ALL,"");
#endif

	// command 라인의 논쟁을 첫번째로넘긴다.
	int s=parseArguments(argc,argv);
	if(s==-1){
		printf("Usage: %s [OPTIONS] ...\n",argv[0]);
		printf("%s","Available options:\n");
		printf("    %-5s%-30s  %s\n","","--data-dir <dir>","Specifies the data directory.");
		printf("    %-5s%-30s  %s\n","","--user-dir <dir>","Specifies the user preferences directory.");
		printf("    %-5s%-30s  %s\n","-f,","--fullscreen","Run the game fullscreen.");
		printf("    %-5s%-30s  %s\n","-w,","--windowed","Run the game windowed.");
		printf("    %-5s%-30s  %s\n","-mv,","--music <volume>","Set the music volume.");
		printf("    %-5s%-30s  %s\n","-sv,","--sound <volume>","Set the sound volume.");
		printf("    %-5s%-30s  %s\n","-s,","--set <setting> <value>","Change a setting to a given value.");
		printf("    %-5s%-30s  %s\n","-v,","--version","Display the version and quit.");
		printf("    %-5s%-30s  %s\n","-h,","--help","Display this help message.");
		return 0;
	}else if(s==0){
		return 0;
	}

	// dataPath, userPath 등등의 환경을 설정한다.
	if(configurePaths()==false){
		fprintf(stderr,"FATAL ERROR: Failed to configure paths.\n");
		return 1;
	}
	// 설정을 불러온다.
	if(loadSettings()==false){
		fprintf(stderr,"FATAL ERROR: Failed to load config file.\n");
		return 1;
	}

	// SDL, 윈도우, SDL_Mixer 같은것을 초기화한다.
	if(init()==false) {
		fprintf(stderr,"FATAL ERROR: Failed to initalize game.\n");
		return 1;
	}
	// 배경음악이나 default테마같은 중요한 파일들을 불러온다.
	if(loadFiles()==false){
		fprintf(stderr,"FATAL ERROR: Failed to load necessary files.\n");
		return 1;
	}

	// 메인메뉴에 현재 상태 id를 설정하고 그걸 생성한다.
	stateID=STATE_MENU;
	currentState=new Menu();

	// 랜덤 seed
	srand((unsigned)time(NULL));

	// 0에 fadeIn 값을 설정한다.
	int fadeIn=0;

	// 마지막으로 사시즈 재정의된 사건을 저장하고 이건 불러오는 한가지이다.
	SDL_Event lastResize={};

	// 게임 순환을 시작한다.
	while(stateID!=STATE_EXIT){
		// 타이머를 시작한다.
		FPS.start();

		// sdl 사건을 loop
		while(SDL_PollEvent(&event)){
			// 만약 사용자가 윈도우를 재사이즈정의하면 체크한다.
			if(event.type==SDL_VIDEORESIZE){
				lastResize=event;


				continue;
			}

			// 만약 꽉찬화면 키가 살짝 눌리면 체크
			if(event.type==SDL_KEYUP && event.key.keysym.sym==SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT)){
				getSettings()->setValue("fullscreen",getSettings()->getBoolValue("fullscreen")?"0":"1");

				// 새로운 화면생셩이 필요하다.
				if(!createScreen()){
					//Screen creation failed so set to safe settings.
					getSettings()->setValue("fullscreen","0");
					getSettings()->setValue("width","800");
					getSettings()->setValue("height","600");

					// 안전한 설정으로 시도한다.
					if(!createScreen()){
						// 모든것이 실패하면 그만둔다.
						setNextState(STATE_EXIT);
						cerr<<"ERROR: Unable to create screen."<<endl;
					}
				}

				// 화면이 생성되면 테마를 불러온다.
				//The screen is created, now load the (menu) theme.
				if(!loadTheme()){
					// 실패하거나 중지되면 테ㅏ를 불러온다.
					setNextState(STATE_EXIT);
					cerr<<"ERROR: Unable to load theme after toggling fullscreen."<<endl;
				}

				continue;
			}

#ifdef RECORD_PICUTRE_SEQUENCE
			if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_F10){
				recordPictureSequence=!recordPictureSequence;
				printf("Record Picture Sequence %s\n",recordPictureSequence?"ON":"OFF");
			}
#endif
			// 입력 관리자 다루는 이벤트를 시켜라
			inputMgr.updateState(true);
			// 현재상태를 다루는 이벤트
			currentState->handleEvents();
			// 또한 GUI로 이벤트를 넘겨라
			GUIObjectHandleEvents();
		}

		// ㅅ다시 사이즈 정의하는 사건의 과정
		if(lastResize.type==SDL_VIDEORESIZE){
			event=lastResize;
			onVideoResize();

			// 크기를 다시 정의한 후 이벤트 타입을 지워라
			lastResize.type=SDL_NOEVENT;
		}

		if(nextState!=STATE_NULL){
			fadeIn=17;
			changeState();
		}
		if(stateID==STATE_EXIT) break;

		// 입력상태를 업데이트
		inputMgr.updateState(false);
		// 이제 그의 논리대로 행동할 시간이다.
		currentState->logic();

		currentState->render();
		if(GUIObjectRoot) GUIObjectRoot->render();
		if(fadeIn>0&&fadeIn<255){
			SDL_BlitSurface(screen,NULL,tempSurface,NULL);
			SDL_FillRect(screen,NULL,0);
			SDL_SetAlpha(tempSurface, SDL_SRCALPHA, fadeIn);
			SDL_BlitSurface(tempSurface,NULL,screen,NULL);
			fadeIn+=17;
		}
#ifdef RECORD_PICUTRE_SEQUENCE
		if(recordPictureSequence){
			char s[64];
			recordPictureIndex++;
			sprintf(s,"pic%08d.bmp",recordPictureIndex);
			printf("Save screen to %s\n",s);
			SDL_SaveBMP(screen,(getUserPath(USER_CACHE)+s).c_str());
		}
#endif
	// 그리고 사실적일 화면이로 표면 화면을 그려라
		flipScreen();

		// 만약 다음상태가 설정되면 체크해라  우리는 희미해지고 상태가 바뀔거라는 의미이다.
		if(nextState!=STATE_NULL){
			fadeIn=17;
			changeState();
		}

		// 이제 계싼해라 우리가 얼마나 기다려야하는지 프레임
		int t=FPS.getTicks();
		t=(1000/g_FPS)-t;
		if(t>0){
			SDL_Delay(t);
		}
	}

	// 게임이 끝나면 설정을 저장한다.
	saveSettings();

	// 모든것을 청소한다.
	clean();

	// 프로그램을 많이끝낸다.
	return 0;
}
