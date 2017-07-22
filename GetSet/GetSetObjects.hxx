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
		Resident(GetSetInternal::Section& section)
			: GetSetInternal::Dictionary::Observer(section.dictionary)
			, dictionary(section.dictionary)
			, path(section.path())
			, ignore_notify(false)
		{}

		/// The section where we store our parameters.
		GetSetGui::Section gui_section() {return GetSetGui::Section(path,dictionary);}

		/// A node in our gui_section() has changed its value. The relative path to where the node resides is passed in section.
		virtual void gui_notify(const std::string& section, const GetSetInternal::Node&) = 0;

		/// Temporarily ignore notifications
		void gui_ignore_notify(bool ignore=true) { ignore_notify=ignore; }

	private:
		/// Check if this notification concerns a node in gui_section()
		void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal) {
			if (!ignore_notify && signal==GetSetInternal::Dictionary::Signal::Change)
			{
				// Is node located somewhere below our section? If so, not our business.
				if (node.super_section.size()<=path.size()) return; 
				// check if the node is located in our section by checking is path is a prefix of the super_section
				bool prefix_ok=true;
				for (int i=0;i<(int)path.size();i++, prefix_ok)
					prefix_ok&=path[i]==node.super_section[i];
				// If, so, we are hereby notified.
				if (prefix_ok) gui_notify(node.super_section.substr(path.size()), node);
			}

		}
	};

	/// Interface for a class which stores and retreives all of its information via GetSet
	class Object : public Configurable, public Resident {
	public:
		
		Object(GetSetGui::Section& section)
			: Resident(section)
		{}
		
		void gui_declare() {
			gui_declare_section(gui_section());
		}

		void gui_retreive() {
			gui_retreive_section(gui_section());
		}

	};

} // namespace GetSetObjects

#endif // __getset_object_hxx
