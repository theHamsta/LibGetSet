//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by André Aichert (aaichert@gmail.com)
//    
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//  
//    http://www.apache.org/licenses/LICENSE-2.0
//    
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include "GetSetDictionary.h"

#include "tinyxml2.h"
#include "GetSet.hxx"


///////////////////////////////////
// Global Dictionary

GetSetDictionary& GetSetDictionary::globalDictionary()
{
	static GetSetDictionary global;
	return global;
}

//
// Dictionary Implementation
//

GetSetInternal::GetSetDataInterface* GetSetDictionary::getDatainterface(const std::string& section, const std::string& key) const
{
	Dictionary::const_iterator secit=properties.find(section);
	if (secit==properties.end())
		return 0x0;
	Section::const_iterator keyit=secit->second.find(key);
	if (keyit==secit->second.end())
		return 0x0;
	else
		return keyit->second;
}

GetSetDictionary::Dictionary& GetSetDictionary::get()
{
	return properties;
}

//
// XML support
//

#define GETSET_DATA_TYPE_STR(X) if (type==#X) param=new GetSetInternal::GetSetData<X>();
#define GETSET_SPECIAL_TYPE_STR(X)   if (type==#X) param=new GetSetInternal::GetSetData##X();
void GetSetDictionary::loadSection(tinyxml2::XMLElement* node, const std::string& section)
{
	while (node)
	{
		std::string kind=node->Name();
		if (kind=="Key")
		{
			std::string key=node->Attribute("Name");
			std::string type=node->Attribute("Type");
			std::string value=node->GetText();
			// Type selection (create a GetSetData* string provided by "Type" attribute)
			GetSetInternal::GetSetDataInterface * param=0x0;
			GETSET_DATA_TYPE_STR(bool)
			GETSET_DATA_TYPE_STR(int)
			GETSET_DATA_TYPE_STR(unsigned)
			GETSET_DATA_TYPE_STR(float)
			GETSET_DATA_TYPE_STR(double)
			GETSET_DATA_TYPE_STR(std::string)
			GETSET_DATA_TYPE_STR(std::vector<std::string>)
			GETSET_DATA_TYPE_STR(std::vector<double>)
			GETSET_DATA_TYPE_STR(std::vector<int>)
			GETSET_SPECIAL_TYPE_STR(Slider)
			GETSET_SPECIAL_TYPE_STR(Enum)
			GETSET_SPECIAL_TYPE_STR(Trigger)
			GETSET_SPECIAL_TYPE_STR(StaticText)
			GETSET_SPECIAL_TYPE_STR(Directory)
			GETSET_SPECIAL_TYPE_STR(File)
			if (param==0x0)
			{
				std::cerr << "Unknown Type \"" << type << "\" ignored. Using std::string!" << std::endl;
				param=new GetSetInternal::GetSetData<std::string>();
			}
			// add to dictionary6
			param->setString(value);
			if (properties[section].find(key) != properties[section].end())
				delete properties[section][key];
			const tinyxml2::XMLAttribute* attrib=node->FirstAttribute();
			// special attributes
			while (attrib)
			{
				if (std::string(attrib->Name())!="Name" && std::string(attrib->Name())!="Type")
					param->attributes[attrib->Name()]=attrib->Value();
				attrib=attrib->Next();
			}
			// finish up
			properties[section][key]=param;
			signalCreate(section,key);
			signalChange(section,key);
		}
		else if (kind=="Section")
		{
			std::string subsection=section;
			if (!subsection.empty()) subsection.push_back('.');
			subsection+=node->Attribute("Name");
			loadSection(node->FirstChildElement(),subsection);
		}
		node=node->NextSiblingElement();
	}
}
#undef GETSET_PARAM_FOR_TYPE_STR
#undef GETSET_SPECIAL_TYPE_STR

void GetSetDictionary::parseXML(const std::string& xml)
{
	tinyxml2::XMLDocument t;
	t.Parse(xml.c_str());
	std::string section, key, value;
	tinyxml2::XMLElement* node = t.FirstChildElement();	
	loadSection(node);
}

