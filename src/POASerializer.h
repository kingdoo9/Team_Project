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

#ifndef POASERIALIZER_H
#define POASERIALIZER_H

#include "ITreeSerializer.h"

//�� Ŭ������ POAformat - files�� to/from, / TreeStorageNodes�� �б� / ���⿡ ���˴ϴ�.
class POASerializer:public ITreeSerializer{
public:
	//�� �޼ҵ�� InputStream�� �а� ���� �м��մϴ�.
// ����� TreeStorageNode�� ��ġ�˴ϴ�.
// �� :���� �� �ִ�  �Է� ��Ʈ��.
// objOut :  TreeStorageNode ����� �Էµɰ�
// loadSubNodesOnly : ���� ��带�ε� �� �ʿ䰡�ִ� ��� �ο�.
// ��ȯ �� : �б�� �м��� �����ϴ� ���� true.
	virtual bool readNode(std::istream& fin,ITreeStorageBuilder* objOut,bool loadSubNodesOnly=false);
	
	//�� �޼ҵ�� OutputStream�� TreeStorageNode�� �ۼ��մϴ�.
// OBJ : TreeStorageNode �����մϴ�.
// FOUT : �Էµ� ��� ��Ʈ��.
// saveSubNodesOnly :���� ��尡 �����ؾ��ϴ� ��� �ο�.
 // ��ȯ �� : ���Ⱑ �������� ���� true.
	virtual void writeNode(ITreeStorageReader* obj,std::ostream& fout,bool bWriteHeader=true,bool saveSubNodeOnly=false);
};
#endif
