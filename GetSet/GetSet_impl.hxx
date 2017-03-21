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

#ifndef __GetSetSpecial_hxx
#define __GetSetSpecial_hxx

#include "GetSet.hxx"

// --- Implementation ---

template <typename BasicType>
GetSet<BasicType>::GetSet(const std::string& pathToSection, const std::string& k, GetSetDictionary& dict)
	: GetSetInternal::Access(dict)
	, section(pathToSection)
	, key(k)
    , property(&declare<GetSetInternal::GetSetKey<BasicType> >(pathToSection.empty()?key:pathToSection+"/"+key,false))
{typedProperty=dynamic_cast<GetSetInternal::GetSetKey<BasicType>*>(property);}

template <typename BasicType>
GetSet<BasicType>::GetSet(const std::string& pathToKey, GetSetDictionary& dict)
	: GetSetInternal::Access(dict)
	, section(pathToKey)
	, key(splitRight(section,"/\\"))
    , property(&declare<GetSetInternal::GetSetKey<BasicType> >(pathToKey,false))
{typedProperty=dynamic_cast<GetSetInternal::GetSetKey<BasicType>*>(property);}

template <typename BasicType>
GetSet<BasicType>& GetSet<BasicType>::setValue(const BasicType& v)
{
	if (typedProperty) typedProperty->value=v;
	else property->setString(toString(v));
	signalChange(section,key);
	return *this;
}

template <typename BasicType>
const BasicType GetSet<BasicType>::getValue() const
{
	if (typedProperty) return typedProperty->value;
	else return stringTo<BasicType>(property->getString());
}

template <typename BasicType>
const std::string GetSet<BasicType>::getType() const
{
	return property->getType();
}

template <typename BasicType>
GetSet<BasicType>& GetSet<BasicType>::setString(const std::string& value)
{
	property->setString(value);
	signalChange(section,key);
	return *this;
}

template <typename BasicType>
std::string GetSet<BasicType>::getString() const { return property->getString(); }

template <typename BasicType>
GetSet<BasicType>& GetSet<BasicType>::setDescription(const std::string& desc)
{
	return setAttribute("Description",desc);
}

template <typename BasicType>
std::string GetSet<BasicType>::getDescription() const
{
	return getAttribute("Description");
}

template <typename BasicType>
GetSet<BasicType>& GetSet<BasicType>::setAttribute(const std::string& attrib, const std::string& value)
{
	property->attributes[attrib]=value;
	signalUpdateAttrib(section,key);
	return *this;
}

template <typename BasicType>
std::string GetSet<BasicType>::getAttribute(const std::string& attrib) const
{
	std::map<std::string,std::string>::const_iterator it=property->attributes.find(attrib);
	if (it!=property->attributes.end()) return it->second;
	else return "";
}

//
// DEFINITION OF SPECIAL TYPES Enum, Button, File, ect.
//

