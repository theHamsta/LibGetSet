#include "GetSetCmdLineParser.h"

#include <algorithm>

#include "tinyxml2.h"

#define SAFE_STR(X) ( X==0 ? "" : X)

GetSetCmdLineParser::GetSetCmdLineParser(bool autoAll, GetSetDictionary& dictionary)
	: dict(dictionary)
{
	if (autoAll)
		for (GetSetDictionary::Dictionary::const_iterator it=dict.get().begin();it!=dict.get().end();++it)
			flagAutoSection(it->first,true);
}

//
// Main function: parse the command line
//

bool GetSetCmdLineParser::parse(int argc, char **argv, bool printSynopsisOnFailure)
{

	for (int i=1;i<argc;i++)
	{
		std::string arg;
		// Make sure the next on argv is a flag and starts with '-'
		std::string flag=argv[i];
		if (flag=="--xml")
		{
			std::string xml=getXML();
			std::cout << xml;
			exit(0); // the app shouldn't do anything else
		}
		if (!flag.empty() && flag[0]!='-')
		{
			// We expected a flag but found an unnamed argument
			arg=flag;
			flag=toString(unnamedArgs.size());
			unnamedArgs.push_back(arg);
		}
		// arg is actually a command line flag
		if (flags.find(flag)!=flags.end())
		{
			const std::string& section=flags[flag].first;
			const std::string& key=flags[flag].second;
			GetSetInternal::GetSetDataInterface *p=dict.getDatainterface(section,key);
			std::string type;
			if (p) type=p->getType();
			if (type=="Trigger") // triggers are just called. No argument value needed either
				GetSetGui::Trigger(section,key).action();
			else
			{
				// It really is a flag and the argument is up next
				if (flag[0]=='-')
				{
					if (i==argc-1)
					{
						// no value coming up. can't handle.
						unhandledArgs.push_back(StringPair(arg,""));
						continue;
					}
					else
						arg=argv[++i];
				}
				GetSet<std::string>(section,key)=arg;
			}
			// if  this section/key pair is on the required list, remove it
			if (required.find(section)!=required.end())
			{
				required[section].erase(key);
				if (required[section].empty())
					required.erase(required.find(section));
			}
		}
		else
			unhandledArgs.push_back(StringPair(arg, (i==argc-1) ? "" : argv[++i]));
	}
	bool success=unhandledArgs.empty() && required.empty();
	// Some information if parsing failed. 2do nicer and more standard output formatting
	if (!success && printSynopsisOnFailure)
	{
		std::cerr << "Faile to parse command line arguments.\n";
		if (!required.empty())
		{
			std::cerr << "The following required arguments were not provided:\n";
			for (std::map<std::string,std::set<std::string> >::iterator it=required.begin();it!=required.end();++it)
				for (std::set<std::string>::iterator it2=it->second.begin();it2!=it->second.end();++it2)
					std::cerr << "[" << it->first << "]" << *it2 << "\n";
		}
		if (!unhandledArgs.empty())
		{
			std::cerr << "The following arguments were not recognized:\n";
			for (std::vector<StringPair>::iterator it=unhandledArgs.begin();it!=unhandledArgs.end();++it)
				std::cerr << it->first << ":" << it->second;
		}
		std::cout << "Known Arguments:\n" << synopsis();

	}
	return success;
	
}

std::string GetSetCmdLineParser::synopsis() const
{
	std::string ret;
	for (MapStrStrPair::const_iterator it=flags.begin();it!=flags.end();++it)
	{
		const std::string& section=it->second.first;
		const std::string& key=it->second.second;
		std::map<std::string,std::set<std::string> >::const_iterator rit=required.find(section);
		bool r=(rit!=required.end() && rit->second.find(key)!= rit->second.end());
		if (r) ret+=it->first + ": [" + section + "]" + key + "\n";
		else ret+="(" + it->first + " [" + section + "]" + key + ")\n";
	}
	return ret;
}


//
// XML support
//

std::string GetSetCmdLineParser::getXML() const
{
	std::ostringstream xml;
	for (MapStrStrPair::const_iterator it=flags.begin();it!=flags.end();++it)
	{
		const std::string& section=it->second.first;
		const std::string& key=it->second.second;
		std::map<std::string,std::set<std::string> >::const_iterator rit=required.find(section);
		bool r=(rit!=required.end() && rit->second.find(key)!= rit->second.end());
		xml << "<CommandLineFlag "
			<< "Section=\"" << section
			<< "\" Key=\"" << key
			<< "\" Required=\"" << toString(r)
			<< "\">";
		xml << it->first;
		xml << "</CommandLineFlag>\n";
	}
	return dict.getXML()+"\n"+xml.str();
}

void GetSetCmdLineParser::parseXML(const std::string& xml)
{
	tinyxml2::XMLDocument t;
	t.Parse(xml.c_str());
	std::string section, key, value;
	tinyxml2::XMLElement* node = t.FirstChildElement();
	while (node)
	{
		std::string tag=node->Name();
		if (tag=="CommandLineFlag")
		{
			std::string section=SAFE_STR(node->Attribute("Section"));
			std::string key=SAFE_STR(node->Attribute("Key"));
			std::string type=SAFE_STR(node->Attribute("Type"));
			std::string cmdflag=node->GetText();
			if (!type.empty())
				dict.declare(section,key,type);
			flag(cmdflag,section,key);
		}
		node=node->NextSiblingElement();
	}
}

//
// Accessors
//

const std::vector<std::string>& GetSetCmdLineParser::getUnnamedArgs() const
{
	return unnamedArgs;
}

const std::vector<GetSetCmdLineParser::StringPair>& GetSetCmdLineParser::getUnhandledArgs() const
{
	return unhandledArgs;
}

//
// Definitions of flag*s
//

// A local helper function turing arbitrary GetSet property names into a all lower-case no whitespace strings
std::string normalizeName(std::string name)
{
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	std::replace(name.begin(), name.end(),' ','-');
	return name;
}

void GetSetCmdLineParser::flag(const std::string& flag, const std::string& section, const std::string& key)
{
	flags[flag]=StringPair(section,key);
}

void GetSetCmdLineParser::flagAuto(const std::string& section, const std::string& key, bool includeSectionInFlag)
{
	flags[includeSectionInFlag ? normalizeName("--"+section+"-"+key) : normalizeName("--"+key)]=StringPair(section,key);
}

void GetSetCmdLineParser::flagAutoSection(const std::string& section, bool includeSectionInFlag)
{
	GetSetDictionary::Dictionary::iterator dictit=dict.get().find(section);
	if (dictit==dict.get().end()) return;
	for (GetSetDictionary::Section::iterator it=dictit->second.begin();it!=dictit->second.end();++it)
		flagAuto(section,it->first,includeSectionInFlag);
}

void GetSetCmdLineParser::flagIndexed(int index, const std::string& section, const std::string& key)
{
	flags[toString(index)]=StringPair(section,key);
}

void GetSetCmdLineParser::require(const std::string& section, const std::string& key)
{
	required[section].insert(key);
}

const GetSetCmdLineParser::MapStrStrPair& GetSetCmdLineParser::getFlags() const
{
	return flags;
}
