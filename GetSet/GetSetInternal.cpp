#include "GetSetInternal.h"
#include "GetSetDictionary.h"

namespace GetSetInternal {

//
// GetSetInternal::Access
//

Access::Access(GetSetDictionary& d) : dictionary(d) {}

GetSetNode* Access::getProperty(const std::string& path)
{
	if (path.empty()) return &dictionary;
	return dictionary.getProperty(stringToVector<std::string>(path,'/'),0);
}

void Access::setProperty(const std::string& path, GetSetNode* p)
{
	return dictionary.setProperty(stringToVector<std::string>(path,'/'),p,0);
}

GetSetSection* Access::getRoot() {return &dictionary;}

void Access::signalChange(const std::string& section, const std::string& key)
{
	typedef std::set<GetSetDictionary::Observer*>::const_iterator ObserverIterator;
	for (ObserverIterator it=dictionary.registered_observers.begin();it!=dictionary.registered_observers.end();++it)
		(*it)->notifyChange(section,key);
}

void Access::signalCreate(const std::string& section, const std::string& key)
{
	typedef std::set<GetSetDictionary::Observer*>::const_iterator ObserverIterator;
	for (ObserverIterator it=dictionary.registered_observers.begin();it!=dictionary.registered_observers.end();++it)
		(*it)->notifyCreate(section,key);
}

void Access::signalDestroy(const std::string& section, const std::string& key)
{
	typedef std::set<GetSetDictionary::Observer*>::const_iterator ObserverIterator;
	for (ObserverIterator it=dictionary.registered_observers.begin();it!=dictionary.registered_observers.end();++it)
		(*it)->notifyDestroy(section,key);
}

//
// GetSetInOut
//

GetSetInOut::GetSetInOut(const std::string& filePath) : file(filePath), stored(0) {}
GetSetInOut::~GetSetInOut() {}

//
// GetSetSection
//

const GetSetSection::PropertyByName& GetSetSection::getSection() const
{
	return properties;
}

GetSetSection::GetSetSection(const std::string& path, GetSetDictionary& dict)
	: Access(dict)
	, absolutePath(path)
{}

void GetSetSection::save(GetSetInOut& file) const
{
	for (PropertyByName::const_iterator it=properties.begin();it!=properties.end();++it)
	{
		GetSetSection* subsection=dynamic_cast<GetSetSection*>(it->second);
		if (subsection) subsection->save(file);
		else file.store(absolutePath,it->first,it->second);
	}
}

GetSetSection::~GetSetSection()
{
	for (PropertyByName::iterator it=properties.begin();it!=properties.end();++it)
		delete it->second;
	properties.clear();
}

std::string GetSetSection::getString() const
{
	if (properties.empty()) return "<null>";
	PropertyByName::const_iterator it=properties.begin();
	std::string ret=it->second->getType();
	for (++it;it!=properties.end();++it)
		ret+=";"+it->second->getType();
	return ret;
}

void GetSetSection::setProperty(const std::vector<std::string>& path, GetSetNode* prop, int i)
{
	// Path up to current level HAS TO BE valid
	std::string key=path[i++];
	// Find the property at the current level of the path
	PropertyByName::iterator it=properties.find(key);
	// The path goes on after this property
	if (i!=path.size())
	{
		GetSetNode* p=0x0;
		if (it!=properties.end())
			p=it->second;
		// If the property was found it HAS TO BE a section
		GetSetSection* s=dynamic_cast<GetSetSection*>(p);
		// The property exists but has a different type => quietly erase it
		if (!s && p)
		{
			signalDestroy(absolutePath,key);
			delete p;
		}
		if (!s)
		{
			// The property does not exist (anymore)
			if (absolutePath.empty())
				s=new GetSetSection(key,dictionary);
			else
				s=new GetSetSection(absolutePath+"/"+key,dictionary);
			properties[key]=s;
			signalCreate(s->absolutePath,"");
		}
		// We follow down the rest of the path in Section s
		return s->setProperty(path,prop,i);
	}
	// We have reached the end of the path, so we set the property and are done
	properties[key]=prop;
	signalCreate(absolutePath,key);
}

GetSetNode* GetSetSection::getProperty(const std::vector<std::string>& path, int i) const
{
	if ((int)path.size()<=i) return 0x0;
	// Find the property at the current level of the path
	std::string key=path[i++];
	PropertyByName::const_iterator it=properties.find(key);
	// Property not found, so the path is invalid
	if (it==properties.end())
		return 0x0;
	// The path goes on after this property
	if (i!=path.size())
	{
		// So this property has to be a section itself
		GetSetSection* s=dynamic_cast<GetSetSection*>(it->second);
		// if it isn't, the path is invalid
		if (!s) return 0x0;
		// else follow down the rest of the path in Section s
		else return s->getProperty(path,i);
	}
	// If we are at the end of the path, we can return the property that we just found
	return it->second;
}

}// namespace GetSetInternal
