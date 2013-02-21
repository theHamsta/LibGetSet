#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetXML.h>

#include <iostream>

std::string g_config_file;

int main(int argc, char **argv)
{
	GetSetGui::File("File/Input/config").setExtensions("Text File (*.txt)");
	GetSetGui::File("File/Input/image").setExtensions("Image File (*.jpg)");

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

	bool bla=GetSet<bool>("Options/Append Hello");


	meinAlgorithmus(bla);


	return 0;
}
