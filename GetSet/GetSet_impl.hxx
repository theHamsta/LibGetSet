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
// DEFNIITION OF SPECIAL TYPES Enum, Button, File, ect.
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
	SPECIAL_TYPE& set##TAG(const TYPE& value) {property->attributes[#TAG]=toString(value);return *this;}	\
	TYPE get##TAG() const {return stringTo<TYPE>(property->attributes[#TAG]);}

// The Enum class is more complex, because it has features of both GetSet<std::string> and GetSet<int>
#define GETSET_ENUM_CLASS_BODY																				\
	GETSET_TAG(Enum,std::vector<std::string>,Choices)														\
	Enum& setChoices(const std::string& c) {property->attributes["Choices"]=c;return *this;}				\
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
GETSET_SPECIALIZATION(SpinBox,double, GETSET_TAG(SpinBox,double,Min) GETSET_TAG(SpinBox,double,Step) GETSET_TAG(SpinBox,double,Max) GETSET_TAG(SpinBox,bool,Periodic) , )

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
namespace GetSetGui {
	class Section : public GetSetInternal::Access
	{
		GetSetInternal::GetSetSection	*section;
		std::string						pathToSection;
	public:
		Section(const std::string& _pathToSection, GetSetDictionary& d=GetSetDictionary::global())
			: GetSetInternal::Access(d)
			, pathToSection(_pathToSection)
		{
			section=dynamic_cast<GetSetInternal::GetSetSection*>(GetSetInternal::Access::getProperty(pathToSection));
		}
	
		// Please make sure that section exists before using it.
		bool exists() const 
		{
			return section!=0x0;
		}

		std::string getAttribute(const std::string& attrib)  const
		{
			if (section)
			{
				auto it=section->attributes.find(attrib);
				if (it!=section->attributes.end()) return it->second;
			}
			return "";
		}

		Section& setAttribute(const std::string& attrib, const std::string& value)
		{
			if (section) section->attributes[attrib]=value;
			signalChange(pathToSection,"");
			return *this;
		}

		Section& setDescription(const std::string& description) { return setAttribute("Description", description); }
		Section& setDisabled   (bool disabled)                  { return setAttribute("Disabled",    toString(disabled)); }
		Section& setGrouped    (bool grouped)                   { return setAttribute("Grouped",     toString(grouped)); }
		Section& setHidden     (bool hidden)                    { return setAttribute("Hidden",      toString(hidden)); }
		
		bool isDisabled() const { return stringTo<bool>(getAttribute("Disabled")); }
		bool isGrouped()  const { return stringTo<bool>(getAttribute("Grouped") ); }
		bool isHidden()   const { return stringTo<bool>(getAttribute("Hidden")  ); }

	};
}

namespace GetSetInternal {
	/// Create a special property by string
	inline GetSetNode* createSpecial(const std::string& type)
	{
		GetSetNode* node=0x0;
		#define GETSET_TYPE_STR(X) if (type==#X) node=new GetSetKey##X();
		// special types
		GETSET_TYPE_STR(Slider)
		GETSET_TYPE_STR(Enum)
		GETSET_TYPE_STR(Button)
		GETSET_TYPE_STR(StaticText)
		GETSET_TYPE_STR(ReadOnlyText)
		GETSET_TYPE_STR(Directory)
		GETSET_TYPE_STR(File)
		#undef GETSET_TYPE_STR
		return node;
	}
}

#undef GETSET_ENUM_CLASS_BODY
#undef GETSET_ENUM_KEY_BODY
#undef GETSET_SPECIALIZATION
#undef GETSET_TAG

#endif // __GetSetSpecial_hxx
