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
// 안지호 수정
#include "GUIObject.h"
#include <iostream>
#include <list>
using namespace std;

//Set the GUIObjectRoot to NULL.
GUIObject* GUIObjectRoot=NULL;
//Initialise the event queue.
list<GUIEvent> GUIEventQueue;


void GUIObjectHandleEvents(bool kill){
	//Check if user resizes the window.(예외처리)
	if(event.type==SDL_VIDEORESIZE){
		//onVideoResize();

		//Don't let other objects process this event (?)
		return;
	}

	//Make sure that GUIObjectRoot isn't null.
	if(GUIObjectRoot)
		GUIObjectRoot->handleEvents();

	//Check for SDL_QUIT.(예외처리)
	if(event.type==SDL_QUIT && kill){
		//We get a quit event so enter the exit state.
		setNextState(STATE_EXIT);
		delete GUIObjectRoot;
		GUIObjectRoot=NULL;
		return;
	}

	//Keep calling events until there are none left.(GUI이벤트목록에 이벤트가 없을 때까지 수행)
	while(!GUIEventQueue.empty()){
		//Get one event and remove it from the queue.
		GUIEvent e=GUIEventQueue.front();
		GUIEventQueue.pop_front();

		//If an eventCallback exist call it.
		if(e.eventCallback){
			e.eventCallback->GUIEventCallback_OnEvent(e.name,e.obj,e.eventType);
		}
	}
	//We empty the event queue just to be sure.(목록 클리어)
	GUIEventQueue.clear();
}

GUIObject::~GUIObject(){
	if(cache){
		SDL_FreeSurface(cache);
		cache=NULL;
	}
	//We need to delete every child we have.
	for(unsigned int i=0;i<childControls.size();i++){
		delete childControls[i];
	}
	//Deleted the childs now empty the childControls vector.
	childControls.clear();
}

