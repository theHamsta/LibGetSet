#include "GetSetIO.h"
#include "GetSet.hxx"

namespace GetSetInternal
{
	//
	// GetSetInOutValuesOnly
	//

	GetSetInOutValuesOnly::GetSetInOutValuesOnly(const std::string file) : GetSetInOut(file) {}

	void GetSetInOutValuesOnly::store(const std::string& section, const std::string& key, GetSetInternal::GetSetNode* value)
	{
		if (value->getType()=="StaticText"||value->getType()=="Button") return;
		stored=1;
		contents[section][key]=value->getString();
	}

	std::string GetSetInOutValuesOnly::retreive(const std::string& section, const std::string& key)
	{
		// Try to load file if it has not been done before
		if (contents.empty())  load();
		// Find section, key and return its value or return empty string
		MapStrMapStrStr::const_iterator s=contents.find(section);
		if (s==contents.end()) return "";
		MapStrStr::const_iterator k=s->second.find(key);
		if (k==s->second.end()) return "";
		return k->second;
	}

	void GetSetInOutValuesOnly::retreiveAll(GetSetDictionary& dictionary)
	{
		// Try to load file if it has not been done before
		if (contents.empty())  load();
		// Iterate over all properties and set respective values in dictionary
		for (MapStrMapStrStr::const_iterator section=contents.begin();section!=contents.end();++section)
		{
			GetSetPath path(section->first,dictionary);
			for (MapStrStr::const_iterator key=section->second.begin();key!=section->second.end();++key)
				path.key<std::string>(key->first)=key->second;
		}
	}

	//
	// GetSetInOutFullDescription
	//

	GetSetInOutFullDescription::GetSetInOutFullDescription(const std::string file) : GetSetInOut(file) {}

	void GetSetInOutFullDescription::store(const std::string& section, const std::string& key, GetSetInternal::GetSetNode* value)
	{
		if (dynamic_cast<GetSetInternal::GetSetSection*>(value)) return;
		stored=1;
		std::string path=section.empty()?key:section+"/"+key;
		contents[path]=value->attributes;
		contents[path]["Value"]=value->getString();
		contents[path]["Type"]=value->getType();
	}

	std::string GetSetInOutFullDescription::retreive(const std::string& section, const std::string& key)
	{
		// Try to load file if it has not been done before
		if (contents.empty()) load();
		// Find path and return its value-attribute or return empty string if not found
		MapStrMapStrStr::const_iterator s=contents.find(section.empty()?key:section+"/"+key);
		if (s==contents.end()) return "";
		MapStrStr::const_iterator a=s->second.find("Value");
		if (a==s->second.end()) return "";
		return a->second;
	}

	void GetSetInOutFullDescription::retreiveAll(GetSetDictionary& dictionary)
	{
		// Try to load file if it has not been done before
		if (contents.empty()) load();
		for (MapStrMapStrStr::iterator it=contents.begin();it!=contents.end();++it)
		{
			GetSetInternal::GetSetNode& p=GetSetInternal::Access::createProperty(dictionary,it->first,it->second["Type"]);
			p.attributes=it->second;
			p.setString(p.attributes["Value"]);
			// These two are handles internally and should not be present in the attributes.
			p.attributes.erase(p.attributes.find("Type"));
			p.attributes.erase(p.attributes.find("Value"));
		}
	}

} // namespace GetSetInternal

namespace GetSetIO {

	//
	// IniFile
	//

	IniFile::IniFile(const std::string& file) : GetSetInOutValuesOnly(file){}
	
	IniFile::~IniFile() { if (stored) save(); }

	void IniFile::save() const
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

	void IniFile::load()
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

	//
	// TxtFileKeyValue
	//

	TxtFileKeyValue::TxtFileKeyValue(const std::string file) : GetSetInOutValuesOnly(file){}
	TxtFileKeyValue::~TxtFileKeyValue() {if (stored) save();}

	void TxtFileKeyValue::save() const
	{
		std::string out;
		for (MapStrMapStrStr::const_iterator section=contents.begin();section!=contents.end();++section)
			for (MapStrStr::const_iterator key=section->second.begin();key!=section->second.end();++key)
				out += section->first+(section->first.empty()?"":"/")+key->first+"="+key->second+"\n";
		fileWriteString(file,out);
	}

	void TxtFileKeyValue::load()
	{
		std::string line,section,key,value;
		std::istringstream strstr(fileReadString(file));
		for (int lineNumber=0; !strstr.eof(); lineNumber++)
		{
			getline(strstr,line,'\n');
			if (line.length()<2||line[0]=='#') continue;
			std::istringstream linestr(line);
			getline(linestr,section,'=');
			getline(linestr,value,'\0');
			key=splitRight(section,"/");
			contents[section][key]=value;
		}
	}

	//
	// TxtFileDescription
	//

	TxtFileDescription::TxtFileDescription(const std::string file) : GetSetInOutFullDescription(file) {}
	TxtFileDescription::~TxtFileDescription() {if (stored) save();}

	void TxtFileDescription::save() const
	{
		std::string out;
		for (MapStrMapStrStr::const_iterator p=contents.begin();p!=contents.end();++p)
		{
			out+="Key=\""+p->first+"\"";
			for (MapStrStr::const_iterator attr=p->second.begin();attr!=p->second.end();++attr)
				out+=" "+attr->first+"=\""+attr->second+"\"";
			out+="\n";
		}
		fileWriteString(file,out);
	}

	void TxtFileDescription::load()
	{
		std::istringstream strstr(fileReadString(file));
		for (int lineNumber=0; !strstr.eof(); lineNumber++)
		{
			std::string line;
			getline(strstr,line,'\n');
			std::map<std::string,std::string> m;
			parseAttribs(line,m);
			if (m["Key"].empty()) continue;
			contents[m["Key"]]=m;
			contents[m["Key"]].erase(contents[m["Key"]].find("Key"));
		}
	}

} // namespace GetSetIO
