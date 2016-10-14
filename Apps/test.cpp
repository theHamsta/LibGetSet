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
	g_app.saveSettings();
}

int main(int argc, char **argv)
{
	std::cout << "Yoopidoo\n";
	std::cerr << "baaam\n";
	std::cout << "Lalala\n";

	std::cout << "2 Yoopidoo\n";
	std::cerr << "2 baaam\n";
	std::cout << "2 Lalala\n";

	std::cout << "3 Yoopidoo\n";
	std::cerr << "3 baaam\n";
	std::cout << "3 Lalala\n";

	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();
	return g_app.exec();
}
