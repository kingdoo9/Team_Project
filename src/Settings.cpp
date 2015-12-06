/* 60142233 강민경
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

#include "Settings.h"
#include <SDL/SDL.h>
#include <string>
#include <stdio.h>
using namespace std;

Settings::Settings(const string fileName): fileName(fileName){
	char s[32];
	settings["sound"]="128";
	settings["music"]="128";
	settings["musiclist"]="default";
	settings["fullscreen"]="0";
	settings["width"]="800";
	settings["height"]="600";
	settings["resizable"]="1";
#ifdef HARDWARE_ACCELERATION
	settings["gl"]="1";
#else
	settings["gl"]="0";
#endif
	settings["theme"]="%DATA%/themes/Cloudscape";
	settings["leveltheme"]="1";
	settings["internet"]="1";
	settings["lastlevelpack"]="tutorial";
	settings["internet-proxy"]="";
	settings["lang"]="";

	//키 설정
	sprintf(s,"%d",(int)SDLK_UP);
	settings["key_up"]=settings["key_jump"]=s;

	sprintf(s,"%d",(int)SDLK_DOWN);
	settings["key_down"]=settings["key_action"]=s; //SDLK_DOWN

	sprintf(s,"%d",(int)SDLK_LEFT);
	settings["key_left"]=s; //SDLK_LEFT

	sprintf(s,"%d",(int)SDLK_RIGHT);
	settings["key_right"]=s; //SDLK_RIGHT

	sprintf(s,"%d",(int)SDLK_SPACE);
	settings["key_space"]=s; //SDLK_SPACE

	sprintf(s,"%d",(int)SDLK_ESCAPE);
	settings["key_escape"]=s; //SDLK_ESCAPE

	sprintf(s,"%d",(int)SDLK_r);
	settings["key_restart"]=s; //SDLK_r

	sprintf(s,"%d",(int)SDLK_TAB);
	settings["key_tab"]=s;

	sprintf(s,"%d",(int)SDLK_F2);
	settings["key_save"]=s; //SDLK_F2

	sprintf(s,"%d",(int)SDLK_F3);
	settings["key_load"]=s; //SDLK_F3

	sprintf(s,"%d",(int)SDLK_F4);
	settings["key_swap"]=s; //SDLK_F4

	sprintf(s,"%d",(int)SDLK_F5);
	settings["key_teleport"]=s; //SDLK_F5

	sprintf(s,"%d",(int)SDLK_F12);
	settings["key_suicide"]=s;

	sprintf(s,"%d",(int)SDLK_RSHIFT);
	settings["key_shift"]=s; //SDLK_RSHIFT
	
	sprintf(s,"%d",(int)SDLK_PAGEUP);
	settings["key_next"]=s; //SDLK_PAGEUP
	
	sprintf(s,"%d",(int)SDLK_PAGEDOWN);
	settings["key_previous"]=s; //SDLK_PAGEDOWN
	
	sprintf(s,"%d",(int)SDLK_RETURN);
	settings["key_select"]=s; //SDLK_RETURN

	settings["key_up2"]=settings["key_down2"]=settings["key_left2"]=settings["key_right2"]=
		settings["key_jump2"]=settings["key_action2"]=
		settings["key_space2"]=settings["key_escape2"]=settings["key_restart2"]=settings["key_tab2"]=
		settings["key_save2"]=settings["key_load2"]=settings["key_swap2"]=settings["key_teleport2"]=
		settings["key_suicide2"]=settings["key_shift2"]=settings["key_next2"]=settings["key_previous2"]=
		settings["key_select2"]="0";
}


void Settings::parseFile(){
	//세팅파일 열기
	ifstream file;

	file.open(fileName.c_str());
	if(!file){
		cout<<"Can't find config file!"<<endl;
		createFile();
	}

	//파일 라인을 따라 읽음
	string line;
	while(getline(file,line)){
		string temp = line;

		unComment(temp);
		if(temp.empty() || empty(temp))
			continue;
		
		//The line is good so we parse it.
		parseLine(temp);
	}

	//파일닫음
	file.close();
}

void Settings::parseLine(const string &line){
	if((line.find('=') == line.npos) || !validLine(line))
		cout<<"Warning illegal line in config file!"<<endl;
	
	string temp = line;
	temp.erase(0, temp.find_first_not_of("\t "));
	int seperator = temp.find('=');

	//키를 취득하고 다듬기
	string key, value;
	key = line.substr(0, seperator);
	if(key.find('\t')!=line.npos || key.find(' ')!=line.npos)
		key.erase(key.find_first_of("\t "));
	
	//값을 얻고 다듬기
	value = line.substr(seperator + 1);
	value.erase(0, value.find_first_not_of("\t "));
	value.erase(value.find_last_not_of("\t ") + 1);
	
	//설정 맵에 설정을 추가합니다.
	setValue(key,value);
}

bool Settings::validLine(const string &line){
	string temp = line;
	temp.erase(0, temp.find_first_not_of("\t "));
	if(temp[0] == '=')
		return false;

	for(size_t i = temp.find('=') + 1; i < temp.length(); i++)
		return true;
	return false;
}

void Settings::unComment(string &line){
	if (line.find('#') != line.npos)
		line.erase(line.find('#'));
}

bool Settings::empty(const string &line){
	return (line.find_first_not_of(' ')==line.npos);
}

string Settings::getValue(const string &key){
	if(settings.find(key) == settings.end()){
		cout<<"Key "<<key<<" couldn't be found!"<<endl;
		return "";
	}
	return settings[key];
}

bool Settings::getBoolValue(const string &key){
	if(settings.find(key) == settings.end()){
		cout<<"Key "<<key<<" couldn't be found!"<<endl;
		return false;
	}
	return (settings[key] != "0");
}

void Settings::setValue(const string &key, const string &value){
	if(settings.find(key) == settings.end()){
		cout<<"Key "<<key<<" couldn't be found!"<<endl;
		return;
	}
	settings[key]=value;
}

void Settings::createFile(){
	ofstream file;
	file.open(fileName.c_str());
	
	//기본 구성 파일.
	file<<"#MeAndMyShadow config file. Created on "<<endl;
	
	map<string, string>::iterator iter;
	for(iter = settings.begin(); iter != settings.end(); ++iter){
		file << iter->first << " = " << iter->second << endl;
	}
	
	//파일닫기
	file.close();
}

void Settings::save(){
	ofstream file;
	file.open(fileName.c_str());
	
	//먼저 날짜와 시간을 갖기
	time_t rawtime;
	struct tm* timedate;
	
	time(&rawtime);
	timedate=localtime(&rawtime);
	
	// 설정 파일의 첫 번째 줄에 기록.
// 참고 : asctime과 (timeinfo)에 이미 있기 때문에 마지막에 더 ENDL가 없다.
	file<<"#MeAndMyShadow config file. Created on "<<asctime(timedate);
	
	//설정을 통해 루프 및 저장합니다.
	map<string,string>::const_iterator iter;
	for(iter=settings.begin(); iter!=settings.end(); ++iter){
		file<<iter->first<<" = "<<iter->second<<endl;
	}
	file.close();
}
