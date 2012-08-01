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

#ifndef __GetSetDictionary_h
#define __GetSetDictionary_h

#include <string>
#include <vector>
#include <map>
#include <set>

#include "StringUtil.hxx"

namespace GetSetInternal {

	/// Variant interface class
	class GetSetDataInterface {
	public:
		virtual std::string getType() const = 0;
		virtual void setString(const std::string& new_value) = 0;
		virtual std::string getString() const = 0;
		std::map<std::string,std::string> attributes;
	};

	/// Templated Variant specialization
	template <typename T>
	class GetSetData : public GetSetDataInterface
	{
	public:
		T value;

		virtual std::string getType() const {
			return getTypeName<T>();
		}

		virtual void setString(const std::string& new_value) {
			value=stringTo<T>(new_value);
		}

		virtual std::string getString() const {
			return toString(value);
		}
	};

	/// Get a c++ type name as string
	template <typename T> inline std::string getTypeName()
	{
		return typeid(T).name();
	}

	// specializations 
	#define DEFINE_TYPE_NAME_STRING(X)  template<> inline std::string getTypeName<X>() {return #X;}
	DEFINE_TYPE_NAME_STRING(bool)
	DEFINE_TYPE_NAME_STRING(int)
	DEFINE_TYPE_NAME_STRING(unsigned)
	DEFINE_TYPE_NAME_STRING(float)
	DEFINE_TYPE_NAME_STRING(double)
	DEFINE_TYPE_NAME_STRING(std::string)
	DEFINE_TYPE_NAME_STRING(std::vector<std::string>)
	DEFINE_TYPE_NAME_STRING(std::vector<double>)
	DEFINE_TYPE_NAME_STRING(std::vector<int>)

} // GetSetInternal

namespace tinyxml2 {
	class XMLElement;
} // namespace tinyxml2

/// Dictionary holding Sections of keys with associated properties. Usually not used directly.
class GetSetDictionary
{
public:
	typedef std::map<std::string, GetSetInternal::GetSetDataInterface*> Section;
	typedef std::map<std::string, Section> Dictionary;

	static GetSetDictionary& globalDictionary();

	~GetSetDictionary();

	/// Add parameters and their values from an xml file.
	void			parseXML(const std::string& xml);
	/// Returns a string with an xml description of this dictionary
	std::string		getXML() const;

	/// Add values from an ini file.
	void			parseIni(const std::string& ini);
	/// Returns a string with all values in Ini-file format
	std::string		getIni() const;

	/// Remove a single parameter from the dictionary
	void erase(const std::string& section, const std::string& key);

	/// Clear a section in this dictionary, deleting all its properties
	void clear(const std::string& section);

	//
	// The folling functions have to be public (design flaw) but should not be used directly.
	//

	/// Check if section/key pair exists in this dictionary. If so, return the entry. (used internally)
	GetSetInternal::GetSetDataInterface* getDatainterface(const std::string& section, const std::string& key) const;

	/// Direct access to sections of the dictionary. (used internally)
	Dictionary& get();

	/// This function is called when a variable in the dictionary changes. Calls Observer's notifyChange(...)
	void signalChange(const std::string& section,const std::string& key);
	/// This function is called when a variable in the dictionary was created (or changed type). Calls Observer's notifyCreate(...)
	void signalCreate(const std::string& section,const std::string& key);
	/// This function is called right before a variable is removed from the dictionary. Key may be empty string if the section is removed completely
	void signalDestroy(const std::string& section,const std::string& key="");

	/// Observers for the signals "create" and "change"
	class Observer {
	protected:
		GetSetDictionary& dict;
		Observer(GetSetDictionary& subject);
		~Observer();
		virtual void notifyChange(const std::string& section,const std::string& key)=0x0;
		virtual void notifyCreate(const std::string& section,const std::string& key) {};
		virtual void notifyDestroy(const std::string& section,const std::string& key) {};
		friend class GetSetDictionary;
	};

protected:
	friend class GetSetDictionary::Observer;
	std::set<Observer*> registered_observers;

	Dictionary properties;

	void loadSection(tinyxml2::XMLElement* node, const std::string& section="");
};

/// A class which calls simple c++ functions to handle change signals from GetSet (eg. GUI input)
class GetSetHandler : public GetSetDictionary::Observer
{
public:
	GetSetHandler(void (*change)(const std::string&,const std::string&));
	GetSetHandler(GetSetDictionary& subject, void (*change)(const std::string&,const std::string&));
protected:
	void (*change_handler)(const std::string&,const std::string&);
	virtual void notifyChange(const std::string& s,const std::string& k);
};

#endif // __GetSetDictionary_h
