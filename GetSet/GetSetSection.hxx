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

#ifndef __GetSetSection_hxx
#define __GetSetSection_hxx

#include "GetSetDictionary.h"

/// Access to sections and their parameters

class GetSetSection : public GetSetInternal::Access
{
	GetSetInternal::Section		*section;
	std::string					path_to_super;
	std::string					section_name;
public:
	GetSetSection(const std::string& path_to_section="", const GetSetDictionary& d = GetSetDictionary::global())
		: GetSetInternal::Access(d)
		, path_to_super(path_to_section)
	{
		path_to_super=path_to_section;
		section_name=splitRight(path_to_super,"/\\");
		section=dynamic_cast<GetSetInternal::Section*>(GetSetInternal::Access::getProperty(path_to_section));
	}

	GetSetSection(const GetSetDictionary& d)
		: GetSetInternal::Access(d)
	{
		section=dynamic_cast<GetSetInternal::Section*>(GetSetInternal::Access::getProperty(""));
	}
	/// Access to dictionary.
	const GetSetDictionary& dict() const {return dictionary;}

	/// Access to dictionary.
	GetSetDictionary& dict() {return dictionary;}

	// Path within current dictionary.
	std::string path() const {return path_to_super.empty()?section_name:path_to_super+"/"+section_name;}

	// Super section. (for root, returns self)
	GetSetSection super() const { return GetSetSection(path_to_super,dictionary); } 

	// Sub-section.
	GetSetSection subsection(const std::string& key) const {return GetSetSection(path(key),dictionary);} 

	// Get absolute path to a key
	std::string path(const std::string& key) const { return (section_name.empty()?key:path()+"/"+key); }

	/// Check whether the section exists.
	bool exists() const {return section!=0x0;}

	/// Discard all keys in this section and the section itself.
	void discard() {dictionary.remove(path());}
	
	/// Get type of a key in this section. Returns empty string if key does not exist.
	std::string getTypeOfKey(const std::string& key)
	{
		if (section->getSection().find(key)!=section->getSection().end())
			return section->getSection().find(key)->second->getType();
		else return "";
	}

	/// Get current value of an attribute.
	std::string getAttribute(const std::string& attrib)  const
	{
		if (section)
		{
			auto it=section->attributes.find(attrib);
			if (it!=section->attributes.end()) return it->second;
		}
		return "";
	}

	/// Set value of an attribute. Erase attribute if no value is given.
	GetSetSection& setAttribute(const std::string& attrib, const std::string& value="")
	{
		if (section && value.empty())
			section->attributes.erase(section->attributes.find("Collapsed"));
		else if (section) section->attributes[attrib]=value;
		signalUpdateAttrib(path_to_super,section_name);
		return *this;
	}

	/// Set Description (e.g. shown as tool tip).
	GetSetSection& setDescription(const std::string& description) { return setAttribute("Description", description);         }

	///Contents of this section will not be modifiable in GUI.
	GetSetSection& setDisabled   (bool disabled=true)             { return setAttribute("Disabled",    toString(disabled));  }

	/// Show contents of this section in a group box.
	GetSetSection& setGrouped    (bool grouped=true)              { return setAttribute("Grouped",     toString(grouped));   }

	/// Collapse group box.
	GetSetSection& setCollapsed  (bool collapsed=true)            { setGrouped(); return setAttribute("Collapsed", toString(collapsed));}
	
	/// This section will not be shown in GUI at all.
	GetSetSection& setHidden     (bool hidden=true)               { return setAttribute("Hidden",      toString(hidden));    }
		
	/// Are contents of this section shown in a collapsible group box?
	bool isCollapsible()
	{
		auto it=section->attributes.find("Collapsed");
		return it!=section->attributes.end();
	}

	/// Show contents of this section in a collapsible group box.
	GetSetSection& setCollapsible  (bool collapsible=true)
	{
		if (isCollapsible()==collapsible)
			return *this; // Noting to be done.
		else
		{
			if (!collapsible)
				setAttribute("Collapsed","");
			else
				{
					setAttribute("Grouped","true");
					setAttribute("Collapsed","false");
				}
		}
		return *this;
	}

	bool isDisabled()  const { return stringTo<bool>(getAttribute("Disabled") ); }
	bool isGrouped()   const { return stringTo<bool>(getAttribute("Grouped")  ); }
	bool isHidden()    const { return stringTo<bool>(getAttribute("Hidden")   ); }

};

#endif // __GetSetSection_hxx
