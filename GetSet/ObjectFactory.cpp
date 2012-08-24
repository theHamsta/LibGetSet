#include "ObjectFactory.h"
#include "Configurator.hxx"

#include <map>

namespace ObjectFactory
{
	typedef std::map<std::string, Product* (*)(const Configurator& config)>	CreateMethodByString;
	typedef std::map<std::string, std::string>						InterfaceByImplementations;

	CreateMethodByString& createMethodByStr() {
		static CreateMethodByString knownTypes;
		return knownTypes;
	}

	InterfaceByImplementations& interfaceByImplementations() {
		static InterfaceByImplementations knownInterfaces;
		return knownInterfaces;
	}

	Factory::Registration::Registration(const std::string& interfaceName, const std::string& className, Product* (*create)(const Configurator& config)) {
		// 2do if (createMethodByStr().find(className)!=createMethodByStr().end()) sodomAndGomorra();
		createMethodByStr()[className]=create;
		interfaceByImplementations()[className]=interfaceName;
	}

	Product* Factory::Create(const std::string& classname,  const std::string& instanceName) {
		if (createMethodByStr().find(classname)!=createMethodByStr().end())
			return createMethodByStr()[classname](m_config(instanceName));
		else
			return 0x0;
	}

	std::vector<std::string> Factory::KnownTypes(const std::string& interfaceClass)
	{
		std::vector<std::string> ret;
		for (CreateMethodByString::iterator it=createMethodByStr().begin();it!=createMethodByStr().end();++it)
			if (interfaceClass.empty()||interfaceClass==interfaceByImplementations()[it->first])
				ret.push_back(it->first);
		return ret;
	}

} // ObjectFactory
