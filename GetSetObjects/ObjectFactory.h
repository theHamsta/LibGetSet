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

#ifndef OBJECT_FACTORY
#define OBJECT_FACTORY

#include <vector>
#include <string>

#include "Configurator.hxx"

/// A makro for factory Objects. Goes into the class body of CLASS_TYPE in its header file. INTERFACE_TYPE should be super to CLASS_NAME or Object.
/// Develper of CLASS_TYPE is expected to define contructor CLASS_TYPE(Configurator& config); and use GETSET_REGISTER_CLASS makro in the cpp file.
#define GETSET_DECLARE_CLASS(CLASS_TYPE, INTERFACE_TYPE)																							\
	public:																																			\
		CLASS_TYPE(GetSetObjects::Configurator& config);																							\
		static void Register();																														\
		static inline GetSetObjects::Object* Create(GetSetObjects::Configurator& config) {															\
			return new CLASS_TYPE(config);																											\
		}																																			\
		static inline std::string InterfaceName() { return #INTERFACE_TYPE; }																		\
		static inline std::string ClassName() { return #CLASS_TYPE; }																				\
		virtual       std::string getClassName() { return #CLASS_TYPE; }																			\
	private:																																		\
		static GetSetObjects::Factory::Registration _factory_registration;


/// A makro for the class header of all factory interfaces. This forces the developer to also use GETSET_DECLARE_CLASS and define a Register() method.
/// In case of static linkage the developer is expected delegate calls of INTERFACE_TYPE::Register() to _all_ known subclasses.
/// In case of dynamic loading, the dllmain() or similar initializer function must register all factory classes. 
#define GETSET_DECLARE_INTERFACE(INTERFACE_TYPE) public:																							\
		static inline std::string ClassName() { return #INTERFACE_TYPE; }																			\
		virtual       std::string getClassName() = 0;																								\
		inline static void Register();


/// The registration of the class type. Goes into the cpp file of class with CLASS_NAME, where INTERFACE_TYPE should be super to CLASS_NAME
/// Although Register() doesn't actually do anything, calling this function ensures that all static objects in the compilation unit are initialized.
#define GETSET_REGISTER_CLASS(CLASS_NAME)																											\
	GetSetObjects::Factory::Registration CLASS_NAME::_factory_registration(CLASS_NAME::InterfaceName(),CLASS_NAME::ClassName(),CLASS_NAME::Create);	\
	void CLASS_NAME::Register() {} // c++ guarantees that the c-tor of static _factory_registration is called _before_ Register().

namespace GetSetObjects
{
		
	/// A common super class to allow dynamic casts
	struct Object {
		GETSET_DECLARE_INTERFACE(Object)
		virtual ~Object() {}
	};

	/// List of all types known to the factory, optionally just for a specific interface.
	std::set<std::string> KnownTypes(const std::string& interfaceClass="");

	/// List of all interfaces known to the factory.
	std::set<std::string> KnownInterafces();

	/// A factory class with configuration of Objects.
	class Factory {
	private:
		Configurator& m_config;

	public:
		/// A class to register class names to create-methods.
		struct Registration {
			Registration(const std::string& interfaceName, const std::string& className, Object* (*create)(Configurator& config));
		};

		/// Exception thrown by this class
		class Exception : public std::exception
		{
			friend class Factory;
			Exception(const std::string& msg) : std::exception(("ObjectFactory: "+msg).c_str()) {}
		};

		/// Instantiate a factory given a Configurator
		Factory(Configurator& config) : m_config(config) {}

		/// Try to create an object with name instanceName of type className.
		Object* Create(const std::string& className, const std::string& instanceName);

		/// Create an object of FactoryClass directly and initialize.
		template <class FactoryClass>
		FactoryClass* Create(const std::string& instanceName)
		{
			FactoryClass::Register();
			return (FactoryClass*)Create(FactoryClass::ClassName(), instanceName);
		}

		/// Try to create an oject that derives from InterfaceClass.
		template <class InterfaceClass>
		InterfaceClass* CreateForInterface(const std::string& className, const std::string& instanceName)
		{
			InterfaceClass::Register();
			Object*	fobj = Factory::Create(className,instanceName);
			InterfaceClass*	dobj = dynamic_cast<InterfaceClass*>(fobj);
			if (dobj) return dobj; // success
			if (fobj)
			{
				delete fobj;
				// 2do: list KnownTypes(InterfaceClass::ClassName);
				throw Exception("The type of " + className + " does not match the requested Interface!");
			}
			else throw Exception("Failed to instantiate object of unkown type " + className + "!");
		}
	
	};

} // namespace GetSetObjects



#endif // OBJECT_FACTORY
