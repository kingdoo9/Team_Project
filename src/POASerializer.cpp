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

#include "POASerializer.h"
#include <sstream>
using namespace std;

static void readString(std::istream& fin,std::string& string){
//이 메소드는 입력 스트림에서 문자열을 읽기 위해 사용된다.
// 핀 : read 원의 입력 스트림.
// 문자열 : 문자열에 결과를 배치한다.
	
	//현재 문자.
	int c;
	c=fin.get();
	
	//Check if there's a '"'.
	if(c=='\"'){
		//There's a '"' so place every character we encounter in the string without parsing.
		while(!fin.eof() & !fin.fail()){
			//First we get the next character to prevent putting the '"' in the string.
			c=fin.get();
			
			//Check if there's a '"' since that could mean the end of the string.
			if(c=='\"'){
				//Get the next character and check if that's also an '"'.
				c=fin.get();
				if(c!='\"'){
					//We have two '"' after each other meaning an escaped '"'.
					//We unget one so there will be one '"' placed in the string.
					fin.unget();
					return;
				}
			}
			
			//다른 모든 문자는 문자열에 넣을 수 있다.
			string.push_back(c);
		}
	}else{
		//따옴표가 없어서 문장이 끝나는지 아닌지 주의깊게 검토할 필요가 있다.
		do{
			switch(c){
			// 문자열의 끝을 의미하는 문자를 확인한다.
			case EOF:
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				return;
			// POA 파일 형식의 일부 문자를 확인
// If so we first unget one character to prevent problems parsing the rest of the file.
			case ',':
			case '=':
			case '(':
			case ')':
			case '{':
			case '}':
			case '#':
				fin.unget();
				return;
			default:
				//다른 경우 문자는 정상.문자열에 넣음
				string.push_back(c);
			}
			
			//다음 문자를 가져옴
			c=fin.get();
		}while(!fin.eof() & !fin.fail());
	}
}

static void skipWhitespaces(std::istream& fin){
	//공백같은 무언가가 있을 때까지 이 함수는 입력 스트림에서 읽는다.
	// 핀 : read 원의 입력 스트림.

	// 현재의 문자.
	int c;
	while(!fin.eof() & !fin.fail()){
		// 문자를 가져옵니다.
		c=fin.get();
		
		//공백 문자 하나가 있는지 아닌지 확인합니다.
		switch(c){
		case EOF:
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			break;
		default:
		// 다른 것들은 공백이 종료 한 것을 의미한다.
 // Unget 마지막 문자 및 반환.
			fin.unget();
			return;
		}
	}
}

static void skipComment(std::istream& fin){
	//라인의 끝 (또한 주석 끝)일때까지 이 함수는 입력 스트림에서 읽습니다.
 // 핀 : read 원의 입력 스트림.
	
	//현재문자
	int c;
	while(!fin.eof() & !fin.fail()){
		//문자를 가져옴
		c=fin.get();
		
		//이 새로운 라인 (주석의 끝)을 확인
		if(c=='\r'||c=='\n'){
			fin.unget();
			break;
		}
	}
}

