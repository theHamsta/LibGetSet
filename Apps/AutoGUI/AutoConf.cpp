#include <iostream>

#include <QApplication>
#include <QPushButton>

#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetCmdLine.hxx>
#include <GetSet/GetSetIO.h>
#include <GetSet/GetSetXML.h>

#include <GetSetGui/GetSetTabWidget.h>

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

	GetSet<>("Window/Title")
		.setDescription("Define a window title other than the ini-File name.")
		="";
	GetSet<>("Window/Button")
		.setDescription("Define a caption for an Ok-button.");
	GetSetGui::Enum("File/Type").setChoices(".xml;.txt")
		.setDescription("Either .xml or .txt, depening on format of file containing type info.")
		.setValue(0);

	GetSetIO::CmdLineParser cmd;
	cmd.declare();
	cmd.parse(argc,argv);

	if (argc<2 || (argc==2 && std::string(argv[1])=="-h") )
	{
		std::cout <<
			"Usage:\n"
			"   AutoConf file.ini [--title window_title] [--button caption] [--type txt]\n"
			"   AutoConf file.ini [...] tab1 tab2 ...\n"
//			"   AutoConf file.ini [...] --hide tab1 tab2 ...\n"
			"Show a GUI to edit contents of an ini-File\n"
			"Types can be provided by an xml-file or special txt-file with the same name.\n"
//			"The latter two mode allows for an explicit list of tabs to be displayed or hidden.\n"
			"\n"
			<< cmd.getSynopsis();
		return 1;
	}

	std::vector<std::string> tabs;
	std::map<int,std::string> arg=cmd.getUnhandledArgs();
	for (int i=2;i<(int)arg.size();i++)
		tabs.push_back(arg[i]);

	// Try to load both ini and XML file
	g_file_ini=argv[1];
	std::string path=g_file_ini,extension;
	extension=splitRight(path,".");
	g_file_xml=path+GetSet<>("File/Type").getString();
	std::string basename=splitRight(path,"/\\");
	bool metaInfoFound=0;
	if (GetSet<int>("File/Type")==0)
		metaInfoFound=GetSetIO::load<GetSetIO::XmlFile>(g_file_xml,g_config);
	else
		metaInfoFound=GetSetIO::load<GetSetIO::TxtFileDescription>(g_file_xml,g_config);
	GetSetIO::load<GetSetIO::IniFile>(g_file_ini,g_config);
	GetSetIO::save<GetSetIO::IniFile>(g_file_ini,g_config);

	// If no XML file was found, create one
	if (!metaInfoFound)
	{
		if (GetSet<int>("File/Type")==0)
			GetSetIO::save<GetSetIO::XmlFile>(g_file_xml,g_config);
		else
			GetSetIO::save<GetSetIO::TxtFileDescription>(g_file_xml,g_config);
	}

	GetSetHandler on_value_change(g_config,save);

	GetSetGui::GetSetTabWidget w("",g_config,GetSet<>("Window/Title"),vectorToString(tabs));
	if (!GetSet<>("Window/Button").getValue().empty())
	{
		w.setCallBack(die);
		w.addButton(GetSet<>("Window/Button"))->setDefault(true);
	}
	w.show();

	app.exec();
	// This application returns 1 by default, unless user clicks the "Ok" button.
	// This allows the caller to check the return code to tell if the widow has been closed (=cancel)
	// or the used clicked the button (=Ok)
	return 1;
}
