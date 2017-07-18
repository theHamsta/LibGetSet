#include <iostream>
#include <functional>

#include <GetSet/GetSet.hxx>
#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetModalDialog.h>
#include <GetSetGui/GetSetTabWidget.h>

#include <GetSet/GetSetIO.h>

/// Application
GetSetGui::GetSetApplication g_app("Test");

void gui(const std::string& section, const std::string& key)
{
	std::cout << key << std::endl;

	if (key=="Input")
	{
		GetSetGui::GetSetModalDialog who_re_you;
		GetSet<int>("Number",who_re_you);

		if (who_re_you.exec())
			GetSet<int>("Input/Bla")=GetSet<int>("Number",who_re_you);

		std::cout << GetSet<int>("Number",who_re_you) << std::endl;
	}

	g_app.saveSettings();
}

int main(int argc, char **argv)
{
	GetSet<double>("Input/Blubb");
	GetSetGui::Button("Input/Input")="Ask User";
	GetSet<int>("Input/Bla");

	GetSetIO::debug_print();

	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();


	return g_app.exec();
}
