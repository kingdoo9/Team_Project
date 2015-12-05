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

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "Globals.h"
#include "FileManager.h"
#include "Functions.h"
#include <archive.h>
#include <archive_entry.h>
using namespace std;

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <direct.h>
#pragma comment(lib,"shlwapi.lib")
#else
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#endif


#ifdef WIN32
string userPath,dataPath,appPath,exeName;
#else
//다른 플랫폼에서 userPath (설정 파일) 및 userDataPath (데이터 파일) 사이의 차이를 확인
//캐시 데이터 userCachePath의 경로가있다.
string userPath,userDataPath,userCachePath,dataPath,appPath,exeName;
#endif

bool configurePaths() {
	//appPath과 EXENAME를 가져옵니다.
	{
		char s[4096];
		int i,m;
		#ifdef WIN32
		m=GetModuleFileNameA(NULL,s,sizeof(s));
		#else
		m=readlink("/proc/self/exe",s,sizeof(s));
		#endif
		s[m]=0;
		for(i=m-1;i>=0;i--){
			if(s[i]=='/'||s[i]=='\\'){
				s[i]=0;
				break;
			}
		}
		appPath=s;
		exeName=s+i+1;
	}
	
	//TODO : userpath이 userPath을 설정하기 전에 비어 있는지 확인
	//userPath이 비어 있는지 확인합니다.
	if(getUserPath().empty()){
#ifdef WIN32
		//userPath를 가져옵니다.
		char s[1024];
		SHGetSpecialFolderPathA(NULL,s,CSIDL_PERSONAL,1);
		userPath=s;
		userPath+="\\My Games\\meandmyshadow\\";		
#else
		//NULL 할당을 방지하는 데 사용되는 임시 변수.
		char* env;
		
		//$XDG_CONFIG_HOME env var를 얻는다
		env=getenv("XDG_CONFIG_HOME");
		//그것이 $ HOME / .config /에 널 (null) 설정 userPath에 있다면.
		if(env!=NULL){
			userPath=env;
		}else{
			userPath=getenv("HOME");
			userPath+="/.config";
		}
		//userPath에 meandmyshadow를 추가 할 수 있습니다.
		userPath+="/meandmyshadow/";
		
		//$XDG_DATA_HOME env var를 얻는다.
		env=getenv("XDG_DATA_HOME");
		//$ HOME /.local/share에 널 (null) 설정 userDataPath이 있다면.
		if(env!=NULL){
			userDataPath=env;
		}else{
			userDataPath=getenv("HOME");
			userDataPath+="/.local/share";
		}
		//userPath에 meandmyshadow를 추가
		userDataPath+="/meandmyshadow/";
		
		//$XDG_CACHE_HOME env var를 얻는다.
		env=getenv("XDG_CACHE_HOME");
		//$ HOME / .cache에 널 (null) 설정 userCachePath이 있다면.
		if(env!=NULL){
			userCachePath=env;
		}else{
			userCachePath=getenv("HOME");
			userCachePath+="/.cache";
		}
		//AuserPath에 meandmyshadow를 추가 할 수 있습니다.
		userCachePath+="/meandmyshadow/";
		
		//null 설정
		env=NULL;
#endif
		
		//userPath을 출력.
		cout<<"User preferences will be fetched from: "<<userPath<<endl;
#ifndef WIN32
		//비 Windows 시스템의 경우에는 사용자 데이터의 경로를 나타낸다.
		cout<<"User data will be fetched from: "<<userDataPath<<endl;
#endif
	}

#ifdef WIN32
	//userPath 폴더와 다른 하위 폴더를 만듭니다.
	createDirectory(userPath.c_str());
	createDirectory((userPath+"levels").c_str());
	createDirectory((userPath+"levelpacks").c_str());
	createDirectory((userPath+"themes").c_str());
	createDirectory((userPath+"progress").c_str());
	createDirectory((userPath+"tmp").c_str());
	//record 폴더
	createDirectory((userPath+"records").c_str());
	createDirectory((userPath+"records\\autosave").c_str());
	//그리고 userpath 내부의 사용자 지정 폴더.
	createDirectory((userPath+"custom").c_str());
	createDirectory((userPath+"custom\\levels").c_str());
	createDirectory((userPath+"custom\\levelpacks").c_str());
#else
	// userPath 생성.
	createDirectory(userPath.c_str());
	createDirectory(userDataPath.c_str());
	createDirectory(userCachePath.c_str());
	//또한 userpath에 다른 폴더를 만들 수 있습니다.
	createDirectory((userDataPath+"/levels").c_str());
	createDirectory((userDataPath+"/levelpacks").c_str());
	createDirectory((userDataPath+"/themes").c_str());
	createDirectory((userDataPath+"/progress").c_str());
	createDirectory((userCachePath+"/tmp").c_str());
	//record 폴더
	createDirectory((userDataPath+"/records").c_str());
	createDirectory((userDataPath+"/records/autosave").c_str());
	//그리고 userpath 내부의 사용자 지정 폴더.
	createDirectory((userDataPath+"/custom").c_str());
	createDirectory((userDataPath+"/custom/levels").c_str());
	createDirectory((userDataPath+"/custom/levelpacks").c_str());
#endif

	//복수의 상대 위치를 시도하여 데이터 경로를 가져옵니다.
	{
		FILE *f;
		string s;
		while(true){
			//시도
			if(!dataPath.empty()){
				s=dataPath+"font/knewave.ttf";
				if((f=fopen(s.c_str(),"rb"))!=NULL){
					fclose(f);
					break;
				}
			}
			//try "./"
			dataPath="./data/";
			s=dataPath+"font/knewave.ttf";
			if((f=fopen(s.c_str(),"rb"))!=NULL){
				fclose(f);
				break;
			}
			//try "../"
			dataPath="../data/";
			s=dataPath+"font/knewave.ttf";
			if((f=fopen(s.c_str(),"rb"))!=NULL){
				fclose(f);
				break;
			}
			//try App.Path
			dataPath=getAppPath()+"/data/";
			s=dataPath+"font/knewave.ttf";
			if((f=fopen(s.c_str(),"rb"))!=NULL){
				fclose(f);
				break;
			}
			//try App.Path+"/../"
			dataPath=getAppPath()+"/../data/";
			s=dataPath+"font/knewave.ttf";
			if((f=fopen(s.c_str(),"rb"))!=NULL){
				fclose(f);
				break;
			}
			//try DATA_PATH
#ifdef DATA_PATH
			dataPath=DATA_PATH;
			s=dataPath+"font/knewave.ttf";
			if((f=fopen(s.c_str(),"rb"))!=NULL){
				fclose(f);
				break;
			}
#endif
			//오류 : 파일을 찾을 수 없습니다
			return false;
		}

		//데이터 경로를 인쇄합니다.
		cout<<"Data files will be fetched from: "<<dataPath<<endl;
	}
	return true;
}

