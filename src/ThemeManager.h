f/* �ڵ�м��� : 60142270 ��ä��
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
// ThemeObject�� �ν��Ͻ� Ŭ������, �ٸ� �ν��Ͻ� Ŭ�����鿡 ���� ���ȴ�.
class ThemeObjectInstance{
public:
	//������ ����Ű�� ������
	ThemePicture* picture;
	//Pointer to the parent the object an instance os is.
	ThemeObject* parent;
	
	//Integer containing the current animation frame. ���� �ִϸ��̼� �������� �����ϴ� ��Ƽ��
	int animation;
	//Integer containing the saved animation frame. ����� �ִϸ��̼� �������� �����ϴ� ��Ƽ�� 
	int savedAnimation;
public:
	// ������
	ThemeObjectInstance():picture(NULL),parent(NULL),animation(0),savedAnimation(0){}
	
	// TimeObject�� �׸��� �Լ�
	// dest : ������ ǥ���� ThemeObject�� �׸�  // �� ��������  ThemeObject�� ǥ���� �����ϰ� ��
	//dest: The destination surface to draw the ThemeObject on.
	//x: dest ǥ�鿡 x ��ġ
	//y: dest ǥ�鿡 y ��ġ
	//clipRect: �ڸ��µ� ���� ���簢��
	void draw(SDL_Surface* dest,int x,int y,SDL_Rect* clipRect=NULL);
	
	// �ִϸ��̼��� ������Ʈ ���ִ� �Լ�
	void updateAnimation();
	
	// �ִϸ��̼��� ���½����ִ� �Լ�
	//save: ����� �ִϸ��̼��� �����Ǵ��� �ƴ��� Boolean type ���� ���� 
	void resetAnimation(bool save){
		animation=0;
		if(save){
			savedAnimation=0;
		}
	}
	// �ִϸ��̼��� �����ϴ� �Լ�
	void saveAnimation(){
		savedAnimation=animation;
	}
	// ����� �ִϸ��̼��� �ҷ����� �Լ�
	void loadAnimation(){
		animation=savedAnimation;
	}
};

// ThemeBlockState �� Instance class��, ThemeBlockInstance�� ���� �������
class ThemeBlockStateInstance{
public:
	//Pointer to the parent the state an instance of is.
	ThemeBlockState *parent;
	//Vector containing the ThemeObjectInstances.
	vector<ThemeObjectInstance> objects;
	
	//Integer containing the current animation frame. ���� �ִϸ��̼� �������� �����ϴ� ��Ƽ��
	int animation;
	//Integer containing the saved animation frame. ����� �ִϸ��̼� �������� �����ϴ� ��Ƽ�� 
	int savedAnimation;
public:
	//������
	ThemeBlockStateInstance():parent(NULL),animation(0),savedAnimation(0){}
	
	//ThemeBlockState�� �׸��� �� ���Ǵ� �Լ� 
	// dest : ������ ǥ���� ThemeBlockState�� �׸�  // �� ��������  ThemeBlockState�� ǥ���� �����ϰ� ��
	//dest: The destination surface to draw the ThemeBlockState on.
	//x: dest ǥ�鿡 x ��ġ
	//y: dest ǥ�鿡 y ��ġ
	//clipRect: �ڸ��µ� ���� ���簢��
	void draw(SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].draw(dest,x,y,clipRect);
		}
	}
	
	// �ִϸ��̼��� ������Ʈ ���ִ� �Լ�
	void updateAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].updateAnimation();
		}
		animation++;
	}
	// �ִϸ��̼��� ���½����ִ� �Լ�
	//save: ����� �ִϸ��̼��� �����Ǵ��� �ƴ��� Boolean type ���� ���� 
	void resetAnimation(bool save){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].resetAnimation(save);
		}
		animation=0;
		if(save){
			savedAnimation=0;
		}
	}
	// �ִϸ��̼��� �����ϴ� �Լ� 
	void saveAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].saveAnimation();
		}
		savedAnimation=animation;
	}
	// ����� �ִϸ��̼��� �ҷ����� �Լ� 
	void loadAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].loadAnimation();
		}
		animation=savedAnimation;
	}
};

// ThemeBlock�� �ν��Ͻ���, ���� ���� ��ϵ��� ���� �ȿ��� �׸��� �ٲٴ� ���� ���� ���ִ� �� ���ȴ�. 
// �װ��� ���� �ִϸ��̼��� ���������� ��ϰ� �Ѵ�.
class ThemeBlockInstance{
public:
	// ���� ������ ������ 
	ThemeBlockStateInstance* currentState;
	// ���� ������ �̸� 
	string currentStateName;
	
	// blockStates �� �����ϴ� ��
	map<string,ThemeBlockStateInstance> blockStates;
	// ����� ������ �̸��� �����ϴ� String
	string savedStateName;
public:
	// ������ 
	ThemeBlockInstance():currentState(NULL){}
	
	// ThemeBlock�� �׸��� �� ���Ǵ� �Լ� 
	// dest : ������ ǥ���� ThemeBlock�� �׸�  // �� ��������  ThemeBlock�� ǥ���� �����ϰ� ��
	//dest: The destination surface to draw the ThemeBlock on.
	//x: dest ǥ�鿡 x ��ġ
	//y: dest ǥ�鿡 y ��ġ
	//clipRect: �ڸ��µ� ���� ���簢��
	//Returns: ���� �װ��� �����Ѵٸ� True 
	bool draw(SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		if(currentState!=NULL){
			currentState->draw(dest,x,y,clipRect);
			return true;
		}
		return false;
	}
	// ��ü���� ���¸� �׸��� �Լ� 
	//s: �׸� ������ �̸� 
	// dest : ������ ǥ���� ThemeBlock�� �׸�  // �� ��������  ThemeBlock�� ǥ���� �����ϰ� ��
	//dest: The destination surface to draw the ThemeBlock on.
	//x: dest ǥ�鿡 x ��ġ
	//y: dest ǥ�鿡 y ��ġ
	//clipRect: �ڸ��µ� ���� ���簢��
	//Returns: ���� �װ��� �����Ѵٸ� True 
	bool drawState(const string& s,SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		map<string,ThemeBlockStateInstance>::iterator it=blockStates.find(s);
		if(it!=blockStates.end()){
			it->second.draw(dest,x,y,clipRect);
			return true;
		}
		return false;
	}

	// ������¸� �ٲٴ� �Լ� 
	//s: ��ȭ��ų ������ �̸� 
	//reset: Boolean if the animation should reset. ���� �ִϸ��̼��� ���µǾ�� �Ѵٸ� // �ִϸ��̼��� ���µǾ�� �ϴ��� �ƴ��� Boolean ��� 
	//Returns: ���� �װ��� �����Ѵٸ� True (exists Ȥ�� �����Ѵٸ�).
	bool changeState(const string& s,bool reset=true){
		//Get the new state. ���ο� ���¸� ���� 
		map<string,ThemeBlockStateInstance>::iterator it=blockStates.find(s);
		//Check if it exists. �װ��� �����ϴ��� �ƴ��� Ȯ���϶�
		if(it!=blockStates.end()){
			//Set the state. ���� ����
			currentState=&(it->second);
			currentStateName=it->first;
			
			//FIXME: Is it needed to set the savedStateName here? savedStateName�� ���⼭ �����ϴ°� �ʿ�Ǿ�����?
			if(savedStateName.empty())
				savedStateName=currentStateName;
			
			//If reset then reset the animation. �缳���� ��� �ִϸ��̼��� �ٽ� �����Ѵ�. 
			if(reset)
				currentState->resetAnimation(true);
			return true;
		}
		
		//It doesn't so return false. �װ� �ȵǸ� false�� ��ȯ�϶� 
		return false;
	}
	
	// �ִϸ��̼��� ������Ʈ ���ִ� �Լ� 
	void updateAnimation();
	// �ִϸ��̼��� ���½����ִ� �Լ� 
	//save: Boolean if the saved state should be deleted. ����� ���°� �����Ǿ���ϴ��� ������ Boolean���� ����
	void resetAnimation(bool save){
		for(map<string,ThemeBlockStateInstance>::iterator it=blockStates.begin();it!=blockStates.end();++it){
			it->second.resetAnimation(save);
		}
		if(save){
			savedStateName.clear();
		}
	}
	// �ִϸ��̼��� �����ϴ� �Լ� 
	void saveAnimation(){
		for(map<string,ThemeBlockStateInstance>::iterator it=blockStates.begin();it!=blockStates.end();++it){
			it->second.saveAnimation();
		}
		savedStateName=currentStateName;
	}
	// ����� �ִϸ��̼��� �����ϴ� �Լ� 
	void loadAnimation(){
		for(map<string,ThemeBlockStateInstance>::iterator it=blockStates.begin();it!=blockStates.end();++it){
			it->second.loadAnimation();
		}
		changeState(savedStateName,false);
	}
};

//Instance class of a ThemeCharacterState, this is used by the ThemeCharacterInstance.
// ThemeCharacterState�� �ν��Ͻ� Ŭ������, ThemeCharacterInstance�� ���� ���ȴ�. 
class ThemeCharacterStateInstance{
public:
	//Pointer to the parent the state an instance of is.
	ThemeCharacterState* parent;
	//Vector containing the ThemeObjectInstances. ThemeObjectInstances�� �����ϴ� Vector 
	vector<ThemeObjectInstance> objects;
	
	//Integer containing the current animation frame. ���� �ִϸ��̼� �������� �����ϴ� ��Ƽ��
	int animation;
	//Integer containing the saved animation frame. ����� �ִϸ��̼� �������� �����ϴ� ��Ƽ�� 
	int savedAnimation;
public:
	// ������ 
	ThemeCharacterStateInstance():parent(NULL),animation(0),savedAnimation(0){}
	
	// ThemeCharacterState�� �׸��� �� ���Ǵ� �Լ� 
	// dest : ������ ǥ���� ThemeCharacterState�� �׸�  // �� ��������  ThemeCharacterState�� ǥ���� �����ϰ� ��
	//dest: The destination surface to draw the ThemeCharacterState on.
	//x: dest ǥ�鿡 x ��ġ
	//y: dest ǥ�鿡 y ��ġ
	//clipRect: �ڸ��µ� ���� ���簢��
	void draw(SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].draw(dest,x,y,clipRect);
		}
	}
	
	// �ִϸ��̼��� ������Ʈ ���ִ� �Լ� 
	void updateAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].updateAnimation();
		}
		animation++;
	}
	// �ִϸ��̼��� ���½����ִ� �Լ� 
	//save: Boolean if the saved state should be deleted. ����� ���°� �����Ǿ���ϴ��� ������ Boolean���� ����
	void resetAnimation(bool save){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].resetAnimation(save);
		}
		animation=0;
		if(save)
			savedAnimation=0;
	}
	// �ִϸ��̼��� �����ϴ� �Լ� 
	void saveAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].saveAnimation();
		}
		savedAnimation=animation;
	}
	// ����� �ִϸ��̼��� �ҷ����� �Լ� 
	void loadAnimation(){
		for(unsigned int i=0;i<objects.size();i++){
			objects[i].loadAnimation();
		}
		animation=savedAnimation;
	}
};

// ThemeCharacter�� �ν��Ͻ� 
class ThemeCharacterInstance{
public:
	//Pointer to the current state. ���� ���¸� ����Ű�� ������
	ThemeCharacterStateInstance* currentState;
	//The name of the current state. ���� ���¸� ����Ű�� �̸� 
	string currentStateName;
	
	//Map containing the ThemeCharacterStates. ThemeCharacterStates�� �����ϴ� Map
	map<string,ThemeCharacterStateInstance> characterStates;
	//String containing the name of the saved state. ����� ������ �̸��� �����ϴ� String
	string savedStateName;
public:
	// ������ 
	ThemeCharacterInstance():currentState(NULL){}
	
	// ThemeCharacter�� �׸��� �� ���Ǵ� �Լ� 
	// dest : ������ ǥ���� ThemeCharacter�� �׸�  // �� ��������  ThemeCharacter�� ǥ���� �����ϰ� ��
	//dest: The destination surface to draw the ThemeCharacterState on.
	//x: dest ǥ�鿡 x ��ġ
	//y: dest ǥ�鿡 y ��ġ
	//clipRect: �ڸ��µ� ���� ���簢��
	//Returns: True if it succeeds. ���� �װ��� �����Ѵٸ� True 
	bool draw(SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		if(currentState!=NULL){
			currentState->draw(dest,x,y,clipRect);
			return true;
		}
		return false;
	}

	//Method that will draw a specific state. ��ü���� ���¸� �׷��ִ� �Լ� 
	//s: The name of the state to draw. �׸� ������ �̸� 
	// dest : ������ ǥ���� ThemeCharacter�� �׸�  // �� ��������  ThemeCharacter�� ǥ���� �����ϰ� ��
	//dest: The destination surface to draw the ThemeCharacterState on.
	//x: dest ǥ�鿡 x ��ġ
	//y: dest ǥ�鿡 y ��ġ
	//clipRect: �ڸ��µ� ���� ���簢��
	//Returns: True if it succeeds. ���� �װ��� �����Ѵٸ� True 
	bool drawState(const string& s,SDL_Surface *dest,int x,int y,SDL_Rect *clipRect=NULL){
		map<string,ThemeCharacterStateInstance>::iterator it=characterStates.find(s);
		if(it!=characterStates.end()){
			it->second.draw(dest,x,y,clipRect);
			return true;
		}
		return false;
	}
	
	//Method that will change the current state. ���� ���¸� ��ȭ�����ִ� �Լ� 
	//s: The name of the state to change to. ��ȭ��ų ������ �̸� 
	//reset: Boolean if the animation should reset. �ִϸ��̼��� �缳���Ǿ�� �ϴ��� �ƴ��� Boolean���� ���� 
	//Returns: True if it succeeds (exists). ���� �װ��� �����Ѵٸ� True (Ȥ�� �����Ѵٸ�)
	bool changeState(const string& s,bool reset=true){
		//Get the new state. ���ο� ���¸� ����
		map<string,ThemeCharacterStateInstance>::iterator it=characterStates.find(s);
		//Check if it exists. �װ��� �����ϴ��� Ȯ��
		if(it!=characterStates.end()){
			//Set the state. ���� ���� 
			currentState=&(it->second);
			currentStateName=it->first;
			
			//FIXME: Is it needed to set the savedStateName here? ���⿡ savedStateName�� �����ϴ°� �ʿ��Ѱ�? 
			if(savedStateName.empty())
				savedStateName=currentStateName;
			
			//If reset then reset the animation. ���� �缳���Ѵٸ� �ִϸ��̼��� �����Ͻÿ�.
			if(reset)
				currentState->resetAnimation(true);
			return true;
		}
		
		//It doesn't so return false. �װ��� �ƴϸ� false�� ��ȯ 
		return false;
	}
	
	// �ִϸ��̼��� ������Ʈ ���ִ� �Լ�
	void updateAnimation();
	// �ִϸ��̼��� �������ִ� �Լ� 
	//save: Boolean if the saved state should be deleted. ����� ���°� �����Ǿ�� �ϴ��� ������ Boolean���� ����
	void resetAnimation(bool save){
		for(map<string,ThemeCharacterStateInstance>::iterator it=characterStates.begin();it!=characterStates.end();++it){
			it->second.resetAnimation(save);
		}
		if(save)
			savedStateName.clear();
	}
	// �ִϸ��̼��� ��������ִ� �Լ� 
	void saveAnimation(){
		for(map<string,ThemeCharacterStateInstance>::iterator it=characterStates.begin();it!=characterStates.end();++it){
			it->second.saveAnimation();
		}
		savedStateName=currentStateName;
	}
	// ����� �ִϸ��̼��� ���������ִ� �Լ� 
	void loadAnimation(){
		for(map<string,ThemeCharacterStateInstance>::iterator it=characterStates.begin();it!=characterStates.end();++it){
			it->second.loadAnimation();
		}
		changeState(savedStateName,false);
	}
};

// offset data�� �����ϴ� Ŭ���� 
class ThemeOffsetData{
public:
	//Vector containing the offsetDatas. offsetData���� �����ϴ� ���� 
	vector<typeOffsetPoint> offsetData;
	//The length of the "animation" in frames. ������ ���� "�ִϸ��̼�" �� ����
	int length;
public:
	// ������
	ThemeOffsetData():length(0){}
	// �Ҹ���
	~ThemeOffsetData(){}
	
	//Method used to destroy the offsetData. offsetData�� �ı��ϴµ� ���Ǵ� �Լ� 
	void destroy(){
		//Set length to zero. 0������ ���� ���� 
		length=0;
		//And clear the offsetData vector. offsetData ���͸� Ŭ���� �� 
		offsetData.clear();
	}
	
	//Method that will load the offsetData from a node. ���κ��� offsetData�� �ҷ����� �Լ� 
	//objNode: Pointer to the TreeStorageNode to read the data from. objNode�κ��� �����͸� �о��ִ� TreeStorageNode�� ����Ű�� ������ 
	//Returns: True if it succeeds without errors. ������ ���� �����Ѵٸ� True
	bool loadFromNode(TreeStorageNode* objNode);
};

//This is the lowest level of the theme system. �̰��� ���� ���� ������ �׸� �ý����̴�. 
//It's a picture with offset data. �װ��� offset data�� ������ �ִ� �����̴�.
class ThemePicture{
public:
	//The SDL_Surface containing the picture. ������ �����ϴ� SDL_Surface
	SDL_Surface* picture;
	//Offset data for the picture. ������ ���� Offset data
	ThemeOffsetData offset;
public:
	// ������
	ThemePicture():picture(NULL){}
	// �Ҹ��� 
	~ThemePicture(){}
	
	//Method used to destroy the picture. ������ �ı��ϴµ� ���Ǵ� �Լ� 
	void destroy(){
		//FIXME: Shouldn't the image be freed? (ImageManager) �̹����� �������� ������? 
		picture=NULL;
		//Destroy the offset data. offset data�� �ı��϶� 
		offset.destroy();
	}
	bool loadFromNode(TreeStorageNode* objNode, string themePath);
	
	// ThemePicture�� �׷��ִ� �Լ� 
	//dest: ������ ǥ�� 
	//x: The x location on the dest to draw the picture. ������ �׸� dest ǥ�鿡 x ��ġ 
	//y: The y location on the dest to draw the picture. ������ �׸� dest ǥ�鿡 y ��ġ 
	//animation: The frame of the animation to draw. �׸� �ִϸ��̼��� ������ 
	//clipRect: Rectangle to clip the picture. ������ �ڸ��� ���簢�� 
	void draw(SDL_Surface* dest,int x,int y,int animation=0,SDL_Rect* clipRect=NULL);
};

//The ThemeObject class is used to contain a basic theme element. ThemeObject�� �⺻ �׸� ��Ҹ� �����ϴµ� ���ȴ�. 
//Contains the picture, animation information, etc... ����, �ִϸ��̼�.... ����� �����Ѵ�. 
class ThemeObject{
public:
	//Integer containing the length of the animation. �ִϸ��̼��� ���̸� �����ϴ� ��Ƽ��
	int animationLength;
	//Integer containing the frame from where the animation is going to loop. 
	// �ִϸ��̼��� ��𿡼� ������ �� ������ �������� �����ϴ� ��Ƽ�� 
	int animationLoopPoint;
	
	//Boolean if the animation is invisible at run time (Game state).
	// �ִϸ��̼��� ��Ÿ�ӿ��� (Ȥ�� ���� ���¿���) ������ �ʴ��� �ƴ��� 
	bool invisibleAtRunTime;
	//Boolean if the animation is invisible at design time (Level editor).
	// �ִϸ��̼��� ������ �ð� (Ȥ�� ���� ����) ���� ������ �ʴ��� �ƴ��� 
	bool invisibleAtDesignTime;
	
	//Picture of the ThemeObject. ThemeObject�� ���� 
	ThemePicture picture;
	//Picture of the ThemeObject shown when in the level editor. ���������Ϳ� ���� �� �������� ThemeObject ���� 
	ThemePicture editorPicture;
	
	//Vector containing optionalPicture for the ThemeObject. ThemeObject�� ���� optionalPicture�� �����ϴ� ���� 
	vector<pair<double,ThemePicture*> > optionalPicture;
	
	//ThemeOffsetData for the ThemeObject. ThemeObject�� ���� ThemeOffsetData 
	ThemeOffsetData offset;
public:
	// ������ 
	ThemeObject():animationLength(0),animationLoopPoint(0),invisibleAtRunTime(false),invisibleAtDesignTime(false){}
	// �Ҹ��� 
	~ThemeObject(){
		//Loop through the optionalPicture and delete them. optionalPicture�� ���ؼ� �����ϰ� �װ͵��� �����϶� 
		for(unsigned int i=0;i<optionalPicture.size();i++){
			delete optionalPicture[i].second;
		}
	}
	
	// ThemeObject�� �ı��ϴ� �Լ� 
	void destroy(){
		//Loop through the optionalPicture and delete them. optionalPicture�� ���ؼ� �����ϰ� �װ͵��� �����϶�
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
	
	// ���κ��� ThemeObject�� �ҷ����� �Լ� 
	//objNode: The TreeStorageNode to read the object from. objNode�κ��� �� ������Ʈ�� �д� TreeStorageNode
	//themePath: Path to the theme. theme���� ��� 
	//Returns: True if it succeeds. ���� �װ��� �����ϸ� True
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
};

//Class containing a single state of a themed block. �׸������ ���� ���¸� �����ϴ� Ŭ���� 
class ThemeBlockState{
public:
	//The length in frames of the oneTimeAnimation. oneTimeAnimation�� �����ӿ����� ���� 
	int oneTimeAnimationLength;
	//String containing the name of the next state. ���� ������ �̸��� �����ϴ� ��Ʈ�� 
	string nextState;
	//Vector containing the themeObjects that make up this state. ���� ���¸� �����ϴ� themeObjects�� �����ϴ� ����
	vector<ThemeObject*> themeObjects;
public:
	// ������
	ThemeBlockState():oneTimeAnimationLength(0){}
	// �Ҹ��� 
	~ThemeBlockState(){
		//Loop through the ThemeObjects and delete them. ThemeObjects�� ���ؼ� �����ϰ� �װ͵��� �����϶� 
		for(unsigned int i=0;i<themeObjects.size();i++){
			delete themeObjects[i];
		}
	}
	
	// ThemeBlockState�� �ı��ϴ� �Լ� 
	void destroy(){
		//Loop through the ThemeObjects and delete them. ThemeObjects�� ���ؼ� �����ϰ� �װ͵��� �����϶� 
		for(unsigned int i=0;i<themeObjects.size();i++){
			delete themeObjects[i];
		}
		//Clear the themeObjects vector.themeObjects ���͸� Ŭ�����϶� 
		themeObjects.clear();
		//Set the length to 0. ���̸� 0���� �����϶� 
		oneTimeAnimationLength=0;
		//Clear the nextState string. �������� ��Ʈ���� Ŭ�����϶� 
		nextState.clear();
	}
	
	// ���κ��� ThemeBlockState�� �ҷ����� �Լ� 
	//objNode: The TreeStorageNode to read the state from. objNode�κ��� ���¸� �д� TreeStorageNode
	//themePath: Path to the theme. �׸����� ��� 
	//Returns: True if it succeeds. ���� �װ��� �����Ѵٸ� True 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
};

//Class containing the needed things for a themed block. �׸������ ���� �ʿ��� �͵��� �����ϴ� Ŭ���� 
class ThemeBlock{
public:
	//Picture that is shown only in the level editor. ���� �����Ϳ����� �������� ���� 
	ThemePicture editorPicture;
	
	//Map containing ThemeBlockStates for the different states of a block. 
	// �� ����� �ٸ����¸� ���� ThemeBlockStates�� �����ϴ� �� 
	map<string,ThemeBlockState*> blockStates;
public:
	// ������ 
	ThemeBlock(){}
	// �Ҹ��� 
	~ThemeBlock(){
		//Loop through the ThemeBlockStates and delete them. ThemeBlockStates�� ���ؼ� �����ϰ� �װ͵��� �����϶�
		for(map<string,ThemeBlockState*>::iterator i=blockStates.begin();i!=blockStates.end();++i){
			delete i->second;
		}
	}
	
	// ThemeBlock�� �ı��ϴ� �Լ� 
	void destroy(){
		//Loop through the ThemeBlockStates and delete them, ThemeBlockStates�� ���ؼ� �����ϰ� �װ͵��� �����϶� 
		for(map<string,ThemeBlockState*>::iterator i=blockStates.begin();i!=blockStates.end();++i){
			delete i->second;
		}
		//Clear the blockStates map. blockStates ���� Ŭ�����϶� 
		blockStates.clear();
		editorPicture.destroy();
	}
	
	// ���κ��� ThemeBlock�� �о���� �Լ� 
	//objNode: The TreeStorageNode to load the ThemeBlock from. objNode�κ��� ThemeBlock�� �д� TreeStorageNode
	//themePath: The path to the theme. �׸����� ���
	//Returns: True if it succeeds. ���� �װ��� �����Ѵٸ� True 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
	
	// ThemeBlockInstance�� ����� �Լ� 
	//obj: Pointer that will be filled with the instance. �ν��Ͻ��� ä���� ������ 
	void createInstance(ThemeBlockInstance* obj);
};

// ThemeCharacter�� �� ���¸� �����ϴ� Ŭ���� 
class ThemeCharacterState{
public:
	//The length in frames of the oneTimeAnimation. oneTimeAnimation�� �����ӿ����� ���� 
	int oneTimeAnimationLength;
	//String containing the name of the next id. ���� ���̵��� ���̸� �����ϴ� ��Ʈ�� 
	string nextState;
	//Vector with the themeObjects in the character state. ĳ���� ���¿��� themeObjects�� �Բ��ϴ� ���� 
	vector<ThemeObject*> themeObjects;
public:
	// ������ 
	ThemeCharacterState():oneTimeAnimationLength(0){}
	// �Ҹ��� 
	~ThemeCharacterState(){
		//Loop through the themeObjects and delete them. themeObjects�� ���ؼ� �����ϰ� �װ͵��� �����϶�
		for(unsigned int i=0;i<themeObjects.size();i++){
			delete themeObjects[i];
		}
	}
	
	// ThemeCharacterState�� �ı��ϴ� �� ���Ǵ� �Լ� 
	void destroy(){
		//Loop through the themeObjects and delete them. themeObjects�� ���ؼ� �����ϰ� �װ͵��� �����϶�
		for(unsigned int i=0;i<themeObjects.size();i++){
			delete themeObjects[i];
		}
		//Clear the themeObjects vector. themeObjects ���͸� Ŭ����
		themeObjects.clear();
		//Set oneTimeAnimation to zero. oneTimeAnimation�� 0���� ����
		oneTimeAnimationLength=0;
		//Clear the nextState string. nextState ��Ʈ�� Ŭ���� 
		nextState.clear();
	}
	
	// ���κ��� ThemeCharacterState�� �ҷ����� �Լ� 
	//objNode: The TreeStorageNode to load the state from. objNode�κ��� ���¸� �д� TreeStorageNode
	//themePath: Path to the theme. �׸����� ���
	//Returns: True if it succeeds. ���� �װ��� �����Ѵٸ� True 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
};

//Class containing the things needed for a themed character. �׸� ĳ���͸� ���� �ʿ��� �͵��� �����ϴ� Ŭ���� 
class ThemeCharacter{
public:
	// �� ĳ������ �ٸ� ���¸� ���� ThemeCharacterStates�� �����ϴ� �� 
	map<string,ThemeCharacterState*> characterStates;
public:
	// ������ 
	ThemeCharacter(){}
	// �Ҹ��� 
	~ThemeCharacter(){
		//Loop through the states and delete them. ���¸� ���ؼ� �����ϰ� �װ͵��� �����϶�
		for(map<string,ThemeCharacterState*>::iterator i=characterStates.begin();i!=characterStates.end();++i){
			delete i->second;
		}
	}
	
	// ThemeCharacter�� �ı��ϴ� �Լ� 
	void destroy(){
		//Loop through the states and delete them. ���¸� ���ؼ� �����ϰ� �װ͵��� �����϶�
		for(map<string,ThemeCharacterState*>::iterator i=characterStates.begin();i!=characterStates.end();++i){
			delete i->second;
		}
		//Clear the characterStates map. characterStates ���� Ŭ���� 
		characterStates.clear();
	}
	
	// ���κ��� ThemeCharacter�� �ҷ����� �Լ� 
	//objNode: The TreeStorageNode to load the ThemeCharacter from. objNode�κ��� ThemeCharacter�� �д� TreeStorageNode
	//themePath: The path to the theme. �׸����� ���
	//Returns: True if it succeeds. ���� �װ��� �����Ѵٸ� True 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
	
	// ThemeCharacterInstance�� ����� �Լ� 
	//obj: Pointer that will be filled with the instance. �ν��Ͻ��� ä���� ������ 
	void createInstance(ThemeCharacterInstance* obj);
};

//ThemeBackgroundPicture �� ����� ���� ������ �����ϴ� Ŭ���� �̴�. 
class ThemeBackgroundPicture{
private:
	//Pointer to the SDL_Surface cached by the ImageManager. ImageManager�� ���� ĳ�õ� SDL_Surface ������
	//This is used to rescale the theme. �̰��� �׸��� ������ �ϴµ� ���δ�.
	SDL_Surface* cachedPicture;
	//Rectangle that should be taken from the picture. �������κ��� �����;��ϴ� ���簢�� 
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
	
	//Boolean if the background picture should be scaled to screen. ��� �׸��� ȭ�� ������ �����ؾ� �ϴ��� �ƴ���
	bool scale;
	
	//Boolean if the image should be repeated over the x-axis. �̹����� x�� ���� �ݺ��Ǿ� �ϴ��� �ƴ��� 
	bool repeatX;
	//Boolean if the image should be repeated over the y-axis. �̹����� y�� ���� �ݺ��Ǿ� �ϴ��� �ƴ��� 
	bool repeatY;
	
	//Float containing the speed the background picture moves along the x-axis.
	// ��� ������ x���� ���� �̵��ӵ� �� �����ϴ� float
	float speedX;
	//Float containing the speed the background picture moves along the y-axis.
	// ��� ������ y���� ���� �̵��ӵ� �� �����ϴ� float
	float speedY;
	
	//Float containing the horizontal speed the picture will have when moving the camera (horizontally).
	// ������ ī�޶� (����)���� �̵��� �� ������ �ִ� ����ӵ��� �����ϴ� float 
	float cameraX;
	//Float containing the vertical speed the picture will have when moving the camera (vertically).
	// ������ ī�޶� (����)���� �̵��� �� ������ �ִ� �����ӵ��� �����ϴ� float 
	float cameraY;
private:
	//Float with the current x position. ���� x��ġ�� ������ �ִ� flaot 
	float currentX;
	//Float with the current y position. ���� y��ġ�� ������ �ִ� flaot 
	float currentY;
	
	//Stored x location for when loading a state. ���¸� �ε��� �� ����Ǵ� x ��ġ
	float savedX;
	//Stored y location for when loading a state. ���¸� �ε��� �� ����Ǵ� y ��ġ 
	float savedY;
public:
	// ������ 
	ThemeBackgroundPicture(){
		// ����Ʈ ���� ���� 
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
	
	// �ִϸ��̼��� ������Ʈ �ϴ� �Լ� 
	void updateAnimation(){
		//Move the picture along the x-axis. x���� ���� ���� �̵�
		currentX+=speedX;
		if(repeatX && destSize.w>0){
			float f=(float)destSize.w;
			if(currentX>f || currentX<-f) currentX-=f*floor(currentX/f);
		}
		
		//Move the picture along the y-axis. y���� ���� ���� �̵� 
		currentY+=speedY;
		if(repeatY && destSize.h>0){
			float f=(float)destSize.h;
			if(currentY>f || currentY<-f) currentY-=f*floor(currentY/f);
		}
	}
	
	// �ִϸ��̼��� ���½�Ű�� �Լ� 
	//save: Boolean if the saved state should be deleted. ����� ���°� �����Ǿ� �ϴ��� �ƴ��� 
	void resetAnimation(bool save){
		currentX=0.0f;
		currentY=0.0f;
		if(save){
			savedX=0.0f;
			savedY=0.0f;
		}
	}
	// �ִϸ��̼��� �����ϴ� �Լ� 
	void saveAnimation(){
		savedX=currentX;
		savedY=currentY;
	}
	// �ִϸ��̼��� �ҷ����� �Լ� 
	void loadAnimation(){
		currentX=savedX;
		currentY=savedY;
	}
	
	// ThemeBackgroundPicture�� �׸��� �� ���Ǵ� �Լ� 
	//dest: Pointer to the SDL_Surface the picture should be drawn. ������ �׷����� �ϴ� SDL_Surface�� ����Ű�� ������ 
	void draw(SDL_Surface *dest);
	
	// ���κ��� ThemeBackgroundPicture�� �ҷ����� �� ���Ǵ� �Լ� 
	//objNode: The TreeStorageNode to load the picture from. objNode�κ��� ������ �ҷ����� TreeStorageNode
	//themePath: The path to the theme. �׸����� ��� 
	bool loadFromNode(TreeStorageNode* objNode,string themePath);
	
	//This method will scale the background picture (if needed and configured) to the current SCREEN_WIDTH and SCREEN_WIDTH.\
	// �� �Լ��� (���� �ʿ��ϰ� �����ȴٸ�) ���� SCREEN_WIDTH�� SCREEN_WIDTH�� �������� �����Ѵ�. 
	void scaleToScreen();
};

//Class that forms the complete background of a theme. �׸��� ������ ����� �����ϴ� Ŭ���� 
//It is in fact nothing more than a vector containing multiple ThemeBackgroundPictures.
class ThemeBackground{
private:
	//Vector containing the ThemeBackgroundPictures. ThemeBackgroundPictures�� �����ϴ� ���� 
	vector<ThemeBackgroundPicture> picture;
public:
	// ��� �������� �ִϸ��̼��� ������Ʈ�ϴ� �Լ� 
	void updateAnimation(){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].updateAnimation();
		}
	}
	
	// ��� �������� �ִϸ��̼��� �����ϴ� �Լ� 
	//save: Boolean if the saved state should be deleted. ����� ���°� �����Ǿ�� �ϴ��� �ƴ��� 
	void resetAnimation(bool save){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].resetAnimation(save);
		}
	}
	
	// ��� �������� �ִϸ��̼��� �����ϴ� �Լ� 
	void saveAnimation(){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].saveAnimation();
		}
	}
	// ��� �������� �ִϸ��̼��� �ҷ����� �Լ� 
	void loadAnimation(){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].loadAnimation();
		}
	}
	
	//Method that will scale the background pictures (if set) to the current screen resolution.
	// (���� �����Ѵٸ�) ���� ��ũ�� �ذ�å���� �������� �����ϴ� �Լ� 
	void scaleToScreen(){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].scaleToScreen();
		}
	}
	
	// ��� �������� �׸��� �Լ� 
	//dest: Pointer to the SDL_Surface to draw them on.
	void draw(SDL_Surface* dest){
		for(unsigned int i=0;i<picture.size();i++){
			picture[i].draw(dest);
		}
	}
	
	// ThemeBackgroundPicture�� ThemeBackground�� �߰��ϴ� �Լ� 
	//objNode: The treeStorageNode to read from. objNode�κ��� �д� treeStorageNode
	//themePath: The path to the theme. �׸����� ���
	//Returns: True if it succeeds. ���� �����Ѵٸ� True 
	bool addPictureFromNode(TreeStorageNode* objNode,string themePath){
		picture.push_back(ThemeBackgroundPicture());
		return picture.back().loadFromNode(objNode,themePath);
	}
};

//The ThemeManager is actaully a whole theme, filled with ThemeBlocks, ThemeCharacter and ThemeBackground.
// ThemeManager�� ������ ��ü �׸��̰�, ThemeBlocks�� ThemeCharacter, �׸��� ThemeBackground���� ä������. 
class ThemeManager{
private:
	//The ThemeCharacter of the shadow. �׸����� ThemeCharacter
	ThemeCharacter* shadow;
	//The ThemeCharacter of the player. �÷��̾��� ThemeCharacter
	ThemeCharacter* player;
	
	//Array containing a ThemeBlock for every block type. ��� ��Ÿ���� ���� ThemeBlock�� �����ϴ� �迭 
	ThemeBlock* objBlocks[TYPE_MAX];
	
	//The ThemeBackground.
	ThemeBackground* objBackground;
public:
	//String containing the path to the string. ��Ʈ�������� ��θ� �����ϴ� ��Ʈ�� 
	string themePath;
	//String containing the theme name. �׸� �̸��� �����ϴ� ��Ʈ�� 
	string themeName;
public:
	// ������ 
	ThemeManager(){
		//Make sure the pointers are set to NULL. �����͸� NULL�� �����ϴ°� Ȯ���� �� 
		objBackground=NULL;
		//Reserve enough memory for the ThemeBlocks. ThemeBlocks�� ���� ����� �޸𸮸� �����϶�  
		memset(objBlocks,0,sizeof(objBlocks));
		shadow=NULL;
		player=NULL;
	}
	// �Ҹ��� 
	~ThemeManager(){
		//Delete the ThemeCharacter of the shadow. �׸����� ThemeCharacter ���� 
		if(shadow)
			delete shadow;
		//Delete the ThemeCharacter of the player. �÷��̾��� ThemeCharacter ���� 
		if(player)
			delete player;
		//Loop through the ThemeBlocks and delete them. ThemeBlocks�� ���� �����ϰ� �װ͵��� �����϶� 
		for(int i=0;i<TYPE_MAX;i++){
			if(objBlocks[i])
				delete objBlocks[i];
		}
		//Delete the ThemeBackground. ������ 
		if(objBackground)
			delete objBackground;
	}

	// ThemeManager�� �ı��ϴ� �� ���Ǵ� �Լ� 
	void destroy(){
		//Delete the ThemeCharacter of the shadow. �׸����� ThemeCharacter ���� 
		if(shadow)
			delete shadow;
		//Delete the ThemeCharacter of the player. �÷��̾��� ThemeCharacter ���� 
		if(player)
			delete player;
		//Loop through the ThemeBlocks and delete them. ThemeBlocks�� ���� �����ϰ� �װ͵��� �����϶� 
		for(int i=0;i<TYPE_MAX;i++){
			if(objBlocks[i])
				delete objBlocks[i];
		}
		//Delete the ThemeBackground. ������ 
		if(objBackground)
			delete objBackground;
		
		//And clear the themeName. �׸��̸� Ŭ���� 
		themeName.clear();
	}
	
	// ���Ϸκ��� �׸��� �ε��ϴ� �Լ� 
	//fileName: The file to load the theme from. �׸��� �ε��� ���� 
	//Returns: True if it succeeds. ���� �����Ѵٸ� True 
	bool loadFile(const string& fileName);
	
	//Method that will scale the theme to the current SCREEN_WIDTH and SCREEN_HEIGHT.
	// ���� SCREEN_WIDTH �� SCREEN_HEIGHT���� �׸��� �����ϴ� �Լ� 
	void scaleToScreen(){
		//We only need to scale the background. �츮�� ���� ����� �����ϴ� ���� �ʿ��ϴ�. 
		if(objBackground)
			objBackground->scaleToScreen();
	}
	
	//Get a pointer to the ThemeBlock of a given block type. �־��� ���Ÿ���� ThemeBlock���� �����͸� ����
	//index: The type of block. ����� Ÿ�� 
	//Returns: Pointer to the ThemeBlock. ThemeBlock������ ������ 
	ThemeBlock* getBlock(int index){
		return objBlocks[index];
	}
	//Get a pointer to the ThemeCharacter of the shadow or the player.�׸��ڳ� �÷��̾��� ThemeCharacter���� ������
	//isShadow: Boolean if it's the shadow �׸������� �ƴ��� 
	//Returns: Pointer to the ThemeCharacter. ThemeCharacter���� ������ 
	ThemeCharacter* getCharacter(bool isShadow){
		if(isShadow)
			return shadow;
		return player;
	}
	//Get a pointer to the ThemeBackground of the theme. �׸��� ThemeBackground�� �����͸� ���� 
	//Returns: Pointer to the ThemeBackground. ��������� ������ 
	ThemeBackground* getBackground(){
		return objBackground;
	}
};

//Class that combines multiple ThemeManager into one stack. �ϳ��� ���ÿ��� �ټ��� ThemeManager�� �����ϴ� Ŭ���� 
//If a file is not in a certain theme it will use one of a lower theme.
class ThemeStack{
private:
	//Vector containing the themes in the stack. ���ÿ��� �� �׸��� �����ϴ� ���� 
	vector<ThemeManager*> objThemes;
public:
	// ������
	ThemeStack(){}
	// �Ҹ��� 
	~ThemeStack(){
		//Loop through the themes and delete them. �׸��� ���ؼ� �����ϰ� �װ͵��� ���� 
		for(unsigned int i=0;i<objThemes.size();i++)
			delete objThemes[i];
	}
	
	// ThemeStack�� �ı��ϴ� �Լ� 
	void destroy(){
		//Loop through the themes and delete them. �׸��� ���ؼ� �����ϰ� �װ͵��� ����
		for(unsigned int i=0;i<objThemes.size();i++)
			delete objThemes[i];
		//Clear the vector to prevent dangling pointers. ���������(�Ŵ޷��ִ�)�� ������ ���� Ŭ����
		objThemes.clear();
	}
	
	// �׸��� ���ÿ� �߰��ϴ� �Լ� 
	//obj: The ThemeManager to add.
	void appendTheme(ThemeManager* obj){
		objThemes.push_back(obj);
	}
	// ���ÿ� �߰��� ������ �׸��� �����ϴ� �Լ� 
	void removeTheme(){
		//Make sure that the stack isn't empty. ������ ������� Ȯ���� �϶� 
		if(!objThemes.empty()){
			delete objThemes.back();
			objThemes.pop_back();
		}
	}
	
	// ���Ϸκ��� �ҷ��� �׸��� �߰��ϴ� �Լ� 
	//fileName: The file to load the theme from.
	//Returns: Pointer to the newly added theme, NULL if failed. ���Ӱ� �߰��� �׸��� ����Ű��, ���� �����Ѵٸ� NULL
	ThemeManager* appendThemeFromFile(const string& fileName){
		// ���ο� themeManager�� ������ 
		ThemeManager* obj=new ThemeManager();
		
		// �־��� ���Ϸκ��� �װ��� �ε��϶� 
		if(!obj->loadFile(fileName)){
			//Failed thus delete the theme and return null. �����ϸ� �׸� ����, null ��ȯ
			cerr<<"ERROR: Failed loading theme "<<fileName<<endl;
			delete obj;
			return NULL;
		}else{
			//Succeeded, add it to the stack and return it. �����ϸ�, �װ��� ���ÿ� �߰��ϰ� �װ� ��ȯ 
			objThemes.push_back(obj);
			return obj;
		}
	}
	
	//Method that is used to let the themes scale. �׸� �Ը� ���ϴ� �� ���Ǵ� �Լ� 
	void scaleToScreen(){
		//Loop through the themes and call their scaleToScreen method. 
		// �׸��� ���� �����ϰ� �װ͵��� scaleToScreen �Լ��� �ҷ��� 
		for(unsigned int i=0;i<objThemes.size();i++)
			objThemes[i]->scaleToScreen();
	}
	
	//Get the number of themes in the stack. ���ÿ� �ִ� �׸� �ѹ��� ���� 
	//Returns: The theme count. �׸� �� ��ȯ 
	int themeCount(){
		return (int)objThemes.size();
	}
	
	//Operator overloading so that the themes can be accesed using the [] operator.
	//i: The index.
	ThemeManager* operator[](int i){
		return objThemes[i];
	}
	//Get a pointer to the ThemeBlock of a given block type. �־��� ��� Ÿ���� ThemeBlock������ �����͸� ����
	//index: The type of block. ���Ÿ�� 
	//Returns: Pointer to the ThemeBlock. ThemeBlock������ ������ 
	ThemeBlock* getBlock(int index){
		//Loop through the themes from top to bottom. ž���� ���ұ��� �׸��� ���� ���� 
		for(int i=objThemes.size()-1;i>=0;i--){
			//Get the block from the theme. �׸��κ��� ����� ���� 
			ThemeBlock* obj=objThemes[i]->getBlock(index);
			//Check if it isn't null. null���� �ƴ��� Ȯ���϶� 
			if(obj)
				return obj;
		}
		
		//Nothing found. �ƹ��͵� ���� 
		return NULL;
	}
	//Get a pointer to the ThemeCharacter of the shadow or the player. �׸��ڳ� �÷��̾��� ThemeCharacter���� �����͸� ���� 
	//isShadow: Boolean if it's the shadow �׸������� �ƴ��� 
	//Returns: Pointer to the ThemeCharacter. ThemeCharacter���� ������ 
	ThemeCharacter* getCharacter(bool isShadow){
		//Loop through the themes from top to bottom. ž���� ���ұ��� �׸��� ���� ���� 
		for(int i=objThemes.size()-1;i>=0;i--){
			//Get the ThemeCharacter from the theme. �׸��� ���� ThemeCharacter�� ���� 
			ThemeCharacter* obj=objThemes[i]->getCharacter(isShadow);
			//Check if it isn't null. null���� �ƴ��� Ȯ�� 
			if(obj)
				return obj;
		}
		
		//Nothing found. �ƹ��͵� ���� 
		return NULL;
	}
	//Get a pointer to the ThemeBackground of the theme. �׸��� ThemeBackground���� �����͸� ���� 
	//Returns: Pointer to the ThemeBackground. ThemeBackground���� ������ 
	ThemeBackground* getBackground(){
		//Loop through the themes from top to bottom. ž���� ���ұ��� �׸��� ���� �����϶� 
		for(int i=objThemes.size()-1;i>=0;i--){
			//Get the ThemeBackground from the theme. �׸��κ��� ThemeBackground�� ���� 
			ThemeBackground* obj=objThemes[i]->getBackground();
			//Check if it isn't null. null���� �ƴ��� Ȯ���϶� 
			if(obj)
				return obj;
		}
		
		//Nothing found. �ƹ��͵� ���� 
		return NULL;
	}
};
 
//The ThemeStack that is be used by the GameState. GameState�� ���� ���� ThemeStack
extern ThemeStack objThemes;

#endif
