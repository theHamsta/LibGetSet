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

#ifndef _factory_h
#define _factory_hs

#include "Initializer.h"
#include "TypeString.hxx"

#include <map>
#include <set>
#include <string>

namespace Factory {

	class Object {
	public:
		virtual ~Object() {}
	};

	/// Create an Object by its type name as string
	Object* CallCreateMethod(const std::string& type);

	/// Declare a string as type name of an object
	void Declare(const std::string& type, Factory::Object* (*create)(), const std::string& interfaces);

	/// Get the set of all known types (optional: of a specific interface)
	const std::set<std::string>& KnownTypes(const std::string& interface="");

	/// Try to create an object by type name and try to cast it to Interface*.
	template <class Interface>
	Interface* Create(const std::string& type) {
		Factory::Object* obj=CallCreateMethod(type);
		Interface* intobj=dynamic_cast<Interface*>(obj);
		if (!intobj && obj) delete obj;
		return intobj;
	}

} // namespace Factory

/// Declare a factory product TYPE which derives from INTERFACES (a semicolon seperated list of types)
#define DECLARE_TYPE(TYPE, INTERFACES)									\
template<> inline std::string getTypeName<TYPE>() { return #TYPE; }		\
Factory::Object* Create##TYPE() {										\
	return new TYPE;													\
}																		\
INITIALIZER(Register##TYPE) {											\
	Factory::Declare(#TYPE,Create##TYPE,#INTERFACES);					\
}


#endif // _factory_h
