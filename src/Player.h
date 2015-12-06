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

#ifndef PLAYER_H
#define PLAYER_H

#include "ThemeManager.h"
#include <vector>
#include <string>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

// 게임 기록파일을 디버그한다.

class GameObject;
class Game;

// 다른 player의 버튼
// 오른쪽 화살표
const int PlayerButtonRight=0x01;
// 왼쪽 화살표
const int PlayerButtonLeft=0x02;
// 점프를 위한 위로 화살표
const int PlayerButtonJump=0x04;
// 움직임을 위한 아래화살표
const int PlayerButtonDown=0x08;
// 녹화을 위핸 스페이스바
const int PlayerButtonSpace=0x10;

class Player{
protected:
	// 벡터는 player의 움직임을 저장하는데 사용된다. 녹화할 때
	// 이건 그림자에게 줄수있게 하고 그는 정확히 그들을 할 수 있다.
	std::vector<int> playerButton;

private:
	// 벡터는 모든 게임의 플레이를 기록하는데 사용한다.
	// 그리고 체크토인트를 기록해서 저장한다.
	std::vector<int> recordButton,savedRecordButton;

	// 기록 인덱스 : -1은 키보드의 입력을 읽어들이는 것을 의미한다.
	// 다른의미로 녹화한 입력을 읽어들인다.
	int recordIndex;

	// 벡터는 포함한다. 네모들의 길을 따라서 player는 가진다 녹화할때
	// 그것은 네모의 흔적들로 그려진다.
	std::vector<SDL_Rect> line;

	// 만약 player가 그의 움직임을 복사한 그림자를 호출하면 boolean형
	bool shadowCall;
	// 만약 player가 그의 움직임을 녹화하면 boolean형
	bool record;

	// 따라오는 변수들은 상태에 저장한다.
	// 사각형은 player의 위치를 저장한다.
	SDL_Rect boxSaved;
	// 만약 player들이 공중에 있으면 boolean형
	bool inAirSaved;
	// 만약 player들ㅇ 점프하면 boolean
	bool isJumpSaved;
	// 만약 player가 땅위에 있으면 boolean
	bool onGroundSaved;
	// 만약 player들이 움직일 수 있으면 boolean
	bool canMoveSaved;
	// 만약 player가 잡은게 다른거면 boolean
	bool holdingOtherSaved;
	// x는 속도이다.
	int xVelSaved;
	// y는 속도이다.
	int yVelSaved;

protected:
	// 사각형은 plyer의 위치는 포함한다.
	SDL_Rect box;

	// x와 y의 위치
	int xVel, yVel;
	// 기본 x와 y의 위치, 움직이는 블록위에 서있는것으로 사용된다.
	int xVelBase, yVelBase;

	// player가 점프한때 소리가 실행된다.
	Mix_Chunk* jumpSound;
	// player가 죽을 때 소리가 실행된다.
	Mix_Chunk* hitSound;
	// 상태가 저장될 때 소리가 실행된다.
	Mix_Chunk* saveSound;
	// player들이 바뀔 때 소리가 실행된다.
	Mix_Chunk* swapSound;
	// player가 버튼이 눌릴 때 소리가 실행된다.
	Mix_Chunk* toggleSound;
	// player들이 무언가를 시도하고 그게 실행안되면 소리가 난다.
	Mix_Chunk* errorSound;
	// player들이 들어서 모으면 소리가 난다.
	Mix_Chunk* collectSound;

	// 만약 공중에 palyer들이 있으면 boolean
	bool inAir;
	// 만약 player들이 점프하면 boolean
	bool isJump;
	// 만약 player들이 땅위에 있으면 boolean
	bool onGround;
	// 만약 player들이 움직일 수 있으면 boolean
	bool canMove;
	// 만약 player들이 움직이면 boolean
	bool dead;

	// player들이 직진으로 걸으면 0=right, 1=left
	int direction;
	// player의 상태에 저장되면 정수형이 포함한다.
	int state;
	// player가 공중에 있는 시간
	int jumpTime;
	// 만약 player들이 그림자인지 사실 boolean
	bool shadow;

	// 게임 상태를가르키는 포인터
	friend class Game;
	Game* objParent;

