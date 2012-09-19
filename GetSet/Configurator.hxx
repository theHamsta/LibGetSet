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
	/// Configurator for the object factory
	class Configurator {
	protected:
		GetSetPath path;

	public:
		Configurator(GetSetDictionary& dict=GetSetDictionary::global(), const std::string& section="") : path(section,dict) {}

		template <typename T>
		void declare(const std::string& param, T& value) {
			if (path.hasKey(param))
				value=path.key<T>(param);
			else
				path.key<T>(param)=value;
		}

		Configurator subsection(const std::string& section) {
			return Configurator(path.getDictionary(),path.getPath(section));
		}

		template <class Type>
		void create(
		config.create("Additional Ve
		hicle 1",m_additionalVehicle1,"Plane");
	};
	
	template <class Interface>
	Interface* CreateAndConfigure(const std::string& type, Configurator& config) {
		Interface* obj=Create<Interface>(type);
		if (!obj) return obj;
		obj->configure(config.subsection(type));
		return obj;
	}

} // namespace Factory

#endif // __Configurator_h
