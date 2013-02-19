#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetXML.h>

#include <iostream>

std::string g_config_file;

int main(int argc, char **argv)
{
	GetSetGui::File("File/Input").setExtensions("Text File (*.txt)");
	GetSetGui::File("File/Output").setExtensions("Text File (*.txt)");

	GetSet<bool>("Options/Append Hello")=0;
	GetSetGui::Slider("Options/Append Value").setMin(0).setMax(2)=0.5;
	GetSetGui::Enum("Options/Next Line").setChoices("Nothing;Roger and Out;Good Bye!")=2;

	if (argc!=2)
	{
		std::cerr << "Usage:\n   testXML --xml\n   testXML file.ini\n";
		return 1;
	}

	if (std::string(argv[1])=="--xml")
	{
		GetSetIO::XmlFile xml(std::cin,std::cout);
		GetSetDictionary::global().save(xml);
		return 0;
	}
	else g_config_file=argv[1];

	GetSetIO::load<GetSetIO::IniFile>(g_config_file);

	std::ofstream out(GetSet<>("File/Output"));
	std::ifstream in(GetSet<>("File/Input"));

	if (!in.good() || !out.good())
	{
		std::cerr << "File access error!\n";
		return 2;
	}

	if (GetSet<bool>("Options/Append Hello"))
		out << "Hello!\n";

	std::string all;
	getline(in,all,'\n');
	out << all;
	out << "\n" << GetSet<double>("Options/Append Value").getValue() << "\n";

	if (GetSetGui::Enum("Options/Next Line").getValue()!=0)
		out << GetSetGui::Enum("Options/Next Line").getString() << std::endl;

	return 0;
}
