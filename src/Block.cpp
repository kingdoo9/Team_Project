/* 60142233 강민경

 * Copyright (C) 2011-2012 Me and My Shadow
 *
 * This file is part of Me and My Shadow.
 *
 * Me and My Shadow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Me And My Shadow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Me and My Shadow.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GameObjects.h"
#include "Game.h"
#include "Player.h"
#include "Block.h"
#include "Functions.h"
#include "Globals.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

Block::Block(int x,int y,int type,Game* parent):
	GameObject(parent),
	temp(0),
	tempSave(0),
	flags(0),
	flagsSave(0),
	dx(0),
	xSave(0),
	dy(0),
	ySave(0),
	loop(true),
	editorFlags(0)
{
	// 첫 번째 상자의 위치와 크기를 설정합니다.
	// 기본 크기는 50 × 50이다.
	box.x=x;
	box.y=y;
	box.w=50;
	box.h=50;

	//설정
	boxBase.x=x;
	boxBase.y=y;

	//타입설정
	this->type=type;

	//일부 유형의 특정 코드를 필요로함
	if(type==TYPE_START_PLAYER){
		//여기에 플레이어를 설정, 시작 플레이어입니다.
	// 우리는 플레이어 중심, 플레이어 23px 넓이
		parent->player.setPosition(box.x+(box.w-23)/2,box.y);
		parent->player.fx=box.x+(box.w-23)/2;
		parent->player.fy=box.y;
	}else if(type==TYPE_START_SHADOW){
		//여기에 그림자를 설정, 시작 그림자입니다.
		// 우리는 그림자를 중심으로, 그림자 23px 넓이
		parent->shadow.setPosition(box.x+(box.w-23)/2,box.y);
		parent->shadow.fx=box.x+(box.w-23)/2;
		parent->shadow.fy=box.y;
	}

	//그리고 모양을 로드합니다.
	objThemes.getBlock(type)->createInstance(&appearance);
}

Block::~Block(){}

void Block::show(){
	//블록이 표시되어 있는지 확인합니다.
	if(checkCollision(camera,box)==true || (stateID==STATE_LEVEL_EDITOR && checkCollision(camera,boxBase)==true)){
		SDL_Rect r={0,0,50,50};

		//그릴 필요가 있는것은 블록의 종류에 따라 다름
		switch(type){
		case TYPE_CHECKPOINT:
			//체크 포인트가 마지막으로 사용되어 있는지 확인합니다.
			if(parent!=NULL && parent->objLastCheckPoint==this){
				if(!temp) appearance.changeState("activated");
				temp=1;
			}else{
				if(temp) appearance.changeState("default");
				temp=0;
			}
			break;
		case TYPE_CONVEYOR_BELT:
		case TYPE_SHADOW_CONVEYOR_BELT:
			if(temp){
				r.x=50-temp;
				r.w=temp;
				appearance.draw(screen,box.x-camera.x-50+temp,box.y-camera.y,&r);
				r.x=0;
				r.w=50-temp;
				appearance.draw(screen,box.x-camera.x+temp,box.y-camera.y,&r);
				return;
			}
			break;
		case TYPE_NOTIFICATION_BLOCK:
			if(message.empty()==false){
				appearance.draw(screen, box.x - camera.x, box.y - camera.y);
				return;
			}
			break;
		}

		//기본을 그립니다.
		appearance.drawState("base", screen, boxBase.x - camera.x, boxBase.y - camera.y);
		//normal을 그립니다
		appearance.draw(screen, box.x - camera.x, box.y - camera.y);

		//일부 유형은 상단에 base/default를 그릴 필요가 있습니다
		switch(type){
		case TYPE_BUTTON:
			if(flags&4){
				if(temp<5) temp++;
			}else{
				if(temp>0) temp--;
			}
			appearance.drawState("button",screen,box.x-camera.x,box.y-camera.y-5+temp);
			break;
		}
	}
}

SDL_Rect Block::getBox(int boxType){
	SDL_Rect r={0,0,0,0};
	switch(boxType){
	case BoxType_Base:
		return boxBase;
	case BoxType_Previous:
		switch(type){
		case TYPE_MOVING_BLOCK:
		case TYPE_MOVING_SHADOW_BLOCK:
		case TYPE_MOVING_SPIKES:
			r.x=box.x-dx;
			r.y=box.y-dy;
			r.w=box.w;
			r.h=box.h;
			return r;
		}
		return box;
	case BoxType_Delta:
		switch(type){
		case TYPE_MOVING_BLOCK:
		case TYPE_MOVING_SHADOW_BLOCK:
		case TYPE_MOVING_SPIKES:
			r.x=dx;
			r.y=dy;
			break;
		}
		return r;
	case BoxType_Velocity:
		switch(type){
		case TYPE_MOVING_BLOCK:
		case TYPE_MOVING_SHADOW_BLOCK:
		case TYPE_MOVING_SPIKES:
			r.x=dx;
			r.y=dy;
			break;
		case TYPE_CONVEYOR_BELT:
		case TYPE_SHADOW_CONVEYOR_BELT:
			r.x=(flags&1)?0:dx;
			break;
		}
		return r;
	case BoxType_Current:
		return box;
	}
	return r;
}

void Block::setPosition(int x,int y){
	box.x=x;
	box.y=y;
	boxBase.x=x;
	boxBase.y=y;
}

void Block::saveState(){
	tempSave=temp;
	flagsSave=flags;
	xSave=box.x-boxBase.x;
	ySave=box.y-boxBase.y;
	appearance.saveAnimation();
}

void Block::loadState(){
	temp=tempSave;
	flags=flagsSave;
	switch(type){
	case TYPE_MOVING_BLOCK:
	case TYPE_MOVING_SHADOW_BLOCK:
		box.x=boxBase.x+xSave;
		box.y=boxBase.y+ySave;
		break;
	}
	appearance.loadAnimation();
}

void Block::reset(bool save){
	//다시 할 필요가 있어서 temp를 지우고 저장합니다.
	if(save){
		temp=tempSave=xSave=ySave=0;
		flags=flagsSave=editorFlags;
	}else{
		temp=0;
		flags=editorFlags;
	}

	switch(type){
	case TYPE_MOVING_BLOCK:
	case TYPE_MOVING_SHADOW_BLOCK:
	case TYPE_MOVING_SPIKES:
		box.x=boxBase.x;
		box.y=boxBase.y;
		break;
	}

	//또한 모양을 다시 설정합니다.
	appearance.resetAnimation(save);
	appearance.changeState("default");

	//이 깨지기 쉬운 블록의 경우 우리는 모양을 업데이트 해야합니다.
	switch(type){
	case TYPE_FRAGILE:
		{
			const char* s=(flags==0)?"default":((flags==1)?"fragile1":((flags==2)?"fragile2":"fragile3"));
			appearance.changeState(s);
		}
		break;
	}
}


void Block::playAnimation(int flags){
	//TODO Why int flags????
	switch(type){
	case TYPE_SWAP:
		appearance.changeState("activated");
		break;
	case TYPE_SWITCH:
		temp^=1;
		appearance.changeState(temp?"activated":"default");
		break;
	}
}

void Block::onEvent(int eventType){
	//이벤트 핸들링
	switch(eventType){
	case GameObjectEvent_PlayerWalkOn:
		switch(type){
		case TYPE_FRAGILE:
			flags++;
			{
				const char* s=(flags==0)?"default":((flags==1)?"fragile1":((flags==2)?"fragile2":"fragile3"));
				appearance.changeState(s);
			}
			break;
		}
		break;
	case GameObjectEvent_PlayerIsOn:
		switch(type){
		case TYPE_BUTTON:
			dx=1;
			break;
		}
		break;
	case GameObjectEvent_OnToggle:
		switch(type){
		case TYPE_MOVING_BLOCK:
		case TYPE_MOVING_SHADOW_BLOCK:
		case TYPE_MOVING_SPIKES:
		case TYPE_CONVEYOR_BELT:
		case TYPE_SHADOW_CONVEYOR_BELT:
			flags^=1;
			break;
		case TYPE_PORTAL:
			appearance.changeState("activated");
			break;
        case TYPE_COLLECTABLE:
			appearance.changeState("inactive");
			flags=1;
            break;
		}
		break;
	case GameObjectEvent_OnSwitchOn:
		switch(type){
		case TYPE_MOVING_BLOCK:
		case TYPE_MOVING_SHADOW_BLOCK:
		case TYPE_MOVING_SPIKES:
		case TYPE_CONVEYOR_BELT:
		case TYPE_SHADOW_CONVEYOR_BELT:
			flags&=~1;
			break;
		case TYPE_EXIT:
			appearance.changeState("default");
			break;
		}
		break;
	case GameObjectEvent_OnSwitchOff:
		switch(type){
		case TYPE_MOVING_BLOCK:
		case TYPE_MOVING_SHADOW_BLOCK:
		case TYPE_MOVING_SPIKES:
		case TYPE_CONVEYOR_BELT:
		case TYPE_SHADOW_CONVEYOR_BELT:
			flags|=1;
			break;
		case TYPE_EXIT:
			appearance.changeState("closed");
			break;
		}
		break;
	}
}

int Block::queryProperties(int propertyType,Player* obj){
	switch(propertyType){
	case GameObjectProperty_PlayerCanWalkOn:
		switch(type){
		case TYPE_BLOCK:
		case TYPE_MOVING_BLOCK:
		case TYPE_CONVEYOR_BELT:
		case TYPE_BUTTON:
			return 1;
		case TYPE_SHADOW_BLOCK:
		case TYPE_MOVING_SHADOW_BLOCK:
		case TYPE_SHADOW_CONVEYOR_BELT:
			if(obj!=NULL && obj->isShadow()) return 1;
			break;
		case TYPE_FRAGILE:
			if(flags<3) return 1;
			break;
		}
		break;
	case GameObjectProperty_IsSpikes:
		switch(type){
		case TYPE_SPIKES:
		case TYPE_MOVING_SPIKES:
			return 1;
		}
		break;
	case GameObjectProperty_Flags:
		return flags;
		break;
	default:
		break;
	}
	return 0;
}

void Block::getEditorData(std::vector<std::pair<std::string,std::string> >& obj){
	//모든 블록은 ID가 있다
	obj.push_back(pair<string,string>("id",id));

	//특정 속성을 차단합니다.
	switch(type){
	case TYPE_MOVING_BLOCK:
	case TYPE_MOVING_SHADOW_BLOCK:
	case TYPE_MOVING_SPIKES:
		{
			char s[64],s0[64];
			sprintf(s,"%d",(int)movingPos.size());
			obj.push_back(pair<string,string>("MovingPosCount",s));
			obj.push_back(pair<string,string>("disabled",(editorFlags&0x1)?"1":"0"));
			obj.push_back(pair<string,string>("loop",loop?"1":"0"));
			for(unsigned int i=0;i<movingPos.size();i++){
				sprintf(s0+1,"%d",i);
				sprintf(s,"%d",movingPos[i].x);
				s0[0]='x';
				obj.push_back(pair<string,string>(s0,s));
				sprintf(s,"%d",movingPos[i].y);
				s0[0]='y';
				obj.push_back(pair<string,string>(s0,s));
				sprintf(s,"%d",movingPos[i].w);
				s0[0]='t';
				obj.push_back(pair<string,string>(s0,s));
			}
		}
		break;
	case TYPE_CONVEYOR_BELT:
	case TYPE_SHADOW_CONVEYOR_BELT:
		{
			char s[64];
			obj.push_back(pair<string,string>("disabled",(editorFlags&0x1)?"1":"0"));
			sprintf(s,"%d",dx);
			obj.push_back(pair<string,string>("speed",s));
		}
		break;
	case TYPE_PORTAL:
		obj.push_back(pair<string,string>("automatic",(editorFlags&0x1)?"1":"0"));
		obj.push_back(pair<string,string>("destination",destination));
		break;
	case TYPE_BUTTON:
	case TYPE_SWITCH:
		{
			string s;
			switch(editorFlags&0x3){
			case 1:
				s="on";
				break;
			case 2:
				s="off";
				break;
			default:
				s="toggle";
				break;
			}
			obj.push_back(pair<string,string>("behaviour",s));
		}
		break;
	case TYPE_NOTIFICATION_BLOCK:
		{
			string value=message;
			//Change \n with the characters '\n'.
			while(value.find('\n',0)!=string::npos){
				size_t pos=value.find('\n',0);
				value=value.replace(pos,1,"\\n");
			}

			obj.push_back(pair<string,string>("message",value));
		}
		break;
	case TYPE_FRAGILE:
		{
			char s[64];
			sprintf(s,"%d",editorFlags);
			obj.push_back(pair<string,string>("state",s));
		}
		break;
	}
}

void Block::setEditorData(std::map<std::string,std::string>& obj){
	//Iterator는 지도가 특정 항목이 포함되어 있는지 확인하는 데 사용됩니다.
	map<string,string>::iterator it;

	//데이터가 ID 블록이 포함되어 있는지 확인합니다.
	it=obj.find("id");
	if(it!=obj.end()){
		//블록의 ID를 설정합니다.
		id=obj["id"];
	}

	//특정 속성을 차단합니다.
	switch(type){
	case TYPE_MOVING_BLOCK:
	case TYPE_MOVING_SHADOW_BLOCK:
	case TYPE_MOVING_SPIKES:
		{
			//에디터 데이터가 MovingPosCount를 포함하는지 확인합니다.
			it=obj.find("MovingPosCount");
			if(it!=obj.end()){
				char s0[64];
				int m=0;
				m=atoi(obj["MovingPosCount"].c_str());
				movingPos.clear();
				for(int i=0;i<m;i++){
					SDL_Rect r={0,0,0,0};
					sprintf(s0+1,"%d",i);
					s0[0]='x';
					r.x=atoi(obj[s0].c_str());
					s0[0]='y';
					r.y=atoi(obj[s0].c_str());
					s0[0]='t';
					r.w=atoi(obj[s0].c_str());
					movingPos.push_back(r);
				}
			}

			//disable 키가 데이터에 있는지 확인합니다.
			it=obj.find("disabled");
			if(it!=obj.end()){
				string s=obj["disabled"];
				editorFlags=0;
				if(s=="true" || atoi(s.c_str())) editorFlags|=0x1;
				flags=flagsSave=editorFlags;
			}

			//루프 키가 데이터에 있는지 확인합니다.
			it=obj.find("loop");
			if(it!=obj.end()){
				string s=obj["loop"];
				loop=false;
				if(s=="true" || atoi(s.c_str()))
					loop=true;
			}

		}
		break;
	case TYPE_CONVEYOR_BELT:
	case TYPE_SHADOW_CONVEYOR_BELT:
		{
			//에디터 데이터 속도 키가 있는지 확인합니다.
			it=obj.find("speed");
			if(it!=obj.end()){
				dx=atoi(obj["speed"].c_str());
			}

			//disable 키가 데이터에 있는지 확인합니다.
			it=obj.find("disabled");
			if(it!=obj.end()){
				string s=obj["disabled"];
				editorFlags=0;
				if(s=="true" || atoi(s.c_str())) editorFlags|=0x1;
				flags=flagsSave=editorFlags;
			}
		}
		break;
	case TYPE_PORTAL:
		{
			//자동 키가 데이터에 있는지 확인합니다.
			it=obj.find("automatic");
			if(it!=obj.end()){
				string s=obj["automatic"];
				editorFlags=0;
				if(s=="true" || atoi(s.c_str())) editorFlags|=0x1;
				flags=flagsSave=editorFlags;
			}

			//destination 키가 데이터에 있는지 확인합니다.
			it=obj.find("destination");
			if(it!=obj.end()){
				destination=obj["destination"];
			}
		}
		break;
	case TYPE_BUTTON:
	case TYPE_SWITCH:
		{
			//behaviour 키가 데이터에 있는지 확인합니다.
			it=obj.find("behaviour");
			if(it!=obj.end()){
				string s=obj["behaviour"];
				editorFlags=0;
				if(s=="on" || s==_("On")) editorFlags|=1;
				else if(s=="off" || s==_("Off")) editorFlags|=2;
				flags=flagsSave=editorFlags;
			}
		}
		break;
	case TYPE_NOTIFICATION_BLOCK:
		{
			//메시지 키가 데이터에 있는지 확인합니다.
			it=obj.find("message");
			if(it!=obj.end()){
				message=obj["message"];
				//Change the characters '\n' to a real \n
				while(message.find("\\n")!=string::npos){
					message=message.replace(message.find("\\n"),2,"\n");
				}
			}
		}
		break;
	case TYPE_FRAGILE:
		{
			//state가 데이터에 있는지 확인합니다.
			it=obj.find("state");
			if(it!=obj.end()){
				editorFlags=atoi(obj["state"].c_str());
				flags=editorFlags;
				{
					const char* s=(flags==0)?"default":((flags==1)?"fragile1":((flags==2)?"fragile2":"fragile3"));
					appearance.changeState(s);
				}
			}
		}
	}
}

/*//debug
int block_test_count=-1;
bool block_test_only=false;*/

