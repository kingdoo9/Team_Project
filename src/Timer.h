/* �ڵ�м��� : 60142270 ��ä��
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
// ������ ���� constant�� �����ϴµ� ���Ǵ� Ÿ�̸�Ŭ����
class Timer{
private:
	//Integer containing the number of ticks when the Timer started.
	// Ÿ�̸Ӱ� ���۵Ǿ��� �� tick �� ���� �����ϴ� integer 
	int ticks;
public:
	//Timer contruction Ÿ�̸� ���� 
	//Just initialize ticks  tick�� �ʱ�ȭ �� 
	Timer():ticks(0){}
	
	//This will start the timer. �̰��� Ÿ�̸Ӹ� ������ ���̴�. 
	//What it does is set ticks(the starttime) to SDL_GetTicks().
	// ƽ(���۽ð�)�� �����ϴ� ....
	void start();
	
	//This will return the number of ticks that have passed.
	// ������ tick�� ���� ��ȯ 
	//Returns: The number of ticks since the start of the timer.
	// Ÿ�̸��� �������κ����� tick�� �� 
	int getTicks();
};
#endif
