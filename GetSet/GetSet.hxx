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

#ifndef __GetSet_h
#define __GetSet_h

#include "GetSetDictionary.h"

/// Syntactic Sugar to access and change GetSet parameters
template <typename BasicType>
class GetSet
{
protected:
	std::string	section;
	std::string	key;
	GetSetDictionary&	dict;

	GetSet(GetSetDictionary& d) : dict(d) {}

	void create()
	{
		using namespace GetSetInternal;
		GetSetDataInterface* p=dict.getDatainterface(section,key);
		if (!p) {
			dict.get()[section][key]=new GetSetData<BasicType>();
			dict.signalCreate(section,key);
		}
	}

public:
	GetSet(GetSetDictionary& d, const std::string& s, const std::string k)
		: dict(d), section(s), key(k) { create(); }

	GetSet(const std::string& s, const std::string k)
		: dict(GetSetDictionary::globalDictionary()), section(s), key(k) { create(); }

	/// Set the value of a GetSet property directly via assignment operator
	void operator=(const BasicType& v)
	{
		using namespace GetSetInternal;
		GetSetDataInterface* p=dict.getDatainterface(section,key);
		GetSetData<BasicType>* sameType=dynamic_cast<GetSetData<BasicType>*>(p);
		if (sameType)
		{
			sameType->value=v;
			dict.signalChange(section,key);
		}
		else
			p->setString(toString(v));
	}

	/// Cast operator directly to BasicType (behaves almost like a  c++ variable of BasicType)
	operator BasicType() const
	{
		using namespace GetSetInternal;
		GetSetDataInterface* p=dict.getDatainterface(section,key);
		GetSetData<BasicType>* sameType=dynamic_cast<GetSetData<BasicType>*>(p);
		if (sameType)
			return sameType->value;
		else
			return stringTo<BasicType>(p->getString());
	}

	/// Get the value of the property as string
	std::string getString() const
	{
		return dict.getDatainterface(section,key)->getString();
	}

	/// Set the value of this property from a string
	void setString(const std::string& value)
	{
		dict.getDatainterface(section,key)->setString(value);
		dict.signalChange(section,key);
	}

};


//
// Special GetSet types: Slider Enum Trigger StaticText Directory File
//

/// Specializations for GUI representations
#define GETSET_SPECIALIZATION(SPECIAL_TYPE,BASE_TYPE,CLASS_BODY)											\
	namespace GetSetInternal {																				\
		class GetSetData##SPECIAL_TYPE : public GetSetData<BASE_TYPE> {										\
			virtual std::string getType() const { return #SPECIAL_TYPE; }									\
			};																								\
	}																										\
	namespace GetSetGui {																					\
		class SPECIAL_TYPE : public GetSet<BASE_TYPE> {														\
		public:																								\
			SPECIAL_TYPE(GetSetDictionary& d, const std::string& s, const std::string k)					\
				: GetSet<BASE_TYPE>(d) { section=s; key=k; forceType(); }									\
			SPECIAL_TYPE(const std::string& s, const std::string k)											\
				: GetSet<BASE_TYPE>(GetSetDictionary::globalDictionary()){ section=s; key=k; forceType(); }	\
			void operator=(const BASE_TYPE& v) { setString(toString(v)); }									\
			operator BASE_TYPE() const { return stringTo<BASE_TYPE>(getString()); }							\
			CLASS_BODY																						\
		protected:																							\
			void forceType() {																				\
				GetSetInternal::GetSetDataInterface* p=dict.getDatainterface(section,key);					\
				if (p && ! dynamic_cast<GetSetInternal::GetSetData##SPECIAL_TYPE*>(p)) { delete p; p=0x0; }	\
				if (!p) {																					\
					dict.get()[section][key]=new GetSetInternal::GetSetData##SPECIAL_TYPE();				\
					dict.signalCreate(section,key);															\
				}																							\
			}																								\
		};																									\
	}

#define GETSET_TAG(TYPE,TAG)																				\
	void set##TAG(const TYPE& value) { dict.get()[section][key]->attributes[#TAG]=toString(value); }		\
	TYPE get##TAG() const {return stringTo<TYPE>(dict.get()[section][key]->attributes[#TAG]);}

/// A GetSet&lt;double&gt; with additional range information, so that it could be represented as a slider
GETSET_SPECIALIZATION(Slider,double, GETSET_TAG(double, Min) GETSET_TAG(double, Max) )

/// A pulldown menu with a number of choices (provided as a semicolon seperated list of strings)
GETSET_SPECIALIZATION(Enum,int, GETSET_TAG(std::string, Choices) )

/// A button
GETSET_SPECIALIZATION(Trigger,std::string,)

/// A static text with some information. StaticTexts are not included in ini-Files and are useful as seperators or user-info in GUI.
GETSET_SPECIALIZATION(StaticText,std::string, )

/// An edit field, but read-only. Intended for output-values that the user can select and copy to clipboard but cannot change.
GETSET_SPECIALIZATION(ReadOnlyText,std::string, )

/// A directory
GETSET_SPECIALIZATION(Directory,std::string, );

/// A file (or multiple semicolon seperated files if Multiple is set). Extensions is astring such as "Images (*.png *.xpm *.jpg);;All files (*)"
GETSET_SPECIALIZATION(File,std::string, GETSET_TAG(std::string,Extensions) GETSET_TAG(bool, CreateNew) GETSET_TAG(bool, Multiple) );

#undef GETSET_SPECIALIZATION
#undef GETSET_TAG

#endif // __GetSet_h
