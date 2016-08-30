#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetIO.h>

// only needed for Qt GUI:
#include <GetSetGui/GetSetGui.h>

#include <iostream>

#include <GetSet/GetSetScripting.hxx>

std::string g_ini_file="Scripting.ini";

GetSetScriptParser g_parser;

/// Handle all kinds of input
void gui(const std::string& section, const std::string& key)
{
	if (key=="Parse Line")
	{
		std::string command=GetSet<>("Console/Parse Line");
		if (!command.empty())
		{
			std::cout << command << std::endl;
			g_parser.parse(command);
		}
	}
	else
	{
		std::string path=section+"/"+key;
		std::cout << "info: " << path << " = " << GetSet<>(path).getString() << std::endl;
		GetSetIO::save<GetSetIO::IniFile>(g_ini_file);
	}
}

/// A typical main function using GetSet
int main(int argc, char** argv)
{

	GetSetIO::load<GetSetIO::IniFile>(g_ini_file);

	GetSet<>("Console/Parse Line")="";

	// Tell GetSet which function to call when something changes
	GetSetHandler call_back(gui);

	// If we have qt:
	return GetSetGui::runQtApp("Settings",argc,argv);
}
