#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetXML.h>

#include <iostream>

std::string g_config_file;


void doSomething(const std::string& path)
{
	// this could be your algorithm
}


int main(int argc, char **argv)
{
	GetSetGui::File("File/Input").setExtensions("Text File (*.txt)");
	// more parameters here
	
	
	// User has to supply exactly one sommand ilne argument: either "--xml" or path to an ini-File
	if (argc!=2)
	{
		std::cerr << "Usage:\n   testXML --xml\n   testXML file.ini\n";
		return 1;
	}

	// In case of "--xml", we print standard configuratino to stdout
	if (std::string(argv[1])=="--xml")
	{
		GetSetIO::XmlFile xml(std::cin,std::cout);
		GetSetDictionary::global().save(xml);
		return 0;
	}
	// Anything else we get must be the path to an ini-File
	else g_config_file=argv[1];

	// This is how you load an ini-File
	GetSetIO::load<GetSetIO::IniFile>(g_config_file);

	std::string myInputFile=GetSet<>("Options/File/Input");


	doSomething(myInputFile);


	return 0;
}
