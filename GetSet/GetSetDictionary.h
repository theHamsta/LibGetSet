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

#ifndef __GetSetInternal_h
#include "GetSetInternal.h"
#endif//__GetSetInternal_h
#ifndef __GetSetDictionary_h
#define __GetSetDictionary_h

/// The root of a propetry tree. This class privately holds all properties. Access only via GetSet&lt;BasicType&gt;.
class GetSetDictionary : public GetSetInternal::GetSetSection
{
public:
	/// Allow instantiation of GetSetDictionaries (not copyable)
	GetSetDictionary();
	virtual ~GetSetDictionary();
	
	/// Save to file
	void save(GetSetInternal::GetSetInOut& file) const;

	/// Load from file
	void load(GetSetInternal::GetSetInOut& file);

	/// Returns true if this dictionary contains nothing
	bool empty();

	/// Delete all properties
	void clear();

	/// Remove a property from the tree
	void remove(const std::string& path);

	/// Tests if a property exists under path
	bool exists(const std::string path);

	/// Tests if a property is a section or any other Node not associated with a parameter (e.g. Button or StaticText)
	bool isValue(const std::string path);

	/// Access to the global GetSetDictionary, which is used whenever no Dictionary is explicitly specified.
	static GetSetDictionary& global();

	// Signal/Observer implementation Observers are notified for the signals "create" "change" and "destroy"
	class Observer : public GetSetInternal::Access {
	public:
		Observer(GetSetDictionary& d);
		~Observer();
        virtual void notifyChange(const std::string& section, const std::string& key)=0;
		virtual void notifyCreate(const std::string& section, const std::string& key) {};
		virtual void notifyDestroy(const std::string& section, const std::string& key) {};
		virtual void notifyUpdateAttrib(const std::string& section, const std::string& key) {};
	};
	friend class GetSetInternal::Access;

protected:
	/// The instance that holds the global() dictionary. Used whenever a GetSetDictionary is not explicitly specified.
	static GetSetDictionary *_instance;

	/// This is where the observers reside
	std::set<Observer*> registered_observers;

	/// Not copy-able
	GetSetDictionary(const GetSetDictionary&);

};


/// A class which calls a c++ function to handle change signals from GetSet (eg. GUI input)
class GetSetHandler : public GetSetDictionary::Observer
{
public:
	GetSetHandler(void (*change)(const std::string& section, const std::string& key), GetSetDictionary& subject=GetSetDictionary::global());
	void ignoreNotifications(bool ignore);
protected:
	bool ignore_notify;
	void (*change_handler)(const std::string&,const std::string&);
	virtual void notifyChange(const std::string& section, const std::string& key);
};

#endif // __GetSetDictionary_h