bool POASerializer::readNode(std::istream& fin,ITreeStorageBuilder* objOut,bool loadSubNodeOnly){
	//The current character.
	int c;
	//현재 읽기 모드.
	//0=read name
	//1=read attribute value
	//2=read subnode value
	//16=add attribute
	//17=add subnode
	int mode;

	//읽기 전에 입력 스트림이 null이 있는지 확인
	if(!fin) return false;
	
	//TreeStorageNodes의 스택을 포함하는 벡터.
	vector<ITreeStorageBuilder*> stack;
	//이름 벡터와 값에 대한 벡터.
	vector<string> names,values;

	// 하위 노드를로드 할 필요가 있는지 확인한다.
 // if : 첫 번째 TreeStorageNode로 objOut을 넣어합니다.
	if(loadSubNodeOnly) stack.push_back(objOut);

	//파일을 통해 루프.
	while(!fin.eof() && !fin.fail()){
		//문자를 가져옴
		c=fin.get();
		
		//어떤건지 그 문자와 어떤걸 할건지 확인
		switch(c){
		case EOF:
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			//공백을 건너 뛴다
			break;
		case '#':
			//코멘트는 스킵.
			skipComment(fin);
			break;
		case '}':
			// 닫는 대괄호는  스택안에서 한 단계를 수행
	// false를 반환하지 않을 경우 TreeStorageNode 왼쪽이 있어야합니다.
			if(stack.empty()) return false;
			
			//스택의 마지막 항목을 제거합니다.
			stack.pop_back();
			// if so than the reading of the node is done.스택이 비었는지 확인
			if(stack.empty()) return true;
			objOut=stack.back();
			break;
		default:
			//It isn't a special character but part of a name/value, so unget it.
			fin.unget();
			
			{
				//, 이름과 값 벡터를 지우고 새로운 이름 / 값을 읽기 시작합니다.
				names.clear();
				values.clear();
				
				//이름 판독 모드로 모드를 설정.
				mode=0;
				
				// while 루프를 중단 또는 오류가있을 때까지 문자를 읽고 보관
				while(!fin.eof() & !fin.fail()){
					//이름이 포함 된 문자열입니다.
					string s;
					
					//먼저 공백을 건너 뜁니다.
					skipWhitespaces(fin);
					//이제 문자열을 얻는다
					readString(fin,s);
					
					//모드체크
					switch(mode){
					case 0:
						//모드 (이름을 읽기) 0 ,이름 벡터에 문자열을 넣는다
						names.push_back(s);
						break;
					case 1:
					case 2:
						//모드 1 또는 2 ,값 벡터에 문자열을 넣는다
						values.push_back(s);
						break;
					}
					//다시 공백을 건너 뛴다
					skipWhitespaces(fin);
					
					//이제 다음 문자를 읽는다
					c=fin.get();
					switch(c){
					case ',':
						//쉼표는 또 하나의 이름이나 값을 의미
						break;
					case '=':
						//An '=' can only occur after a name (mode=0).
						if(mode==0){
	  						//다음 문자열은 1 모드값을 설정
						}else{
							//In any other case there's something wrong so return false.
							return false;
						}
						break;
					case '(':
						//An '(' can only occur after a name (mode=0).
						if(mode==0){
							//다음 문자열은 2 모드를 설정, 블록의 값이된다
							mode=2;
						}else{
							//In any other case there's something wrong so return false.
							return false;
						}
						break;
					case ')':
						//A ')' can only occur after an attribute (mode=2).
						if(mode==2){
							// 17 모드를 설정 새로운 하위 노드가 될것
							mode=17;
						}else{
							//In any other case there's something wrong so return false.
							return false;
						}
						break;
					case '{':
						//A '{' can only mean a new subNode (mode=17).
						fin.unget();
						mode=17;
						break;
					default:
						//The character is not special so unget it.
						fin.unget();
						mode=16;
						break;
					}
					
					//모드가 16 (추가 속성) 또는 17 (하위 노드를 추가 할) 경우  탈출해야함
					if(mode>=16) break;
				}
				
				//모드체크
				switch(mode){
				case 16:
					// 모드는 16 ,속성에 이름과 값을 변경해야함
				// 스택은 비워 둘 수 없다
					if(stack.empty()) return false;
					
					//결과 TreeStorageNode이 null이 있는지 확인
					if(objOut!=NULL){
						//이름벡터가 비어있는지 아닌지 확인, if so add an empty name.
						if(names.empty()) names.push_back("");
						
						//모든 그냥 이름을 빈 값을 넣는다
						while(values.size()<names.size()) values.push_back("");
						
						//이제 이름을 통해 루프.
						for(unsigned int i=0;i<names.size()-1;i++){
							//값을 포함 온도 벡터.
							vector<string> v;
							v.push_back(values[i]);
							
							//그리고 속성을 추가 할 수 있다
							objOut->newAttribute(names[i],v);
						}
						
						if(names.size()>1) values.erase(values.begin(),values.begin()+(names.size()-1));
						objOut->newAttribute(names.back(),values);
					}
					break;
				case 17:
					//모드는 17 ,하위 노드를 추가 할 필요가있음
					{
						//이름 벡터가 비어있는지,  빈 이름을 추가 할 경우인지 확인.
						if(names.empty()) names.push_back("");
						else if(names.size()>1){
							if(stack.empty()) return false;
							while(values.size()<names.size()) values.push_back("");
							for(unsigned int i=0;i<names.size()-1;i++){
								vector<string> v;
								v.push_back(values[i]);
								objOut->newAttribute(names[i],v);
							}
							values.erase(values.begin(),values.begin()+(names.size()-1));
						}
						
						//서브노드생성
						ITreeStorageBuilder* objNew=NULL;
						
						//스택이 비어 있으면 새로운 하위 노드는 TreeStorageNode
						if(stack.empty()) objNew=objOut;
						//하지 않으면 것은 새로운 하위 노드는 결과 TreeStorageNode의 하위 노드가 될 것
						else if(objOut!=NULL) objNew=objOut->newNode();
						
						//스택추가
						stack.push_back(objNew);
						if(objNew!=NULL){
							//이름과 값추가
							objNew->setName(names.back());
							objNew->setValue(values);
						}
						objOut=objNew;

						//공백 건너 뛰기.
						skipWhitespaces(fin);
						//그리고 다음 문자를 얻음
						c=fin.get();
						if(c!='{'){
							//The character isn't a '{' meaning the block hasn't got a body.
							fin.unget();
							stack.pop_back();
							
							//완료되었는지, 스택이 비어있는지 확인
							if(stack.empty()) return true;
							objOut=stack.back();
						}
					}
					break;
				default:
					// 모드는 16 또는 17이 아니라 여전히 while 루프를 끊었다.
 // 뭔가 잘못 때문에 false를 반환
					return false;
				}
			}
			break;
		}
	}
	return true;
}

