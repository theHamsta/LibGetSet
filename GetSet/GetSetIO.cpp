#include "GetSetIO.h"

#include "StringUtil.hxx"

#include <fstream>

namespace GetSetIO {

	//
	// IniFile
	//

	InputOutput& IniFile::loadStream(std::istream& istr)
	{
		std::string section,key,value;
		// Inerpret lines individually
		for (int lineNumber=0; !istr.eof() && istr.good(); lineNumber++)
		{
			std::string line;
			getline(istr,line,'\n');
			// Ignore comments
			if (line.length()<2||line[0]=='#') continue;
			// Parse section paths
			if (line[0]=='[')
			{
				section=line.substr(1,line.length()-2);
				continue;
			}
			// Otherwise, interpret line as a "key = value" assignment
			std::istringstream linestr(line);
			getline(linestr,key,'=');
			getline(linestr,value,'\0');
			trim(key);
			trim(value);
			std::string path=section+(section.empty()?"":"/")+key;
			contents[path]["Value"]=value;
		}
		return *this;
	}

	void IniFile::saveStream(std::ostream& ostr) const 
	{
		// Copy everything to helper first, to group entries by section
		MapStrStr helper;
		for (MapStrMapStrStr::const_iterator sectit=contents.begin();sectit!=contents.end();++sectit)
		{
			// Get a key and its section
			std::string section=sectit->first;
			// Create a "[...]" paragraph for each (sub-)section when key is empty
			std::string key=splitRight(section,"/\\");
			// Access its value
			MapStrStr::const_iterator value=sectit->second.find("Value");
			// Access its type
			MapStrStr::const_iterator type_it=sectit->second.find("Type");
			std::string type=type_it==sectit->second.end()?"":type_it->second;
			// Check that it is not a simple GUI element without a value.
			if (type!="Button" && type!="StaticText")
				// Store a string "key = value" in the corresponding "[...]" paragraph
				if (value!=sectit->second.end())
					helper[std::string("[")+section+"]\n"] += key + " = " + value->second + "\n";
		}
		// Then in the end just write contents of helper to ostr
		for (MapStrStr::iterator it=helper.begin();it!=helper.end();++it)
			ostr << std::endl << it ->first << it->second << std::endl;
	}
	
	//
	// TxtFileKeyValue
	//

	InputOutput& TxtKeyValue::loadStream(std::istream& istr)
	{
		std::string line,path,value;
		// Interpret lines individually
		for (int lineNumber=0; !istr.eof(); lineNumber++)
		{
			getline(istr,line,'\n');
			// Ignore comments
			if (line.length()<2||line[0]=='#') continue;
			// Parse "path to key = value" style strings
			std::istringstream linestr(line);
			getline(linestr,path,'=');
			getline(linestr,value,'\0');
			contents[path]["Value"]=value;
		}
		return *this;
	}

	void TxtKeyValue::saveStream(std::ostream& ostr) const
	{
		// Simply iterate over all keys and store their value
		for (MapStrMapStrStr::const_iterator sectit=contents.begin();sectit!=contents.end();++sectit)
		{
			// Access its type
			MapStrStr::const_iterator tit=sectit->second.find("Type");
			std::string type=tit==sectit->second.end()?"":tit->second;
			// Check that it is not a simple GUI element without a value.
			if (type!="Button" && type!="StaticText")
			{
				// Write
				MapStrStr::const_iterator cit=sectit->second.find("Value");
				if (cit!=sectit->second.end())
					ostr << sectit->first+"="+cit->second+"\n";
			}
		}
	}

	//
	// TxtDetailed
	//

	InputOutput& TxtDetailed::loadStream(std::istream& istr)
	{
		// Interpret lines individually. 
		for (int lineNumber=0; !istr.eof(); lineNumber++)
		{
			std::string line;
			getline(istr,line,'\n');
			std::map<std::string,std::string> m;
			// Parse XML attribute="value" style string
			attrib_parse(line,m);
			// Make sure that all entries contain the "Key" attribute, otherwise ignore that line.
			if (m["Key"].empty()) continue;
			contents[m["Key"]]=m;
			contents[m["Key"]].erase(contents[m["Key"]].find("Key"));
		}
		return *this;
	}

	void TxtDetailed::saveStream(std::ostream& ostr) const
	{
		for (MapStrMapStrStr::const_iterator p=contents.begin();p!=contents.end();++p)
		{
			// Output all keys along with additional info (like GUI type etc.) in an XML attribute="value" style
			MapStrStr tmp=p->second;
			tmp["Key"]=p->first;
			ostr << attrib_list(tmp) << std::endl;
		}
	}

	//
	// SingleLine
	//

	InputOutput& SingleLine::loadStream(std::istream& istr)
	{
		std::string all;
		getline(istr,all,'\0');
		std::map<std::string,std::string> m;
		// Parse XML attribute="value" style string
		attrib_parse(all,m);
		for (auto it=m.begin();it!=m.end();++it)
			contents[it->first]["Value"]=it->second;
		return *this;
	}

	void SingleLine::saveStream(std::ostream& ostr) const
	{
		std::map<std::string,std::string> tmp;
		for (MapStrMapStrStr::const_iterator p=contents.begin();p!=contents.end();++p) {
			auto it=p->second.find("Value");
			if (it!=p->second.end())
				tmp[p->first]=it->second;
		}
		ostr << attrib_list(tmp) << std::endl;
	}


} // namespace GetSetIO
