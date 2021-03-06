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

#include "MD5.h"

#ifdef WIN32

#include <windows.h>

typedef struct MD5_CTX {
  ULONG         i[2];
  ULONG         buf[4];
  unsigned char in[64];
  unsigned char digest[16];
} MD5_CTX;

typedef void (WINAPI* F_MD5Init)(MD5_CTX *context);
typedef void (WINAPI* F_MD5Update)(MD5_CTX *context, const unsigned char *input, unsigned int inlen);
typedef void (WINAPI* F_MD5Final)(MD5_CTX *context);

// 첫번째로 부른 API는 이 함수를 다시보낼 것 이다.
static void WINAPI myMD5Init(MD5_CTX *context);
static void WINAPI myMD5Update(MD5_CTX *context, const unsigned char *input, unsigned int inlen);
static void WINAPI myMD5Final(MD5_CTX *context);

static F_MD5Init MD5Init=myMD5Init;
static F_MD5Update MD5Update=myMD5Update;
static F_MD5Final MD5Final=myMD5Final;

#else

#include <openssl/md5.h>

#endif

#include <assert.h>
#include <string.h>

#ifdef WIN32

static void loadMD5Functions(){
	HMODULE h;

	h=LoadLibraryA("cryptdll.dll");
	if(h!=NULL){
		MD5Init=(F_MD5Init)GetProcAddress(h,"MD5Init");
		MD5Update=(F_MD5Update)GetProcAddress(h,"MD5Update");
		MD5Final=(F_MD5Final)GetProcAddress(h,"MD5Final");
		if(MD5Init==NULL || MD5Update==NULL || MD5Final==NULL) FreeLibrary(h);
		else return;
	}

	h=LoadLibraryA("advapi32.dll");
	if(h!=NULL){
		MD5Init=(F_MD5Init)GetProcAddress(h,"MD5Init");
		MD5Update=(F_MD5Update)GetProcAddress(h,"MD5Update");
		MD5Final=(F_MD5Final)GetProcAddress(h,"MD5Final");
		if(MD5Init==NULL || MD5Update==NULL || MD5Final==NULL) FreeLibrary(h);
		else return;
	}

	FatalAppExitA(-1,"Can't find MD5 functions!!!");
}

static void WINAPI myMD5Init(MD5_CTX *context){
	loadMD5Functions();
	MD5Init(context);
}

static void WINAPI myMD5Update(MD5_CTX *context, const unsigned char *input, unsigned int inlen){
	loadMD5Functions();
	MD5Update(context,input,inlen);
}

static void WINAPI myMD5Final(MD5_CTX *context){
	loadMD5Functions();
	MD5Final(context);
}

#endif

// n 바이트의 digest메세지는 계산하고 md에 존재한다.
// 만약 MD가 NULL이면, 각각은 출력을 위해 16바이트의 스페이스를 가져야한다.
// digest는 정적배열 안에 위치한다
unsigned char *Md5::calc(const void *d, unsigned long n, unsigned char *md){
#ifdef WIN32
	static MD5_CTX ctx;

	MD5Init(&ctx);
	MD5Update(&ctx,(const unsigned char*)d,n);
	MD5Final(&ctx);

	if(md){
		memcpy(md,ctx.digest,16);
		return md;
	}else{
		return ctx.digest;
	}
#else
	return MD5((const unsigned char*)d,n,md);
#endif
}

char *Md5::toString(unsigned char *md){
	static char s[40];
	const char* hex="0123456789abcdef";

	for(int i=0;i<16;i++){
		s[i*2]=hex[(md[i]&0xF0)>>4];
		s[i*2+1]=hex[md[i]&0xF];
	}
	s[32]='\0';

	return s;
}

// MD5 계산을 위해 클래스를 초기화한다
void Md5::init(){
  // 첫번째로 사이즈를 체크한다
	assert(sizeof(MD5_CTX)<=MD5_CTX_SIZE);

#ifdef WIN32
	MD5Init((MD5_CTX*)md5_ctx);
#else
	MD5_Init((MD5_CTX*)md5_ctx);
#endif
}

// hash에 메세지의 구문을 더한다
void Md5::update(const void *data, unsigned long len){
  // 첫번째로 사이즈를 체크한다
	assert(sizeof(MD5_CTX)<=MD5_CTX_SIZE);

#ifdef WIN32
	MD5Update((MD5_CTX*)md5_ctx,(const unsigned char*)data,len);
#else
	MD5_Update((MD5_CTX*)md5_ctx,data,len);
#endif
}

// 계산을 마치고, md에있는 메시지의 digest를 위치시킨다
// 출력의 16바이트를 위해 스페이스를 반드시 가져야한다.
unsigned char *Md5::final(unsigned char *md){
	static unsigned char digest[16];
  // 첫번째로 크기를 체크한다
	assert(sizeof(MD5_CTX)<=MD5_CTX_SIZE);

#ifdef WIN32
	MD5_CTX *ctx=(MD5_CTX*)md5_ctx;
	MD5Final(ctx);
	if(md==NULL) md=digest;
	memcpy(md,ctx->digest,16);
	return md;
#else
	if(md==NULL) md=digest;
	MD5_Final(md,(MD5_CTX*)md5_ctx);
	return md;
#endif
}
