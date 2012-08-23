#include "GetSetGui.h"

#include "GetSetSettingsWindow.h"

#include <QtGui/QApplication>

namespace GetSetGui
{
	int runQtApp(const std::string& windowTitle, int argc, char **argv)
	{
		QApplication app(argc,argv);

		GetSetSettingsWindow window;
		window.setWindowTitle(windowTitle.c_str());
		window.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
		window.show();
	
		return app.exec();
	}

} // namespace GetSetGui
