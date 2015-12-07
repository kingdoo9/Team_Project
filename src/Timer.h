/* 코드분석자 : 60142270 남채린
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

#ifndef TIMER_H
#define TIMER_H

//Timer class used to keep the framerate constant. 
// 프래임 비율 constant를 유지하는데 사용되는 타이머클래스
class Timer{
private:
	//Integer containing the number of ticks when the Timer started.
	// 타이머가 시작되었을 때 tick 의 수를 포함하는 integer 
	int ticks;
public:
	//Timer contruction 타이머 생성 
	//Just initialize ticks  tick을 초기화 함 
	Timer():ticks(0){}
	
	//This will start the timer. 이것은 타이머를 시작할 것이다. 
	//What it does is set ticks(the starttime) to SDL_GetTicks().
	// 틱(시작시간)을 설정하는 ....
	void start();
	
	//This will return the number of ticks that have passed.
	// 지나간 tick의 수를 반환 
	//Returns: The number of ticks since the start of the timer.
	// 타이머의 시작으로부터의 tick의 수 
	int getTicks();
};
#endif
