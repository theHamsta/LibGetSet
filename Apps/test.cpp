#include <iostream>
#include <functional>

#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetTabWidget.h>
#include <GetSetObjects/ObjectFactory.h>

/// Name
class Name : public GetSetObjects::Object {
	GETSET_DECLARE_CLASS(Name,Object)
public:
	std::string first;
	std::string last;

	void stateName()
	{
		std::cout << "My name is" << first << " " << last << std::endl;
	}

};

void select_member_function(const std::string& function, void *instance_ptr)
{
	std::cout << "callback: " << function << "\n";
	if (function == "State Name")
		((Name*)instance_ptr)->stateName();
}

Name::Name(GetSetObjects::Configurator& config)
{
	first=config.declare<std::string>("First Name","Hans");
	last =config.declare<std::string>("Last Name","Mustermann");
	config.declareFunction("State Name", this, select_member_function);
}

GETSET_REGISTER_CLASS(Name);

/// Person
class Person : public GetSetObjects::Object {
	GETSET_DECLARE_CLASS(Person,Object)
public:
	Name *name;
	std::string occupation;
};

Person::Person(GetSetObjects::Configurator& config)
{
	name=GetSetObjects::CreateAndConfigure<Name>("Name",config);
	occupation=config.declare<std::string>("Occupation","Unemployed");
}

GETSET_REGISTER_CLASS(Person);

/// Child
class Child : public Person {
	GETSET_DECLARE_CLASS(Child,Person)
public:
	Name *father;
	Name *mother;
};

Child::Child(GetSetObjects::Configurator& config)
	: Person(config)
{
	father=GetSetObjects::CreateAndConfigure<Name>("Mother",config);
	mother=GetSetObjects::CreateAndConfigure<Name>("Father",config);
}

GETSET_REGISTER_CLASS(Child);


/// Application
GetSetGui::GetSetApplication g_app("Test");

void gui(const std::string& section, const std::string& key)
{
	g_app.saveSettings();
}

int main(int argc, char **argv)
{
	auto interfaces=GetSetObjects::KnownInterafces();
	for (auto it=interfaces.begin(); it!=interfaces.end(); ++it)
	{
		std::cout << "Interface: " << *it << "\n";
		auto types=GetSetObjects::KnownTypes(*it);
		for (auto it=types.begin(); it!=types.end(); ++it)
			std::cout << "   " << *it << std::endl;
	}

	GetSetObjects::Configurator config;

	Child *self=GetSetObjects::CreateAndConfigure<Child>("Self",config);

	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();
	return g_app.exec();
}

