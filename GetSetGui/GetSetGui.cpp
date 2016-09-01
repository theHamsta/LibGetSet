#include "GetSetGui.h"
#include "GetSetSettingsWindow.h"
#include "GetSetProgressWindow.h"
#include "../GetSet/GetSetIO.h"
#include "../GetSet/GetSetScripting.h"
#include "GetSetSettingsWindow.h"

#include <QApplication>
#include <QMessageBox>

namespace GetSetGui
{

	GetSetApplication::GetSetApplication(std::string _appname)
		: appname(_appname)
		, ini_file(_appname+".ini")
		, qt_app(0x0)
		, callback(0x0)
		, main_window(0x0)
		, progress_window(0x0)
	{}

	GetSetApplication::~GetSetApplication()
	{
		if (main_window) delete main_window;
		if (qt_app) delete qt_app;
		if (callback) delete callback;
	}

	GetSetIO::CmdLineParser& GetSetApplication::commandLine()
	{
		return cmd;
	}

	bool GetSetApplication::init(int &argc, char **argv, void (*gui)(const std::string&, const std::string&))
	{
		qt_app=new QApplication(argc,argv);
		if ( argc==2 && (std::string(argv[1])=="--help"||std::string(argv[1])=="-h") )
		{
			std::cerr <<
				"Usage:\n"
				"   " << appname << " " << appname << ".ini\n";
			if (!cmd.getFlags().empty())
				std::cerr << 
					"   " << appname << " --parameter value\n"
					"With:\n"
					<< cmd.getSynopsis();
			return false;
		}
		else if (argc<=2)
		{
			if (argc==1)
				loadSettings();
			else
			{
				std::string arg=argv[1];
				std::string ext=splitRight(arg,".");
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
				if (ext=="ini")
				{
					ini_file=argv[1];
					loadSettings();
				}
				else if (ext=="getset")
					return parseScript(std::string("file run ")+argv[1]);
				else
				{
					std::cerr << "Unrecognized command line argument.\n";
					return false;
				}
			}
		}
		else if (!cmd.parse(argc,argv))
		{
			std::cerr <<
				"Failed to parse command line arguments!\n"
				"Try:\n"
				"   " << appname << " --help\n";
			return false;
		}
		else if (cmd.getUnhandledArgs().size()>1)
		{
			std::cerr << "Unrecognized command line arguments:\n";
			for (auto it=cmd.getUnhandledArgs().begin();it!=cmd.getUnhandledArgs().end();++it)
				std::cout << "   " << it->first << "\t" << it->second << std::endl;
			return false;
		}

		callback=new GetSetHandler(gui);
		callback->setIgnoreNotifications(true);
		return true;
	}

	void GetSetApplication::setIgnoreNotifications(bool ignore)
	{
		if (callback) callback->setIgnoreNotifications(ignore);
	}
		
	GetSetSettingsWindow& GetSetApplication::window()
	{
		if (!main_window)
		{
			main_window=new GetSetSettingsWindow();
			main_window->setWindowTitle(appname.c_str());
			main_window->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
		}
		return *main_window;
	}

	GetSetProgressWindow& GetSetApplication::progress()
	{
		if (!progress_window)
		{
			progress_window=new GetSetProgressWindow();
			progress_window->setWindowTitle(appname.c_str());
		}
		return *progress_window;
	}

	void GetSetApplication::progressStart(const std::string& name, const std::string& info, int maximum, bool *cancel_clicked)
	{
		window().hide();
		auto &p=progress();
		p.setWindowTitle(name.c_str());
		p.info->setText(info.c_str());
		p.info->show();
		p.progress_bar->setMaximum(maximum);
		p.progress_bar->setValue(0);
		p.progress_bar->show();
		if (cancel_clicked)
		{
			*cancel_clicked=false;
			p.button->show();
			p.cancel_clicked=cancel_clicked;
		}
		else
			p.button->hide();
		progress().show();
	}

	void GetSetApplication::progressUpdate(int i)
	{
		progress().progress_bar->setValue(i);
		progress().progress_bar->show();
		QApplication::processEvents();
	}

	void GetSetApplication::progressEnd()
	{
		progress().hide();
		progress().progress_bar->hide();
		window().show();
	}

	int warn(const std::string& who, const std::string& what, bool only_inormative=true)
	{
		if (only_inormative)
			return QMessageBox::information(0x0,who.c_str(),what.c_str(),QMessageBox::Ok);
		else
			return QMessageBox::warning(0x0,who.c_str(),what.c_str(),QMessageBox::Ok);
	}

	void GetSetApplication::saveSettings() const
	{
		GetSetIO::save<GetSetIO::IniFile>(ini_file);
	}

	void GetSetApplication::loadSettings()
	{
		GetSetIO::load<GetSetIO::IniFile>(ini_file);		
	}

	bool GetSetApplication::parseScript(const std::string& script)
	{
		GetSetScriptParser::global().parse(script);
		return GetSetScriptParser::global().good();
	}

	int GetSetApplication::exec()
	{
		window().show();
		callback->setIgnoreNotifications(false);
		return qt_app->exec();			
	}


} // namespace GetSetGui