std::string GetSetDictionary::getXML() const
{
	tinyxml2::XMLPrinter printer;
	for (Dictionary::const_iterator sectionit=properties.begin();sectionit!=properties.end();++sectionit)
	{
		const std::string& section(sectionit->first);
		printer.OpenElement("Section");
		printer.PushAttribute("Name",section.c_str());
		for (Section::const_iterator keyit=sectionit->second.begin();keyit!=sectionit->second.end();++keyit)
		{
			const std::string& key(keyit->first);
			const GetSetInternal::GetSetDataInterface& p(*(keyit->second));
			printer.OpenElement("Key");
			printer.PushAttribute("Name",key.c_str());
			printer.PushAttribute("Type",p.getType().c_str());
			for (std::map<std::string,std::string>::const_iterator tag=p.attributes.begin();tag!=p.attributes.end(); ++tag)
				printer.PushAttribute(tag->first.c_str(),tag->second.c_str());
			printer.PushText(p.getString().c_str());	
			printer.CloseElement();
		}
		printer.CloseElement();
	}
	return printer.CStr();
}

//
// ini-File support
//

void GetSetDictionary::parseIni(const std::string& ini)
{
	std::string section="Global";
	std::istringstream strstr(ini);
	for (int lineNumber=0; !strstr.eof(); lineNumber++)
	{
		std::string line;
		getline(strstr,line,'\n');
		// ignore comments and empty lines
		if (line.length()<2 || line[0]=='#' || (line[0]=='/' && line[1]=='/') )
			continue;
		if (line[0]=='[')
		{
			section=line.substr(1,line.length()-2);
			continue;
		}
		std::istringstream linestr(line);
		std::string key,value;
		getline(linestr,key,'=');
		getline(linestr,value,'\0');
		trim(key);
		trim(value);
		GetSet<std::string>(section,key)=value;
	} // for lines
}

std::string GetSetDictionary::getIni() const
{
	std::ostringstream ini;
	for (Dictionary::const_iterator it=properties.begin();it!=properties.end();++it)
	{
		ini << "\n[" << it->first << "]\n\n";
		const Section& s(it->second);
		for (Section::const_iterator pit=s.begin();pit!=s.end();++pit)
		{
			const GetSetInternal::GetSetDataInterface& property(*(pit->second));
			if (property.attributes.find("SaveToFile")!=property.attributes.end()
				&& property.getType()!="Trigger" && property.getType()!="StaticText")
					continue; // skip Triggers and StaticText
			ini << pit->first << " = " << property.getString() << "\n";
		}
		ini << "\n\n";
	}
	return ini.str();
}

//
// Cleaning up
//

GetSetDictionary::~GetSetDictionary()
{
	while(!properties.empty())
		clear(properties.begin()->first);
}

void GetSetDictionary::erase(const std::string& section, const std::string& key)
{
	Dictionary::iterator s=properties.find(section);
	if (s!=properties.end())
	{
		Section::iterator p=s->second.find(key);
		if (p!=s->second.end())
		{
			delete p->second;
			s->second.erase(p);
			signalDestroy(section,key);
			if (s->second.empty())
			{
				properties.erase(s);
				signalDestroy(section);
			}
		}
	}
}

void GetSetDictionary::clear(const std::string& section)
{
	Dictionary::iterator it=properties.find(section);
	if (it!=properties.end())
		for (Section::iterator keyit=it->second.begin();keyit!=it->second.end();++keyit)
		{
			delete keyit->second;
			signalDestroy(section,keyit->first);
		}
	signalDestroy(section);
	properties.erase(it);
}

//
// Signal/Observer implementation
// 

void GetSetDictionary::signalChange(const std::string& section,const std::string& key)
{
	for (std::set<Observer*>::const_iterator it=registered_observers.begin();it!=registered_observers.end();++it)
		(*it)->notifyChange(section,key);
}

void GetSetDictionary::signalCreate(const std::string& section,const std::string& key)
{
	for (std::set<Observer*>::const_iterator it=registered_observers.begin();it!=registered_observers.end();++it)
		(*it)->notifyCreate(section,key);
}

void GetSetDictionary::signalDestroy(const std::string& section,const std::string& key)
{
	for (std::set<Observer*>::const_iterator it=registered_observers.begin();it!=registered_observers.end();++it)
		(*it)->notifyDestroy(section,key);
}

GetSetDictionary::Observer::Observer(GetSetDictionary& subject) : dict(subject)
{
	dict.registered_observers.insert(this);
}

GetSetDictionary::Observer::~Observer()
{
	dict.registered_observers.erase(this);
}

GetSetHandler::GetSetHandler(void (*change)(const std::string&,const std::string&))
	: GetSetDictionary::Observer(GetSetDictionary::globalDictionary())
	, change_handler(change)
{}
	
GetSetHandler::GetSetHandler(GetSetDictionary& subject, void (*change)(const std::string&,const std::string&))
	: GetSetDictionary::Observer(subject)
	, change_handler(change)
{}

void GetSetHandler::notifyChange(const std::string& s,const std::string& k)
{
	if (change_handler) change_handler(s,k);
}
