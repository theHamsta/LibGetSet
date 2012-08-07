#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetCmdLineParser.h"

#include <iostream>

int main(int argc, char** argv)
{
	if (argc!=2)
		for (int i=0;i<argc;i++)
			std::cout << argv[i] << std::endl;

	GetSet<int>("Bla Settings","Negative Value")=-456;
	GetSet<unsigned>("Bla Settings","Positive Value")=123;
	GetSet<double>("Bla Settings","Exact Size")=123.456;
	GetSet<std::string>("Bla Settings","Gretting")="Hello World";

	GetSetGui::Directory("Blubb Config","Output Directory")="./out";
	GetSetGui::Enum("Blubb Config","Capital Letter").setChoices("A;B;C;D");
	GetSetGui::Enum("Blubb Config","Capital Letter")="A";
	GetSetGui::File("Blubb Config","Input File")="in.file";
	GetSetGui::ReadOnlyText("Blubb Config","Information")="Help yourself!";
	GetSetGui::Slider slider("Blubb Config","Slider Value");
	slider.setMin(-1.414);
	slider.setMax(3.1415);
	slider=0;
	GetSetGui::Trigger("Blubb Config","Do Something")="Somthing";

	bool success=GetSetCmdLineParser()
		.defineIndex<std::string>(0,"Blubb Config","Input File",1)
		.parse(argc, argv);

	std::cout << "Started!\n";

	if (success)
		fileWriteString("CLITestApp.ini",GetSetDictionary::globalDictionary().getIni());
	else
		std::cerr << "Some arguments could not be watched or some compulsory arguments were missing!\n";

	return 0
;}
