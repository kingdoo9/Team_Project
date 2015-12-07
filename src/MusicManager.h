//신수빈
/*
 * Copyright (C) 2012 Me and My Shadow
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

#ifndef MUSICMANAGER_H
#define MUSICMANAGER_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <string>
#include <map>
#include <vector>

// 음악을 로딩해오거나 실행하기 위한 함수이다.
class MusicManager{
private:
	// 이 구조체는 음악 정보들을 담기위해 사용된다.
	struct Music{
		// 음악 포인터
		Mix_Music* music;

		// 음악이 반복된다면, 음악의 반복을 가르치는 포인터
		Mix_Music* loop;

		// string이 음악의 이름을 포함한다.
		// 음악모아놓은것과 같은 이름
		std::string name;


		// 음악 목록이름들을 포함한 string
		std::string trackName;

		// 저자의 이름을 포함하는 string
		std::string author;
		// 음악의 저작권을 포함하는 string
		std::string license;

		// 어디서 플레이를 시작한 시간을 포함하는 정수형
		int start;

		// 플레이할때 음악의 볼륨    범위는 0~128
		int volume;

		// 음악반복을 포함하는 정수형
		int loopStart;
		// 음악반복의 끝을 포함하는 정수형
		int loopEnd;

		// 각 음악들이 플레이될때 목록을 유지하는데 사용되는 정수형
		int lastTime;
	};
public:
	// 구조체
	MusicManager();
	// 구조체 파괴
	~MusicManager();

	// 음악을 없앨때
	void destroy();

	//각각 음악을 틀게하거나 안틀게하는 함수
	// 틀게하는거 : boolean형 음악관리자가 틀게하거나 안틀게한다.
	void setEnabled(bool enable=true);

	// 음악의 볼륨을 세팅하는 기능
	void setVolume(int volume);

	// 이 기능은 음악파일 하나를 불러오고 불러온 음악을 목록에 추가한다.
  // file : 이 음악파일의 파일이름
	// returns : 불러온 음악을 콤마로 분리 실패하면 그 음악은 비어있다 >> 이걸 포함하는 string
	std::string loadMusic(const std::string &file);

	// 이 method는 음악 목록으로부터 불러온다.
	// file ; 음악 리스트 파일의 파일이름
	// returns : 만약 에러가 없으면 참이다.
	bool loadMusicList(const std::string &file);

	// 이 method는 음악 파일을 실행한다.
	// name : 노래의 이름
	// fade : 만약 희미해진 현재 하나가 나가거나 아니면 boolean
	void playMusic(const std::string &name,bool fade=true);

	// 이 method는 현재 음악 목록에서 음악을 고른다.
	void pickMusic();

	// 음악이 멈췄을때 불러오는 method
	void musicStopped();

	// 음악 목록을 정리해준다.
	// list : 목록의 이름
	void setMusicList(const std::string &list);

	// 음악목록들의 크래딧을 생성하는 기능
	// returns : 크래딧의 줄을 함하는 vector
	std::vector<std::string> createCredits();
private:
	//  만약 음악관리자가 실행하거나 안되는 경우 boolean형
	// 이 default값이 잘못된 값이라면 음악관리자는 가능할 것이다. 실행하기 전에
	bool enabled;

	// 마지막 플레이된 노래의 리스트를 보존하는 정수형
	int lastTime;

	// 최근 실행된 음악 구조체를 가르킴
	Music* playing;

	// 하나가 멈추기 전에 실행되는 음악의 이름을 포함하는 string이다.
	//  이것은 그것이 음악멈추는 method 에서 체크된다는것을 의미한다.
	std::string nextMusic;

	// 현재 음악목록의 이름을 포함하는 string
	std::string currentList;

	// 음악을 포함하는 map
	// key는 음악의 이름이고, 값은 Mix_Music의 포인터이다.
	std::map<std::string,Music*> musicCollection;

	// 음악목록을 포함하는 map
	// key는 목록의 이름이고 값은 음악 이름들의 배열이다.
	std::map<std::string,std::vector<std::string> > musicLists;
};

#endif
