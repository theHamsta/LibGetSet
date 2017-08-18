//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andre Aichert (aaichert@gmail.com)
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

#ifndef __getset_object_hxx
#define __getset_object_hxx

#include "GetSetIO.h"

#include "ProgressInterface.hxx"

#include <set>
#include <sstream>

namespace GetSetObjects {
	
	/// Interface for a class which can be represented directly by a GetSet Section.
	struct Configurable {
	public:
		virtual void gui_declare_section (const GetSetGui::Section& ) = 0;
		virtual void gui_retreive_section(const GetSetGui::Section& ) = 0;

		/// Load this object from an ini-File
		inline bool gui_load(const std::string& file) {
			GetSetInternal::Dictionary dict;
			if (!GetSetIO::load(file,dict))
				return false;
			gui_retreive_section(dict);
			return true;
		}

		/// Save this object to an ini-File
		inline bool gui_save(const std::string& file) {
			GetSetInternal::Dictionary dict;
			gui_declare_section(dict);
			return GetSetIO::save(file,dict);
		}

	};

	/// An interface for objects, which store their settings in a specific Section and are notified for changes relative to that section only.
	class Object : public GetSetInternal::Dictionary::Observer {
	protected:
		GetSetInternal::Dictionary&           dictionary;
		const std::string                     path;
		mutable GetSetGui::ProgressInterface& app;
		mutable bool                          ignore_notify;

	public:
		/// Contructor for all objects. For default ProgressInterface, see factory_use_progress_interface.
		Object(const GetSetGui::Section& section, GetSetGui::ProgressInterface *app=0x0);

		/// The section where we store our parameters.
		GetSetGui::Section gui_section() const;
		
		/// Two-stage construction. By default, tries to cast to Configurable and call gui_declare.
		virtual void gui_init();

		/// A node in our gui_section() has changed its value. The relative path to where the node resides is passed in section.
		virtual void gui_notify(const std::string& relative_section, const GetSetInternal::Node& node) {}

		/// Temporarily ignore notifications
		void gui_ignore_notify(bool ignore=true) const ;

		/// Dynamic cast to a specific type
		template <class Derived> Derived* cast() {return dynamic_cast<Derived*>(this);}

	private:
		/// Check if this notification concerns a node in gui_section() and, if so, call gui_notify with its raltive path.
		void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal);
	};

	/// A simple object which lives in a fixed section
	template <class ConfigurableDerived>
	struct Struct : public ConfigurableDerived, public Object {
		Struct(const GetSetGui::Section& section, GetSetGui::ProgressInterface *app=0x0) : Object(section, app) {}
	};

} // namespace GetSetObjects

// 
// Object Factory (use is optional)
// 

// For Human-readable object names across compiliation units and compilers put this in your header.
#define GETSET_OBJECT_DECLARE(CLASS_NAME) template<> inline std::string typeName<CLASS_NAME>() { return #CLASS_NAME; }

// If you want your GetSetObjects::Object CLASS_NAME registered with the factory, put this in your c++ file.
#define GETSET_OBJECT_REGISTER(CLASS_NAME)                                                                                   \
	GetSetObjects::Object * factory_create##CLASS_NAME(const GetSetGui::Section& section, GetSetGui::ProgressInterface& app) \
	{ CLASS_NAME * obj=new CLASS_NAME(section,&app); obj->gui_init(); return obj; }                                          \
	GetSetObjects::FactoryRegistration<CLASS_NAME> factory_register##CLASS_NAME(factory_create##CLASS_NAME);

// Same as GETSET_OBJECT_DECLARE, except that for typedef GetSetObjects::Struct<CLASS_NAME> ##CLASS_NAMEGui
#define GETSET_OBJECT_STRUCT_DECLARE(CLASS_NAME)                                                                     \
	typedef GetSetObjects::Struct<CLASS_NAME> ##CLASS_NAME##Gui;                                                       \
	GETSET_OBJECT_DECLARE(##CLASS_NAME##Gui)

// Same as GETSET_OBJECT_REGISTER, except with ##CLASS_NAMEGui.
#define GETSET_OBJECT_STRUCT_REGISTER(CLASS_NAME) GETSET_OBJECT_REGISTER(##CLASS_NAME##Gui)

namespace GetSetObjects {
	/// Set the default ProgressInterface which GetSetTypes use.
	void factory_progress_set_default_interface(GetSetGui::ProgressInterface&);

	/// Set the default ProgressInterface which GetSetTypes use.
	GetSetGui::ProgressInterface& factory_progress_default_interface();

	/// A set of known types. To register a type, use GETSET_OBJECT_REGISTER(CLASS_NAME) in your c++ file.
	std::set<std::string> factory_known_types();

	/// Try to create an object of a specified type. Returns null for unknown types.
	Object* factory_create(const std::string& class_name, const GetSetGui::Section& section,
		GetSetGui::ProgressInterface& progress_interface=factory_progress_default_interface());

	/// Try to create an object of a specified type. Returns null for unknown types.
	inline Object* factory_create(const std::string& class_name) { factory_create(class_name,class_name); }

	// 
	// Please do not use the following factory_register_type and FactoryRegistration directly.
	// Instead, use GETSET_OBJECT_REGISTER(CLASS_NAME) in your c++ file.
	// 

	void factory_register_type(const std::string&, Object* (*)(const GetSetGui::Section&, GetSetGui::ProgressInterface&));
	template <class GetSetObject>
	struct FactoryRegistration {
		FactoryRegistration(Object* (*instantiate)(const GetSetGui::Section&, GetSetGui::ProgressInterface&))
		                       {factory_register_type(typeName<GetSetObject>(),instantiate);}
	};

} // namespace GetSetObjects

#endif // __getset_object_hxx
