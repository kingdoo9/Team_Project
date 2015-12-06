/*60142233 ���ΰ�
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
	//����� ���.
	ThemeBlockInstance appearance;
		
	//��� �繰�� ��� ������ ��� �� ���ִ� ����.
	int temp;
	//����� ���°� ���� �� �� �µ��� ���� ���庯��.
	int tempSave;
	
	//flags:
	//��ü 0x1�� �̵� = ��Ȱ��ȭ
	//button bit0-1=behavior 0x4=pressed
	//switch bit0-1=behavior
	int flags;
	//����� ���°� ���� �� �� �÷��׿� ���� ����.
	int flagsSave;

	//���� ����� �̵��մϴ�.
	//����� �̵��ϱ����� �����.
	SDL_Rect boxBase;
	//�̵� ����� ��ġ�� �����ϴ� ����.
	std::vector<SDL_Rect> movingPos;
	int dx;
	int xSave;
	int dy;
	int ySave;
	//�̵� ����� ���� �������� �ݺ��ϴ� ��� �ο�.
	//�⺻���� True
	bool loop;
	
	// ������ ����� ���.
	// �̵� ��ü 0x1 = ��Ȱ��ȭ
	// ���� 0x1 = �ڵ�
	// fragile = state
	int editorFlags;
public:
	//����� ID
	std::string id;
	//������ ����� ID�� �����ϴ� ���ڿ�
	std::string destination;
	//���� �޽����� ����� �����ϴ� ���ڿ�.
	std::string message;
	
	//Constructor.
	//x: The x location of the block.
	//y: The y location of the block.
	//objParent: Pointer to the Game object.
	Block(int x,int y,int type,Game* objParent);
	//Desturctor.
	~Block();

	//����� �׸��� �� ����ϴ� ���
	void show();

	// ������ ������ �ڽ��� ������
	// boxType : ��ȯ�Ǵ� ������ ����
	// ���� GameObjects.h�� ����
	// ��ȯ �� : ����.
	virtual SDL_Rect getBox(int boxType=BoxType_Current);
	
	// �޼ҵ� ����� ��ġ�� �����ϴµ� ���.
	// �̰� ���̽� �ڽ� x �� y ��ġ�� �����Ѵ�.
	//x: The new x location.
	//y: The new y location.
	virtual void setPosition(int x,int y);
	
	// ���߿� �ε� �� ���ִ� ����� ���¸� ����
	virtual void saveState();
	// ����� ����� ���¸��ε�
	virtual void loadState();
	//����� �缳��
	//save: Boolean if the saved state should also be deleted.
	virtual void reset(bool save);
	
	//Play an animation.
	//flags: TODO???
	virtual void playAnimation(int flags);
	
	// �̺�Ʈ������ �� ȣ��Ǵ� �޼���.
	 // EVENTTYPE : �̺�Ʈ ����
	 // eventtypes�� ���� GameObjects.h�� ����
	virtual void onEvent(int eventType);
	
	// ����� �Ӽ��� �˻��ϴ� �� ���Ǵ� ���
	 // propertyType : ��û �� �Ӽ��� ����
	 // GameObjects.h�� �����Ͻʽÿ�.
	 // OBJ : �÷��̾��� ������.
	 // ��ȯ �� : �Ӽ� ���� �����ϴ� ����.
	virtual int queryProperties(int propertyType,Player* obj);

	// ����� ������ �����͸� ������
 // OBJ : editorData���� ä������ ����
	virtual void getEditorData(std::vector<std::pair<std::string,std::string> >& obj);

	//��� ������ �����͸� ���� 
 // OBJ : ���ο� ������ ������.
	virtual void setEditorData(std::map<std::string,std::string>& obj);
	
	//�̵� ��� �Ǵ� ����� ��Ҹ� ������Ʈ�ϴ� ���
	virtual void move();
};

#endif
