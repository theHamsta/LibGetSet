#include "GetSet/GetSetSpecial.hxx"
#include "GetSet/GetSetXML.h"

#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>

#include <QtGui/QApplication>

#include "Process.h"

#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>


void gui(const std::string& section, const std::string& key)
{
	GetSetIO::save<GetSetIO::IniFile>("config.ini");
}

int main(int argc, char **argv)
{
	QApplication app(argc,argv);

	//QScrollArea *frame_scroll=new QScrollArea();
	//QFrame *container=new QFrame();
	//QVBoxLayout *layout=new QVBoxLayout(container);
 // 
	//for(int i=0;i<20;i++)
	//{  
	//	QLabel* w=new QLabel("Whatever.");
	//	layout->addWidget(w);
	//}
 // 
	//frame_scroll->setWidget(container);

	//frame_scroll->show();

	GetSetGui::File("Run/Binary File").setExtensions("Executable File (*.exe)").setDescription("This is a tooltip");

	GetSet<>("Run/abca");
	GetSet<>("Run/abca1");
	GetSet<>("Run/abca2");
	GetSet<>("Run/abca3");
	GetSet<>("Run/abca4");
	GetSet<>("Run/abca5");
	GetSet<>("Run/abca6");
	GetSet<>("Run/abca7");
	GetSet<>("Run/abca8");
	GetSet<>("Run/abca9");
	GetSet<>("Run/abcb");
	GetSet<>("Run/abcb1");
	GetSet<>("Run/abcb2");
	GetSet<>("Run/abcb3");
	GetSet<>("Run/abcb4");
	GetSet<>("Run/abcb5");
	GetSet<>("Run/abcb6");
	GetSet<>("Run/abcc1");
	GetSet<>("Run/abcc2");
	GetSet<>("Run/abcc3");
	GetSet<>("Run/abcc4");
	GetSet<>("Run/abcc6");
	GetSet<>("Run/abcc7");
	GetSet<>("Run/abcc8");
	GetSet<>("Run/abcd1");
	GetSet<>("Run/abcd2");
	GetSet<>("Run/abcd3");
	GetSet<>("Run/abcd4");
	GetSet<>("Run/abcd5");
	GetSet<>("Run/abca");
	GetSet<>("Runabs/abca1");
	GetSet<>("Runabs/abca2");
	GetSet<>("Runabs/abca3");
	GetSet<>("Runabs/abca4");
	GetSet<>("Runabs/abca5");
	GetSet<>("Runabs/abca6");
	GetSet<>("Runabs/abca7");
	GetSet<>("Runabs/abca8");
	GetSet<>("Runabs/abca9");
	GetSet<>("Runabs/abcb");
	GetSet<>("Runabs/abcb1");
	GetSet<>("Runabs/abcb2");
	GetSet<>("Runabs/abcb3");
	GetSet<>("Runabs/abcb4");
	GetSet<>("Runabs/abcb5");
	GetSet<>("Runabs/abcb6");
	GetSet<>("Runabs/abcc1");
	GetSet<>("Runabs/abcc2");
	GetSet<>("Runabs/abcc3");
	GetSet<>("Runabs/abcc4");
	GetSet<>("Runabs/abcc6");
	GetSet<>("Runabs/abcc7");
	GetSet<>("Runabs/abcc8");
	GetSet<>("Runabs/abcd1");
	GetSet<>("Runabs/abcd2");
	GetSet<>("Runabs/abcd3");
	GetSet<>("Runabs/abcd4");
	GetSet<>("Runabs/abcd5");
//	GetSetIO::load<GetSetIO::IniFile>("config.ini");
	
	GetSetHandler callback(gui);

	GetSetSettingsWindow window;
	window.setWindowTitle("RCL launcher");
	window.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	window.show();
	
	return app.exec();

}