std::vector<std::string> enumAllFiles(std::string path,const char* extension,bool containsPath){
	vector<string> v;
#ifdef WIN32
	string s1;
	WIN32_FIND_DATAA f;
	if(!path.empty()){
		char c=path[path.size()-1];
		if(c!='/'&&c!='\\') path+="\\";
	}
	s1=path;
	if(extension!=NULL && *extension){
		s1+="*.";
		s1+=extension;
	}else{
		s1+="*";
	}
	HANDLE h=FindFirstFileA(s1.c_str(),&f);
	if(h==NULL||h==INVALID_HANDLE_VALUE) return v;
	do{
		if(!(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
			if(containsPath){
				v.push_back(path+f.cFileName);
			}else{
				v.push_back(f.cFileName);
			}
		}
	}while(FindNextFileA(h,&f));
	FindClose(h);
	return v;
#else
	int len=0;
	if(extension!=NULL && *extension) len=strlen(extension);
	if(!path.empty()){
		char c=path[path.size()-1];
		if(c!='/'&&c!='\\') path+="/";
	}
	DIR *pDir;
	struct dirent *pDirent;
	pDir=opendir(path.c_str());
	if(pDir==NULL) return v;
	while((pDirent=readdir(pDir))!=NULL){
		if(pDirent->d_name[0]=='.'){
			if(pDirent->d_name[1]==0||
				(pDirent->d_name[1]=='.'&&pDirent->d_name[2]==0)) continue;
		}
		string s1=path+pDirent->d_name;
		struct stat S_stat;
		lstat(s1.c_str(),&S_stat);
		if(!S_ISDIR(S_stat.st_mode)){
			if(len>0){
				if((int)s1.size()<len+1) continue;
				if(s1[s1.size()-len-1]!='.') continue;
				if(strcasecmp(&s1[s1.size()-len],extension)) continue;
			}

			if(containsPath){
				v.push_back(s1);
			}else{
				v.push_back(string(pDirent->d_name));
			}
		}
	}
	closedir(pDir);
	return v;
#endif
}

std::vector<std::string> enumAllDirs(std::string path,bool containsPath){
	vector<string> v;
#ifdef WIN32
	string s1;
	WIN32_FIND_DATAA f;
	if(!path.empty()){
		char c=path[path.size()-1];
		if(c!='/'&&c!='\\') path+="\\";
	}
	s1=path+"*";
	HANDLE h=FindFirstFileA(s1.c_str(),&f);
	if(h==NULL||h==INVALID_HANDLE_VALUE) return v;
	do{
		// skip '.' and '..' and hidden folders
		if(f.cFileName[0]=='.'){
			/*if(f.cFileName[1]==0||
				(f.cFileName[1]=='.'&&f.cFileName[2]==0))*/ continue;
		}
		if(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			if(containsPath){
				v.push_back(path+f.cFileName);
			}else{
				v.push_back(f.cFileName);
			}
		}
	}while(FindNextFileA(h,&f));
	FindClose(h);
	return v;
#else
	if(!path.empty()){
		char c=path[path.size()-1];
		if(c!='/'&&c!='\\') path+="/";
	}
	DIR *pDir;
	struct dirent *pDirent;
	pDir=opendir(path.c_str());
	if(pDir==NULL) return v;
	while((pDirent=readdir(pDir))!=NULL){
		if(pDirent->d_name[0]=='.'){
			if(pDirent->d_name[1]==0||
				(pDirent->d_name[1]=='.'&&pDirent->d_name[2]==0)) continue;
		}
		string s1=path+pDirent->d_name;
		struct stat S_stat;
		lstat(s1.c_str(),&S_stat);
		if(S_ISDIR(S_stat.st_mode)){
			//숨겨진 폴더를 건너 뜁니다.
			s1=string(pDirent->d_name);
			if(s1.find('.')==0) continue;
			
			//벡터에 결과를 추가합니다.
			if(containsPath){
				v.push_back(path+pDirent->d_name);
			}else{
				v.push_back(s1);
			}
		}
	}
	closedir(pDir);
	return v;
#endif
}

std::string processFileName(const std::string& s){
	string prefix=dataPath;
  
	//FIXME: Do we still need those last three?
	//REMARK: maybe 'return prefix+s;' is not needed (?)
	// 그러한 수준의 진행 상황을 저장할 수 없습니다 같은 몇 가지 버그가 발생합니다
	if(s.compare(0,6,"%DATA%")==0){
		if(s.size()>6 && (s[6]=='/' || s[6]=='\\')){
			return dataPath+s.substr(7);
		}else{
			return dataPath+s.substr(6);
		}
	}else if(s.compare(0,6,"%USER%")==0){
		if(s.size()>6 && (s[6]=='/' || s[6]=='\\')){
			return getUserPath(USER_DATA)+s.substr(7);
		}else{
			return getUserPath(USER_DATA)+s.substr(6);
		}
	}else if(s.compare(0,9,"%LVLPACK%")==0){
		if(s.size()>9 && (s[9]=='/' || s[9]=='\\')){
			return prefix+"levelpacks/"+s.substr(10);
		}else{
			return prefix+"levelpacks/"+s.substr(9);
		}
	}else if(s.compare(0,5,"%LVL%")==0){
		if(s.size()>5 && (s[5]=='/' || s[5]=='\\')){
			return prefix+"levels/"+s.substr(6);
		}else{
			return prefix+"levels/"+s.substr(5);
		}
	}else if(s.compare(0,8,"%THEMES%")==0){
		if(s.size()>8 && (s[8]=='/' || s[8]=='\\')){
			return prefix+"themes/"+s.substr(9);
		}else{
			return prefix+"themes/"+s.substr(8);
		}
	}else if(s.size()>0 && (s[0]=='/' || s[0]=='\\')){
		return s;
#ifdef WIN32
	// Another fix for Windows :(
	}else if(s.size()>1 && (s[1]==':')){
		return s;
#endif
	}else{
		return prefix+s;
	}
}

std::string fileNameFromPath(const std::string &path, const bool webURL){
	std::string filename;
	size_t pos;
#ifdef WIN32
	// FIXME: '/' in string should be '/' not '\/',
	// we don't need to escape it
	if(webURL){
		pos = path.find_last_of("/");
	}else{
		// 참고 : 때때로 Windows의 경로 구분은 '/'를 할 수있다,
		// 그래서 우리는 '\'와 '/'를 확인해야한다 
		pos = path.find_last_of("\\/");
	}
#else
	// FIXME: '/' in string should be '/' not '\/',
	// we don't need to escape it
	pos = path.find_last_of("/");
#endif
	if(pos != std::string::npos)
		filename.assign(path.begin() + pos + 1, path.end());
	else
		filename=path;
	
	return filename;
}

std::string pathFromFileName(const std::string &filename){
	std::string path;
	// FIXME: '/' in string should be '/' not '\/',
	// we don't need to escape it
#ifdef WIN32
	// 참고 : 때때로 Windows의 경로 구분은 '/'를 할 수있다,
	//  그래서 우리는 '\'와 '/'를 확인해야한다 
	size_t pos = filename.find_last_of("\\/");
#else
	size_t pos = filename.find_last_of("/");
#endif
	if(pos != std::string::npos)
		path.assign(filename.begin(), filename.begin() + pos +1);
	else
		path=filename;
	
	return path;
}

bool downloadFile(const string &path, const string &destination) {
	string filename=fileNameFromPath(path,true);
	
	FILE* file = fopen((destination+filename).c_str(), "wb");
	bool status=downloadFile(path,file);
	fclose(file);
	
	//그리고 상태를 반환합니다.
	return status;
}

bool downloadFile(const string &path, FILE* destination) {
	CURL* curl=curl_easy_init();

	// 프록시 테스트 (테스트 전용)
	string internetProxy = getSettings()->getValue("internet-proxy");
	size_t pos = internetProxy.find_first_of(":");
	if(pos!=string::npos){
		curl_easy_setopt(curl,CURLOPT_PROXYPORT,atoi(internetProxy.substr(pos+1).c_str()));
		internetProxy = internetProxy.substr(0,pos);
		curl_easy_setopt(curl,CURLOPT_PROXY,internetProxy.c_str());
	}

	curl_easy_setopt(curl,CURLOPT_URL,path.c_str());
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writeData);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,destination);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	
	return (res==0);
}

