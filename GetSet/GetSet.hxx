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
	/// The path to the section where this property resides
	std::string section;
	/// The name to the associated property in section
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

	/// Set a brief description for this property
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
class GetSetPath : public GetSetInternal::Access
{
protected:
	//// The path to a GetSetSection
	std::string absolutePath;
public:
	GetSetPath(const std::string& path="", GetSetDictionary& dict=GetSetDictionary::global())
		: Access(dict)
		, absolutePath(path)
	{}

	/// Use parenthesis operator to navigate
	GetSetPath operator()(const std::string& key)
	{
		return GetSetPath(getPath(key),dictionary);
	}

	/// Access to absolute path (optionally: of a key)
	std::string getPath(const std::string& key="")
	{
		if (key.empty()) return absolutePath;
		else return absolutePath.empty()?key:absolutePath+"/"+key;
	}

	/// Access to dictionary.
	GetSetDictionary& getDictionary() {return dictionary;}

	bool hasKey(const std::string& k)
	{
		return getProperty(getPath(k))==0x0;
	}

	/// Use key&lt;BasicType&gt;("MyKey") to get/set a property value
	template <typename BasicType>
	inline GetSet<BasicType> key(const std::string& k)
	{
		return GetSet<BasicType>(getPath(k),dictionary);
	}

};

// Lots of preprocessor magic to generate sub-classes from GetSet<T> with different GUI behaviour
#include "GetSetSpecial.hxx"

#endif // __GetSet_hxx
