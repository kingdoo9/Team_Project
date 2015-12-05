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
	//Boolean if the player is in the air.
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
	//Pointer to the Game state.
	friend class Game;
	Game* objParent;

	//Boolean if the downkey is pressed.
	bool downKeyPressed;
	//Boolean if the space keu is pressed.
	bool spaceKeyPressed;
	//Pointer to the object that is currently been stand on by the player.
	//This is always a valid pointer.
	GameObject* objCurrentStand;
	//Pointer to the object the player stood last on.
	//NOTE: This is a weak reference only.
	GameObject* objLastStand;
	//Pointer to the teleporter the player last took.
	//NOTE: This is a weak reference only.
	GameObject* objLastTeleport;
	//Pointer to the notification block the player is in front of.
	//This is always a valid pointer.
	GameObject* objNotificationBlock;
	//Pointer to the shadow block the player is in front of.
	//This is always a valid pointer.
	GameObject* objShadowBlock;

public:

	//X and y location where the player starts and gets when reseted.
	int fx, fy;
	//The appearance of the player.
	ThemeCharacterInstance appearance;
	//Boolean if the player is holding the other.
	bool holdingOther;

	//Constructor.
	//objParent: Pointer to the Game state.
	Player(Game* objParent);
	//Destructor.
	~Player();

	//Method used to set the position of the player.
	//x: The new x location of the player.
	//y: The new y location of the player.
	void setPosition(int x,int y);

	//Method used to handle (key) input.
	//shadow: Pointer to the shadow used for recording/calling.
	void handleInput(class Shadow* shadow);
	//Method used to do the movement of the player.
	//levelObjects: Array containing the levelObjects, used to check collision.
	void move(std::vector<GameObject*> &levelObjects);
	//Method used to check if the player can jump and executes the jump.
	void jump();

	//This method will render the player to the screen.
	void show();
	//Method that stores the actions if the player is recording.
	void shadowSetState();

	//Method that will reset the state to 0.
	virtual void stateReset();

	//This method checks the player against the other to see if they stand on eachother.
	//other: The shadow or the player.
	void otherCheck(class Player* other);

	//Method that will ease the camera so that the player is in the center.
	void setMyCamera();
	//This method will reset the player to it's initial position.
	//save: Boolean if the saved state should also be deleted.
	void reset(bool save);
	//Method used to retrieve the current location of the player.
	//Returns: SDL_Rect containing the player's location.
	SDL_Rect getBox();

	//This method will
	void shadowGiveState(class Shadow* shadow);

	//Method that will save the current state.
	//NOTE: The special <name>Saved variables will be used.
	virtual void saveState();
	//Method that will retrieve the last saved state.
	//If there is none it will reset the player.
	virtual void loadState();
	//Method that checks if the player can save the state.
	//Returns: True if the player can save his state.
	virtual bool canSaveState();
	//Method that checks if the player can load a state.
	//Returns: True if the player can load a state.
	virtual bool canLoadState();
	//Method that will swap the state of the player with the other.
	//other: The player or the shadow.
	void swapState(Player* other);

	//Check if this player is in fact the shadow.
	//Returns: True if this is the shadow.
	inline bool isShadow(){
		return shadow;
	}

	//Let the player die when he falls of or hits spikes.
	//animation: Boolean if the death animation should be played, default is true.
	void die(bool animation=true);

	//Check if currently it's play from record file.
	bool isPlayFromRecord();

	//get the game record object.
	std::vector<int>* getRecord();

#ifdef RECORD_FILE_DEBUG
	std::string& keyPressLog();
	std::vector<SDL_Rect>& playerPosition();
#endif

	//play the record.
	void playRecord();

private:
	//The space key is down. call this function from handleInput and another function.
	void spaceKeyDown(class Shadow* shadow);
};

#endif