size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream){
	return fwrite(ptr, size, nmemb, (FILE *)stream);
}


bool extractFile(const string &fileName, const string &destination) {
	//우리가 추출하는 아카이브를 만듭니다.
	archive* file=NULL;
	//우리가 추출하는 대상을 만듭니다.
	archive* dest=NULL;
	
	file=archive_read_new();
	dest=archive_write_disk_new();
	archive_write_disk_set_options(dest, ARCHIVE_EXTRACT_TIME);
	
	archive_read_support_format_zip(file);
	
	//이제 아카이브를 읽는다.
	if(archive_read_open_file(file,fileName.c_str(),10240)) {
		cerr<<"Error while reading archive "+fileName<<endl;
		return false;
	}
	
	//이제 디스크에있는 모든 항목을 작성합니다.
	int status;
	archive_entry* entry=NULL;
	while(true) {
		status=archive_read_next_header(file,&entry);
		if(status==ARCHIVE_EOF){
			break;
		}
		if(status!=ARCHIVE_OK){
			cerr<<"Error while reading archive "+fileName<<endl;
			return false;
		}
		archive_entry_copy_pathname(entry,(destination+archive_entry_pathname(entry)).c_str());
		
		status=archive_write_header(dest,entry);
		if(status!=ARCHIVE_OK){
			cerr<<"Error while extracting archive "+fileName<<endl;
			return false;
		}else{
			copyData(file, dest);
			status=archive_write_finish_entry(dest);
			if(status!=ARCHIVE_OK){
				cerr<<"Error while extracting archive "+fileName<<endl;
				return false;
			}

		}
	}
	
	//마지막으로 아카이브를 닫습니다.
	archive_read_close(file);
	archive_read_finish(file);
	return true;
}

