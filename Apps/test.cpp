#include <iostream>
#include <functional>

#include <GetSet/GetSet.hxx>
#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetTabWidget.h>
#include <GetSetObjects/ObjectFactory.h>

#include <GetSet/GetSetLog.hxx>

template <class Type>
class NamedObject
{
	const std::string name;
public:
	NamedObject(const std::string& _name)  : name(_name) {}

	const std::string getName();
	static std::map<std::string, Type*> instances;
	
	operator Type& () {return *this;}
	static Type& getInstance() { return NamedObject<Type>(name); }
};


/// Application
GetSetGui::GetSetApplication g_app("Test");

void gui(const std::string& section, const std::string& key)
{
	std::cout << key << std::endl;
	if (key=="Can Change Color")
	{
		GetSetGui::Section("Bla/Color").setDisabled(!GetSet<bool>("Bla/Can Change Color"));
	}
	g_app.saveSettings();
}

int main(int argc, char **argv)
{
	GetSetGui::RangedInt("Bla/Test Int").setMin(-7).setMax(17).setPeriodic(true).setDescription("An int from -7 to 17");
	GetSetGui::RangedDouble("Bla/Test").setPeriodic(true);
	GetSetGui::Slider("Bla/Test 3");
	GetSet<>("Bla/Test 2")="Blubb";

	GetSet<int>("Bla/Test 123");

	GetSet<double>("Bla/Color/Red");
	GetSet<double>("Bla/Color/Green");
	GetSet<double>("Bla/Color/Blue");
	GetSetGui::Section("Bla/Color").setGrouped(true);

	GetSet<bool>("Bla/Can Change Color");
	gui("Bla","Can Change Color");

	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();
	return g_app.exec();
}
