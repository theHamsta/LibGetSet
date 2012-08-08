#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>

std::string g_iniFileName;
std::string g_xmlFileName;

// We have a separate GetSetDictionaries for controlling the program itself
GetSetDictionary control;

// And here we define what's in it.
void defineControlProperties()
{
	GetSetGui::Directory(control,"Input","Directory");
	GetSetGui::File(control,"Input","file.xml").setExtensions("xml-Files (*.xml);;All Files (*.*)");
	GetSetGui::File(control,"Input","file.ini").setExtensions("ini-Files (*.ini);;All Files (*.*)");
}

// A window for the properties that are being edited
GetSetSettingsWindow *setup=0x0;
void createSetupWidnow()
{
	if (setup)
	{
		setup->close();
		delete setup;
	}
	setup=new GetSetSettingsWindow();
	setup->setWindowTitle(GetSet<std::string>(control,"Input","file.ini").getString().c_str());
	setup->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	setup->show();
}

// Handles changes in the control window
void control_callback(const std::string& section, const std::string& key)
{
	GetSetDictionary& dict=GetSetDictionary::globalDictionary();

	if (key=="Section")
	{
		std::string s=GetSet<std::string>(control,section,key).getString();
		if (dict.get().find(s)==dict.get().end())
			std::cout << "Warning: The section \"" << s << "\" does not exist (yet).\n";
	}

	if (key=="Key")
	{
		std::string s=GetSet<std::string>(control,section,"Section");
		std::string k=GetSet<std::string>(control,section,key).getString();
		if (dict.get()[s].find(k)==dict.get()[s].end())
			std::cout << "Warning: The key \"" << k << "\" does not exist in section \"" << s << "\" (yet).\n";
	}

	if (key=="Erase Property")
	{
		dict.erase(
			GetSet<std::string>(control,"Erase","Section"),
			GetSet<std::string>(control,"Erase","Key")
			);
		createSetupWidnow();
	}

	if (key=="Declare Property")
	{
		std::string section=GetSet<std::string>(control,"Declare","Section");
		std::string key=GetSet<std::string>(control,"Declare","Key");
		std::string type=GetSetGui::Enum(control,"Declare","Type");
		if (!dict.declare(section,key,type))
			std::cerr << "Failed to Create property of type " << type << std::endl;
		createSetupWidnow();
	}

	if (section=="Declare Tags" || key=="Declare Property")
	{
		std::string section=GetSet<std::string>(control,"Declare","Section");
		std::string key=GetSet<std::string>(control,"Declare","Key");
		GetSetInternal::GetSetDataInterface *p=dict.getDatainterface(section,key);
		if (!p) return;
		std::string type=p->getType();
		if (type=="Slider")
		{
			p->attributes["Min"]=GetSet<double>(control,"Declare Tags","Slider Min").getString();
			p->attributes["Max"]=GetSet<double>(control,"Declare Tags","Slider Max").getString();
		}
		if (type=="File")
			p->attributes["Extensions"]=GetSet<std::string>(control,"Declare Tags","File Extensions");
		if (type=="Enum")
			p->attributes["Choices"]=GetSet<std::string>(control,"Declare Tags","Enum Choices");
	}

	if (key=="Refresh Window")
		createSetupWidnow();
	
	if (key=="Save XML")
		fileWriteString(GetSet<std::string>(control,"Input","Directory").getString()+"/"+GetSet<std::string>(control,"Input","file.xml").getString(),dict.getXML());
	if (key=="Save .ini")
		fileWriteString(GetSet<std::string>(control,"Input","Directory").getString()+"/"+GetSet<std::string>(control,"Input","file.ini").getString(),dict.getIni());

}

int main(int argc, char **argv)
{
	if (argc!=2)
	{
		std::cerr << "Usage:\n\t./GetSetConfig file.xml\n-or-\n./GetSetConfig file.ini\n";
		return 1;
	}

	// Input file
	std::string path=argv[1];

	// Decompose path
	std::string directory=path;
	std::string basename;
	std::string extension;
	basename=splitNameFromPath(directory);
	extension=splitRight(basename,".");

	// Define GetSetConfig's properties in the control window
	defineControlProperties();

	// Control->Input
	GetSet<std::string>(control,"Input","Directory")=directory;
	GetSet<std::string>(control,"Input","file.ini")=basename+".ini";
	GetSet<std::string>(control,"Input","file.xml")=basename+".xml";

	// Control->Declare
	GetSet<std::string>(control,"Declare","Section")="Section";
	GetSet<std::string>(control,"Declare","Key")="Key";
	GetSetGui::Enum(control,"Declare","Type").setChoices("bool;int;double;std::string;Slider;Enum;File;Directory;StaticText;ReadOnlyText");
	GetSet<std::string>(control,"Declare","Type")="std::string";
	GetSetGui::Trigger(control,"Declare","Declare Property")="Declare";

	// Control->Declare Special Tags
	GetSet<double>(control,"Declare Tags","Slider Min")=0;
	GetSet<double>(control,"Declare Tags","Slider Max")=1;
	GetSet<std::string>(control,"Declare Tags","File Extensions")="Text Files (*.txt);;All Files (*.*)";
	GetSet<std::string>(control,"Declare Tags","Enum Choices")="A;B";
	GetSetGui::Trigger(control,"Declare Tags","Refresh Window")="Refresh";

	// Control->Erase
	GetSet<std::string>(control,"Erase","Section")="Section";
	GetSet<std::string>(control,"Erase","Key")="Key";
	GetSetGui::Trigger(control,"Erase","Erase Property")="Erase";

	// Control->Output
	GetSetGui::Trigger(control,"Output","Save XML")="Save Types";
	GetSetGui::Trigger(control,"Output","Save .ini")="Save Values";

	g_iniFileName=directory+"/"+basename+".ini";
	g_xmlFileName=directory+"/"+basename+".xml";

	std::cout << g_xmlFileName << std::endl;
	std::cout << g_iniFileName << std::endl;

	GetSetDictionary::globalDictionary().parseIni(fileReadString(g_iniFileName));
	GetSetDictionary::globalDictionary().parseXML(fileReadString(g_xmlFileName));

	// Handle GUI input in control window
	GetSetHandler gui(control,control_callback);

	QApplication app(argc,argv);

	createSetupWidnow();

	GetSetSettingsWindow controlWindow(control);
	controlWindow.setWindowTitle("GetSetConfig");
	controlWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	controlWindow.show();
	
	return app.exec();
}