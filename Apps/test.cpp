#include <iostream>

#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetIO.h"
#include "GetSet/GetSetCmdLine.hxx"

#include "GetSetGui/GetSetTabWidget.h"

#include <QApplication>

void gui(const std::string& section, const std::string& key)
{
	GetSetIO::save<GetSetIO::IniFile>("config.ini");
}

int main(int argc, char **argv)
{
	QApplication app(argc,argv);

	std::cout << "Command Line Args:\n";
	for (int i=0;i<argc;i++)
		std::cout << i << ":\t" << argv[i] << std::endl;
	std::cout << std::endl;
	//GetSetGui::File("Run/Binary File").setExtensions("Executable File (*.exe)").setDescription("This is a tooltip");

	GetSet<>("Algorithm/Number of Iterations");
	GetSet<>("Algorithm/Epsilon");
	GetSet<>("File/Output");
	GetSet<>("File/Input");

//	GetSetIO::load<GetSetIO::IniFile>("config.ini");
	
	GetSetIO::CmdLineParser cmdl;
	cmdl.index("File/Input",1);
	cmdl.index("File/Output",2);
	cmdl.declare("Algorithm/Number of Iterations");
	std::cout << cmdl.getSynopsis() << std::endl;

	if (!cmdl.parse(argc,argv))
		std::cout << "Unrecocgnizd Commad Line Args!\n";

	GetSetIO::TxtFileKeyValue out(std::cin,std::cout);
	GetSetDictionary::global().save(out);

	GetSetHandler callback(gui);

	return 0;
//	return app.exec();

}
