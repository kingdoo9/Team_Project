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

#ifndef OBJECTS_H
#define OBJECTS_H

#include "Timer.h"
#include "LevelPack.h"
#include "GameState.h"

// 타이머가 FPS의 상수를 유지하는걸 하용
extern Timer FPS;
// 현재의 levelpack을 가르킨다.
extern LevelPack* levels;
// 현재상태
extern GameState* currentState;

#endif
