#include "GetSetIO.h"

#include <fstream>

namespace GetSetInternal {

	void InputOutput::retreive(const Section& section, const std::string& path_prefix)
	{
		const auto& keys=section.getChildren();
		for (auto it=keys.begin();it!=keys.end();++it)
		{
			std::string path=path_prefix.empty()?it->first:path_prefix+"/"+it->first;
			auto& attribs=contents[path];
			attribs=it->second->attributes;
			// Add an attribute for Type
			attribs["Type"]=it->second->getType();
			Section* section=dynamic_cast<Section*>(it->second);
			// And add an attribute for value
			if (section) retreive(*section, path);
			else attribs["Value"]=it->second->getString();
		}
	}

	void InputOutput::restore(Section& section) const
	{
		for (auto it=contents.begin();it!=contents.end();++it)
		{
			// Get path and type
			const std::string& path(it->first);
			auto type_it=it->second.find("Type");
			// Silently ignore keys, for which the type is missing.
			if (type_it==it->second.end()) continue;
			// Create Node and copy attributes
			Node &node=GetSetGui::Section(section).createNode(it->first,type_it->second);
			node.attributes=it->second;
			// Set value and remove already handled attributes "Value" and "Type"
			node.setString(node.attributes["Value"]);
			node.attributes.erase("Type");
			node.attributes.erase("Value");
		}
	}

} // namespace GetSetInternal

namespace GetSetIO {

	//
	// IniFile
	//

	void IniFile::loadStream(std::istream& istr)
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

	void TxtKeyValue::loadStream(std::istream& istr)
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

	void TxtDetailed::loadStream(std::istream& istr)
	{
		// Interpret lines individually. 
		for (int lineNumber=0; !istr.eof(); lineNumber++)
		{
			std::string line;
			getline(istr,line,'\n');
			std::map<std::string,std::string> m;
			// Parse XML attribute="value" style string
			parseAttribs(line,m);
			// Make sure that all entries contain the "Key" attribute, otherwise ignore that line.
			if (m["Key"].empty()) continue;
			contents[m["Key"]]=m;
			contents[m["Key"]].erase(contents[m["Key"]].find("Key"));
		}
	}

	void TxtDetailed::saveStream(std::ostream& ostr) const
	{
		for (MapStrMapStrStr::const_iterator p=contents.begin();p!=contents.end();++p)
		{
			// Output all keys along with additional info (like GUI type etc.) in an XML attribute="value" style
			ostr << std::string("Key=\"")+p->first+"\"";
			for (MapStrStr::const_iterator attr=p->second.begin();attr!=p->second.end();++attr)
				ostr << std::string(" ")+attr->first+"=\""+attr->second+"\"";
			ostr << std::endl;
		}
	}

} // namespace GetSetIO
