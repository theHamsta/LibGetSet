#include <iostream>

#include <GetSetGui/GetSetGui.h>

#include <GetSetGui/GetSetTabWidget.h>

GetSetGui::Application g_app("Test");


struct Address : public GetSetGui::Configurable {
	
	std::string street       ="Mittelweg";
	int         house_number =51;
	std::string city         ="Pommelsbrunn";
	int         zip          =91224;

	virtual void gui_declare_section (const GetSetGui::Section &section) {
		GetSet<>   ("Street"      ,section)=street       ;
		GetSet<int>("House_number",section)=house_number ;
		GetSet<>   ("City"        ,section)=city         ;
		GetSet<int>("Zip"         ,section)=zip          ;
	}

	virtual void gui_retreive_section(const GetSetGui::Section &section) {
		street       =GetSet<>   ("Street"      ,section);
		house_number =GetSet<int>("House_number",section);
		city         =GetSet<>   ("City"        ,section);
		zip          =GetSet<int>("Zip"         ,section);
	}

};

GETSET_OBJECT_STRUCT_DECLARE (Address)
GETSET_OBJECT_STRUCT_REGISTER(Address)

void gui(const GetSetInternal::Node& node)
{

	if (node.name=="Bla...")
	{
		Address().gui_declare_section(GetSetGui::Section("Addresses/Some/Home"));
		Address().gui_declare_section(GetSetGui::Section("Addresses/Some/Work"));
		Address().gui_declare_section(GetSetGui::Section("Addresses/Some/Vacation"));
	}
	g_app.saveSettings();
}

int main(int argc, char **argv)
{
	GetSetInternal::Dictionary::global().clear();

	GetSetGui::Button("Addresses/Some/Bla")="Blubb";
	GetSetGui::Section("Addresses/Some").setGrouped();

	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();
	g_app.window().addMenuItem("Edit","Bla...");
	return g_app.exec();
}


