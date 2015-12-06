/* 코드 분석자 : 60142270 남채린 
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

#ifndef TREESTORAGENODE_H
#define TREESTORAGENODE_H

#include "ITreeStorage.h"
#include <map>
#include <vector>
#include <string>

//This class is used to store data in a tree-structured way. 
// tree-structured 방법에서 데이터를 저장하는 데 사용된 클래스 
//Every (TreeStorage)Node has a vector with subNodes and every Node contains a hashmap with attributes.
// 모든(TreeStorage)노드는 속성을 가진 hashmap을 포함하는 서브노드와 모든 노드를 가진 벡터가 있다
class TreeStorageNode:public ITreeStorageBuilder,public ITreeStorageReader{
private:
	//Iterator used to iterate the hashmap with attributes.
	// 속성을 가진 해시맵을 반복(iterate)하는데 사용되는 iterator
	//Used by the methods getNextAttribute and getNextNode.
	// getNextAttribute와 getNextNode 함수에 의해 사용됨
	std::map<std::string,std::vector<std::string> >::iterator objAttrIterator;
public:
	//Vector containing the subnodes of the TreeStorageNode.
	// TreeStorageNode의 서브노드를 포함하는 벡터 
	std::vector<TreeStorageNode*> subNodes;
	//String containing the name of the TreeStorageNode.
	// TreeStorageNode의 이름을 포함하는 스트링 
	std::string name;
	//Vector containing the value(s) of the TreeStorageNode.
	// TreeStorageNode의 변수를 포함하는 벡터 
	std::vector<std::string> value;
	//Hashmap containing the attributes of the TreeStorageNode.
	// TreeStorageNode의 속성을 포함하는 해시맵 
	std::map<std::string,std::vector<std::string> > attributes;
	
	// 생성자 
	TreeStorageNode(){}
	// 소멸자 
	virtual ~TreeStorageNode();
	// TreeStorageNode 를 파괴하는 데 사용되는 함수 
	//Also called when the deconstructor is called. 소멸자가 불려질 때 또한 불림 
	void destroy();

	// TreeStorageNode의 이름 설정 
	//name: The name to give. 
	virtual void setName(std::string& name);
	//Sets the parameter name to the name of the TreeStorageNode.
	// TreeStorageNode의 이름에서 파라메터의 이름 설정 
	//name: The string to fill with the name; 이름으로 채워진 스트링 
 	virtual void getName(std::string& name);
	
	//Set the value of the TreeStorageNode. TreeStorageNode의 변수 설정 
	//value: The value to give.
	virtual void setValue(std::vector<std::string>& value);
	//Sets the parameter value to the value of the TreeStorageNode.
	// TreeStorageNode의 변수에서 파라메터의 변수 설정 
	//value: The string to fill with the name; 이름으로 채워진 스트링 
	virtual void getValue(std::vector<std::string>& value);
	
	//Creates a new node in the TreeStorageNode. TreeStorageNode에서 새로운 노드를 만들어라
	//The new node will be added to the subnodes. 새로운 노드는 서브노드에서 추가될 것이다
	//Returns: a pointer to the new node. 새로운 노드에서의 포인터 
	virtual ITreeStorageBuilder* newNode();
	//Creates a new attribute in the TreeStorageNode. TreeStorageNode에서의 새로운 속성 만듬 
	//The attribute will be added to the attributes map. 그 속성은 속성 맵에 추가될것이다 
	//name: The name for the new attribute. 새로운 속성을 위한 이름 
	//value: The value for the new attribute. 새로운 속성을 위한 변수 
	virtual void newAttribute(std::string& name,std::vector<std::string>& value);

	
	//Method used for iterating through the attributes of the TreeStorageNode.
	// TreeStorageNode의 속성을 통해서 반복되는데 사용되는 함수 
	//pUserData: Pointer TODO???
	//name: The string fill with the name of the attribute. 스트링이 속성의 이름을 채움
	//value: Vector to fill with the value(s) of the attribute. 벡터가 속성의 변수를 채움 
	virtual void* getNextAttribute(void* pUserData,std::string& name,std::vector<std::string>& value);
	//Method used for iterating through the subnodes of the TreeStorageNode.
	// TreeStorageNode의 서브노드를 통해서 반복하는데 사용되는 함수 
	//pUserData: Pointer TODO???
	//obj: Pointer that will be pointed to the nextNode, if present. 만약 부모일때, 다음노드를 가리키는 포인터 
	virtual void* getNextNode(void* pUserData,ITreeStorageReader*& obj);

	//Calculate the MD5 of node based on the data structure.
	// 데이터 구조체에 기본이 된 노드의 MD5 계산 
	//places the message digest in md, 
	//which must have space for 16 bytes of output (or NULL).
	unsigned char* calcMD5(unsigned char* md);
};
#endif
