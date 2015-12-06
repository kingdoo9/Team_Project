/* 60142233 ���ΰ�

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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <ctime>

//���� Ŭ���� ���� ������ �����ϰ� ������ ���� / �ε��� ���� ���ȴ�.
class Settings{
private:
	//settingsfile�� �̸��Դϴ�.
	const std::string fileName;
	
	//��� ������ �����ϴ�����.
 // Ű ������ �̸��̴�.
	std::map<std::string,std::string> settings;
	
	//���� ������ ����� ���
	void createFile();
	// �־��� ���� ���� �м��ϰ�, ��ȿ�� ���, ���������� �߰��մϴ�.
 // ���� : ������ ���� �м��մϴ�.
	void parseLine(const std::string &line);
	// �־��� ������ Ȯ���մϴ�. (�� ���ԵǾ� �ִ��� Ȯ�� '=')
	// ���� : ������ ��ȿ���� �˻��մϴ�.
	// ��ȯ �� : ������ ��ȿ�� ���� true.
	bool validLine(const std::string &line);
	�־��� �࿡�� �ּ��� ���� //.
 // �ּ��� '#'���� ���� ������ ������ ��ӵȴ�.
 // ���� : ������ �ּ��� �����մϴ�.
	void unComment(std::string &line);
	//üũ ������ ����ִ� ��� Ȯ��
 // ���� : ������ Ȯ���մϴ�.
 // ��ȯ �� : ������ emtpy ���� true.
	bool empty(const std::string &line);
public:
	//Constructor.
	//���� �̸� : ������ �̸��� ����մϴ�.
	Settings(std::string fileName);
	
	//�� ����� ���� ������ ����,  ���� ������ ���� �ȳ��մϴ�.
	void parseFile();
	//�� ����� ���� ���Ͽ� ���� ������ ����մϴ�.
	void save();
	

	std::string getValue(const std::string &key);
	// ����� Ű�� ���ϴ� �ο��� ������ ������
	// Ű : TEH ������ �̸��Դϴ�. 
	// Returns: The (bool)value if the key exists and false if the key doesn't exist.
	bool getBoolValue(const std::string &key);
	
	// �̴� �־��� Ű�� ���� �����Ѵ�.
 //���� : Ű�� �������� �ʴ� ���� �������� ���� ���̴�. //FIXME: If the key doesn't exist why not create it?

 // Ű ������ Ű�� �����մϴ�.
 // �� : Ű�� ���ο� ��.
	void setValue(const std::string &key, const std::string &value);
};

#endif