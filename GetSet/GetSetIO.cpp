#include "GetSetIO.h"
#include "GetSet.hxx"

namespace GetSetIO {

	//
	// IniFile
	//

	IniFile::IniFile(std::istream& input, std::ostream& output) : GetSetInternal::GetSetInOut(input, output) {}

	void IniFile::write() const
	{
		MapStrStr helper;
		for (MapStrMapStrStr::const_iterator sectit=contents.begin();sectit!=contents.end();++sectit)
		{
			std::string section=sectit->first;
			std::string key=splitRight(section,"/\\");
			MapStrStr::const_iterator value=sectit->second.find("Value");
			if (value!=sectit->second.end())
				helper[std::string("[")+section+"]\n"] += key + " = " + value->second + "\n";
		}
		for (MapStrStr::iterator it=helper.begin();it!=helper.end();++it)
			ostr << std::endl << it ->first << it->second << std::endl;
	}

	void IniFile::read()
	{
		std::string section,key,value;
		for (int lineNumber=0; !istr.eof() && istr.good(); lineNumber++)
		{
			std::string line;
			getline(istr,line,'\n');
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
			std::string path=section+(section.empty()?"":"/")+key;
			contents[path]["Value"]=value;
		}
	}


	//
	// TxtFileKeyValue
	//

	TxtFileKeyValue::TxtFileKeyValue(std::istream& input, std::ostream& output) : GetSetInternal::GetSetInOut(input, output) {}

	void TxtFileKeyValue::write() const
	{
		for (MapStrMapStrStr::const_iterator sectit=contents.begin();sectit!=contents.end();++sectit)
		{
			MapStrStr::const_iterator cit=sectit->second.find("Value");
			if (cit!=sectit->second.end())
				ostr << sectit->first+"="+cit->second+"\n";
		}
	}

	void TxtFileKeyValue::read()
	{
		std::string line,path,value;
		for (int lineNumber=0; !istr.eof(); lineNumber++)
		{
			getline(istr,line,'\n');
			if (line.length()<2||line[0]=='#') continue;
			std::istringstream linestr(line);
			getline(linestr,path,'=');
			getline(linestr,value,'\0');
			contents[path]["Value"]=value;
		}
	}

	//
	// TxtFileDescription
	//

	TxtFileDescription::TxtFileDescription(std::istream& input, std::ostream& output) : GetSetInternal::GetSetInOut(input, output) {}

	void TxtFileDescription::write() const
	{
		for (MapStrMapStrStr::const_iterator p=contents.begin();p!=contents.end();++p)
		{
			ostr << std::string("Key=\"")+p->first+"\"";
			for (MapStrStr::const_iterator attr=p->second.begin();attr!=p->second.end();++attr)
				ostr << std::string(" ")+attr->first+"=\""+attr->second+"\"";
			ostr << std::endl;
		}
	}

	void TxtFileDescription::read()
	{
		for (int lineNumber=0; !istr.eof(); lineNumber++)
		{
			std::string line;
			getline(istr,line,'\n');
			std::map<std::string,std::string> m;
			parseAttribs(line,m);
			if (m["Key"].empty()) continue;
			contents[m["Key"]]=m;
			contents[m["Key"]].erase(contents[m["Key"]].find("Key"));
		}
	}

} // namespace GetSetIO
