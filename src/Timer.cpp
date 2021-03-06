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

#include "Timer.h"
#include <SDL/SDL.h>

void Timer::start(){
	//Store the current number of ticks (start time).
	// tick(현재 시간)의 현재이 수 저장 
	ticks=SDL_GetTicks();
}

int Timer::getTicks(){
	//Return the differents with the current ticks and the start time.
	// 현재의 tick과 시작시간을 가진 차이 반환 
	return (SDL_GetTicks()-ticks);
}
