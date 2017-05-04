#include <iostream>
#include <functional>

#include <GetSet/GetSet.hxx>
#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetTabWidget.h>

/// Application
GetSetGui::GetSetApplication g_app("Test");

// Managing saving/loading parametzers and automatic GUI
#include <GetSetGui/GetSetTabWidget.h>

void gui(const std::string& section, const std::string& key)
{
	std::cout << key << std::endl;

	if (section=="Test/Range")
	{
		std::cout << "Reset range.\n";
		GetSetGui::RangedDouble("Test/Value")
			.setMin(GetSet<int>("Test/Range/Min"))
			.setMax(GetSet<int>("Test/Range/Max"))
			;
	}

//	g_app.saveSettings();
}

int main(int argc, char **argv)
{

	GetSet<int>("Test/Range/Min")=5;
	GetSet<int>("Test/Range/Max")=9;

	GetSetGui::RangedDouble("Test/Value")
		.setMin(GetSet<int>("Test/Range/Min"))
		.setMax(GetSet<int>("Test/Range/Max"))
		;
	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();
	return g_app.exec();
}
