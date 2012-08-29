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

#ifndef __GetSet_hxx
#define __GetSet_hxx

#include "GetSetDictionary.h"

/// Syntactic sugar to access and change GetSet properties
template <typename BasicType=std::string>
class GetSet : public GetSetInternal::Access
{
protected:
	//// The path to the section where this property resides
	std::string section;
	//// The name to the associated property in section
	std::string key;

	/// Keep track of the associated property (not actually owned by this class)
	GetSetInternal::GetSetNode* property;
	/// Same as property, if the type is an exact match. Else it is null.
	GetSetInternal::GetSetKey<BasicType>* typedProperty;

	/// c-tor for subclasses: initialize property from there.
	GetSet(GetSetDictionary& dict) : GetSetInternal::Access(dict) {}

public:
	/// Access a GetSet property by section and key (and optionally explicitly a dictionary)
	GetSet(const std::string& pathToSection, const std::string& k, GetSetDictionary& dict=GetSetDictionary::global())
		: GetSetInternal::Access(dict)
		, section(pathToSection)
		, key(k)
		, property(&declare<GetSetInternal::GetSetKey<BasicType>>(pathToSection.empty()?key:pathToSection+"/"+key,false))
	{typedProperty=dynamic_cast<GetSetInternal::GetSetKey<BasicType>*>(property);}

	/// Access a GetSet property by the absolute path to its key (and optionally explicitly a dictionary)
	GetSet(const std::string& pathToKey, GetSetDictionary& dict=GetSetDictionary::global())
		: GetSetInternal::Access(dict)
		, section(pathToKey)
		, key(splitRight(section,"/\\"))
		, property(&declare<GetSetInternal::GetSetKey<BasicType>>(pathToKey,false))
	{typedProperty=dynamic_cast<GetSetInternal::GetSetKey<BasicType>*>(property);}

	/// Set the value of a GetSet property (same as: assigment operator)
	void setValue(const BasicType& v)
	{
		if (typedProperty) typedProperty->value=v;
		else property->setString(toString(v));
		signalChange(section,key);
	}

	/// Get the value of a GetSet property (same as: cast operator)
	const BasicType getValue() const
	{
		if (typedProperty) return typedProperty->value;
		else return stringTo<BasicType>(property->getString());
	}

	/// Set the value of a GetSet property directly via assignment operator
	inline void operator=(const BasicType& v) { setValue(v); }

	/// Cast operator directly to BasicType (behaves almost like a c++ variable of BasicType)
	inline operator BasicType() const { return getValue(); }

	/// Get the value of the property as string
	virtual std::string getString() const
	{
		return property->getString();
	}

	/// Set the value of this property from a string
	virtual void setString(const std::string& value)
	{
		property->setString(value);
		signalChange(section,key);
	}

	/// Set a brief descriptino for this property 
	void setDescription(const std::string& desc)
	{
		property->attributes["Description"]=desc;
	}

	/// Get a brief descriptino for this property 
	const std::string& setDescription()
	{
		return property->attributes["Description"];
	}

};

/// A very simple helper class to navigate
class GetSetPath
{
protected:
	//// The path to a GetSetSection
	std::string absolutePath;
	/// The dictionary this path is rooted in
	mutable GetSetDictionary& dictionary;
public:
	GetSetPath(const std::string& path="", GetSetDictionary& dict=GetSetDictionary::global())
		: dictionary(dict)
		, absolutePath(path)
	{}

	/// Use parenthesis operator to navigate
	GetSetPath operator()(const std::string& key) const
	{
		return GetSetPath(getPath(key),dictionary);
	}

	/// Access to absolute path (optionally: of a key)
	std::string getPath(const std::string& key="") const
	{
		if (key.empty()) return absolutePath;
		else return absolutePath.empty()?key:absolutePath+"/"+key;
	}

	/// Access to dictionary.
	GetSetDictionary& getDictionary() const {return dictionary;}

	/// Use key&lt;BasicType&gt;("MyKey") to get/set a property value
	template <typename BasicType>
	inline GetSet<BasicType> key(const std::string& k) const
	{
		return GetSet<BasicType>(getPath(k),dictionary);
	}

};


//
// Special GetSet types: Slider Enum Button StaticText Directory File
//

/// Specializations for GUI representations
#define GETSET_SPECIALIZATION(SPECIAL_TYPE,BASE_TYPE,CLASS_BODY)											\
	namespace GetSetInternal																				\
	{																										\
		class GetSetKey##SPECIAL_TYPE : public GetSetKey<BASE_TYPE>											\
		{																									\
			virtual std::string getType() const { return #SPECIAL_TYPE; }									\
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
				key=splitRight(section,":\\");																\
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
			void operator=(const BASE_TYPE& v) { setValue(v); }												\
			operator BASE_TYPE() const { return getValue(); }												\
			CLASS_BODY																						\
		};																									\
	}
// end of GETSET_SPECIALIZATION

#define GETSET_TAG(SPECIAL_TYPE,TYPE,TAG)																	\
	SPECIAL_TYPE& set##TAG(const TYPE& value) {property->attributes[#TAG]=toString(value);return *this;}	\
	TYPE get##TAG() const {return stringTo<TYPE>(property->attributes[#TAG]);}

// The Enum class is a little more complex, because it has features of both GetSet<std::string> and GetSet<int>
#define GETSET_ENUM_CODE																					\
	GETSET_TAG(Enum,std::vector<std::string>,Choices)														\
	Enum& setChoices(const std::string& c) {property->attributes["Choices"]=c;return *this;}				\
	inline void operator=(const std::string& v) { setString(v); }											\
	inline operator std::string() const { return getString(); }												\
	virtual std::string getString() const																	\
	{																										\
		std::vector<std::string> c=getChoices();int i=getValue();											\
		if (i<0||i>=(int)c.size()) return ""; else return c[i];												\
	}																										\
	virtual void setString(const std::string& in)															\
	{																										\
		std::vector<std::string> c=getChoices();															\
		for (int i=0;i<(int)c.size();i++)																	\
			if (c[i]==in)																					\
				return setValue(i);																			\
		setValue(stringTo<int>(in));																		\
	}

/// A pulldown menu with a number of choices.
GETSET_SPECIALIZATION(Enum,int,GETSET_ENUM_CODE)

/// A GetSet&lt;double&gt; with additional range information, so that it could be represented as a slider
GETSET_SPECIALIZATION(Slider,double, GETSET_TAG(Slider,double,Min) GETSET_TAG(Slider,double,Max) )

/// A button that creates a GetSet change event when pressed.
GETSET_SPECIALIZATION(Button,std::string, void action() {signalChange(section,key);} )

/// A static text with some information. StaticTexts are not included in ini-Files (user-info in GUI)
GETSET_SPECIALIZATION(StaticText,std::string, )

/// An edit field, but read-only. Intended for output-values that the user can select and copy to clipboard.
GETSET_SPECIALIZATION(ReadOnlyText,std::string, )

/// A directory
GETSET_SPECIALIZATION(Directory,std::string, )

/// A file (or multiple semicolon seperated files). Extensions is a string such as "Images (*.png *.xpm *.jpg);;All files (*)"
GETSET_SPECIALIZATION(File,std::string, GETSET_TAG(File,std::string,Extensions) GETSET_TAG(File,bool, CreateNew) GETSET_TAG(File,bool, Multiple) )

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

#undef GETSET_ENUM_CODE
#undef GETSET_SPECIALIZATION
#undef GETSET_TAG

#endif // __GetSet_hxx
