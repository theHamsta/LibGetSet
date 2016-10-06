#include "ObjectFactory.h"
#include "Configurator.hxx"

#include <map>

namespace GetSetObjects
{
	typedef std::map<std::string, Object* (*)(Configurator& config)> CreateMethodByString;
	typedef std::map<std::string, std::string>                       InterfaceByImplementations;

	CreateMethodByString& createMethodByStr() {
		static CreateMethodByString knownTypes;
		return knownTypes;
	}

	InterfaceByImplementations& interfaceByImplementations() {
		static InterfaceByImplementations knownInterfaces;
		return knownInterfaces;
	}

	std::set<std::string> KnownInterafces()
	{
		std::set<std::string> ret;
		for (InterfaceByImplementations::iterator it=interfaceByImplementations().begin();it!=interfaceByImplementations().end();++it)
			ret.insert(it->second);
		return ret;
	}

	std::set<std::string> KnownTypes(const std::string& interfaceClass)
	{
		std::set<std::string> ret;
		for (CreateMethodByString::iterator it=createMethodByStr().begin();it!=createMethodByStr().end();++it)
			if (interfaceClass.empty()||interfaceClass==interfaceByImplementations()[it->first])
				ret.insert(it->first);
		return ret;
	}

	Factory::Registration::Registration(const std::string& interfaceName, const std::string& className, Object* (*create)(Configurator& config)) {
		// 2do if (createMethodByStr().find(className)!=createMethodByStr().end()) sodomAndGomorra();
		createMethodByStr()[className]=create;
		interfaceByImplementations()[className]=interfaceName;
	}

	Object* Factory::Create(const std::string& classname,  const std::string& instanceName) {
		if (createMethodByStr().find(classname)!=createMethodByStr().end())
			return createMethodByStr()[classname](m_config(instanceName));
		else
			return 0x0;
	}


} // ObjectFactory
