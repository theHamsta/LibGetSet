#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetCmdLineParser.h"

#include <iostream>


void defineSomeArgs();

// This is a simple text/example app, which receives properties via command line and stores them in an ini-File
// You can run this program from within GetSetCLi to get an automatically generated GUI
int main(int argc, char** argv)
{
	
	// Print command line arguments
	// argc==2 we don't want to generate any output, because it could be that the --xml option was provided
	if (argc!=2)
		for (int i=0;i<argc;i++)
			std::cout << argv[i] << std::endl;

	defineSomeArgs();

	// Parse the command line arguments (this is the important line)
	bool success=GetSetCmdLineParser()
		.defineIndex<std::string>(0,"Blubb Config","Input File",1) // we add a required indexed argument for show (this would be typical usage)
		.parse(argc, argv);

	// Say hello to the world (This does not happen if the --xml option was provided)
	std::cout << "Started!\n";

	// Then if the command line arguments were all parsed correctly, we save their values to an ini-File
	if (success)
		fileWriteString("CLITestApp.ini",GetSetDictionary::globalDictionary().getIni());
	else
		std::cerr << "Some arguments could not be matched or some compulsory arguments were missing!\n";

	return 0
;}


// Just some properties for demo purposes
void defineSomeArgs()
{



	// Define some differently typed properties
	GetSet<int>("Bla Settings","Negative Value")=-456;
	GetSet<bool>("Bla Settings","Boolean Value")=false;
	GetSet<unsigned>("Bla Settings","Positive Value")=123;
	GetSet<double>("Bla Settings","Exact Value")=123.456;
	GetSet<std::string>("Bla Settings","Some String")="Hello World";

	// And some more properties with special GUI types
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
}