static void writeString(std::ostream& fout,std::string& s){
		//이 메소드는 문자열을 작성
	// FOUT : 출력 스트림에 기록합
	// S : 문자열 쓰기.

	// 새로운 : 문자열이 비어 있는지 확인
	if(s.empty()){
		//because of the new changes of loader, we should output 2 quotes '""'
		fout<<"\"\"";
	}else
	//문자열이 탈출 필요한 모든 특수 문자가 포함되어 있는지 확인
	if(s.find_first_of(" \r\n\t,=(){}#\"")!=string::npos){
		//It does so we put '"' around them.
		fout<<'\"';
		
		//현재 문자.
		int c;
		
		//문자를 통해 루프.
		for(unsigned int i=0;i<s.size();i++){
			c=s[i];
			
			//If there's a '"' character it needs to be counter escaped. ("")
			if(c=='\"'){
				fout<<"\"\"";
			}else{
				//그렇지 않은 경우, 우리는 단지 캐릭터를 얻어 작성할 수있다.
				fout<<(char)c;
			}
		}
		fout<<'\"';
	}else{
		//그것을 멀리 쓸 수 있도록 어떤 특수 문자가 포함되어 있지 않습니다.
		fout<<s;
	}
}

static void writeStringArray(std::ostream& fout,std::vector<std::string>& s){
	//이 방법은 멀리 문자열 배열을 작성합니다.
	// FOUT : 출력 스트림에 기록합니다.
	// S : 쓸 수있는 문자열을 포함하는 벡터.

	// 루프 문자열.
	for(unsigned int i=0;i<s.size();i++){
		//If it's the second or more there must be a ",".
		if(i>0) fout<<',';
		//문자열을 쓴다
		writeString(fout,s[i]);
	}
}

static void pWriteNode(ITreeStorageReader* obj,std::ostream& fout,int indent,bool saveSubNodeOnly){
	// 지정된 출력 스트림에 TreeStorageNode를 작성=
// OBJ : TreeStorageNode 멀리 쓰기
// FOUT : 출력 스트림에 기록
// 들여 쓰기 : 들여 쓰기의 수를 포함하는 정수가 필요
// saveSubNodeOnly : 만 하위 노드가 저장해야하는 경우 부울.
  
// 노드가 하위 노드가있는 경우 부울.
	bool haveSubNodes=false;
	void* lpUserData=NULL;
	ITreeStorageReader* objSubNode=NULL;
	string s;
	vector<string> v;
	//---
	if(obj==NULL) return;
	//---
	if(!saveSubNodeOnly){
		for(int i=0;i<indent;i++) fout<<'\t';
		s.clear();
		obj->getName(s);
		writeString(fout,s);
		fout<<'(';
		v.clear();
		obj->getValue(v);
		writeStringArray(fout,v);
		fout<<')';
		indent++;
	}
	//특성
	lpUserData=NULL;
	for(;;){
		s.clear();
		v.clear();
		lpUserData=obj->getNextAttribute(lpUserData,s,v);
		if(lpUserData==NULL) break;
		if(!haveSubNodes && !saveSubNodeOnly) fout<<"{\n";
		haveSubNodes=true;
		for(int i=0;i<indent;i++) fout<<'\t';
		writeString(fout,s);
		fout<<'=';
		writeStringArray(fout,v);
		fout<<'\n';
	}
	//서브노드들
	lpUserData=NULL;
	for(;;){
		lpUserData=obj->getNextNode(lpUserData,objSubNode);
		if(lpUserData==NULL) break;
		if(objSubNode!=NULL){
			if(!haveSubNodes && !saveSubNodeOnly) fout<<"{\n";
			haveSubNodes=true;
			pWriteNode(objSubNode,fout,indent,false);
		}
	}
	//---
	if(!saveSubNodeOnly){
		indent--;
		if(haveSubNodes){
			for(int i=0;i<indent;i++) fout<<'\t';
			fout<<'}';
		}
		fout<<'\n';
	}
}

void POASerializer::writeNode(ITreeStorageReader* obj,std::ostream& fout,bool writeHeader,bool saveSubNodeOnly){
	//출력 스트림이 null이 있는지 확인합니다.
	if(!fout) return;
	
	//It isn't so start writing the node.
	pWriteNode(obj,fout,0,saveSubNodeOnly);
}
