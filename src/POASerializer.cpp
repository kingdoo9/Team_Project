/* 60142233 ���ΰ�

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
//�� �޼ҵ�� �Է� ��Ʈ������ ���ڿ��� �б� ���� ���ȴ�.
// �� : read ���� �Է� ��Ʈ��.
// ���ڿ� : ���ڿ��� ����� ��ġ�Ѵ�.
	
	//���� ����.
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
			
			//�ٸ� ��� ���ڴ� ���ڿ��� ���� �� �ִ�.
			string.push_back(c);
		}
	}else{
		//����ǥ�� ��� ������ �������� �ƴ��� ���Ǳ�� ������ �ʿ䰡 �ִ�.
		do{
			switch(c){
			// ���ڿ��� ���� �ǹ��ϴ� ���ڸ� Ȯ���Ѵ�.
			case EOF:
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				return;
			// POA ���� ������ �Ϻ� ���ڸ� Ȯ��
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
				//�ٸ� ��� ���ڴ� ����.���ڿ��� ����
				string.push_back(c);
			}
			
			//���� ���ڸ� ������
			c=fin.get();
		}while(!fin.eof() & !fin.fail());
	}
}

static void skipWhitespaces(std::istream& fin){
	//���鰰�� ���𰡰� ���� ������ �� �Լ��� �Է� ��Ʈ������ �д´�.
	// �� : read ���� �Է� ��Ʈ��.

	// ������ ����.
	int c;
	while(!fin.eof() & !fin.fail()){
		// ���ڸ� �����ɴϴ�.
		c=fin.get();
		
		//���� ���� �ϳ��� �ִ��� �ƴ��� Ȯ���մϴ�.
		switch(c){
		case EOF:
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			break;
		default:
		// �ٸ� �͵��� ������ ���� �� ���� �ǹ��Ѵ�.
 // Unget ������ ���� �� ��ȯ.
			fin.unget();
			return;
		}
	}
}

static void skipComment(std::istream& fin){
	//������ �� (���� �ּ� ��)�϶����� �� �Լ��� �Է� ��Ʈ������ �н��ϴ�.
 // �� : read ���� �Է� ��Ʈ��.
	
	//���繮��
	int c;
	while(!fin.eof() & !fin.fail()){
		//���ڸ� ������
		c=fin.get();
		
		//�� ���ο� ���� (�ּ��� ��)�� Ȯ��
		if(c=='\r'||c=='\n'){
			fin.unget();
			break;
		}
	}
}

bool POASerializer::readNode(std::istream& fin,ITreeStorageBuilder* objOut,bool loadSubNodeOnly){
	//The current character.
	int c;
	//���� �б� ���.
	//0=read name
	//1=read attribute value
	//2=read subnode value
	//16=add attribute
	//17=add subnode
	int mode;

	//�б� ���� �Է� ��Ʈ���� null�� �ִ��� Ȯ��
	if(!fin) return false;
	
	//TreeStorageNodes�� ������ �����ϴ� ����.
	vector<ITreeStorageBuilder*> stack;
	//�̸� ���Ϳ� ���� ���� ����.
	vector<string> names,values;

	// ���� ��带�ε� �� �ʿ䰡 �ִ��� Ȯ���Ѵ�.
 // if : ù ��° TreeStorageNode�� objOut�� �־��մϴ�.
	if(loadSubNodeOnly) stack.push_back(objOut);

	//������ ���� ����.
	while(!fin.eof() && !fin.fail()){
		//���ڸ� ������
		c=fin.get();
		
		//����� �� ���ڿ� ��� �Ұ��� Ȯ��
		switch(c){
		case EOF:
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			//������ �ǳ� �ڴ�
			break;
		case '#':
			//�ڸ�Ʈ�� ��ŵ.
			skipComment(fin);
			break;
		case '}':
			// �ݴ� ���ȣ��  ���þȿ��� �� �ܰ踦 ����
	// false�� ��ȯ���� ���� ��� TreeStorageNode ������ �־���մϴ�.
			if(stack.empty()) return false;
			
			//������ ������ �׸��� �����մϴ�.
			stack.pop_back();
			// if so than the reading of the node is done.������ ������� Ȯ��
			if(stack.empty()) return true;
			objOut=stack.back();
			break;
		default:
			//It isn't a special character but part of a name/value, so unget it.
			fin.unget();
			
			{
				//, �̸��� �� ���͸� ����� ���ο� �̸� / ���� �б� �����մϴ�.
				names.clear();
				values.clear();
				
				//�̸� �ǵ� ���� ��带 ����.
				mode=0;
				
				// while ������ �ߴ� �Ǵ� ���������� ������ ���ڸ� �а� ����
				while(!fin.eof() & !fin.fail()){
					//�̸��� ���� �� ���ڿ��Դϴ�.
					string s;
					
					//���� ������ �ǳ� �ݴϴ�.
					skipWhitespaces(fin);
					//���� ���ڿ��� ��´�
					readString(fin,s);
					
					//���üũ
					switch(mode){
					case 0:
						//��� (�̸��� �б�) 0 ,�̸� ���Ϳ� ���ڿ��� �ִ´�
						names.push_back(s);
						break;
					case 1:
					case 2:
						//��� 1 �Ǵ� 2 ,�� ���Ϳ� ���ڿ��� �ִ´�
						values.push_back(s);
						break;
					}
					//�ٽ� ������ �ǳ� �ڴ�
					skipWhitespaces(fin);
					
					//���� ���� ���ڸ� �д´�
					c=fin.get();
					switch(c){
					case ',':
						//��ǥ�� �� �ϳ��� �̸��̳� ���� �ǹ�
						break;
					case '=':
						//An '=' can only occur after a name (mode=0).
						if(mode==0){
	  						//���� ���ڿ��� 1 ��尪�� ����
						}else{
							//In any other case there's something wrong so return false.
							return false;
						}
						break;
					case '(':
						//An '(' can only occur after a name (mode=0).
						if(mode==0){
							//���� ���ڿ��� 2 ��带 ����, ����� ���̵ȴ�
							mode=2;
						}else{
							//In any other case there's something wrong so return false.
							return false;
						}
						break;
					case ')':
						//A ')' can only occur after an attribute (mode=2).
						if(mode==2){
							// 17 ��带 ���� ���ο� ���� ��尡 �ɰ�
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
					
					//��尡 16 (�߰� �Ӽ�) �Ǵ� 17 (���� ��带 �߰� ��) ���  Ż���ؾ���
					if(mode>=16) break;
				}
				
				//���üũ
				switch(mode){
				case 16:
					// ���� 16 ,�Ӽ��� �̸��� ���� �����ؾ���
				// ������ ��� �� �� ����
					if(stack.empty()) return false;
					
					//��� TreeStorageNode�� null�� �ִ��� Ȯ��
					if(objOut!=NULL){
						//�̸����Ͱ� ����ִ��� �ƴ��� Ȯ��, if so add an empty name.
						if(names.empty()) names.push_back("");
						
						//��� �׳� �̸��� �� ���� �ִ´�
						while(values.size()<names.size()) values.push_back("");
						
						//���� �̸��� ���� ����.
						for(unsigned int i=0;i<names.size()-1;i++){
							//���� ���� �µ� ����.
							vector<string> v;
							v.push_back(values[i]);
							
							//�׸��� �Ӽ��� �߰� �� �� �ִ�
							objOut->newAttribute(names[i],v);
						}
						
						if(names.size()>1) values.erase(values.begin(),values.begin()+(names.size()-1));
						objOut->newAttribute(names.back(),values);
					}
					break;
				case 17:
					//���� 17 ,���� ��带 �߰� �� �ʿ䰡����
					{
						//�̸� ���Ͱ� ����ִ���,  �� �̸��� �߰� �� ������� Ȯ��.
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
						
						//���������
						ITreeStorageBuilder* objNew=NULL;
						
						//������ ��� ������ ���ο� ���� ���� TreeStorageNode
						if(stack.empty()) objNew=objOut;
						//���� ������ ���� ���ο� ���� ���� ��� TreeStorageNode�� ���� ��尡 �� ��
						else if(objOut!=NULL) objNew=objOut->newNode();
						
						//�����߰�
						stack.push_back(objNew);
						if(objNew!=NULL){
							//�̸��� ���߰�
							objNew->setName(names.back());
							objNew->setValue(values);
						}
						objOut=objNew;

						//���� �ǳ� �ٱ�.
						skipWhitespaces(fin);
						//�׸��� ���� ���ڸ� ����
						c=fin.get();
						if(c!='{'){
							//The character isn't a '{' meaning the block hasn't got a body.
							fin.unget();
							stack.pop_back();
							
							//�Ϸ�Ǿ�����, ������ ����ִ��� Ȯ��
							if(stack.empty()) return true;
							objOut=stack.back();
						}
					}
					break;
				default:
					// ���� 16 �Ǵ� 17�� �ƴ϶� ������ while ������ ������.
 // ���� �߸� ������ false�� ��ȯ
					return false;
				}
			}
			break;
		}
	}
	return true;
}

static void writeString(std::ostream& fout,std::string& s){
		//�� �޼ҵ�� ���ڿ��� �ۼ�
	// FOUT : ��� ��Ʈ���� �����
	// S : ���ڿ� ����.

	// ���ο� : ���ڿ��� ��� �ִ��� Ȯ��
	if(s.empty()){
		//because of the new changes of loader, we should output 2 quotes '""'
		fout<<"\"\"";
	}else
	//���ڿ��� Ż�� �ʿ��� ��� Ư�� ���ڰ� ���ԵǾ� �ִ��� Ȯ��
	if(s.find_first_of(" \r\n\t,=(){}#\"")!=string::npos){
		//It does so we put '"' around them.
		fout<<'\"';
		
		//���� ����.
		int c;
		
		//���ڸ� ���� ����.
		for(unsigned int i=0;i<s.size();i++){
			c=s[i];
			
			//If there's a '"' character it needs to be counter escaped. ("")
			if(c=='\"'){
				fout<<"\"\"";
			}else{
				//�׷��� ���� ���, �츮�� ���� ĳ���͸� ��� �ۼ��� ���ִ�.
				fout<<(char)c;
			}
		}
		fout<<'\"';
	}else{
		//�װ��� �ָ� �� �� �ֵ��� � Ư�� ���ڰ� ���ԵǾ� ���� �ʽ��ϴ�.
		fout<<s;
	}
}

static void writeStringArray(std::ostream& fout,std::vector<std::string>& s){
	//�� ����� �ָ� ���ڿ� �迭�� �ۼ��մϴ�.
	// FOUT : ��� ��Ʈ���� ����մϴ�.
	// S : �� ���ִ� ���ڿ��� �����ϴ� ����.

	// ���� ���ڿ�.
	for(unsigned int i=0;i<s.size();i++){
		//If it's the second or more there must be a ",".
		if(i>0) fout<<',';
		//���ڿ��� ����
		writeString(fout,s[i]);
	}
}

static void pWriteNode(ITreeStorageReader* obj,std::ostream& fout,int indent,bool saveSubNodeOnly){
	// ������ ��� ��Ʈ���� TreeStorageNode�� �ۼ�=
// OBJ : TreeStorageNode �ָ� ����
// FOUT : ��� ��Ʈ���� ���
// �鿩 ���� : �鿩 ������ ���� �����ϴ� ������ �ʿ�
// saveSubNodeOnly : �� ���� ��尡 �����ؾ��ϴ� ��� �ο�.
  
// ��尡 ���� ��尡�ִ� ��� �ο�.
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
	//Ư��
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
	//�������
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
	//��� ��Ʈ���� null�� �ִ��� Ȯ���մϴ�.
	if(!fout) return;
	
	//It isn't so start writing the node.
	pWriteNode(obj,fout,0,saveSubNodeOnly);
}
