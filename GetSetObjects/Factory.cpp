#include "Factory.h"

#include <iostream>
#include <set>

#include "StringUtil.hxx"

namespace Factory {

	typedef std::map<std::string, Factory::Object* (*)()> CreateByString;
	CreateByString& MakeNew()
	{
		static CreateByString _make_new;
		return _make_new;
	}

	typedef std::map<std::string, std::set<std::string> > ImplementationsByString;
	ImplementationsByString& Implementations()
	{
		static ImplementationsByString _implementations; 
		return _implementations;
	}

	Factory::Object* CallCreateMethod(const std::string& type)
	{
		if (MakeNew().find(type)!=MakeNew().end())
			return MakeNew()[type]();
		else
			return 0x0;
	}

	void Declare(const std::string& type, Factory::Object* (*create)(), const std::string& interfaces)
	{
		std::cout << "Declared " << type << " implementing " << interfaces << std::endl;
		std::vector<std::string> ivec=stringToVector<std::string>(interfaces,';');
		Implementations()[""].insert(type);
		Implementations()[type].insert(type);
		for (std::vector<std::string>::iterator it=ivec.begin();it!=ivec.end();++it)
		{
			trim(*it); // make sure there is no white space around the type
			Implementations()[*it].insert(type);
		}
		MakeNew()[type]=create;
	}

	const std::set<std::string>& KnownTypes(const std::string& interface)
	{
		if (interface.empty()) return Implementations()[""];
		else return Implementations()[interface];
	}

} // namespace Factory
