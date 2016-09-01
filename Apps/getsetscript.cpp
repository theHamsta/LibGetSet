#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetIO.h>

// only needed for Qt GUI:
#include <GetSetGui/GetSetGui.h>

#include <iostream>

GetSetGui::GetSetApplication g_app("script");

/// Handle all kinds of input
void gui(const std::string& section, const std::string& key)
{
	if (key=="Run Script")
		GetSet<>("Console/Parse Line")="file run script.getset";
	else if (key=="Parse Line")
	{
		std::string command=GetSet<>("Console/Parse Line");
		if (!command.empty())
		{
			std::cout << command << std::endl;
			g_app.parseScript(command);
		}
	}
	else
	{
		std::string path=section+"/"+key;
		std::cout << "info: " << path << " = " << GetSet<>(path).getString() << std::endl;
		g_app.saveSettings();
	}
}

/// A typical main function using GetSet
int main(int argc, char** argv)
{
	GetSet<>("Console/Parse Line")="";
	GetSetGui::Button("Console/Run Script")="Run";

	g_app.init(argc,argv,gui);
	return g_app.exec();
}
