#include "Factory.h"

#include <iostream>

namespace Factory {
	std::map<std::string,Factory::Object* (*)()> _make_new;

	Factory::Object* CallCreateMethod(const std::string& type)
	{
		if (_make_new.find(type)!=_make_new.end())
			return _make_new[type]();
		else
			return 0x0;
	}

	void Declare(const std::string& type, Factory::Object* (*create)())
	{
		std::cout << "Declared " << type << std::endl;
		_make_new[type]=create;
	}

} // namespace Factory
