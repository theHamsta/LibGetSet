#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetIO.h>

// only needed for Qt GUI:
#include <GetSetGui/GetSetGui.h>

#include <iostream>

std::string g_ini_file="ExampleAdvanced.ini";

/// Predeclaration of a callback function to handle events
void gui(const std::string& section, const std::string& key);

/// A typical main function using GetSet
int main(int argc, char** argv)
{
	// Define some parameters with arbitrary types
	GetSet<int>("Setup","Number Of Iterations")=123;
	GetSet<double>("Setup","The Value of X")=123.456;
	GetSet<double>("Setup","A Value between 0 and 1")=0.5;
	GetSet<bool>("Setup","Check Me")=false;
	GetSet<std::string>("Setup","Some Text")="Hello World";

	// Declare Advanced types (for better GUI representation). Here: multiple files
	GetSet<std::string>("More","Input Files")="foo.bar;baz.qux";
	GetSetGui::File("More/Input Files").setExtensions("Bar Files (*.bar);;All Files (*)").setMultiple(true);
	// This is how you get access to files:
	std::vector<std::string> files=GetSet<std::vector<std::string> >("More","Input Files");

	// A slider
	GetSet<double>("More","Another Value between 0 and 1")=0.5;
	GetSetGui::Slider("More","Another Value between 0 and 1").setMin(0).setMax(1);

	// A button with the caption "Print Some Text"
	GetSetGui::Button("More","Do Something")="Start";

	// A selector with three options: "Choice 1", "Choice 2" and "And so on", whith current value choice 2
	GetSetGui::Enum("More/Choose!").setChoices("Choice 1;Choice 2;And so on").setValue(1);

	// And you can have subsections, here with some more GetSetGui elements
	GetSetGui::Directory("More/Advanced/Output Directory")="./out";
	GetSetGui::ReadOnlyText("More/Advanced/Some Result")="Some Value";
	GetSetGui::StaticText("More/Advanced/Some Info")=	"I  have no idea what \"Some Result\" is.\n"
														"But I'm sure it's there for your convenience.";

	GetSetIO::load<GetSetIO::IniFile>(g_ini_file);

	// Tell GetSet which function to call when something changes
	GetSetHandler call_back(gui);

	// If we have qt:
	return GetSetGui::runQtApp("Settings",argc,argv);
}

/// Handle all kinds of input
void gui(const std::string& section, const std::string& key)
{
	std::cout << "Key \"" << key << "\" in section \"" << section << "\" has chaged to " << GetSet<>(section,key).getString() << std::endl;

	// Make sure your value really is between 0 and 1
	if (section=="Setup"&&key=="A Value between 0 and 1")
	{
		// But be careful with recursion!
		if (GetSet<double>(section,key)<0.0) GetSet<double>(section,key)=0.0;
		if (GetSet<double>(section,key)>1.0) GetSet<double>(section,key)=1.0;
	}

	// This is how to handle a button. Note you can change it's caption (eg. "Start" to "Stop" and such)
	if (key=="Do Something")
	{
		if (GetSet<>("Setup","Some Text").getString()=="Bla")
			GetSet<>("Setup","Some Text")="Blubb";
		else
			GetSet<>("Setup","Some Text")="Bla";
	}

	GetSetIO::save<GetSetIO::IniFile>(g_ini_file);
}
