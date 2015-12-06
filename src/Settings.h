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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <ctime>

//설정 클래스 구성 파일을 저장하고 설정을 추적 / 로딩을 위해 사용된다.
class Settings{
private:
	//settingsfile의 이름입니다.
	const std::string fileName;
	
	//모든 설정을 포함하는지도.
 // 키 설정의 이름이다.
	std::map<std::string,std::string> settings;
	
	//설정 파일을 만드는 방법
	void createFile();
	// 주어진 줄을 구문 분석하고, 유효한 경우, 설정지도에 추가합니다.
 // 라인 : 라인이 구문 분석합니다.
	void parseLine(const std::string &line);
	// 주어진 라인을 확인합니다. (이 포함되어 있는지 확인 '=')
	// 라인 : 라인의 유효성을 검사합니다.
	// 반환 값 : 라인이 유효한 경우는 true.
	bool validLine(const std::string &line);
	주어진 행에서 주석을 제거 //.
 // 주석이 '#'으로 시작 라인의 끝까지 계속된다.
 // 라인 : 라인의 주석을 해제합니다.
	void unComment(std::string &line);
	//체크 라인이 비어있는 경우 확인
 // 라인 : 라인을 확인합니다.
 // 반환 값 : 라인이 emtpy 경우는 true.
	bool empty(const std::string &line);
public:
	//Constructor.
	//파일 이름 : 파일의 이름을 사용합니다.
	Settings(std::string fileName);
	
	//이 방법은 설정 파일을 열고,  라인 라인을 통해 안내합니다.
	void parseFile();
	//이 방법은 설정 파일에 맵의 설정을 기록합니다.
	void save();
	

	std::string getValue(const std::string &key);
	// 저장된 키에 속하는 부울을 값으로 가져옴
	// 키 : TEH 설정의 이름입니다. 
	// Returns: The (bool)value if the key exists and false if the key doesn't exist.
	bool getBoolValue(const std::string &key);
	
	// 이는 주어진 키의 값을 설정한다.
 //주의 : 키가 존재하지 않는 경우는 설정되지 않을 것이다. //FIXME: If the key doesn't exist why not create it?

 // 키 설정의 키를 변경합니다.
 // 값 : 키의 새로운 값.
	void setValue(const std::string &key, const std::string &value);
};

#endif