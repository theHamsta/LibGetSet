
#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetTabWidget.h>
#include <GetSetGui/GetSetScriptEdit.h>

#include <iostream>
#include <string>

GetSetGui::GetSetApplication g_app("GetSetScript");

void gui(const std::string& section, const std::string& key)
{
	std::cout << "Changed: " << section << " -> " << key << std::endl;
	g_app.saveSettings();
}

int main(int argc, char** argv)
{
	g_app.init(argc,argv,gui);
	g_app.window().setWindowTitle("GetSet Keys");
	GetSetGui::GetSetScriptEdit script_edit;
	script_edit.setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
	script_edit.openFile("GetSetScript.getset");
	script_edit.show();
	GetSetGui::GetSetWidget global(GetSetDictionary::global(),"");
	global.setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
	global.setWindowTitle("Global GetSet Dictionary (including hidden keys)");
	global.show();
	return g_app.exec();
}
