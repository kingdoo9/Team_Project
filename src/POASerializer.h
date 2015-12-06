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

#ifndef POASERIALIZER_H
#define POASERIALIZER_H

#include "ITreeSerializer.h"

//이 클래스는 POAformat - files의 to/from, / TreeStorageNodes의 읽기 / 쓰기에 사용됩니다.
class POASerializer:public ITreeSerializer{
public:
	//이 메소드는 InputStream를 읽고 구문 분석합니다.
// 결과는 TreeStorageNode에 배치됩니다.
// 핀 :읽을 수 있는  입력 스트림.
// objOut :  TreeStorageNode 결과가 입력될것
// loadSubNodesOnly : 하위 노드를로드 할 필요가있는 경우 부울.
// 반환 값 : 읽기와 분석이 성공하는 경우는 true.
	virtual bool readNode(std::istream& fin,ITreeStorageBuilder* objOut,bool loadSubNodesOnly=false);
	
	//이 메소드는 OutputStream에 TreeStorageNode를 작성합니다.
// OBJ : TreeStorageNode 저장합니다.
// FOUT : 입력될 출력 스트림.
// saveSubNodesOnly :하위 노드가 저장해야하는 경우 부울.
 // 반환 값 : 쓰기가 성공했을 경우는 true.
	virtual void writeNode(ITreeStorageReader* obj,std::ostream& fout,bool bWriteHeader=true,bool saveSubNodeOnly=false);
};
#endif