/// Specializations for GUI representations
#define GETSET_SPECIALIZATION(SPECIAL_TYPE,BASE_TYPE,CLASS_BODY,KEY_BODY)									\
	namespace GetSetInternal																				\
	{																										\
		class GetSetKey##SPECIAL_TYPE : public GetSetKey<BASE_TYPE>											\
		{																									\
		public:																								\
			virtual std::string getType() const { return #SPECIAL_TYPE; }									\
			KEY_BODY																						\
		};																									\
	}																										\
	namespace GetSetGui																						\
	{																										\
		class SPECIAL_TYPE : public GetSet<BASE_TYPE>														\
		{																									\
		public:																								\
			SPECIAL_TYPE(const std::string& pathToKey, GetSetDictionary& d=GetSetDictionary::global())		\
				: GetSet<BASE_TYPE>(d)																		\
			{																								\
				section=pathToKey;																			\
				key=splitRight(section,"/\\");																\
				property=&declare<GetSetInternal::GetSetKey##SPECIAL_TYPE>(pathToKey,true);					\
				typedProperty=dynamic_cast<GetSetInternal::GetSetKey##SPECIAL_TYPE*>(property);				\
			}																								\
			SPECIAL_TYPE(const std::string& pathToSection, const std::string& k,							\
					GetSetDictionary& d=GetSetDictionary::global())											\
						: GetSet<BASE_TYPE>(d)																\
			{																								\
				section=pathToSection; key=k;std::string path=section.empty()?key:section+"/"+key;			\
				property=&declare<GetSetInternal::GetSetKey##SPECIAL_TYPE>(path,true);						\
				typedProperty=dynamic_cast<GetSetInternal::GetSetKey##SPECIAL_TYPE*>(property);				\
			}																								\
			GetSet<BASE_TYPE>& operator=(const BASE_TYPE& v) { return setValue(v); }						\
			operator BASE_TYPE() const { return getValue(); }												\
			CLASS_BODY																						\
		};																									\
	}
// end of GETSET_SPECIALIZATION

#define GETSET_TAG(SPECIAL_TYPE,TYPE,TAG)																	\
	SPECIAL_TYPE& set##TAG(const TYPE& value) {setAttribute(#TAG,toString(value));return *this;}			\
	TYPE get##TAG() const {return stringTo<TYPE>(property->attributes[#TAG]);}

// The Enum class is more complex, because it has features of both GetSet<std::string> and GetSet<int>
#define GETSET_ENUM_CLASS_BODY																				\
	GETSET_TAG(Enum,std::vector<std::string>,Choices)														\
	Enum& setChoices(const std::string& c) {setAttribute("Choices",c);return *this;}						\
	inline GetSet<int>& operator=(const std::string& v) { return setString(v); }							\
	inline operator std::string() const { return getString(); }

#define GETSET_ENUM_KEY_BODY																				\
	virtual std::string getString() const																	\
	{																										\
		std::map<std::string,std::string>::const_iterator it=attributes.find("Choices");					\
		if (it==attributes.end()) return toString(value);													\
		std::vector<std::string> c=stringTo<std::vector<std::string> >(it->second);							\
		if (value<0||value>=(int)c.size()) return toString(value); else return c[value];					\
	}																										\
	virtual void setString(const std::string& in)															\
	{																										\
		std::vector<std::string> c=stringTo<std::vector<std::string> >(attributes["Choices"]);				\
		for (int i=0;i<(int)c.size();i++)																	\
			if (c[i]==in) { value=i; return; }																\
		value=stringTo<int>(in);																			\
}

#define GETSET_BUTTON_KEY_BODY																				\
	void (*callback)(const std::string& info, void* user_data);												\
	std::string	caller_info;																				\
	void*		caller_data;																				\

#define GETSET_BUTTON_CLASS_BODY																			\
	virtual GetSet<std::string>& setString(const std::string& in)											\
	{																										\
		GetSet<std::string>::setString(in);																	\
		trigger();																							\
		return *this;																						\
	}																										\
	virtual GetSet<std::string>& setValue(const std::string& in)											\
	{																										\
		GetSet<std::string>::setValue(in);																	\
		trigger();																							\
		return *this;																						\
	}																										\
	Button& setCallback(void (*c)(const std::string&, void*), const std::string& info, void* data)			\
	{																										\
		auto *exactlyTypedProperty=dynamic_cast<GetSetInternal::GetSetKeyButton*>(property);				\
		if (!exactlyTypedProperty) { std::cerr << "GetSetGui::Button Wrong key type.\n"; return *this;}		\
		exactlyTypedProperty->caller_info=info;																\
		exactlyTypedProperty->caller_data=data;																\
		exactlyTypedProperty->callback=exactlyTypedProperty->caller_info.empty()?0x0:c;						\
		return *this;																						\
	}																										\
	void trigger()																							\
	{																										\
		auto *exactlyTypedProperty=dynamic_cast<GetSetInternal::GetSetKeyButton*>(property);				\
		if (!exactlyTypedProperty) { std::cerr << "GetSetGui::Button Wrong key type.\n"; return;}			\
		if (!exactlyTypedProperty->caller_info.empty())														\
			exactlyTypedProperty->callback(																	\
				exactlyTypedProperty->caller_info,															\
				exactlyTypedProperty->caller_data);															\
		signalChange(section,key);																			\
	}

/// A pulldown menu with a number of choices.
GETSET_SPECIALIZATION(Enum,int,GETSET_ENUM_CLASS_BODY, GETSET_ENUM_KEY_BODY)

/// A GetSet&lt;double&gt; with additional range information, so that it could be represented as a slider
GETSET_SPECIALIZATION(Slider,double, GETSET_TAG(Slider,double,Min) GETSET_TAG(Slider,double,Max), )

/// A GetSet&lt;double&gt; with additional range information, so that it could be represented as a SpinBox
GETSET_SPECIALIZATION(RangedDouble,double, GETSET_TAG(RangedDouble,double,Min) GETSET_TAG(RangedDouble,double,Step) GETSET_TAG(RangedDouble,double,Max) GETSET_TAG(RangedDouble,bool,Periodic) , )

/// A GetSet&lt;int&gt; with additional range information, so that it could be represented as a SpinBox
GETSET_SPECIALIZATION(RangedInt,int, GETSET_TAG(RangedInt,int,Min) GETSET_TAG(RangedInt,int,Step) GETSET_TAG(RangedInt,int,Max) GETSET_TAG(RangedInt,bool,Periodic) , )

/// A button that creates a GetSet change event when pressed.
GETSET_SPECIALIZATION(Button,std::string,GETSET_BUTTON_CLASS_BODY, GETSET_BUTTON_KEY_BODY)

/// A static text with some information. StaticTexts are not included in ini-Files (user-info in GUI)
GETSET_SPECIALIZATION(StaticText,std::string, , )

/// An edit field, but read-only. Intended for output-values that the user can select and copy to clipboard.
GETSET_SPECIALIZATION(ReadOnlyText,std::string, , )

/// A directory
GETSET_SPECIALIZATION(Directory,std::string, , )

/// A file (or multiple semicolon seperated files). Extensions is a string such as "Images (*.png *.xpm *.jpg);;All files (*)"
GETSET_SPECIALIZATION(File,std::string, GETSET_TAG(File,std::string,Extensions) GETSET_TAG(File,bool, CreateNew) GETSET_TAG(File,bool, Multiple), )


/// Access to sections and its parameters

class GetSetSection : public GetSetInternal::Access
{
	GetSetInternal::Section		*section;
	std::string					pathToSection;
	std::string					superSection;
	std::string					thisKey;
public:
	GetSetSection(const std::string& _pathToSection, GetSetDictionary& d=GetSetDictionary::global())
		: GetSetInternal::Access(d)
		, pathToSection(_pathToSection)
	{
		section=dynamic_cast<GetSetInternal::Section*>(GetSetInternal::Access::getProperty(pathToSection));
		superSection=_pathToSection;
		thisKey=splitRight(superSection,"/");
	}

	GetSetSection(const std::string& _superSection, const std::string& _thisKey, GetSetDictionary& d=GetSetDictionary::global())
		: GetSetInternal::Access(d)
		, pathToSection(_superSection.empty()?_thisKey:_superSection+"/"+_thisKey)
		, superSection(_superSection)
		, thisKey(_thisKey)
	{
		section=dynamic_cast<GetSetInternal::Section*>(GetSetInternal::Access::getProperty(pathToSection));
	}

	// Super section.
	GetSetSection super() const {return GetSetSection(superSection,dictionary);} 

	// Subsection.
	GetSetSection subsection(const std::string& key) const {return GetSetSection(pathToSection+"/"+key,dictionary);} 

	// Path within current dictionary.
	std::string path() const {return pathToSection;}

	/// Access to dictionary.
	GetSetDictionary& dict() {return dictionary;}

	/// Check whether the section exists.
	bool exists() const {return section!=0x0;}

	/// Discard all keys in this section and the section itself.
	void discard() const {dictionary.remove(pathToSection);}
	
	// Get or set a key in this section. See also special(...)
	template <typename BasicType=std::string>
	GetSet<BasicType> key(const std::string& key) const
	{
		return GetSet<BasicType>(pathToSection,key,dictionary);
	}

	// Get or set a key in this section. Specify GetSetGui:: types (GetSetGui::File for example);
	template <typename GetSetGuiType>
	GetSetGuiType special(const std::string& key) const 
	{
		return GetSetGuiType(pathToSection,key,dictionary);
	}

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

	/// Set value of an attribute.
	GetSetSection& setAttribute(const std::string& attrib, const std::string& value)
	{
		if (section) section->attributes[attrib]=value;
		signalUpdateAttrib(superSection,thisKey);
		return *this;
	}

	/// Set Description (e.g. shown as tool tip).
	GetSetSection& setDescription(const std::string& description) { return setAttribute("Description", description);         }

	///Contents of this section will not be modifiable in GUI.
	GetSetSection& setDisabled   (bool disabled=true)             { return setAttribute("Disabled",    toString(disabled));  }

	/// Show contents of this section in a group box.
	GetSetSection& setGrouped    (bool grouped=true)              { return setAttribute("Grouped",     toString(grouped));   }

	/// Collapse group box.
	GetSetSection& setCollapsed  (bool collapsed=true)            { setGrouped(true); return setAttribute("Collapsed", toString(collapsed));}
	
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
		if (isCollapsible())
		{
			if (collapsible)
				return *this;
			else
			{
				section->attributes.erase(section->attributes.find("Collapsed"));
				signalUpdateAttrib(superSection,thisKey);
			}
		}
		else
		{
			if (!collapsible)
				return *this;
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

namespace GetSetInternal {
	/// Create a special property by string.
	inline Node* createSpecial(const std::string& type)
	{
		Node* node=0x0;
		#define GETSET_TYPE_STR(X) if (type==#X) node=new GetSetKey##X();
		// special types
		GETSET_TYPE_STR(Slider)
		GETSET_TYPE_STR(RangedDouble)
		GETSET_TYPE_STR(Enum)
		GETSET_TYPE_STR(Button)
		GETSET_TYPE_STR(StaticText)
		GETSET_TYPE_STR(ReadOnlyText)
		GETSET_TYPE_STR(Directory)
		GETSET_TYPE_STR(File)
		#undef GETSET_TYPE_STR
		// Not actually that special at all
		if (type=="RangedInt") node=new GetSetKey<int>();
		return node;
	}
}

#undef GETSET_ENUM_CLASS_BODY
#undef GETSET_ENUM_KEY_BODY
#undef GETSET_SPECIALIZATION
#undef GETSET_TAG

#endif // __GetSetSpecial_hxx
