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

#ifndef ADDONS_H
#define ADDONS_H

#include "GameState.h"
#include "GameObjects.h"
#include "GUIObject.h"
#include "GUIListBox.h"
#include <vector>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

//addons menu.
class Addons: public GameState,public GUIEventCallback{
private:
	//An addon entry.
	struct Addon{
		//addon �̸�
		string name;
		//addon Ÿ��(Level, Levelpack, Theme)
		string type;
		//Taddon file ��ũ
		string file;
		//������ �׸��� levelpacks�� ���, �ֵ���� ��ġ�մϴ�.
		string folder;
		//������ �̸�
		string author;
		
		//�ֵ���� �ֽ� ����.
		int version;
		//��ġ �� ��� ����ڰ� ��ġ �� �����Դϴ�.
		int installedVersion;
		
		//�ֵ���� ��ġ�Ǿ��ִ� ��� �ο�.
		bool installed;
		//�ֵ���� �ֽ� ���� �� ��� �ο�. (installedVersion == ����)
		bool upToDate;
	};

	//The title.
	SDL_Surface* title;
	
	//��� �ֵ���� �����ϴ� ����.
	std::vector<Addon>* addons;
	
	//userpath�� �ֵ�� ������ ����Ű�� ����.
	FILE* addon;
	
	//���𰡰� ���� �� �� ������ �����ؾ� ���ڿ�.
	string error;
	
	//���� ���õ� �ֵ���� ����
	string type;
	//���� �ֵ���� ������.
	Addon* selected;
	
	//����� ��Ͽ� ���Ǵ� ����Դϴ�.
	GUIListBox* list;
	//install/uninstall ��ư
	GUIObject* actionButton;
	//������Ʈ�� ���Ǵ� ��ư
	GUIObject* updateButton;
	
	//The possible actions for the action button.
	//�� : ������Ʈ�� ������ ��ư���� �̵��ߴ�.
	enum Action{
		NONE, INSTALL, UNINSTALL
	};
	//������ �׼�.
	Action action;
public:
	//Constructor.
	Addons();
	//Destructor.
	~Addons();
	
	//GUI�� ����� ���.
	void createGUI();
	
	// �ֵ�� ����� �ٿ�ε� ���.
// ���� : ������ ���Ϸ� ����� �ٿ�ε��մϴ�.
// ��ȯ �� : ������ ���������� �ٿ�ǵ�Ǹ� Ʈ��
	bool getAddonsList(FILE* file);
	//
	void fillAddonList(std::vector<Addons::Addon> &list,TreeStorageNode &addons,TreeStorageNode &installed);
	// ���Ϳ� �־��� ������ ��� �ֵ���� �ֽ��ϴ�.
	// Ÿ�� : �ֵ���� �־���ϴ� Ÿ��
	// ��ȯ �� : �ֵ���� �����ϴ� ����.
	std::vector<std::string> addonsToList(const string &type);
	
	// installed_addons ���Ͽ� ��ġ�� �ֵ���� ���� �� �� ���.
 // ��ȯ �� : ������ ���������� ����Ǹ� Ʈ��
	bool saveInstalledAddons();
	
	//GameState���� ��ӵ˴ϴ�.
	void handleEvents();
	void logic();
	void render();
	void resize();
	
	// GUI �̺�Ʈ�� ó���ϱ� ���� ���Ǵ� ���.
// �̸� : �ݹ��� �̸�
// OBJ : �̺�Ʈ�� �߻���Ų GUIObject ������.
// EVENTTYPE : �̺�Ʈ ���� : Ŭ��, ����, ��
	void GUIEventCallback_OnEvent(std::string name,GUIObject* obj,int eventType);
	
	//������ �׼ǿ� �׼� ��ư�� �ؽ�Ʈ�� ������Ʈ�մϴ�.
	void updateActionButton();
	//������Ʈ ��ư�� Ȱ��ȭ ����ġ / ��Ȱ��ȭ
	void updateUpdateButton();
};
#endif
