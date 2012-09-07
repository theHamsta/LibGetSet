//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by André Aichert (aaichert@gmail.com) and Thomas Koehler 
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

#ifndef __Configurator_h
#define __Configurator_h

#include "GetSet.hxx"

namespace Factory {
{
	/// Configurator for the object factory
	class Configurator : public GetSetInternal::Access {
	public:
		template <typename T>
		void declare(const std::string& param)
		{
			
		}

	};
	
	template <class Interface>
	Type* CreateAndConfigure(const std::string& type, Configurator& config) {
		Type* obj=Create<Type>(type);
		if (!obj) return obj;
		obj->configure(config.)
		return obj;
	}

} // namespace Factory

#endif // __Configurator_h
