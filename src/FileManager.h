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

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

//extractFile ����� ���� ���ԵǾ� �ֽ��ϴ�.
#include <archive.h>
//downloadFile ����� ���� ���ԵǾ� �ֽ��ϴ�.
#include <curl/curl.h>


//���� : ��� ����� ó�� ��� �̸��� �Բ� �۵��մϴ�.
//So %DATA%, %USER%, ���� ������ �ʽ��ϴ�
//With exception of processFileName().

��� ���� ��ġ���ؾ� �� �� ���� ���ڿ��� //.
//  userPath = ����� ���� (�ֵ��, ����)�� ����˴ϴ�.
// EXENAME=  ���� ������ �̸�
// dataPath = ������ �������ִ� ���
// appPath =  ���� �������ִ� ���
extern std::string userPath,exeName,dataPath,appPath;

// ������ ���� �� ���� ��ΰ� non- Windows �ý����� ������.
//userDataPath =����� ���� ������ �� ����ڿ� ���� ���,  ������ (�ֵ��)�� �ٿ�ε�.
//userCachePath = �ӽ÷� ���� �� ���.
#ifndef WIN32
extern std::string userDataPath,userCachePath;
#endif

// �ٸ� userPath ���� ������ ENUM.
// ���� : non- Windows �÷����� �ʿ� ..
enum UserPaths{
	//���� ������ �����ϴ� userpath.
	//�⺻�� $HOME/.config/meandmyshadow/
	USER_CONFIG,
	//����� �����͸� ���� userpath.
	//�⺻�� $HOME/.local/share/meandmyshadow/
	USER_DATA,
	//�ӽ� ������ ���� userpath.
	//�⺻�� $HOME/.cache/meandmyshadow/
	USER_CACHE
};

//userPath�� �˻��ϱ����� ���.
//type: The type of userpath to return, only used on non-Windows platforms.
//Returns: The userPath.
inline const std::string& getUserPath(int type=0){
#ifdef WIN32
	return userPath;
#else
	switch(type){
		case USER_CONFIG:
			return userPath;
			break;
		case USER_DATA:
			return userDataPath;
			break;
		case USER_CACHE:
			return userCachePath;
			break;
		default:
			std::cerr<<"WARNING: Illegal userpath type, returning user config path."<<std::endl;
			return userPath;
			break;
	}
#endif
}
//EXENAME�� ����ϱ����� ���.
//Returns: The exeName.
inline const std::string& getEXEName(){
	return exeName;
}
//dataPath�� ����ϱ����� ���.
//Returns: The dataPath.
inline const std::string& getDataPath(){
	return dataPath;
}
//appPath�� ����ϱ����� ���.
//Returns: The appPath.
inline const std::string& getAppPath(){
	return appPath;
}

//�� ����� userPath, ������ ���, appPath �� EXENAME�� ���� ��θ� ã�� ���� ����ϴ� �κ�.
//Returns: True if nothing went wrong.
bool configurePaths();

//Ư�� ���丮���ִ� ��� ������ ����� ��ȯ�ϴ� ���.
//��� : ����� ������ �����մϴ�.
//Ȯ���� : ������ �־�� Ȯ��.
//containsPath�� : ���� ���� �̸��� ��θ� �������� ���θ� �����մϴ�.
//��� : ���� �̸��� �����ϴ� ����.
std::vector<std::string> enumAllFiles(std::string path,const char* extension=NULL,bool containsPath=false);
//Ư�� ���丮���ִ� ��� ���丮�� ����� ��ȯ�ϴ� ���.
//��� : ����� ���丮�� �����մϴ�.
//containsPath�� : ���� ���� �̸��� ��θ� �������� ���θ� �����մϴ�.
//��ȯ �� : ���丮�� �̸��� �����ϴ� ����.
std::vector<std::string> enumAllDirs(std::string path,bool containsPath=false);

//���ڿ��� �ؼ��ϴ� ���.
//��ο� ���� ��ȯ�Ѵ� %USER%, %DATA%, ��.
//S : ó�� �� �ʿ䰡�ִ� ���ڿ��Դϴ�.
//��ȯ �� : ó�� �� ���ڿ��Դϴ�.
std::string processFileName(const std::string& s);
//����� ��ü ��ο��� ���� �̸��� �˻��ϴ� �� ���.
//��� : ���� �̸��� ����Դϴ�.
//WEBURL : ��ΰ� WEBURL ��� �ο�.
//��ȯ �� : ���ڿ� ���� �̸��� ����.
std::string fileNameFromPath(const std::string &path, const bool webURL=false);
//����� ��ü ��ο��� ���� �̸����� ��θ� �˻��ϴ� �� ���.
//���� �̸� : ���� �̸��� ����Դϴ�.
//��ȯ �� : ��ΰ� ���� �� ���ڿ�.
std::string pathFromFileName(const std::string &filename);

//������ �ٿ�ε� �ϴ� ���.
//��� : ������ �ٿ�ε��մϴ�.
//��� : ������ �ٿ�ε� �� ��� ���.
//��ȯ �� :�� �������� �����ϸ� ��.
bool downloadFile(const std::string &path, const std::string &destination);
//������ �ٿ�ε� �� ���.
//��� : ������ �ٿ�ε��մϴ�.
//��� : ������Ͽ� �ٿ�ε� �� ���Ͽ� ��ϵ˴ϴ�
//��ȯ �� :�� �������� �����ϸ� ��.
bool downloadFile(const std::string &path, FILE* destination);
//�ÿ��� ����ϴ� ����� �������� ����� �����մϴ�.
size_t writeData(void* ptr,size_t size,size_t nmemb,void* stream);

// ��ī�̺긦 �����Ͽ� ��� ������ ��ġ�ϴ� ���.
// ���� �̸� : ��ī�̺��� �̸��Դϴ�.
// ��� : �����ġ�� ���� �� ������ ��
// ��ȯ �� :�������� �����ϸ� ��.
bool extractFile(const std::string &fileName, const std::string &destination);
// ��ī�̺꿡�� ������ ����� �а� ��ī�̺꿡 ����ϴ� �� ���Ǵ� ���
 // ���� :��ī�̺긦 ���� �� �ֽ��ϴ�.
 // dest : ��ī�̺꿡 ����մϴ�.
void copyData(archive* file, archive* dest);

// ���丮�� ����� ���.
 // ��� : ���丮�� ����ϴ�.
 // ��ȯ �� : �����ϸ� ��.
bool createDirectory(const char* path);
// ���丮�� �����ϴ� ���.
// ��� : ���丮�� �����մϴ�.
// ��ȯ �� : �����ϸ� ��.
bool removeDirectory(const char* path);
// ���丮�� �̸��� �ٲٴ� ���.
 // oldpath : ���� ���.
 // newpath : ��� ���� �̸�
 // ��ȯ �� : �����ϸ� ��.
bool renameDirectory(const char* oldPath,const char* newPath);

// ������ ����� ���.
// ���� : ������ ���� �̸��� ����ϴ�.
// ��ȯ �� : �����ϸ� ��.
bool createFile(const char* file);
// ������ �����ϴ� ���.
// �ҽ� : �Է� ����.
 // ��� : ��� ����.
 // ��ȯ �� : �����ϸ� ��.
bool copyFile(const char* source,const char* dest);
// ������ �����ϴ� ���.
// ���� : ������ �����մϴ�.
// ��ȯ �� : �����ϸ� ��.
bool removeFile(const char* file);

#endif