bool GUIObject::handleEvents(int x,int y,bool enabled,bool visible,bool processed){
	//Boolean if the event is processed.(GUI 오브젝트 내에 이벤트를 처리해주는 함수)
	bool b=processed;

	//The GUIObject is only enabled when he and his parent are enabled.
	enabled=enabled && this->enabled;
	//The GUIObject is only enabled when he and his parent are enabled.
	visible=visible && this->visible;

	//Get the absolute position.
	x+=left-gravityX;
	y+=top;

	//Type specific event handling.
	switch(type){
	case GUIObjectButton:
		//Set state to 0.
		state=0;

		//Only check for events when the object is both enabled and visible.(오브젝트가 사용가능하고 보일때만 수행)
		if(enabled&&visible){
			//The mouse location (x=i, y=j) and the mouse button (k).
			int i,j,k;
			k=SDL_GetMouseState(&i,&j);

			//Check if the mouse is inside the GUIObject.(마우스가 GUI오브젝트로 들어왔을때)
			if(i>=x&&i<x+width&&j>=y&&j<y+height){
				//We have hover so set state to one.
				state=1;
				//Check for a mouse button press.(마우스가 클릭됬는지 확인)
				if(k&SDL_BUTTON(1))
					state=2;

				//Check if there's a mouse press and the event hasn't been already processed.(마우스는 클릭 했지만 이벤트가 수행되지 않았을때)
				if(event.type==SDL_MOUSEBUTTONUP && event.button.button==SDL_BUTTON_LEFT && !b){
					//If event callback is configured then add an event to the queue.(이벤트를 이벤트목록에 추가한다)
					if(eventCallback){
						GUIEvent e={eventCallback,name,this,GUIEventClick};
						GUIEventQueue.push_back(e);
					}

					//Event has been processed.(이벤트가 수행되었을때)
					b=true;
				}
			}
		}
		break;
	case GUIObjectCheckBox: //체크박스 GUI일때
		//Set state to 0.
		state=0;

		//Only check for events when the object is both enabled and visible.(오브젝트가 사용가능하고 보일때만 수행)
		if(enabled&&visible){
			//The mouse location (x=i, y=j) and the mouse button (k).
			int i,j,k;
			k=SDL_GetMouseState(&i,&j);

			//Check if the mouse is inside the GUIObject.(마우스가 들어왔을 때)
			if(i>=x&&i<x+width&&j>=y&&j<y+height){
				//We have hover so set state to one.
				state=1;
				//Check for a mouse button press.(버튼을 클릭했을 때)
				if(k&SDL_BUTTON(1))
					state=2;

				//Check if there's a mouse press and the event hasn't been already processed.
				if(event.type==SDL_MOUSEBUTTONUP && event.button.button==SDL_BUTTON_LEFT && !b){
					//It's a checkbox so toggle the value.(value를 확인한다.)
					value=value?0:1;

					//If event callback is configured then add an event to the queue.(목록에 추가)
					if(eventCallback){
						GUIEvent e={eventCallback,name,this,GUIEventClick};
						GUIEventQueue.push_back(e);
					}

					//Event has been processed.
					b=true;
				}
			}
		}
		break;
	case GUIObjectTextBox: //(텍스트박스 GUI일때)
		//NOTE: We don't reset the state to have a "focus" effect.

		//Only check for events when the object is both enabled and visible.(보이는지 사용가능한지 확인)
		if(enabled&&visible){
			//Check if there's a key press and the event hasn't been already processed. //키입력이 들어왔을때
			if(state==2 && event.type==SDL_KEYDOWN && !b){
				//Get the keycode.
				int key=(int)event.key.keysym.unicode;

				//Check if the key is supported.(키가 지원되는지 확인)
				if(key>=32&&key<=126){
					//Add the key to the text after the carrot.
					caption.insert((size_t)value,1,char(key));
					value=clamp(value+1,0,caption.length());

					//If there is an event callback then call it.
					if(eventCallback){
						GUIEvent e={eventCallback,name,this,GUIEventChange};
						GUIEventQueue.push_back(e);
					}
				}else if(event.key.keysym.sym==SDLK_BACKSPACE){
					//We need to remove a character so first make sure that there is text.(백스페이스를 눌렀을때 글자를 지움)
					if(caption.length()>0&&value>0){
						//Remove the character before the carrot.
						value=clamp(value-1,0,caption.length());
						caption.erase((size_t)value,1);

						//If there is an event callback then call it.
						if(eventCallback){
							GUIEvent e={eventCallback,name,this,GUIEventChange};
							GUIEventQueue.push_back(e);
						}
					}
				}else if(event.key.keysym.sym==SDLK_DELETE){
					//We need to remove a character so first make sure that there is text.(딜리트 또한 글자를 지움)
					if(caption.length()>0){
						//Remove the character after the carrot.
						value=clamp(value,0,caption.length());
						caption.erase((size_t)value,1);

						//If there is an event callback then call it.(콜백)
						if(eventCallback){
							GUIEvent e={eventCallback,name,this,GUIEventChange};
							GUIEventQueue.push_back(e);
						}
					}
				}else if(event.key.keysym.sym==SDLK_RIGHT){//커서 이동 오른쪽
					value=clamp(value+1,0,caption.length());
				}else if(event.key.keysym.sym==SDLK_LEFT){//커서 이동 왼쪽
					value=clamp(value-1,0,caption.length());
				}

				//The event has been processed.
				b=true;
			}

			//The mouse location (x=i, y=j) and the mouse button (k).
			int i,j,k;
			k=SDL_GetMouseState(&i,&j);

			//Check if the mouse is inside the GUIObject.(마우스가 안쪽으로 들어왔는지 체크)
			if(i>=x&&i<x+width&&j>=y&&j<y+height){
				//We can only increase our state. (nothing->hover->focus).
				if(state!=2){
					state=1;// 상태를 1로 바꿈
				}

				//Check for a mouse button press.
				if(k&SDL_BUTTON(1)){
					//We have focus. 클릭한후 상태
					state=2;
					//TODO Move carrot to place clicked
					value=caption.length();
				}
			}else{
				//The mouse is outside the TextBox.
				//If we don't have focus but only hover we lose it.
				if(state==1){
					state=0;
				}

				//If it's a click event outside the textbox then we blur.(텍스트 박스 밖을 클릭했을때)
				if(event.type==SDL_MOUSEBUTTONUP && event.button.button==SDL_BUTTON_LEFT){
					//Set state to 0.
					state=0;
				}
			}
		}
		break;
	}

	//Also let the children handle their events.
	for(unsigned int i=0;i<childControls.size();i++){
		bool b1=childControls[i]->handleEvents(x,y,enabled,visible,b);

		//The event is processed when either our or the childs is true (or both).
		b=b||b1;
	}
	return b;
}

