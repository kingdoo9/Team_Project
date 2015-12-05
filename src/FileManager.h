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

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

//extractFile 방법에 대한 포함되어 있습니다.
#include <archive.h>
//downloadFile 방법에 대한 포함되어 있습니다.
#include <curl/curl.h>


//참고 : 모든 방법을 처리 경로 이름과 함께 작동합니다.
//So %DATA%, %USER%, 등은 사용되지 않습니다
//With exception of processFileName().

모든 파일 위치와해야 할 몇 가지 문자열을 //.
//  userPath = 사용자 파일 (애드온, 설정)에 저장됩니다.
// EXENAME=  실행 파일의 이름
// dataPath = 데이터 파일이있는 경로
// appPath =  실행 파일이있는 경로
extern std::string userPath,exeName,dataPath,appPath;

// 다음과 같은 두 가지 경로가 non- Windows 시스템을 따른다.
//userDataPath =사용자 제작 콘텐츠 및 사용자에 대한 경로,  콘텐츠 (애드온)을 다운로드.
//userCachePath = 임시로 저장 될 경로.
#ifndef WIN32
extern std::string userDataPath,userCachePath;
#endif

// 다른 userPath 형을 포함한 ENUM.
// 참고 : non- Windows 플랫폼에 필요 ..
enum UserPaths{
	//설정 파일을 포함하는 userpath.
	//기본값 $HOME/.config/meandmyshadow/
	USER_CONFIG,
	//사용자 데이터를 포함 userpath.
	//기본값 $HOME/.local/share/meandmyshadow/
	USER_DATA,
	//임시 파일을 포함 userpath.
	//기본값 $HOME/.cache/meandmyshadow/
	USER_CACHE
};

//userPath를 검색하기위한 방법.
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
//EXENAME를 취득하기위한 방법.
//Returns: The exeName.
inline const std::string& getEXEName(){
	return exeName;
}
//dataPath를 취득하기위한 방법.
//Returns: The dataPath.
inline const std::string& getDataPath(){
	return dataPath;
}
//appPath를 취득하기위한 방법.
//Returns: The appPath.
inline const std::string& getAppPath(){
	return appPath;
}

//이 방법은 userPath, 데이터 경로, appPath 및 EXENAME에 대한 경로를 찾기 위해 노력하는 부분.
//Returns: True if nothing went wrong.
bool configurePaths();

//특정 디렉토리에있는 모든 파일의 목록을 반환하는 방법.
//경로 : 경로의 파일을 나열합니다.
//확장자 : 파일이 있어야 확장.
//containsPath는 : 리턴 파일 이름이 경로를 포함할지 여부를 지정합니다.
//결과 : 파일 이름을 포함하는 벡터.
std::vector<std::string> enumAllFiles(std::string path,const char* extension=NULL,bool containsPath=false);
//특정 디렉토리에있는 모든 디렉토리의 목록을 반환하는 방법.
//경로 : 경로의 디렉토리를 나열합니다.
//containsPath는 : 리턴 파일 이름이 경로를 포함할지 여부를 지정합니다.
//반환 값 : 디렉토리의 이름을 포함하는 벡터.
std::vector<std::string> enumAllDirs(std::string path,bool containsPath=false);

//문자열을 해석하는 방법.
//경로에 따라 변환한다 %USER%, %DATA%, 등.
//S : 처리 할 필요가있는 문자열입니다.
//반환 값 : 처리 된 문자열입니다.
std::string processFileName(const std::string& s);
//방법은 전체 경로에서 파일 이름을 검색하는 데 사용.
//경로 : 파일 이름과 경로입니다.
//WEBURL : 경로가 WEBURL 경우 부울.
//반환 값 : 문자열 파일 이름을 포함.
std::string fileNameFromPath(const std::string &path, const bool webURL=false);
//방법은 전체 경로에서 파일 이름없이 경로를 검색하는 데 사용.
//파일 이름 : 파일 이름과 경로입니다.
//반환 값 : 경로가 포함 된 문자열.
std::string pathFromFileName(const std::string &filename);

//파일을 다운로드 하는 방법.
//경로 : 파일을 다운로드합니다.
//대상 : 파일을 다운로드 할 대상 경로.
//반환 값 :이 오류없이 성공하면 참.
bool downloadFile(const std::string &path, const std::string &destination);
//파일을 다운로드 할 방법.
//경로 : 파일을 다운로드합니다.
//대상 : 대상파일에 다운로드 한 파일에 기록됩니다
//반환 값 :이 오류없이 성공하면 참.
bool downloadFile(const std::string &path, FILE* destination);
//컬에서 사용하는 방법은 데이터의 블록을 복사합니다.
size_t writeData(void* ptr,size_t size,size_t nmemb,void* stream);

// 아카이브를 추출하여 대상 폴더에 배치하는 방법.
// 파일 이름 : 아카이브의 이름입니다.
// 대상 : 대상위치에 추출 된 파일이 옴
// 반환 값 :오류없이 성공하면 참.
bool extractFile(const std::string &fileName, const std::string &destination);
// 아카이브에서 데이터 블록을 읽고 아카이브에 기록하는 데 사용되는 방법
 // 파일 :아카이브를 읽을 수 있습니다.
 // dest : 아카이브에 기록합니다.
void copyData(archive* file, archive* dest);

// 디렉토리를 만드는 방법.
 // 경로 : 디렉토리를 만듭니다.
 // 반환 값 : 성공하면 참.
bool createDirectory(const char* path);
// 디렉토리를 제거하는 방법.
// 경로 : 디렉토리를 제거합니다.
// 반환 값 : 성공하면 참.
bool removeDirectory(const char* path);
// 디렉토리의 이름을 바꾸는 방법.
 // oldpath : 폴더 경로.
 // newpath : 대상 폴더 이름
 // 반환 값 : 성공하면 참.
bool renameDirectory(const char* oldPath,const char* newPath);

// 파일을 만드는 방법.
// 파일 : 파일의 파일 이름을 만듭니다.
// 반환 값 : 성공하면 참.
bool createFile(const char* file);
// 파일을 복사하는 방법.
// 소스 : 입력 파일.
 // 대상 : 출력 파일.
 // 반환 값 : 성공하면 참.
bool copyFile(const char* source,const char* dest);
// 파일을 제거하는 방법.
// 파일 : 파일이 제거합니다.
// 반환 값 : 성공하면 참.
bool removeFile(const char* file);

#endif
