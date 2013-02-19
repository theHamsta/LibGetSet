//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by André Aichert (aaichert@gmail.com)
//    
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//  
//    http://www.apache.org/licenses/LICENSE-2.0
//    
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#ifndef __GetSetXml_h
#define __GetSetXml_h

#include "GetSetIO.h"

namespace GetSetIO {

	/// GetSet full description in XML formtat
	class XmlFile : public GetSetInternal::GetSetInOut
	{
	public:
		XmlFile(std::istream&, std::ostream&);
	protected:
		virtual void write() const;
		virtual void read();
	};

} // namespace GetSetIO


#endif // __GetSetXml_h
