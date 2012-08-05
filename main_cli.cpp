#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetCmdLineParser.h"

#include <iostream>

int main(int argc, char** argv)
{

	GetSet<int>("Bla Settings","Negative Value")=-456;
	GetSet<unsigned>("Bla Settings","Positive Value")=123;
	GetSet<double>("Bla Settings","Exact Size")=123.456;
	GetSet<std::string>("Bla Settings","Gretting")="Hello World";

	GetSetGui::Directory("Blubb Config","Output Directory")="./out";
	GetSetGui::Enum("Blubb Config","Capital Letter").setChoices("A;B;C;D");
	GetSetGui::Enum("Blubb Config","Capital Letter")="A";
	GetSetGui::File("Blubb Config","Input File")="in.file";
	GetSetGui::ReadOnlyText("Blubb Config","Information")="Help yourself!";
	GetSetGui::Slider("Blubb Config","Slider Value");
	GetSetGui::Trigger("Blubb Config","Do Something");

	bool success=GetSetCmdLineParser()
		.defineIndex<std::string>(0,"Blubb Config","Input File",1)
		.parse(argc, argv);

	if (success)
		saveToFile("CLITestApp.ini",GetSetDictionary::globalDictionary().getIni());
	else
		std::cerr << "Some arguments could not be watched or some compulsory arguments were missing!\n";

	return 0
;}
