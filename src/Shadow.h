/*
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

#ifndef SHADOW_H
#define SHADOW_H

#include "Player.h"

// 그림자 클래스이다. 이 클래스는 player클래스와 거의 같기때문에 palyer클래스를 상속받는다.
class Shadow : public Player{
protected:

	// 만약 그림자가 player로부터 호출받으면 boolean형 호출
	// 만약 그림자가 player의 움직임을 복사하면ㅎㅎ
	bool called;

	friend class Player;
public:
	// 구조체  이 구조체는 몇가지 변수들을 설정하고  부른다 player의 생성자를
	// objParent : 게임 instance의 포인터
	Shadow(Game* objParent);

	// move 함수를 호출전의 method
 //시킨다 그림자를 그의 음직이거나 뛰는 로직으로 사용됐다.
	void moveLogic();

	// 그가 불린 그림자를 알리는데 사용된 method
	// 그는 그때 움직임을 복사해야한다. 그에게 주어진
	void meCall();

	// method는 상태를 reset하는데 사용된다.
	virtual void stateReset();
	//상태를 불러오는데 사용되는 method
	virtual void loadState();
};
#endif
