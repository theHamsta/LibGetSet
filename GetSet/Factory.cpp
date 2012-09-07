#include "GetSetFactory.h"

#include <iostream>
#include <set>

#include "StringUtil.hxx"

namespace Factory {
	std::map<std::string, Factory::Object* (*)()> _make_new;
	std::map<std::string, std::set<std::string> > _implementations; 

	Factory::Object* CallCreateMethod(const std::string& type)
	{
		if (_make_new.find(type)!=_make_new.end())
			return _make_new[type]();
		else
			return 0x0;
	}

	void Declare(const std::string& type, Factory::Object* (*create)(), const std::string& interfaces)
	{
		std::cout << "Declared " << type << " implementing " << interfaces << std::endl;
		std::vector<std::string> ivec=stringToVector<std::string>(interfaces,';');
		_implementations[""].insert(type);
		for (std::vector<std::string>::iterator it=ivec.begin();it!=ivec.end();++it)
		{
			trim(*it); // make sure there is no white space around the type
			_implementations[*it].insert(type);
		}
		_make_new[type]=create;
	}

	const std::set<std::string>& KnownTypes(const std::string& interface)
	{
		return _implementations[interface];
	}

} // namespace Factory
