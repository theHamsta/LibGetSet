#include "GetSet/GetSetSpecial.hxx"
#include "GetSet/GetSetXML.h"

#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>

#include <QtGui/QApplication>

namespace GetSetIO {

	/// GetSet full description in XML formtat
	class RCLConfiglFile : public GetSetInternal::GetSetInOutValuesOnly
	{
	public:
		RCLConfiglFile(const std::string& file);
		virtual ~RCLConfiglFile();

		virtual void save() const;
		virtual void load();
	};


	RCLConfiglFile::RCLConfiglFile(const std::string& file) : GetSetInOutValuesOnly(file){}
	
	RCLConfiglFile::~RCLConfiglFile() { if (stored) save(); }

	void RCLConfiglFile::save() const
	{
		std::ostringstream ini;
		// Iterate over all values in properties map and write them to ini
		for (MapStrMapStrStr::const_iterator section=contents.begin();section!=contents.end();++section)
		{
			ini << "\n[" << section->first << "]\n";
			for (MapStrStr::const_iterator key=section->second.begin();key!=section->second.end();++key)
				ini << key->first << " = " << key->second << "\n";
		}
		fileWriteString(file,ini.str());
	}

	void RCLConfiglFile::load()
	{
		std::istringstream strstr(fileReadString(file));
		std::string section,key,value;
		for (int lineNumber=0; !strstr.eof(); lineNumber++)
		{
			std::string line;
			getline(strstr,line,'\n');
			if (line.length()<2||line[0]=='#') continue;
			if (line[0]=='[')
			{
				section=line.substr(1,line.length()-2);
				continue;
			}
			std::istringstream linestr(line);
			getline(linestr,key,'=');
			getline(linestr,value,'\0');
			trim(key);
			trim(value);
			contents[section][key]=value;
		}
	}


} // namespace GetSetIO



void gui(const std::string& section, const std::string& key)
{
	std::cout << section << " - " << key << std::endl;
}

int main(int argc, char **argv)
{
	//QApplication app(argc,argv);

	//GetSetIO::save(GetSetIO::XmlFile("out.xml"));
	//
	//GetSetHandler callback(gui);

	//GetSetSettingsWindow window;
	//window.setWindowTitle("RCL launcher");
	//window.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	//window.show();
	//
	//return app.exec();
}
