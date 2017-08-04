#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetIO.h>

// only needed for Qt GUI:
#include <GetSetGui/GetSetGui.h>

#include <iostream>

#include <GetSetGui/GetSetTabWidget.h>

#include <GetSetGui/GetSetScriptEdit.h>

#include <GetSet/GetSetLog.hxx>

#include <GetSet/GetSetObjects.hxx>

#include <GetSet/StringHexData.hxx>

//  #include <GetSetGui/GetSetMouseKeyboardInteraction.h>

GetSetGui::GetSetApplication g_app("ExampleAdvanced");

/// Handle all kinds of input
void gui(const std::string& section, const std::string& key)
{
	// Window title indicates either a button or a menu item
	if (section=="ExampleAdvanced")
	{
		if (key=="Do Something")
			GetSetGui::Button("More/Do Something").trigger();
		return;
	}
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
		if (GetSet<>("Setup/Some Text").getString()=="Bla")
			GetSet<>("Setup/Some Text")="Blubb";
		else
			GetSet<>("Setup/Some Text")="Bla";
	}

	g_app.saveSettings();
}

/// A typical main function using GetSet
int main(int argc, char** argv)
{

	// Useful for debugging yout applications!
//	GetSetLog::debugSignals();

	// Define some parameters with arbitrary types
	GetSet<int>("Setup/Number Of Iterations")=123;
	GetSet<double>("Setup/The Value of X")=123.456;
	GetSet<double>("Setup/A Value between 0 and 1")=0.5;
	GetSet<bool>("Setup/Check Me")=false;
	GetSet<std::string>("Setup/Some Text")="Hello World";

	// Declare Advanced types (for better GUI representation). Here: multiple files
	GetSet<std::string>("More/Input Files")="foo.bar;baz.qux";
	GetSetGui::File("More/Input Files").setExtensions("Bar Files (*.bar);;All Files (*)").setMultiple(true);
	// This is how you get access to files:
	std::vector<std::string> files=GetSet<std::vector<std::string> >("More/Input Files");

	// A slider
	GetSet<double>("More/Another Value between 0 and 1")=0.5;
	GetSetGui::Slider("More/Another Value between 0 and 1").setMin(0).setMax(1);

	// A button with the caption "Print Some Text"
	GetSetGui::Button("More/Do Something")="Start";

	// A selector with three options: "Choice 1", "Choice 2" and "And so on", whith current value choice 2
	GetSetGui::Enum("More/Choose!").setChoices("Choice 1;Choice 2;And so on").setValue(1);

	// And you can have subsections, here with some more GetSetGui elements
	GetSetGui::Directory("More/Advanced/Output Directory")="./out";
	GetSetGui::ReadOnlyText("More/Advanced/Some Result")="Some Value";
	GetSetGui::StaticText("More/Advanced/Some Info")=	"I  have no idea what \"Some Result\" is.\n"
														"But I'm sure it's there for your convenience.";

	// Declare command line interface
	g_app.commandLine().declare();

	// Show Qt window
	g_app.init(argc,argv,gui);

	g_app.window().addMenuItem("Edit/Functions","Do Something","Ctrl+D");

	return g_app.exec();
}
