#include "GetSetDictionary.h"

//
// GetSetDictionary
//

GetSetDictionary* GetSetDictionary::_instance=0x0;

GetSetDictionary::GetSetDictionary()
	// "this" is the root of a tree and all nodes hold a reference to the root, including the root itself.
	// This is correct as long as GetSetSection(...) only stores the reference for later use.
	: GetSetSection("",*this)
{}

GetSetDictionary::~GetSetDictionary() {}
	
void GetSetDictionary::save(GetSetInternal::GetSetInOut& file) const
{
	GetSetSection::store(file);
	file.write();
}

void GetSetDictionary::load(GetSetInternal::GetSetInOut& file)
{
	file.read();
	file.retreiveAll(*this);
}

GetSetDictionary& GetSetDictionary::global()
{
	if (!_instance) _instance=new GetSetDictionary;
	return *_instance;
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

GetSetHandler::GetSetHandler(void (*change)(const std::string& section, const std::string& key))
	: GetSetDictionary::Observer(GetSetDictionary::global())
	, change_handler(change)
{}

GetSetHandler::GetSetHandler(GetSetDictionary& subject, void (*change)(const std::string& section, const std::string& key))
	: GetSetDictionary::Observer(subject)
	, change_handler(change)
{}

void GetSetHandler::setIgnoreNotifications(bool ignore)
{
	ignore_notify=ignore;
}

void GetSetHandler::notifyChange(const std::string& section, const std::string& key)
{
	if (!ignore_notify) change_handler(section,key);
}
