#include "GetSetGui.h"
#include "../GetSet/GetSetIO.h"
#include "GetSetSettingsWindow.h"

#include <QApplication>

namespace GetSetGui
{

	GetSetApplication::GetSetApplication(std::string _appname, void (*gui)(const std::string&, const std::string&), int argc, char **argv)
		: appname(_appname)
		, qt_app(argc,argv)
		, callback(gui)
		, main_window(0x0)
	{
		callback.setIgnoreNotifications(true);
	}

	void GetSetApplication::showSettings()
	{
		if (!main_window)
		{
			main_window=new GetSetSettingsWindow();
			main_window->setWindowTitle(appname.c_str());
			main_window->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
		}
		main_window->show();
	}

	void GetSetApplication::saveSettings() const
	{
		GetSetIO::save<GetSetIO::IniFile>(appname+".ini");
	}

	void GetSetApplication::loadSettings()
	{
		GetSetIO::load<GetSetIO::IniFile>(appname+".ini");		
	}

	bool GetSetApplication::parse(const std::string& script)
	{
		parser.parse(script);
		return parser.good();
	}

	void GetSetApplication::setIgnoreNotifications(bool ignore)
	{
		callback.setIgnoreNotifications(false);
	}

	int GetSetApplication::exec()
	{
		showSettings();
		loadSettings();
		setIgnoreNotifications(false);
		return qt_app.exec();			
	}


} // namespace GetSetGui
