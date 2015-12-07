/* �ڵ� �м��� : 60142270 ��ä�� 
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
// tree-structured ������� �����͸� �����ϴ� �� ���� Ŭ���� 
//Every (TreeStorage)Node has a vector with subNodes and every Node contains a hashmap with attributes.
// ���(TreeStorage)���� �Ӽ��� ���� hashmap�� �����ϴ� ������� ��� ��带 ���� ���Ͱ� �ִ�
class TreeStorageNode:public ITreeStorageBuilder,public ITreeStorageReader{
private:
	//Iterator used to iterate the hashmap with attributes.
	// �Ӽ��� ���� �ؽø��� �ݺ�(iterate)�ϴµ� ���Ǵ� iterator
	//Used by the methods getNextAttribute and getNextNode.
	// getNextAttribute�� getNextNode �Լ��� ���� ����
	std::map<std::string,std::vector<std::string> >::iterator objAttrIterator;
public:
	//Vector containing the subnodes of the TreeStorageNode.
	// TreeStorageNode�� �����带 �����ϴ� ���� 
	std::vector<TreeStorageNode*> subNodes;
	//String containing the name of the TreeStorageNode.
	// TreeStorageNode�� �̸��� �����ϴ� ��Ʈ�� 
	std::string name;
	//Vector containing the value(s) of the TreeStorageNode.
	// TreeStorageNode�� ������ �����ϴ� ���� 
	std::vector<std::string> value;
	//Hashmap containing the attributes of the TreeStorageNode.
	// TreeStorageNode�� �Ӽ��� �����ϴ� �ؽø� 
	std::map<std::string,std::vector<std::string> > attributes;
	
	// ������ 
	TreeStorageNode(){}
	// �Ҹ��� 
	virtual ~TreeStorageNode();
	// TreeStorageNode �� �ı��ϴ� �� ���Ǵ� �Լ� 
	//Also called when the deconstructor is called. �Ҹ��ڰ� �ҷ��� �� ���� �Ҹ� 
	void destroy();

	// TreeStorageNode�� �̸� ���� 
	//name: The name to give. 
	virtual void setName(std::string& name);
	//Sets the parameter name to the name of the TreeStorageNode.
	// TreeStorageNode�� �̸����� �Ķ������ �̸� ���� 
	//name: The string to fill with the name; �̸����� ä���� ��Ʈ�� 
 	virtual void getName(std::string& name);
	
	//Set the value of the TreeStorageNode. TreeStorageNode�� ���� ���� 
	//value: The value to give.
	virtual void setValue(std::vector<std::string>& value);
	//Sets the parameter value to the value of the TreeStorageNode.
	// TreeStorageNode�� �������� �Ķ������ ���� ���� 
	//value: The string to fill with the name; �̸����� ä���� ��Ʈ�� 
	virtual void getValue(std::vector<std::string>& value);
	
	//Creates a new node in the TreeStorageNode. TreeStorageNode���� ���ο� ��带 ������
	//The new node will be added to the subnodes. ���ο� ���� �����忡�� �߰��� ���̴�
	//Returns: a pointer to the new node. ���ο� ��忡���� ������ 
	virtual ITreeStorageBuilder* newNode();
	//Creates a new attribute in the TreeStorageNode. TreeStorageNode������ ���ο� �Ӽ� ���� 
	//The attribute will be added to the attributes map. �� �Ӽ��� �Ӽ� �ʿ� �߰��ɰ��̴� 
	//name: The name for the new attribute. ���ο� �Ӽ��� ���� �̸� 
	//value: The value for the new attribute. ���ο� �Ӽ��� ���� ���� 
	virtual void newAttribute(std::string& name,std::vector<std::string>& value);

	
	//Method used for iterating through the attributes of the TreeStorageNode.
	// TreeStorageNode�� �Ӽ��� ���ؼ� �ݺ��Ǵµ� ���Ǵ� �Լ� 
	//pUserData: Pointer TODO???
	//name: The string fill with the name of the attribute. ��Ʈ���� �Ӽ��� �̸��� ä��
	//value: Vector to fill with the value(s) of the attribute. ���Ͱ� �Ӽ��� ������ ä�� 
	virtual void* getNextAttribute(void* pUserData,std::string& name,std::vector<std::string>& value);
	//Method used for iterating through the subnodes of the TreeStorageNode.
	// TreeStorageNode�� �����带 ���ؼ� �ݺ��ϴµ� ���Ǵ� �Լ� 
	//pUserData: Pointer TODO???
	//obj: Pointer that will be pointed to the nextNode, if present. ���� �θ��϶�, ������带 ����Ű�� ������ 
	virtual void* getNextNode(void* pUserData,ITreeStorageReader*& obj);

	//Calculate the MD5 of node based on the data structure.
	// ������ ����ü�� �⺻�� �� ����� MD5 ��� 
	//places the message digest in md, 
	//which must have space for 16 bytes of output (or NULL).
	unsigned char* calcMD5(unsigned char* md);
};
#endif
