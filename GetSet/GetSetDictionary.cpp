#include "GetSetDictionary.h"

#include "GetSet.hxx"

//
// GetSetDictionary
//

GetSetDictionary* GetSetDictionary::_instance=0x0;

GetSetDictionary::GetSetDictionary()
	// "this" is the root of a tree and all nodes hold a reference to the root, including the root itself.
	// This is correct as long as Section(...) only stores the reference for later use.
	: Section("",*this)
{}

GetSetDictionary::~GetSetDictionary() {}
	
void GetSetDictionary::save(GetSetInternal::InputOutput& file) const
{
	Section::store(file);
	file.write();
}

void GetSetDictionary::load(GetSetInternal::InputOutput& file)
{
	file.read();
	file.retreiveAll(*this);
}

GetSetDictionary& GetSetDictionary::global()
{
	if (!_instance) _instance=new GetSetDictionary;
	return *_instance;
}

bool GetSetDictionary::empty()
{
	return properties.empty();
}

void GetSetDictionary::clear()
{
	while (!properties.empty())
		remove(properties.begin()->first);
}

void GetSetDictionary::remove(const std::string& path)
{
	if (path.empty())
		clear();
	else
	dictionary.setProperty(stringToVector<std::string>(path,'/'),0x0,0);
}

bool GetSetDictionary::exists(const std::string path)
{
	return GetSetInternal::Access::getProperty(path)!=0x0;
}

/// Tests if a property is a section or any other Node not associated with a parameter (e.g. Button or StaticText)
bool GetSetDictionary::isValue(const std::string path)
{
	auto p=GetSetInternal::Access::getProperty(path);
	if (dynamic_cast<GetSetInternal::Section*>(p)!=0x0) return false;
	if (dynamic_cast<GetSetInternal::GetSetKeyButton*>(p)!=0x0) return false;
	if (dynamic_cast<GetSetInternal::GetSetKeyStaticText*>(p)!=0x0) return false;
	return true;
}


//
// GetSetDictionary::Observer
//

GetSetDictionary::Observer::Observer(GetSetDictionary& d)
	: GetSetInternal::Access(d)
{
	d.registered_observers.insert(this);
}

GetSetDictionary::Observer::~Observer()
{
	dictionary.registered_observers.erase(this);
}

//
// GetSetHandler
//

GetSetHandler::GetSetHandler(void (*change)(const std::string& section, const std::string& key),GetSetDictionary& subject)
	: GetSetDictionary::Observer(subject)
	, change_handler(change)
	, ignore_notify(false)
{}

void GetSetHandler::ignoreNotifications(bool ignore)
{
	ignore_notify=ignore;
}

void GetSetHandler::notifyChange(const std::string& section, const std::string& key)
{
	if (!ignore_notify) change_handler(section,key);
}
