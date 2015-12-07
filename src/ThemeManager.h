f/* 코드분석자 : 60142270 남채린
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

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include "Globals.h"
#include "TreeStorageNode.h"
#include <SDL/SDL_rotozoom.h>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
using namespace std;

//Structure containing offset data for one frame.
struct typeOffsetPoint{
	//The location (x,y) and size (w,h).
	int x,y,w,h;
	//The frame to which this offset applies.
	int frameCount;
	//The number of frames this offset is shown.
	int frameDisplayTime;
};

//We already need the classes so declare them here.
class ThemeOffsetData;
class ThemePicture;
class ThemeObject;
class ThemeBlockState;
class ThemeBlock;
class ThemeCharacterState;
class ThemeCharacter;


//Instance class of a ThemeObject, this is used by the other Instance classes.
// ThemeObject의 인스턴스 클래스로, 다른 인스턴스 클래스들에 의해 사용된다.
class ThemeObjectInstance{
public:
	//사진을 가리키는 포인터
	ThemePicture* picture;
	//Pointer to the parent the object an instance os is.
	ThemeObject* parent;
	
	//Integer containing the current animation frame. 현재 애니메이션 프레임을 포함하는 인티져
	int animation;
	//Integer containing the saved animation frame. 저장된 애니메이션 프레임을 포함하는 인티져 
	int savedAnimation;
public:
	// 생성자
	ThemeObjectInstance():picture(NULL),parent(NULL),animation(0),savedAnimation(0){}
	
	// TimeObject를 그리는 함수
	// dest : 목적지 표면이 ThemeObject를 그림  // 그 목적지가  ThemeObject의 표면을 편평하게 함
	//dest: The destination surface to draw the ThemeObject on.
	//x: dest 표면에 x 위치
	//y: dest 표면에 y 위치
	//clipRect: 자르는데 사용된 직사각형
	void draw(SDL_Surface* dest,int x,int y,SDL_Rect* clipRect=NULL);
	
	// 애니메이션을 업데이트 해주는 함수
	void updateAnimation();
	
	// 애니메이션을 리셋시켜주는 함수
	//save: 저장된 애니메이션이 삭제되는지 아닌지 Boolean type 으로 정함 
	void resetAnimation(bool save){
		animation=0;
		if(save){
			savedAnimation=0;
		}
	}
	// 애니메이션을 저장하는 함수
	void saveAnimation(){
		savedAnimation=animation;
	}
	// 저장된 애니메이션을 불러오는 함수
	void loadAnimation(){
		animation=savedAnimation;
	}
};

// ThemeBlockState 의 Instance class로, ThemeBlockInstance에 의해 만들어짐
class ThemeBlockStateInstance{
public:
	//Pointer to the parent the state an instance of is.
	ThemeBlockState *parent;
	//Vector containing the ThemeObjectInstances.
	vector<ThemeObjectInstance> objects;
	
	//Integer containing the current animation frame. 현재 애니메이션 프레임을 포함하는 인티져
	int animation;
	//Integer containing the saved animation frame. 저장된 애니메이션 프레임을 포함하는 인티져 
	int savedAnimation;
public:
	//생성자
	ThemeBlockStateInstance():parent(NULL),animation(0),savedAnimation(0){}
	
	//ThemeBlockState을 그리는 데 사용되는 함수 
	// dest : 목적지 표면이 ThemeBlockState를 그림  // 그 목적지가  ThemeBlockState의 표면을 편평하게 함
	//dest: The destination surface to draw the ThemeBlockState on.
	//x: dest 표면에 x 위치
	//y: dest 표면에 y 위치
	//clipRect: 자르는데 사용된 직사각형
	void draw(SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].draw(dest,x,y,clipRect);
		}
	}
	
	// 애니메이션을 업데이트 해주는 함수
	void updateAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].updateAnimation();
		}
		animation++;
	}
	// 애니메이션을 리셋시켜주는 함수
	//save: 저장된 애니메이션이 삭제되는지 아닌지 Boolean type 으로 정함 
	void resetAnimation(bool save){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].resetAnimation(save);
		}
		animation=0;
		if(save){
			savedAnimation=0;
		}
	}
	// 애니메이션을 저장하는 함수 
	void saveAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].saveAnimation();
		}
		savedAnimation=animation;
	}
	// 저장된 애니메이션을 불러오는 함수 
	void loadAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].loadAnimation();
		}
		animation=savedAnimation;
	}
};

// ThemeBlock의 인스턴스로, 게임 안의 블록들이 게임 안에서 테마를 바꾸는 것을 막게 해주는 데 사용된다. 
// 그것은 또한 애니메이션을 독립적으로 운영하게 한다.
class ThemeBlockInstance{
public:
	// 현재 상태의 포인터 
	ThemeBlockStateInstance* currentState;
	// 현재 상태의 이름 
	string currentStateName;
	
	// blockStates 을 포함하는 맵
	map<string,ThemeBlockStateInstance> blockStates;
	// 저장된 상태의 이름을 포함하는 String
	string savedStateName;
public:
	// 생성자 
	ThemeBlockInstance():currentState(NULL){}
	
	// ThemeBlock을 그리는 데 사용되는 함수 
	// dest : 목적지 표면이 ThemeBlock를 그림  // 그 목적지가  ThemeBlock의 표면을 편평하게 함
	//dest: The destination surface to draw the ThemeBlock on.
	//x: dest 표면에 x 위치
	//y: dest 표면에 y 위치
	//clipRect: 자르는데 사용된 직사각형
	//Returns: 만약 그것을 성공한다면 True 
	bool draw(SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		if(currentState!=NULL){
			currentState->draw(dest,x,y,clipRect);
			return true;
		}
		return false;
	}
	// 구체적인 상태를 그리는 함수 
	//s: 그릴 상태의 이름 
	// dest : 목적지 표면이 ThemeBlock를 그림  // 그 목적지가  ThemeBlock의 표면을 편평하게 함
	//dest: The destination surface to draw the ThemeBlock on.
	//x: dest 표면에 x 위치
	//y: dest 표면에 y 위치
	//clipRect: 자르는데 사용된 직사각형
	//Returns: 만약 그것을 성공한다면 True 
	bool drawState(const string& s,SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		map<string,ThemeBlockStateInstance>::iterator it=blockStates.find(s);
		if(it!=blockStates.end()){
			it->second.draw(dest,x,y,clipRect);
			return true;
		}
		return false;
	}

	// 현재상태를 바꾸는 함수 
	//s: 변화시킬 상태의 이름 
	//reset: Boolean if the animation should reset. 만약 애니메이션이 리셋되어야 한다면 // 애니메이션이 리셋되어야 하는지 아닌지 Boolean 사용 
	//Returns: 만약 그것을 성공한다면 True (exists 혹은 존재한다면).
	bool changeState(const string& s,bool reset=true){
		//Get the new state. 새로운 상태를 얻어라 
		map<string,ThemeBlockStateInstance>::iterator it=blockStates.find(s);
		//Check if it exists. 그것이 존재하는지 아닌지 확인하라
		if(it!=blockStates.end()){
			//Set the state. 상태 설정
			currentState=&(it->second);
			currentStateName=it->first;
			
			//FIXME: Is it needed to set the savedStateName here? savedStateName을 여기서 설정하는게 필요되어지니?
			if(savedStateName.empty())
				savedStateName=currentStateName;
			
			//If reset then reset the animation. 재설정할 경우 애니메이션을 다시 설정한다. 
			if(reset)
				currentState->resetAnimation(true);
			return true;
		}
		
		//It doesn't so return false. 그게 안되면 false를 반환하라 
		return false;
	}
	
	// 애니메이션을 업데이트 해주는 함수 
	void updateAnimation();
	// 애니메이션을 리셋시켜주는 함수 
	//save: Boolean if the saved state should be deleted. 저장된 상태가 삭제되어야하는지 마는지 Boolean으로 설정
	void resetAnimation(bool save){
		for(map<string,ThemeBlockStateInstance>::iterator it=blockStates.begin();it!=blockStates.end();++it){
			it->second.resetAnimation(save);
		}
		if(save){
			savedStateName.clear();
		}
	}
	// 애니메이션을 저장하는 함수 
	void saveAnimation(){
		for(map<string,ThemeBlockStateInstance>::iterator it=blockStates.begin();it!=blockStates.end();++it){
			it->second.saveAnimation();
		}
		savedStateName=currentStateName;
	}
	// 저장된 애니메이션을 복구하는 함수 
	void loadAnimation(){
		for(map<string,ThemeBlockStateInstance>::iterator it=blockStates.begin();it!=blockStates.end();++it){
			it->second.loadAnimation();
		}
		changeState(savedStateName,false);
	}
};

//Instance class of a ThemeCharacterState, this is used by the ThemeCharacterInstance.
// ThemeCharacterState의 인스턴스 클래스로, ThemeCharacterInstance에 의해 사용된다. 
class ThemeCharacterStateInstance{
public:
	//Pointer to the parent the state an instance of is.
	ThemeCharacterState* parent;
	//Vector containing the ThemeObjectInstances. ThemeObjectInstances를 포함하는 Vector 
	vector<ThemeObjectInstance> objects;
	
	//Integer containing the current animation frame. 현재 애니메이션 프래임을 포함하는 인티져
	int animation;
	//Integer containing the saved animation frame. 저장된 애니메이션 프래임을 포함하는 인티져 
	int savedAnimation;
public:
	// 생성자 
	ThemeCharacterStateInstance():parent(NULL),animation(0),savedAnimation(0){}
	
	// ThemeCharacterState를 그리는 데 사용되는 함수 
	// dest : 목적지 표면이 ThemeCharacterState를 그림  // 그 목적지가  ThemeCharacterState의 표면을 편평하게 함
	//dest: The destination surface to draw the ThemeCharacterState on.
	//x: dest 표면에 x 위치
	//y: dest 표면에 y 위치
	//clipRect: 자르는데 사용된 직사각형
	void draw(SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].draw(dest,x,y,clipRect);
		}
	}
	
	// 애니메이션을 업데이트 해주는 함수 
	void updateAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].updateAnimation();
		}
		animation++;
	}
	// 애니메이션을 리셋시켜주는 함수 
	//save: Boolean if the saved state should be deleted. 저장된 상태가 삭제되어야하는지 마는지 Boolean으로 설정
	void resetAnimation(bool save){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].resetAnimation(save);
		}
		animation=0;
		if(save)
			savedAnimation=0;
	}
	// 애니메이션을 저장하는 함수 
	void saveAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].saveAnimation();
		}
		savedAnimation=animation;
	}
	// 저장된 애니메이션을 불러오는 함수 
	void loadAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].loadAnimation();
		}
		animation=savedAnimation;
	}
};

// ThemeCharacter의 인스턴스 
class ThemeCharacterInstance{
public:
	//Pointer to the current state. 현재 상태를 가리키는 포인터
	ThemeCharacterStateInstance* currentState;
	//The name of the current state. 현재 상태를 가리키는 이름 
	string currentStateName;
	
	//Map containing the ThemeCharacterStates. ThemeCharacterStates를 포함하는 Map
	map<string,ThemeCharacterStateInstance> characterStates;
	//String containing the name of the saved state. 저장된 상태의 이름을 포함하는 String
	string savedStateName;
public:
	// 생성자 
	ThemeCharacterInstance():currentState(NULL){}
	
	// ThemeCharacter를 그리는 데 사용되는 함수 
	// dest : 목적지 표면이 ThemeCharacter를 그림  // 그 목적지가  ThemeCharacter의 표면을 편평하게 함
	//dest: The destination surface to draw the ThemeCharacterState on.
	//x: dest 표면에 x 위치
	//y: dest 표면에 y 위치
	//clipRect: 자르는데 사용된 직사각형
	//Returns: True if it succeeds. 만약 그것이 성공한다면 True 
	bool draw(SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		if(currentState!=NULL){
			currentState->draw(dest,x,y,clipRect);
			return true;
		}
		return false;
	}

	//Method that will draw a specific state. 구체적인 상태를 그려주는 함수 
	//s: The name of the state to draw. 그릴 상태의 이름 
	// dest : 목적지 표면이 ThemeCharacter를 그림  // 그 목적지가  ThemeCharacter의 표면을 편평하게 함
	//dest: The destination surface to draw the ThemeCharacterState on.
	//x: dest 표면에 x 위치
	//y: dest 표면에 y 위치
	//clipRect: 자르는데 사용된 직사각형
	//Returns: True if it succeeds. 만약 그것이 성공한다면 True 
	bool drawState(const string& s,SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		map<string,ThemeCharacterStateInstance>::iterator it=characterStates.find(s);
		if(it!=characterStates.end()){
			it->second.draw(dest,x,y,clipRect);
			return true;
		}
		return false;
	}
	
	//Method that will change the current state. 현재 상태를 변화시켜주는 함수 
	//s: The name of the state to change to. 변화시킬 상태의 이름 
	//reset: Boolean if the animation should reset. 애니메이션이 재설정되어야 하는지 아닌지 Boolean으로 설정 
	//Returns: True if it succeeds (exists). 만약 그것이 성공한다면 True (혹은 존재한다면)
	bool changeState(const string& s,bool reset=true){
		//Get the new state. 새로운 상태를 얻어옴
		map<string,ThemeCharacterStateInstance>::iterator it=characterStates.find(s);
		//Check if it exists. 그것이 존재하는지 확인
		if(it!=characterStates.end()){
			//Set the state. 상태 설정 
			currentState=&(it->second);
			currentStateName=it->first;
			
			//FIXME: Is it needed to set the savedStateName here? 여기에 savedStateName를 설정하는게 필요한가? 
			if(savedStateName.empty())
				savedStateName=currentStateName;
			
			//If reset then reset the animation. 만약 재설정한다면 애니메이션을 설정하시오.
			if(reset)
				currentState->resetAnimation(true);
			return true;
		}
		
		//It doesn't so return false. 그것이 아니면 false를 반환 
		return false;
	}
	
	// 애니메이션을 업데이트 해주는 함수
	void updateAnimation();
	// 애니메이션을 리셋해주는 함수 
	//save: Boolean if the saved state should be deleted. 저장된 상태가 삭제되어야 하는지 마는지 Boolean으로 설정
	void resetAnimation(bool save){
		for(map<string,ThemeCharacterStateInstance>::iterator it=characterStates.begin();it!=characterStates.end();++it){
			it->second.resetAnimation(save);
		}
		if(save)
			savedStateName.clear();
	}
	// 애니메이션을 저장시켜주는 함수 
	void saveAnimation(){
		for(map<string,ThemeCharacterStateInstance>::iterator it=characterStates.begin();it!=characterStates.end();++it){
			it->second.saveAnimation();
		}
		savedStateName=currentStateName;
	}
	// 저장된 애니메이션을 복구시켜주는 함수 
	void loadAnimation(){
		for(map<string,ThemeCharacterStateInstance>::iterator it=characterStates.begin();it!=characterStates.end();++it){
			it->second.loadAnimation();
		}
		changeState(savedStateName,false);
	}
};

// offset data를 포함하는 클래스 
class ThemeOffsetData{
public:
	//Vector containing the offsetDatas. offsetData들을 포함하는 벡터 
	vector<typeOffsetPoint> offsetData;
	//The length of the "animation" in frames. 프레임 안의 "애니메이션" 의 길이
	int length;
public:
	// 생성자
	ThemeOffsetData():length(0){}
	// 소멸자
	~ThemeOffsetData(){}
	
	//Method used to destroy the offsetData. offsetData를 파괴하는데 사용되는 함수 
	void destroy(){
		//Set length to zero. 0까지의 길이 설정 
		length=0;
		//And clear the offsetData vector. offsetData 벡터를 클리어 함 
		offsetData.clear();
	}
	
	//Method that will load the offsetData from a node. 노드로부터 offsetData를 불러오는 함수 
	//objNode: Pointer to the TreeStorageNode to read the data from. objNode로부터 데이터를 읽어주는 TreeStorageNode를 가리키는 포인터 
	//Returns: True if it succeeds without errors. 에러가 없이 성공한다면 True
	bool loadFromNode(TreeStorageNode* objNode);
};

//This is the lowest level of the theme system. 이것은 가장 낮은 수준의 테마 시스템이다. 
//It's a picture with offset data. 그것은 offset data를 가지고 있는 사진이다.
class ThemePicture{
public:
	//The SDL_Surface containing the picture. 사진을 포함하는 SDL_Surface
	SDL_Surface* picture;
	//Offset data for the picture. 사진을 위한 Offset data
	ThemeOffsetData offset;
public:
	// 생성자
	ThemePicture():picture(NULL){}
	// 소멸자 
	~ThemePicture(){}
	
	//Method used to destroy the picture. 사진을 파괴하는데 사용되는 함수 
	void destroy(){
		//FIXME: Shouldn't the image be freed? (ImageManager) 이미지를 해제하지 않을래? 
		picture=NULL;
		//Destroy the offset data. offset data를 파괴하라 
		offset.destroy();
	}
	bool loadFromNode(TreeStorageNode* objNode, string themePath);
	
	// ThemePicture를 그려주는 함수 
	//dest: 목적지 표면 
	//x: The x location on the dest to draw the picture. 사진을 그릴 dest 표면에 x 위치 
	//y: The y location on the dest to draw the picture. 사진을 그릴 dest 표면에 y 위치 
	//animation: The frame of the animation to draw. 그릴 애니메이션의 프래임 
	//clipRect: Rectangle to clip the picture. 사진을 자르는 직사각형 
	void draw(SDL_Surface* dest,int x,int y,int animation=0,SDL_Rect* clipRect=NULL);
};

//The ThemeObject class is used to contain a basic theme element. ThemeObject는 기본 테마 요소를 포함하는데 사용된다. 
//Contains the picture, animation information, etc... 사진, 애니메이션.... 등등을 포함한다. 
class ThemeObject{
public:
	//Integer containing the length of the animation. 애니메이션의 길이를 포함하는 인티져
	int animationLength;
	//Integer containing the frame from where the animation is going to loop. 
	// 애니메이션이 어디에서 루프를 돌 것인지 프래임을 포함하는 인티져 
	int animationLoopPoint;
	
	//Boolean if the animation is invisible at run time (Game state).
	// 애니메이션이 런타임에서 (혹은 게임 상태에서) 보이지 않는지 아닌지 
	bool invisibleAtRunTime;
	//Boolean if the animation is invisible at design time (Level editor).
	// 애니메이션이 설계한 시간 (혹은 레벨 편집) 에서 보이지 않는지 아닌지 
	bool invisibleAtDesignTime;
	
	//Picture of the ThemeObject. ThemeObject의 사진 
	ThemePicture picture;
	//Picture of the ThemeObject shown when in the level editor. 레벨에디터에 있을 때 보여지는 ThemeObject 사진 
	ThemePicture editorPicture;
	
	//Vector containing optionalPicture for the ThemeObject. ThemeObject를 위한 optionalPicture를 포함하는 벡터 
	vector<pair<double,ThemePicture*> > optionalPicture;
	
	//ThemeOffsetData for the ThemeObject. ThemeObject를 위한 ThemeOffsetData 
	ThemeOffsetData offset;
public:
	// 생성자 
	ThemeObject():animationLength(0),animationLoopPoint(0),invisibleAtRunTime(false),invisibleAtDesignTime(false){}
	// 소멸자 
	~ThemeObject(){
		//Loop through the optionalPicture and delete them. optionalPicture를 통해서 루프하고 그것들을 삭제하라 
		for(unsigned int i=0;i<optionalPicture.size();i++){
			delete optionalPicture[i].second;
		}
	}
	
	// ThemeObject를 파괴하는 함수 
	void destroy(){
		//Loop through the optionalPicture and delete them. optionalPicture를 통해서 루프하고 그것들을 삭제하라
		for(unsigned int i=0;i<optionalPicture.size();i++){
			delete optionalPicture[i].second;
		}
		optionalPicture.clear();
		animationLength=0;
		animationLoopPoint=0;
		invisibleAtRunTime=false;
		invisibleAtDesignTime=false;
		picture.destroy();
		editorPicture.destroy();
		offset.destroy();
	}
	
	// 노드로부터 ThemeObject를 불러오는 함수 
	//objNode: The TreeStorageNode to read the object from. objNode로부터 그 오브젝트를 읽는 TreeStorageNode
	//themePath: Path to the theme. theme로의 경로 
	//Returns: True if it succeeds. 만약 그것이 성공하면 True
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
};

//Class containing a single state of a themed block. 테마블록의 단일 상태를 포함하는 클래스 
class ThemeBlockState{
public:
	//The length in frames of the oneTimeAnimation. oneTimeAnimation의 프레임에서의 길이 
	int oneTimeAnimationLength;
	//String containing the name of the next state. 다음 상태의 이름을 포함하는 스트링 
	string nextState;
	//Vector containing the themeObjects that make up this state. 다음 상태를 구성하는 themeObjects를 포함하는 벡터
	vector<ThemeObject*> themeObjects;
public:
	// 생성자
	ThemeBlockState():oneTimeAnimationLength(0){}
	// 소멸자 
	~ThemeBlockState(){
		//Loop through the ThemeObjects and delete them. ThemeObjects를 통해서 루프하고 그것들을 삭제하라 
		for(unsigned int i=0;i<themeObjects.size();i++){
			delete themeObjects[i];
		}
	}
	
	// ThemeBlockState를 파괴하는 함수 
	void destroy(){
		//Loop through the ThemeObjects and delete them. ThemeObjects를 통해서 루프하고 그것들을 삭제하라 
		for(unsigned int i=0;i<themeObjects.size();i++){
			delete themeObjects[i];
		}
		//Clear the themeObjects vector.themeObjects 벡터를 클리어하라 
		themeObjects.clear();
		//Set the length to 0. 길이를 0으로 설정하라 
		oneTimeAnimationLength=0;
		//Clear the nextState string. 다음상태 스트링을 클리어하라 
		nextState.clear();
	}
	
	// 노드로부터 ThemeBlockState를 불러오는 함수 
	//objNode: The TreeStorageNode to read the state from. objNode로부터 상태를 읽는 TreeStorageNode
	//themePath: Path to the theme. 테마로의 경로 
	//Returns: True if it succeeds. 만약 그것이 성공한다면 True 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
};

//Class containing the needed things for a themed block. 테마블록을 위해 필요한 것들을 포함하는 클래스 
class ThemeBlock{
public:
	//Picture that is shown only in the level editor. 레벨 에디터에서만 보여지는 사진 
	ThemePicture editorPicture;
	
	//Map containing ThemeBlockStates for the different states of a block. 
	// 한 블록의 다른상태를 위한 ThemeBlockStates을 포함하는 맵 
	map<string,ThemeBlockState*> blockStates;
public:
	// 생성자 
	ThemeBlock(){}
	// 소멸자 
	~ThemeBlock(){
		//Loop through the ThemeBlockStates and delete them. ThemeBlockStates를 통해서 루프하고 그것들을 삭제하라
		for(map<string,ThemeBlockState*>::iterator i=blockStates.begin();i!=blockStates.end();++i){
			delete i->second;
		}
	}
	
	// ThemeBlock을 파괴하는 함수 
	void destroy(){
		//Loop through the ThemeBlockStates and delete them, ThemeBlockStates을 통해서 루프하고 그것들을 삭제하라 
		for(map<string,ThemeBlockState*>::iterator i=blockStates.begin();i!=blockStates.end();++i){
			delete i->second;
		}
		//Clear the blockStates map. blockStates 맵을 클리어하라 
		blockStates.clear();
		editorPicture.destroy();
	}
	
	// 노드로부터 ThemeBlock을 읽어오는 함수 
	//objNode: The TreeStorageNode to load the ThemeBlock from. objNode로부터 ThemeBlock를 읽는 TreeStorageNode
	//themePath: The path to the theme. 테마로의 경로
	//Returns: True if it succeeds. 만약 그것이 성공한다면 True 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
	
	// ThemeBlockInstance을 만드는 함수 
	//obj: Pointer that will be filled with the instance. 인스턴스로 채워질 포인터 
	void createInstance(ThemeBlockInstance* obj);
};

// ThemeCharacter의 한 상태를 포함하는 클래스 
class ThemeCharacterState{
public:
	//The length in frames of the oneTimeAnimation. oneTimeAnimation의 프레임에서의 길이 
	int oneTimeAnimationLength;
	//String containing the name of the next id. 다음 아이디의 길이를 포함하는 스트링 
	string nextState;
	//Vector with the themeObjects in the character state. 캐릭터 상태에서 themeObjects와 함께하는 벡터 
	vector<ThemeObject*> themeObjects;
public:
	// 생성자 
	ThemeCharacterState():oneTimeAnimationLength(0){}
	// 소멸자 
	~ThemeCharacterState(){
		//Loop through the themeObjects and delete them. themeObjects을 통해서 루프하고 그것들을 삭제하라
		for(unsigned int i=0;i<themeObjects.size();i++){
			delete themeObjects[i];
		}
	}
	
	// ThemeCharacterState를 파괴하는 데 사용되는 함수 
	void destroy(){
		//Loop through the themeObjects and delete them. themeObjects을 통해서 루프하고 그것들을 삭제하라
		for(unsigned int i=0;i<themeObjects.size();i++){
			delete themeObjects[i];
		}
		//Clear the themeObjects vector. themeObjects 벡터를 클리어
		themeObjects.clear();
		//Set oneTimeAnimation to zero. oneTimeAnimation을 0으로 설정
		oneTimeAnimationLength=0;
		//Clear the nextState string. nextState 스트링 클리어 
		nextState.clear();
	}
	
	// 노드로부터 ThemeCharacterState를 불러오는 함수 
	//objNode: The TreeStorageNode to load the state from. objNode로부터 상태를 읽는 TreeStorageNode
	//themePath: Path to the theme. 테마로의 경로
	//Returns: True if it succeeds. 만약 그것이 성공한다면 True 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
};

//Class containing the things needed for a themed character. 테마 캐릭터를 위해 필요한 것들을 포함하는 클래스 
class ThemeCharacter{
public:
	// 한 캐릭터의 다른 상태를 위한 ThemeCharacterStates를 포함하는 맵 
	map<string,ThemeCharacterState*> characterStates;
public:
	// 생성자 
	ThemeCharacter(){}
	// 소멸자 
	~ThemeCharacter(){
		//Loop through the states and delete them. 상태를 통해서 루프하고 그것들을 삭제하라
		for(map<string,ThemeCharacterState*>::iterator i=characterStates.begin();i!=characterStates.end();++i){
			delete i->second;
		}
	}
	
	// ThemeCharacter를 파괴하는 함수 
	void destroy(){
		//Loop through the states and delete them. 상태를 통해서 루프하고 그것들을 삭제하라
		for(map<string,ThemeCharacterState*>::iterator i=characterStates.begin();i!=characterStates.end();++i){
			delete i->second;
		}
		//Clear the characterStates map. characterStates 맵을 클리어 
		characterStates.clear();
	}
	
	// 노드로부터 ThemeCharacter를 불러오는 함수 
	//objNode: The TreeStorageNode to load the ThemeCharacter from. objNode로부터 ThemeCharacter를 읽는 TreeStorageNode
	//themePath: The path to the theme. 테마로의 경로
	//Returns: True if it succeeds. 만약 그것이 성공한다면 True 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
	
	// ThemeCharacterInstance를 만드는 함수 
	//obj: Pointer that will be filled with the instance. 인스턴스로 채워진 포인터 
	void createInstance(ThemeCharacterInstance* obj);
};

//ThemeBackgroundPicture 는 배경을 위한 사진을 포함하는 클래스 이다. 
class ThemeBackgroundPicture{
private:
	//Pointer to the SDL_Surface cached by the ImageManager. ImageManager에 의해 캐시된 SDL_Surface 포인터
	//This is used to rescale the theme. 이것은 테마를 재조정 하는데 쓰인다.
	SDL_Surface* cachedPicture;
	//Rectangle that should be taken from the picture. 사진으로부터 가져와야하는 직사각형 
	//NOTE The size is pixels of the image.
	SDL_Rect cachedSrcSize;
	//Rectangle with the size it will have on the destination (screen).
	//NOTE The size is in pixels or in precentages (if scaleToScreen is true).
	SDL_Rect cachedDestSize;
	
	//SDL_Surface containing the picture.
	//NOTE: This could point to the same surface as cachedPicture.
	SDL_Surface* picture;
	//Rectangle that should be taken from the picture.
	//NOTE The size is pixels of the image.
	SDL_Rect srcSize;
	//Rectangle with the size it will have on the destination (screen).
	//NOTE The size is in pixels even though the loaded value from the theme description file can be in precentages (if scaleToScreen is true).
	SDL_Rect destSize;
	
	//Boolean if the background picture should be scaled to screen. 배경 그림이 화면 배율을 조정해야 하는지 아닌지
	bool scale;
	
	//Boolean if the image should be repeated over the x-axis. 이미지가 x축 위에 반복되야 하는지 아닌지 
	bool repeatX;
	//Boolean if the image should be repeated over the y-axis. 이미지가 y축 위에 반복되야 하는지 아닌지 
	bool repeatY;
	
	//Float containing the speed the background picture moves along the x-axis.
	// 배경 사진이 x축을 따라 이동속도 를 포함하는 float
	float speedX;
	//Float containing the speed the background picture moves along the y-axis.
	// 배경 사진이 y축을 따라 이동속도 를 포함하는 float
	float speedY;
	
	//Float containing the horizontal speed the picture will have when moving the camera (horizontally).
	// 사진은 카메라를 (수평)으로 이동할 때 가지고 있는 수평속도를 포함하는 float 
	float cameraX;
	//Float containing the vertical speed the picture will have when moving the camera (vertically).
	// 사진은 카메라를 (수직)으로 이동할 때 가지고 있는 수직속도를 포함하는 float 
	float cameraY;
private:
	//Float with the current x position. 현재 x위치를 가지고 있는 flaot 
	float currentX;
	//Float with the current y position. 현재 y위치를 가지고 있는 flaot 
	float currentY;
	
	//Stored x location for when loading a state. 상태를 로딩할 때 저장되는 x 위치
	float savedX;
	//Stored y location for when loading a state. 상태를 로딩할 때 저장되는 y 위치 
	float savedY;
public:
	// 생성자 
	ThemeBackgroundPicture(){
		// 디폴트 변수 설정 
		picture=NULL;
		cachedPicture=NULL;
		memset(&srcSize,0,sizeof(srcSize));
		memset(&destSize,0,sizeof(destSize));
		memset(&cachedSrcSize,0,sizeof(cachedSrcSize));
		memset(&cachedDestSize,0,sizeof(cachedDestSize));
		scale=true;
		repeatX=true;
		repeatY=true;
		speedX=0.0f;
		speedY=0.0f;
		cameraX=0.0f;
		cameraY=0.0f;
		currentX=0.0f;
		currentY=0.0f;
		savedX=0.0f;
		savedY=0.0f;
	}
	
	// 애니메이션을 업데이트 하는 함수 
	void updateAnimation(){
		//Move the picture along the x-axis. x축을 따라 사진 이동
		currentX+=speedX;
		if(repeatX && destSize.w>0){
			float f=(float)destSize.w;
			if(currentX>f || currentX<-f) currentX-=f*floor(currentX/f);
		}
		
		//Move the picture along the y-axis. y축을 따라 사진 이동 
		currentY+=speedY;
		if(repeatY && destSize.h>0){
			float f=(float)destSize.h;
			if(currentY>f || currentY<-f) currentY-=f*floor(currentY/f);
		}
	}
	
	// 애니메이션을 리셋시키는 함수 
	//save: Boolean if the saved state should be deleted. 저장된 상태가 삭제되야 하는지 아닌지 
	void resetAnimation(bool save){
		currentX=0.0f;
		currentY=0.0f;
		if(save){
			savedX=0.0f;
			savedY=0.0f;
		}
	}
	// 애니메이션을 저장하는 함수 
	void saveAnimation(){
		savedX=currentX;
		savedY=currentY;
	}
	// 애니메이션을 불러오는 함수 
	void loadAnimation(){
		currentX=savedX;
		currentY=savedY;
	}
	
	// ThemeBackgroundPicture를 그리는 데 사용되는 함수 
	//dest: Pointer to the SDL_Surface the picture should be drawn. 사진이 그려져야 하는 SDL_Surface를 가리키는 포인터 
	void draw(SDL_Surface *dest);
	
	// 노드로부터 ThemeBackgroundPicture를 불러오는 데 사용되는 함수 
	//objNode: The TreeStorageNode to load the picture from. objNode로부터 사진을 불러오는 TreeStorageNode
	//themePath: The path to the theme. 테마로의 경로 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
	
	//This method will scale the background picture (if needed and configured) to the current SCREEN_WIDTH and SCREEN_WIDTH.\
	// 이 함수는 (만약 필요하고 구성된다면) 현재 SCREEN_WIDTH와 SCREEN_WIDTH의 배경사진을 조정한다. 
	void scaleToScreen();
};

//Class that forms the complete background of a theme. 테마의 완전한 배경을 형성하는 클래스 
//It is in fact nothing more than a vector containing multiple ThemeBackgroundPictures.
class ThemeBackground{
private:
	//Vector containing the ThemeBackgroundPictures. ThemeBackgroundPictures를 포함하는 벡터 
	vector<ThemeBackgroundPicture> picture;
public:
	// 모든 배경사진의 애니메이션을 업데이트하는 함수 
	void updateAnimation(){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].updateAnimation();
		}
	}
	
	// 모든 배경사진의 애니메이션을 리셋하는 함수 
	//save: Boolean if the saved state should be deleted. 저장된 상태가 삭제되어야 하는지 아닌지 
	void resetAnimation(bool save){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].resetAnimation(save);
		}
	}
	
	// 모든 배경사진의 애니메이션을 저장하는 함수 
	void saveAnimation(){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].saveAnimation();
		}
	}
	// 모든 배경사진의 애니메이션을 불러오는 함수 
	void loadAnimation(){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].loadAnimation();
		}
	}
	
	//Method that will scale the background pictures (if set) to the current screen resolution.
	// (만약 설정한다면) 현재 스크린 해결책에서 배경사진을 조정하는 함수 
	void scaleToScreen(){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].scaleToScreen();
		}
	}
	
	// 모든 배경사진을 그리는 함수 
	//dest: Pointer to the SDL_Surface to draw them on.
	void draw(SDL_Surface* dest){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].draw(dest);
		}
	}
	
	// ThemeBackgroundPicture에 ThemeBackground를 추가하는 함수 
	//objNode: The treeStorageNode to read from. objNode로부터 읽는 treeStorageNode
	//themePath: The path to the theme. 테마로의 경로
	//Returns: True if it succeeds. 만약 성공한다면 True 
	bool addPictureFromNode(TreeStorageNode* objNode,string themePath){
		picture.push_back(ThemeBackgroundPicture());
		return picture.back().loadFromNode(objNode,themePath);
	}
};

//The ThemeManager is actaully a whole theme, filled with ThemeBlocks, ThemeCharacter and ThemeBackground.
// ThemeManager는 실제로 전체 테마이고, ThemeBlocks와 ThemeCharacter, 그리고 ThemeBackground으로 채워진다. 
class ThemeManager{
private:
	//The ThemeCharacter of the shadow. 그림자의 ThemeCharacter
	ThemeCharacter* shadow;
	//The ThemeCharacter of the player. 플레이어의 ThemeCharacter
	ThemeCharacter* player;
	
	//Array containing a ThemeBlock for every block type. 모든 블럭타입을 위한 ThemeBlock을 포함하는 배열 
	ThemeBlock* objBlocks[TYPE_MAX];
	
	//The ThemeBackground.
	ThemeBackground* objBackground;
public:
	//String containing the path to the string. 스트링으로의 경로를 포함하는 스트링 
	string themePath;
	//String containing the theme name. 테마 이름을 포함하는 스트링 
	string themeName;
public:
	// 생성자 
	ThemeManager(){
		//Make sure the pointers are set to NULL. 포인터를 NULL로 설정하는걸 확실히 함 
		objBackground=NULL;
		//Reserve enough memory for the ThemeBlocks. ThemeBlocks를 위한 충분한 메모리를 예비하라  
		memset(objBlocks,0,sizeof(objBlocks));
		shadow=NULL;
		player=NULL;
	}
	// 소멸자 
	~ThemeManager(){
		//Delete the ThemeCharacter of the shadow. 그림자의 ThemeCharacter 삭제 
		if(shadow)
			delete shadow;
		//Delete the ThemeCharacter of the player. 플레이어의 ThemeCharacter 삭제 
		if(player)
			delete player;
		//Loop through the ThemeBlocks and delete them. ThemeBlocks을 통해 루프하고 그것들을 삭제하라 
		for(int i=0;i<TYPE_MAX;i++){
			if(objBlocks[i])
				delete objBlocks[i];
		}
		//Delete the ThemeBackground. 배경삭제 
		if(objBackground)
			delete objBackground;
	}

	// ThemeManager를 파괴하는 데 사용되는 함수 
	void destroy(){
		//Delete the ThemeCharacter of the shadow. 그림자의 ThemeCharacter 삭제 
		if(shadow)
			delete shadow;
		//Delete the ThemeCharacter of the player. 플레이어의 ThemeCharacter 삭제 
		if(player)
			delete player;
		//Loop through the ThemeBlocks and delete them. ThemeBlocks을 통해 루프하고 그것들을 삭제하라 
		for(int i=0;i<TYPE_MAX;i++){
			if(objBlocks[i])
				delete objBlocks[i];
		}
		//Delete the ThemeBackground. 배경삭제 
		if(objBackground)
			delete objBackground;
		
		//And clear the themeName. 테마이름 클리어 
		themeName.clear();
	}
	
	// 파일로부터 테마를 로드하는 함수 
	//fileName: The file to load the theme from. 테마를 로드한 파일 
	//Returns: True if it succeeds. 만약 성공한다면 True 
	bool loadFile(const string& fileName);
	
	//Method that will scale the theme to the current SCREEN_WIDTH and SCREEN_HEIGHT.
	// 현재 SCREEN_WIDTH 와 SCREEN_HEIGHT에서 테마를 조정하는 함수 
	void scaleToScreen(){
		//We only need to scale the background. 우리는 오직 배경을 조정하는 것이 필요하다. 
		if(objBackground)
			objBackground->scaleToScreen();
	}
	
	//Get a pointer to the ThemeBlock of a given block type. 주어진 블록타입의 ThemeBlock에서 포인터를 얻어라
	//index: The type of block. 블록의 타입 
	//Returns: Pointer to the ThemeBlock. ThemeBlock에서의 포인터 
	ThemeBlock* getBlock(int index){
		return objBlocks[index];
	}
	//Get a pointer to the ThemeCharacter of the shadow or the player.그림자나 플레이어의 ThemeCharacter로의 포인터
	//isShadow: Boolean if it's the shadow 그림자인지 아닌지 
	//Returns: Pointer to the ThemeCharacter. ThemeCharacter로의 포인터 
	ThemeCharacter* getCharacter(bool isShadow){
		if(isShadow)
			return shadow;
		return player;
	}
	//Get a pointer to the ThemeBackground of the theme. 테마의 ThemeBackground의 포인터를 얻음 
	//Returns: Pointer to the ThemeBackground. 배경으로의 포인터 
	ThemeBackground* getBackground(){
		return objBackground;
	}
};

//Class that combines multiple ThemeManager into one stack. 하나의 스택에서 다수의 ThemeManager를 결합하는 클래스 
//If a file is not in a certain theme it will use one of a lower theme.
class ThemeStack{
private:
	//Vector containing the themes in the stack. 스택에서 그 테마를 포함하는 벡터 
	vector<ThemeManager*> objThemes;
public:
	// 생성자
	ThemeStack(){}
	// 소멸자 
	~ThemeStack(){
		//Loop through the themes and delete them. 테마를 통해서 루프하고 그것들을 삭제 
		for(unsigned int i=0;i<objThemes.size();i++)
			delete objThemes[i];
	}
	
	// ThemeStack을 파괴하는 함수 
	void destroy(){
		//Loop through the themes and delete them. 테마를 통해서 루프하고 그것들을 삭제
		for(unsigned int i=0;i<objThemes.size();i++)
			delete objThemes[i];
		//Clear the vector to prevent dangling pointers. 허상포인터(매달려있는)를 예비한 벡터 클리어
		objThemes.clear();
	}
	
	// 테마를 스택에 추가하는 함수 
	//obj: The ThemeManager to add.
	void appendTheme(ThemeManager* obj){
		objThemes.push_back(obj);
	}
	// 스택에 추가된 마지막 테마를 삭제하는 함수 
	void removeTheme(){
		//Make sure that the stack isn't empty. 스택이 비었는지 확실히 하라 
		if(!objThemes.empty()){
			delete objThemes.back();
			objThemes.pop_back();
		}
	}
	
	// 파일로부터 불러온 테마를 추가하는 함수 
	//fileName: The file to load the theme from.
	//Returns: Pointer to the newly added theme, NULL if failed. 새롭게 추가된 테마를 가리키고, 만약 실패한다면 NULL
	ThemeManager* appendThemeFromFile(const string& fileName){
		// 새로운 themeManager를 만들어라 
		ThemeManager* obj=new ThemeManager();
		
		// 주어진 파일로부터 그것을 로드하라 
		if(!obj->loadFile(fileName)){
			//Failed thus delete the theme and return null. 실패하면 테마 삭제, null 반환
			cerr<<"ERROR: Failed loading theme "<<fileName<<endl;
			delete obj;
			return NULL;
		}else{
			//Succeeded, add it to the stack and return it. 성공하면, 그것을 스택에 추가하고 그것 반환 
			objThemes.push_back(obj);
			return obj;
		}
	}
	
	//Method that is used to let the themes scale. 테마 규모를 정하는 데 사용되는 함수 
	void scaleToScreen(){
		//Loop through the themes and call their scaleToScreen method. 
		// 테마를 통해 루프하고 그것들의 scaleToScreen 함수를 불러라 
		for(unsigned int i=0;i<objThemes.size();i++)
			objThemes[i]->scaleToScreen();
	}
	
	//Get the number of themes in the stack. 스택에 있는 테마 넘버를 얻어라 
	//Returns: The theme count. 테마 수 반환 
	int themeCount(){
		return (int)objThemes.size();
	}
	
	//Operator overloading so that the themes can be accesed using the [] operator.
	//i: The index.
	ThemeManager* operator[](int i){
		return objThemes[i];
	}
	//Get a pointer to the ThemeBlock of a given block type. 주어진 블록 타입의 ThemeBlock으로의 포인터를 얻어라
	//index: The type of block. 블록타입 
	//Returns: Pointer to the ThemeBlock. ThemeBlock으로의 포인터 
	ThemeBlock* getBlock(int index){
		//Loop through the themes from top to bottom. 탑에서 바텀까지 테마를 통해 루프 
		for(int i=objThemes.size()-1;i>=0;i--){
			//Get the block from the theme. 테마로부터 블록을 얻어라 
			ThemeBlock* obj=objThemes[i]->getBlock(index);
			//Check if it isn't null. null인지 아닌지 확인하라 
			if(obj)
				return obj;
		}
		
		//Nothing found. 아무것도 없음 
		return NULL;
	}
	//Get a pointer to the ThemeCharacter of the shadow or the player. 그림자나 플레이어의 ThemeCharacter로의 포인터를 얻음 
	//isShadow: Boolean if it's the shadow 그림자인지 아닌지 
	//Returns: Pointer to the ThemeCharacter. ThemeCharacter로의 포인터 
	ThemeCharacter* getCharacter(bool isShadow){
		//Loop through the themes from top to bottom. 탑에서 바텀까지 테마를 통해 루프 
		for(int i=objThemes.size()-1;i>=0;i--){
			//Get the ThemeCharacter from the theme. 테마를 통해 ThemeCharacter를 얻음 
			ThemeCharacter* obj=objThemes[i]->getCharacter(isShadow);
			//Check if it isn't null. null인지 아닌지 확인 
			if(obj)
				return obj;
		}
		
		//Nothing found. 아무것도 없음 
		return NULL;
	}
	//Get a pointer to the ThemeBackground of the theme. 테마의 ThemeBackground로의 포인터를 얻어라 
	//Returns: Pointer to the ThemeBackground. ThemeBackground로의 포인터 
	ThemeBackground* getBackground(){
		//Loop through the themes from top to bottom. 탑에서 바텀까지 테마를 통해 루프하라 
		for(int i=objThemes.size()-1;i>=0;i--){
			//Get the ThemeBackground from the theme. 테마로부터 ThemeBackground를 얻어라 
			ThemeBackground* obj=objThemes[i]->getBackground();
			//Check if it isn't null. null인지 아닌지 확인하라 
			if(obj)
				return obj;
		}
		
		//Nothing found. 아무것도 없음 
		return NULL;
	}
};
 
//The ThemeStack that is be used by the GameState. GameState에 의해 사용된 ThemeStack
extern ThemeStack objThemes;

#endif