	// 만약 아래키가 눌리면 boolean
	bool downKeyPressed;
	// 만약 스페이스키가 눌리면 boolean
	bool spaceKeyPressed;
	// 객체를 가르기고 플레이어 옆에 서있다.
	// 이것은 항상 유효한 포인터
	GameObject* objCurrentStand;
	// 플레이어가 서있을때 객체를 가르킨다.
	GameObject* objLastStand;
	// 플레이어가 마지막으로 탄 텔레포트를 가르킨다.
	GameObject* objLastTeleport;
	// 플레이어가 앞에있는 알림창을 가르킨다.
	// 이것은 항상 유효한 값이다.
	GameObject* objNotificationBlock;
	// 플레이어가 앞에있으면 그림자 블록을 가르킨다.
	// 이것은 항상 유효한 값이다.
	GameObject* objShadowBlock;

public:

	// x와 y는 플레이어가 시작하고 리셋됐을때 있는곳에 위치한다.
	int fx, fy;
	// 플레이어가 나타난다.
	ThemeCharacterInstance appearance;
	// 만약 플레이어가 다른것을 잡으면 boolean
	bool holdingOther;


	// 생성자
	// objParent : 게임상태를 가르킨다.
	Player(Game* objParent);
	// 생성자 파괴
	~Player();

	// 플레이어의 상태를 설정하는데 사용하는 메소드
	// x:플레이어가 위치한 새로운 x
	// 플레이어가 위치한 새로운 y
	void setPosition(int x,int y);

	// 입력을 다루든데 사용하는 메소드
	// shadow :  그림자는 녹화나 부르는데 포인터사용
	void handleInput(class Shadow* shadow);
	// 플레이어의 움직임을 하는데 사용하는 메소드
	// levelObjects : LevelObject를 포함한 배열 충돌을 체크하는데 사용
	void move(std::vector<GameObject*> &levelObjects);
	//  만약 플레이어가 점프할수있고 점프가 확실하다면 체크하는데 사용하는 메소드
	void jump();

	// 이 메소드는 플레이어가 화면에 나오도록 만든다
	void show();
	// 만약 플레이어가 녹화하면 그 행동을 저장하는 메소드
	void shadowSetState();

	// 상태를 0으로 하는 메소드
	virtual void stateReset();

	// 만약 그들이 서로 서있으면 플레이어를 반대해서각각 보여지게하는 메소드
	// OTHER : 그림자나 플레이어
	void otherCheck(class Player* other);

	// 카메라를 용이하게 해서 플레이어가 가운데 있게 하는 메소드
	void setMyCamera();
	// 이것은 플레이어를 시작 위치로 리셋시키는 메소드
	// SAVE : 만약 저장한 상태가 지워지면 BOOLEAN
	void reset(bool save);
	// 플레이어의 현재위치는 되찾아오는데 사용하는 메소드
	// returns : SDL_Rect 은 플레이어의 현재 위치를 포함한다.
	SDL_Rect getBox();


	void shadowGiveState(class Shadow* shadow);

	// 현재 상태를 저장하는 메소드
	virtual void saveState();
	// 마지막 저장한 상태를 되찾아오는 메소드
	// 만약 거기 아무것도 없으면 플레이어를 리셋한다.
	virtual void loadState();
	// 만약 플레이어가 저장가능한 상태이면 체크하는 메소드
	// returns : 만약 플레이어가 그의 상태를 저장할 수 있으면 참
	virtual bool canSaveState();
	// 만약 플레이어가 상태를 불러올 수 있으면 체크하는 메소드
	// RETURNS : 	만약 플레이거가 상태를 불러올 수 있으면 참
	virtual bool canLoadState();
	// 플레이어와 다른것의 상태를 바꾸는 메소드
	// other : 플레이어나 그림자
	void swapState(Player* other);

	// 만약 이 플레이어가 그림자안에 있으면 체크
	// returns : 만약  이게 그림자면 참이다.
	inline bool isShadow(){
		return shadow;
	}

	// 만약 플레이어가 떨어지거나 그가 때리면 플에이어가 죽게한다.
	// animation : 만약 죽는 화면이 실행되면 boolean .
	void die(bool animation=true);

	// 만약 현재 녹음파일로부터 실행되면 체크
	bool isPlayFromRecord();

	// 녹음객체로부터 헤임을 한다.
	std::vector<int>* getRecord();

#ifdef RECORD_FILE_DEBUG
	std::string& keyPressLog();
	std::vector<SDL_Rect>& playerPosition();
#endif

	// 녹음을 실행한다.
	void playRecord();

private:
	// 스페이스 키가 눌리면 handleInput함수와 또다른 함수를 불러온다.
	void spaceKeyDown(class Shadow* shadow);
};

#endif