void GUIObject::render(int x,int y,bool draw){
	//Rectangle the size of the GUIObject, used to draw borders.(드로우 보더를 사용하여 GUI오브젝트를 네모 모양으로 사이즈를 조정하여 만든다.)
	SDL_Rect r;

	//There's no need drawing the GUIObject when it's invisible.(보이지 않을 때는 그리지 않는다.)
	if(!visible)
		return;

	//Get the absolute x and y location.(가로 세로 위치를 정한다.)
	x+=left;
	y+=top;

	//Check if the enabled state changed or the caption, if so we need to clear the (old) cache.
	if(enabled!=cachedEnabled || caption.compare(cachedCaption)!=0 || width<=0){
		//Free the cache.(캐쉬를 프리시킨다.)
		SDL_FreeSurface(cache);
		cache=NULL;

		//And cache the new values.(새로운 값을 넣는다.)
		cachedEnabled=enabled;
		cachedCaption=caption;

		//Finally resize the widget(마지막으로 위젯 사이즈를 재조정한다.)
		if(autoWidth)
			width=-1;
	}


	//Now do the type specific rendering.(이제 렌더링 시작)
	switch(type){
	case GUIObjectLabel://라벨일때
		{
			//The rectangle is simple.
			r.x=x;
			r.y=y;
			r.w=width;
			r.h=height;

			//We don't draw a background and/or border since that label is transparent.
			//Get the caption and make sure it isn't empty.
			const char* lp=caption.c_str();
			if(lp!=NULL && lp[0]){
				//Render the text using the small font.
				if(cache==NULL){
					//Color the text will be: black.
					SDL_Color black={0,0,0,0};
					cache=TTF_RenderUTF8_Blended(fontText,lp,black);

					if(width<=0)
						width=cache->w;
				}

				if(draw){
				  	if(gravity==GUIGravityCenter)
						gravityX=(width-cache->w)/2;
					else if(gravity==GUIGravityRight)
						gravityX=width+cache->w;
					else
						gravityX=0;

					//Center the text vertically and draw it to the screen.(스크린에 그려준다.)
					r.y=y+(height - cache->h)/2;
					r.x+=gravityX;
					SDL_BlitSurface(cache,NULL,screen,&r);
				}
			}
		}
		break;
	case GUIObjectCheckBox://체크 박스일때
		{
			//The rectangle is simple.
			r.x=x;
			r.y=y;
			r.w=width;
			r.h=height;

			//Get the text.
			const char* lp=caption.c_str();
			//Make sure it isn't empty.
			if(lp!=NULL && lp[0]){
				//We render black text.(빈 텍스트를 만든다.)
				if(!cache){
					SDL_Color black={0,0,0,0};
					cache=TTF_RenderUTF8_Blended(fontText,lp,black);
				}

				if(draw){
					//Calculate the location, center it vertically.
					r.x=x;
					r.y=y+(height - cache->h)/2;

					//Draw the text and free the surface.(텍스트를 그려준다.)
					SDL_BlitSurface(cache,NULL,screen,&r);
				}
			}

			if(draw){
				//Draw the check (or not).(체크를 만든다.)
				SDL_Rect r1={0,0,16,16};
				if(value==1||value==2)
					r1.x=value*16;
				r.x=x+width-20;
				r.y=y+(height-16)/2;
				SDL_BlitSurface(bmGUI,&r1,screen,&r);
			}
		}
		break;
	case GUIObjectButton://버튼일 때
		{
			//Get the text.
			const char* lp=caption.c_str();
			//Make sure the text isn't empty.
			if(lp!=NULL && lp[0]){
				if(!cache){
					//Draw black text.
					SDL_Color black={0,0,0,0};
					//Draw in gray when disabled. 이용불가능할때는 회색으로 만든다.
					if(!enabled)
						black.r=black.g=black.b=96;

					if(!smallFont)
						cache=TTF_RenderUTF8_Blended(fontGUI,lp,black);
					else
						cache=TTF_RenderUTF8_Blended(fontGUISmall,lp,black);

					if(width<=0){
						width=cache->w+50;
						if(gravity==GUIGravityCenter){
							gravityX=int(width/2);
						}else if(gravity==GUIGravityRight){
							gravityX=width;
						}else{
							gravityX=0;
						}
					}
				}

				if(draw){
					//Center the text both vertically as horizontally.(텍스트 추가)
					r.x=x-gravityX+(width-cache->w)/2;
					r.y=y+(height-cache->h)/2-GUI_FONT_RAISE;

					//Check if the arrows don't fall of.
					if(cache->w+32<=width){
						//Create a rectangle that selects the right image from bmGUI,(이미지 파일 추가)
						SDL_Rect r2={64,0,16,16};
						if(state==1){
							applySurface(x-gravityX+(width-cache->w)/2-25,y+(height-cache->h)/2+((cache->h-16)/2),bmGUI,screen,&r2);
							r2.x-=16;
							applySurface(x-gravityX+(width-cache->w)/2+4+cache->w+5,y+(height-cache->h)/2+((cache->h-16)/2),bmGUI,screen,&r2);
						}else if(state==2){
							applySurface(x-gravityX+(width-cache->w)/2-20,y+(height-cache->h)/2+((cache->h-16)/2),bmGUI,screen,&r2);
							r2.x-=16;
							applySurface(x-gravityX+(width-cache->w)/2+4+cache->w,y+(height-cache->h)/2+((cache->h-16)/2),bmGUI,screen,&r2);
						}
					}

					//Draw the text and free the surface.
					SDL_BlitSurface(cache,NULL,screen,&r);
				}
			}
		}
		break;
	case GUIObjectTextBox://텍스트 일때
		{
			if(draw){
				//Default background opacity
				int clr=50;
				//If hovering or focused make background more visible.
				if(state==1)
					clr=128;
				else if (state==2)
					clr=100;

				//Draw the box.박스를 그린다.
				Uint32 color=0xFFFFFF00|clr;
				drawGUIBox(x,y,width,height,screen,color);
			}

			//Get the text.
			const char* lp=caption.c_str();
			//Make sure it isn't empty.
			if(lp!=NULL && lp[0]){
				if(!cache){
					//Draw the black text. 텍스트 만든다.
					SDL_Color black={0,0,0,0};
					cache=TTF_RenderUTF8_Blended(fontText,lp,black);
				}

				if(draw){
					//Calculate the location, center it vertically.위치를 계산한다.
					r.x=x+2;
					r.y=y+(height - cache->h)/2;

					//Draw the text.
					SDL_Rect tmp={0,0,width-2,25};
					SDL_BlitSurface(cache,&tmp,screen,&r);
					//Only draw the carrot when focus.
					if(state==2){
						r.x=x;
						r.y=y+4;
						r.w=2;
						r.h=height-8;

						int advance;
						for(int n=0;n<value;n++){
							TTF_GlyphMetrics(fontText,caption[n],NULL,NULL,NULL,NULL,&advance);
							r.x+=advance;
						}

						//Make sure that the carrot is inside the textbox.
						if(r.x<x+width)
							SDL_FillRect(screen,&r,0);
					}
				}
			}else{
				//Only draw the carrot when focus.
				if(state==2&&draw){
					r.x=x+4;
					r.y=y+4;
					r.w=2;
					r.h=height-8;
					SDL_FillRect(screen,&r,0);
				}
			}
		}
		break;
	case GUIObjectFrame:// 프레임 일때
		{
			if(draw){
				//Create a rectangle the size of the button and fill it.
				Uint32 color=0xDDDDDDFF;
				drawGUIBox(x,y,width,height,screen,color);
			}

			//Get the title text.
			const char* lp=caption.c_str();
			//Make sure it isn't empty.
			if(lp!=NULL && lp[0]){
				if(!cache){
					//The colors black and white used to render the title with white background.
					SDL_Color black={0,0,0,0};
					cache=TTF_RenderUTF8_Blended(fontGUI,lp,black);
				}

				if(draw){
					//Calculate the location, center horizontally and vertically relative to the top.
					r.x=x+(width-cache->w)/2;
					r.y=y+6-GUI_FONT_RAISE;

					//Draw the text and free the surface.
					SDL_BlitSurface(cache,NULL,screen,&r);
				}
			}
		}
		break;
	}

	//We now need to draw all the children of the GUIObject.
	for(unsigned int i=0;i<childControls.size();i++){
		childControls[i]->render(x,y,draw);
	}
}
