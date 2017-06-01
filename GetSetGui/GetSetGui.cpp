#include "GetSetGui.h"
#include "GetSetTabWidget.h"
#include "GetSetProgressWindow.h"
#include "../GetSet/GetSetLog.hxx"
#include "../GetSet/GetSetIO.h"
#include "../GetSet/GetSetScripting.h"

#include <QApplication>
#include <QMessageBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>

void gui_update(const std::string&, void*)
{
	QApplication::processEvents();
}

namespace GetSetGui
{

	GetSetApplication::GetSetApplication(std::string _appname, GetSetDictionary& _dict)
		: dict(_dict)
		, cmd(dict)
		, qt_app(0x0)
		, callback(0x0)
		, main_window(0x0)
		, progress_window(0x0)
		, log(new GetSetLog(_appname+".log"))
	{
		GetSet<>("Application",dict)=_appname;
		GetSet<>("ini-File",dict)=_appname+".ini";
	}

	GetSetApplication::~GetSetApplication()
	{
		delete log;
//		if (main_window) delete main_window;
//		if (callback) delete callback;
//		if (qt_app) delete qt_app; // Crashed on delete on windows (why?)
	}

	GetSetIO::CmdLineParser& GetSetApplication::commandLine()
	{
		return cmd;
	}

	bool GetSetApplication::init(int &argc, char **argv, void (*gui)(const std::string&, const std::string&))
	{
		std::string appname=GetSet<>("Application",dict);
		qt_app=new QApplication(argc,argv);
		bool single_unhandled_arg=false;
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
		// Default is to try and load ini-file or run a script
		else if (argc<=2)
		{
			std::string arg=argc>1?argv[1]:"";
			std::string ext=splitRight(arg,".");
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			if (ext=="ini")
				GetSet<>("ini-File",dict)=argv[1];
			loadSettings();
			callback=new GetSetHandler(gui,dict);
			// Run script
			if (ext=="getset")
			{
				std::string script=fileReadString(argv[1]);
				if (script.empty())
				{
					std::cerr <<
						"Failed to parse command line arguments!\n"
						"Try:\n"
						"   " << appname << " --help\n";
					return false;
				}
				else return parseScript(script);
			}
			else
				single_unhandled_arg=ext=="ini";
		}

		// We have multiple command line args or we did not understand the first one.
		if (argc>2 || single_unhandled_arg)
		{  
			if (!cmd.parse(argc,argv))
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
		}

		window().setCallBack(gui);
		return true;
	}

	void GetSetApplication::ignoreNotifications(bool ignore)
	{
		if (callback) callback->ignoreNotifications(ignore);
	}
		
	GetSetTabWidget& GetSetApplication::window()
	{
		if (!main_window)
		{
			main_window=new GetSetTabWidget("",dict,GetSet<>("Application",dict));
			main_window->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
		}
		return *main_window;
	}

	GetSetProgressWindow& GetSetApplication::progress()
	{
		if (!progress_window)
		{
			progress_window=new GetSetProgressWindow();
			progress_window->setWindowTitle(GetSet<>("Application",dict).getString().c_str());
		}
		return *progress_window;
	}

	void GetSetApplication::progressStart(const std::string& name, const std::string& info, int maximum, bool *cancel_clicked)
	{
		window().hide();
		progress().start(name,info,maximum,cancel_clicked);
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

	void GetSetApplication::info(const std::string& who, const std::string& what, bool show_dialog)
	{
		if (show_dialog)
			QMessageBox::information(0x0,who.c_str(),what.c_str(),QMessageBox::Ok);
	}

	void GetSetApplication::warn(const std::string& who, const std::string& what, bool show_dialog)
	{
		if (show_dialog)
			QMessageBox::warning(0x0,who.c_str(),what.c_str(),QMessageBox::Ok);
		else std::cerr << who << ": " << what << std::endl;
	}

	void GetSetApplication::saveSettings() const
	{
		GetSetIO::save<GetSetIO::IniFile>(GetSet<>("ini-File",dict));
	}

	void GetSetApplication::loadSettings()
	{
		GetSetIO::load<GetSetIO::IniFile>(GetSet<>("ini-File",dict));		
	}

	bool GetSetApplication::parseScript(const std::string& script)
	{
		GetSetScriptParser parser(dict);
		parser.addErrorCallback(0x0,gui_update);
		parser.parse(script);
		return parser.good();
	}

	int GetSetApplication::exec()
	{
		window().show();
		return qt_app->exec();			
	}


} // namespace GetSetGui
