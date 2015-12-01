// 신수빈
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
#include "FileManager.h"
#include "Game.h"
#include "Player.h"
#include "Shadow.h"
#include <vector>
#include <iostream>
using namespace std;

Shadow::Shadow(Game* objParent):Player(objParent){
	// 초기설정의 대부분은 player의 생성자에서 발생한다.
	// 우리는 몇가지 그림자의 특별한 옵션을 설정할 것 이다.
	called=false;
	shadow=true;
}

void Shadow::moveLogic(){

	// 만약 우리가 호출당하고 거기 여전히 움직임이 남겨진다 우리가 움직이는게 글로
	if(called && state < (signed)playerButton.size()){
		int currentKey=playerButton[state];

		xVel=0;
		// 만약 현재 움직임이 걷는거면 체크한다.
		if(currentKey & PlayerButtonRight) xVel=7;
		if(currentKey & PlayerButtonLeft) xVel=-7;

		// 만약 현재 움직임이 뛰는거면 체크한다.
		if((currentKey & PlayerButtonJump) && !inAir){
			isJump=true;
		}else{
			isJump=false;
		}

		// 만약 현재 움직임이 액션이라면 체크한다.
		if(currentKey & PlayerButtonDown){
			downKeyPressed=true;
		}else{
			downKeyPressed=false;
		}

		// 우리는 움직인다. 다음으로 움직인다.
		state++;
	}else{
		// 우리는 움직임을 내달린다 그래서 리셋된다.
		called=false;
		state=0;
		xVel=0;
	}
}

void Shadow::meCall(){
	called=true;
}

void Shadow::stateReset(){
	state=0;
	called=false;
}

void Shadow::loadState(){
	Player::loadState();
	called=false;
	playerButton.clear();
}
