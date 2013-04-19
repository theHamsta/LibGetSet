#include <iostream>

#include <QtGui/QApplication>

#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetCmdLine.hxx>
#include <GetSet/GetSetIO.h>
#include <GetSet/GetSetXML.h>

#include <GetSetGui/GetSetSettingsWindow.h>

std::string g_file_xml;
std::string g_file_ini;

GetSetDictionary g_config;

void die(const std::string& section, const std::string& key)
{
	exit(0);
}

void save(const std::string& section, const std::string& key)
{	
	GetSetIO::save<GetSetIO::IniFile>(g_file_ini,g_config);
}

int main(int argc, char ** argv)
{
	QApplication app(argc,argv);

	GetSet<>("Window/Titel")
		.setDescription("Define a window titel other than the ini-File name.")
		="";
	GetSet<>("Window/Button")
		.setDescription("Define a caption for an Ok-button.");
	GetSetGui::Enum("File/Type").setChoices(".xml;.txt")
		.setDescription("Either xml or txt, depening on format of file containing type info.")
		.setValue(0);

	GetSetIO::CmdLineParser cmd;
	cmd.declare();
	cmd.parse(argc,argv);

	if (argc<2 || (argc==2 && std::string(argv[1])=="-h") )
	{
		std::cout <<
			"Usage:\n"
			"   AutoConf file.ini [--titel window_titel] [--button caption] [--type txt]\n"
			"   AutoConf file.ini [...] tab1 tab2 ...\n"
			"Show a GUI to edit contents of an ini-File\n"
			"Types can be provided by an xml-file or special txt-file with the same name.\n"
			"\n"
			<< cmd.getSynopsis();
		return 0;
	}

	std::vector<std::string> tabs;
	auto arg=cmd.getUnhandledArgs();
	for (int i=2;i<(int)arg.size();i++)
		tabs.push_back(arg[i]);

	// Try to load both ini and XML file
	g_file_ini=argv[1];
	std::string path=g_file_ini,extension;
	extension=splitRight(path,".");
	g_file_xml=path+GetSet<>("File/Type").getString();
	std::string basename=splitRight(path,"/\\");
	bool metaInfoFound=1;
	if (GetSet<int>("File/Type")==0)
		metaInfoFound&=GetSetIO::load<GetSetIO::XmlFile>(g_file_xml,g_config);
	else
		metaInfoFound&=GetSetIO::load<GetSetIO::TxtFileDescription>(g_file_xml,g_config);
	GetSetIO::load<GetSetIO::IniFile>(g_file_ini,g_config);

	// If no XML file was found, create one
	if (!metaInfoFound)
	{
		if (GetSet<int>("File/Type")==0)
			GetSetIO::load<GetSetIO::XmlFile>(g_file_xml,g_config);
		else
			GetSetIO::load<GetSetIO::TxtFileDescription>(g_file_xml,g_config);
	}

	GetSetHandler on_value_change(g_config,save);

	GetSetSettingsWindow w("",g_config,GetSet<>("Window/Titel"),vectorToString(tabs));
	if (!GetSet<>("Window/Button").getValue().empty())
		w.setButton(GetSet<>("Window/Button"),die);
	w.show();

	return app.exec();
}
