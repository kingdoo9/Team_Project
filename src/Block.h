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

#ifndef BLOCK_H
#define BLOCK_H

#include "GameObjects.h"
#include "Globals.h"
#include "ThemeManager.h"
#include <vector>
#include <SDL/SDL.h>

class Game;

class Block: public GameObject{
private:
	//블록의 모양.
	ThemeBlockInstance appearance;
		
	//블록 사물의 모든 종류에 사용 할 수있는 정수.
	int temp;
	//블록의 상태가 저장 될 때 온도에 대한 저장변수.
	int tempSave;
	
	//flags:
	//객체 0x1로 이동 = 비활성화
	//button bit0-1=behavior 0x4=pressed
	//switch bit0-1=behavior
	int flags;
	//블록의 상태가 저장 될 때 플래그에 대해 저장.
	int flagsSave;

	//다음 블록을 이동합니다.
	//블록을 이동하기위한 출발점.
	SDL_Rect boxBase;
	//이동 블록의 위치를 포함하는 벡터.
	std::vector<SDL_Rect> movingPos;
	int dx;
	int xSave;
	int dy;
	int ySave;
	//이동 블록이 그의 움직임을 반복하는 경우 부울.
	//기본값은 True
	bool loop;
	
	// 에디터 블록의 깃발.
	// 이동 객체 0x1 = 비활성화
	// 포털 0x1 = 자동
	// fragile = state
	int editorFlags;
public:
	//블록의 ID
	std::string id;
	//포털의 대상의 ID를 포함하는 문자열
	std::string destination;
	//통지 메시지의 블록을 포함하는 문자열.
	std::string message;
	
	//Constructor.
	//x: The x location of the block.
	//y: The y location of the block.
	//objParent: Pointer to the Game object.
	Block(int x,int y,int type,Game* objParent);
	//Desturctor.
	~Block();

	//블록을 그리는 데 사용하는 방법
	void show();

	// 지정된 형태의 박스를 돌려줌
	// boxType : 반환되는 상자의 유형
	// 유형 GameObjects.h를 참조
	// 반환 값 : 상자.
	virtual SDL_Rect getBox(int boxType=BoxType_Current);
	
	// 메소드 블록의 위치를 설정하는데 사용.
	// 이건 베이스 박스 x 및 y 위치를 설정한다.
	//x: The new x location.
	//y: The new y location.
	virtual void setPosition(int x,int y);
	
	// 나중에 로드 할 수있는 블록의 상태를 저장
	virtual void saveState();
	// 블록의 저장된 상태를로드
	virtual void loadState();
	//블록을 재설정
	//save: Boolean if the saved state should also be deleted.
	virtual void reset(bool save);
	
	//Play an animation.
	//flags: TODO???
	virtual void playAnimation(int flags);
	
	// 이벤트가있을 때 호출되는 메서드.
	 // EVENTTYPE : 이벤트 유형
	 // eventtypes에 대한 GameObjects.h를 참조
	virtual void onEvent(int eventType);
	
	// 블록의 속성을 검색하는 데 사용되는 방법
	 // propertyType : 요청 된 속성의 유형
	 // GameObjects.h를 참조하십시오.
	 // OBJ : 플레이어의 포인터.
	 // 반환 값 : 속성 값을 포함하는 정수.
	virtual int queryProperties(int propertyType,Player* obj);

	// 블록의 에디터 데이터를 가져옴
 // OBJ : editorData으로 채워지는 벡터
	virtual void getEditorData(std::vector<std::pair<std::string,std::string> >& obj);

	//블록 편집기 데이터를 설정 
 // OBJ : 새로운 에디터 데이터.
	virtual void setEditorData(std::map<std::string,std::string>& obj);
	
	//이동 블록 또는 블록의 요소를 업데이트하는 방법
	virtual void move();
};

#endif