bool createDirectory(const char* path){
#ifdef WIN32
	char s0[1024],s[1024];

	GetCurrentDirectoryA(sizeof(s0),s0);
	PathCombineA(s,s0,path);

	for(unsigned int i=0;i<sizeof(s);i++){
		if(s[i]=='\0') break;
		else if(s[i]=='/') s[i]='\\';
	}

	//printf("createDirectory:%s\n",s);
	return SHCreateDirectoryExA(NULL,s,NULL)!=0;
#else
	return mkdir(path,0777)==0;
#endif
}

bool removeDirectory(const char *path){
#ifdef WIN32
	WIN32_FIND_DATAA f;
	HANDLE h = FindFirstFileA((string(path)+"\\*").c_str(),&f);
#else
	//제거 할 필요가있는 디렉토리를 엽니다.
	DIR* d=opendir(path);
#endif
	//경로 길이를 가져옵니다
	size_t path_len = strlen(path);
	//디렉토리가 비어있는 경우 부울.
	//True: succees		False: failure
	//디렉토리가 빈 상태 (empty)의 경우 while 루프를 입력하지 않습니다,하지만 성공을 가지고 있기 때문에 기본값은 true.
	bool r = true;

#ifdef WIN32
	if(h!=NULL && h!=INVALID_HANDLE_VALUE) {
#else
	//디렉토리가 존재하는지 확인합니다.
	if(d) {
		//디렉토리 엔트리의 포인터.
		struct dirent* p;
#endif

#ifdef WIN32
		do{
#else
		//오류가 없습니다 같은 제거 할 필요가있는 디렉토리의 항목들을 반복해라 
		while(r && (p=readdir(d))) {
#endif

			// Skip the names "." and ".." as we don't want to recurse on them.
#ifdef WIN32
			if (!strcmp(f.cFileName, ".") || !strcmp(f.cFileName, "..")) {
#else
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
#endif
				//The filename is . or .. so we continue to the next entry.
				continue;
			} else {
				//항목이 삭제 된 경우 R2
				//True: succees		False: failure
				//기본값은 false입니다.
				bool r2 = false;
				char* buf;
				size_t len;

#ifdef WIN32
				//경로 + 디렉토리 엔트리 이름의 길이를 가져옵니다.
				len = path_len + strlen(f.cFileName) + 2; 
#else
				//경로 + 디렉토리 엔트리 이름의 길이를 가져옵니다.
				len = path_len + strlen(p->d_name) + 2; 
#endif
				buf = (char*) malloc(len);

				if(buf) {
#ifdef WIN32
					_snprintf(buf, len, "%s\\%s", path, f.cFileName);

					if(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
						r2 = removeDirectory(buf);
					}else{
						r2 = unlink(buf)==0;
					}
#else
					struct stat statbuf;
					snprintf(buf, len, "%s/%s", path, p->d_name);

					if(!stat(buf, &statbuf)){
						//항목이 디렉토리 나 파일인지 확인합니다.
						if (S_ISDIR(statbuf.st_mode)){
							//반복적으로 해결할 (removeDirectory)호출
							//성공에 true를 돌려줍니다.
							r2 = removeDirectory(buf);
						}else{
							//unlink() returns zero on succes so we set r2 to the unlink(buf)==0.
							r2 = unlink(buf)==0;
						}
					}
#endif
					//buf free
					free(buf);
				}
				//R2는 최근 삭제의 상태를 포함하고 있기 때문에 R2로 R을 설정합니다.
				r = r2;
			}
#ifdef WIN32
		}while(r && FindNextFileA(h,&f));
		FindClose(h);
#else
		}
		//디렉토리를 닫습니다.
		closedir(d);
#endif
	}
	
	//while 루프는 우리가 (시도) 의미, 종료는 디렉토리를 삭제.
	//r은 오류를 의미한다면, r은 에러없음을 의미. 디렉토리를 삭제할 수 있습니다.
	if(r){
		//이 때 RmDir을 성공의 반환 값은 0이다.
		r = rmdir(path)==0;
	}
	
	//상태를 돌려줍니다.
	return r;
}

bool renameDirectory(const char* oldPath,const char* newPath){
	return rename(oldPath,newPath)==0;
}


void copyData(archive* file, archive* dest) {
	int status;
	const void* buff;
	size_t size;
#if ARCHIVE_VERSION_NUMBER < 3000000
	off_t offset;
#else
	int64_t offset;
#endif


	while(true) {
		status=archive_read_data_block(file, &buff, &size, &offset);
		if(status==ARCHIVE_EOF){
			return;
		}
		if(status!=ARCHIVE_OK){
			cerr<<"Error while writing data to disk."<<endl;
			return;
		}
		status=archive_write_data_block(dest, buff, size, offset);
		if(status!=ARCHIVE_OK) {
			cerr<<"Error while writing data to disk."<<endl;
			return;
		}
	}
}

bool copyFile(const char* source,const char* dest){
	//소스 파일을 엽니다.
	ifstream fin(source,fstream::binary);
	if(!fin)
		return false;
	
	//DEST 파일을 엽니다.
	ofstream fout(dest,fstream::trunc|fstream::binary);
	if(!fout)
		return false;
	
	//복사
	fout<<fin.rdbuf();
	return true;
}

bool removeFile(const char* file){
	return remove(file)==0;
}

bool createFile(const char* file){
	//쓰기 권한이있는 파일을 엽니 다.
	FILE* f=fopen(file,"wb");
	
	//아무런 문제가없는 경우 확인합니다.
	if(f){
		//파일닫기
		fclose(f);
		return true;
	}else{
		return false;
	}
}
