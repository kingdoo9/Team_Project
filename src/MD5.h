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

#ifndef MD5_H
#define MD5_H

#define MD5_CTX_SIZE 128

class Md5{
public:
	// digest는 정적 배열에 위치해있다.
	static unsigned char *calc(const void *d, unsigned long n, unsigned char *md);

	// 16byte digest를 string 대표로 변환한다.
	// 반환값은 정적 배열이다.
	static char *toString(unsigned char *md);

	// 따라오는 것은 낮은 레벨의 함수이다.
	// MD5를 계산하기 위해 클래스를 초기화한다.

	void init();
	// HASH에 메세지 구분을 더한다.
	void update(const void *data, unsigned long len);
	// 계산이 끝나면, 메세지 DIGEST를 MD에 위치시킨다.
	// 출력을 위해 16바이트의 띄어쓰기가 필요하다.
	unsigned char *final(unsigned char *md);
private:
	// BYTE 배열, MD5_CTX를 가질만큼 충분히 커야한다.
	char md5_ctx[MD5_CTX_SIZE];
};

#endif
