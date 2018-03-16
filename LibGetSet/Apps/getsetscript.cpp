
#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetTabWidget.h>
#include <GetSetGui/GetSetScriptEdit.h>

#include <iostream>
#include <string>

GetSetGui::Application g_app("GetSetScript");

void gui(const GetSetInternal::Node& node)
{
	std::cout << "Changed: " << node.super_section << " -> " << node.name << std::endl;
	g_app.saveSettings();
}

int main(int argc, char** argv)
{
	g_app.init(argc,argv,gui);
	GetSetGui::GetSetScriptEdit script_edit;
	script_edit.setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
	script_edit.openFile("GetSetScript.getset");
	script_edit.show();
	GetSetGui::GetSetWidget *global=new GetSetGui::GetSetWidget();
	global->setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
	global->setWindowTitle("Global GetSet Dictionary (including hidden keys)");
	global->show();
	return g_app.exec();
}