void Block::move(){
	appearance.updateAnimation();
	switch(type){
	case TYPE_MOVING_BLOCK:
	case TYPE_MOVING_SHADOW_BLOCK:
	case TYPE_MOVING_SPIKES:
		{
			/*//debug
			if(block_test_only || parent->time==416){
				cout<<"Time:"<<(parent->time)<<" Recorded:"<<block_test_count<<" Coord:"<<box.x<<","<<box.y<<endl;
				block_test_only=false;
			}*/

			if(!(flags&0x1)) temp++;
			int t=temp;
			SDL_Rect r0={0,0,0,0},r1;
			dx=0;
			dy=0;
			for(unsigned int i=0;i<movingPos.size();i++){
				r1.x=movingPos[i].x;
				r1.y=movingPos[i].y;
				r1.w=movingPos[i].w;
				if(t==0&&r1.w==0){
					r1.w=1;
					flags|=0x1;
				}
				if(t>=0 && t<(int)r1.w){
					int newX=boxBase.x+(int)(float(r0.x)+(float(r1.x)-float(r0.x))*float(t)/float(r1.w));
					int newY=boxBase.y+(int)(float(r0.y)+(float(r1.y)-float(r0.y))*float(t)/float(r1.w));
					dx=newX-box.x;
					dy=newY-box.y;
					box.x=newX;
					box.y=newY;
					return;
				}else if(t==(int)r1.w){
					// 시간 movingPosition의 시간이면 위치가 동일하게 설정.
					// 우리는 일반 블록과 이동 블록 사이에 약간의 가장자리를 방지하기 위해 이 작업을 수행.
					int newX=boxBase.x+r1.x;
					int newY=boxBase.y+r1.y;
					dx=newX-box.x;
					dy=newY-box.y;
					box.x=newX;
					box.y=newY;
					return;
				}
				t-=r1.w;
				r0.x=r1.x;
				r0.y=r1.y;
			}
			//반복하는 경우에만 물건을 다시 설정합니다.
			if(loop){
				temp=0;
				if(!movingPos.empty() && movingPos.back().x==0 && movingPos.back().y==0){
					dx=boxBase.x-box.x;
					dy=boxBase.y-box.y;
				}
				box.x=boxBase.x;
				box.y=boxBase.y;
			}
		}
		break;
	case TYPE_BUTTON:
		{
			int new_flags=dx?4:0;
			if((flags^new_flags)&4){
				flags=(flags&~4)|new_flags;
				if(parent && (new_flags || (flags&3)==0)){
					//ID가 비어 있지 않은지 확인
					if(!id.empty()){
						parent->broadcastObjectEvent(0x10000|(flags&3),-1,id.c_str());
					}else{
						cerr<<"Warning: invalid button id!"<<endl;
					}
				}
			}
			dx=0;
		}
		break;
	case TYPE_CONVEYOR_BELT:
	case TYPE_SHADOW_CONVEYOR_BELT:
		if((flags&1)==0){
			temp=(temp+dx)%50;
			if(temp<0) temp+=50;
		}
		break;
	}
}
