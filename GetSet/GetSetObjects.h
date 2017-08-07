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

#include "GetSet.hxx"

namespace GetSetObjects {

	/// Interface for a class which can be represented directly by a GetSet section.
	class Configurable {
	public:
		virtual void gui_declare_section(GetSetGui::Section&) = 0;
		virtual void gui_retreive_section(GetSetGui::Section&) = 0;
	};

	/// An interface for objects, which store their settings in a specific Section and are notified for changes relative to that section only.
	class Resident : public GetSetInternal::Dictionary::Observer {
	protected:
		GetSetInternal::Dictionary& dictionary;
		const std::string           path;
		bool ignore_notify;
	public:
		Resident(GetSetInternal::Section& section);

		/// The section where we store our parameters.
		GetSetGui::Section gui_section();
		
		/// A node in our gui_section() has changed its value. The relative path to where the node resides is passed in section.
		virtual void gui_notify(const std::string& section, const GetSetInternal::Node&) = 0;

		/// Temporarily ignore notifications
		void gui_ignore_notify(bool ignore=true);

	private:
		/// Check if this notification concerns a node in gui_section()
		void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal);
	};

	/// Interface for a class which stores and retreives all of its information via GetSet
	class Object : public Configurable, public Resident {
	public:
		Object(GetSetGui::Section& section);
		void gui_declare();
		void gui_retreive();
	};

} // namespace GetSetObjects

#endif // __getset_object_hxx
