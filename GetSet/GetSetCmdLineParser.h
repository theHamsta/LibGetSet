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

#ifndef __GetSetCmdLineParser_h
#define __GetSetCmdLineParser_h

#include "GetSet\GetSet.hxx"

/// Set properties via command line. Assumes the properties are already defined via GetSet<type>(...) or XML.
class GetSetCmdLineParser {
public:
	typedef std::pair<std::string, std::string> StringPair;
	typedef std::map<std::string, StringPair>	MapStrStrPair;

	/// Construct a GetSetCmdLineParser. If autoAll is specified, all GetSet properties will be automatically added with a long flag --section-key.
	GetSetCmdLineParser(bool autoAll=true, GetSetDictionary& dictionary=GetSetDictionary::globalDictionary());

	/// Parse command line arguments. Set properties for all section/key pairs as defined by calls to flag*(...) or XML
	bool parse(int argc, char **argv, bool printSynopsisOnFailure=true);

	/// A brief text listing all command line flags
	std::string synopsis() const;

	/// Returns an XML that defines the command line flags as defined by calls to flag*(...) or XML
	std::string getXML() const;

	/// Reads the command line flags from an XML
	void parseXML(const std::string& xml);


	/// Arguments that are indexed but not named (eg. ./myapp unnamedArgument0 -o valueOfO unnamedArgument1 --more-stuff 123)
	const std::vector<std::string>& getUnnamedArgs() const;

	/// Unhandled command line argument and value such as StringPair("--unknown-flag","valueOfUnhandledArg") or StringPair("","valueOfUnnamedAndUnhandledArg")
	const std::vector<StringPair>& getUnhandledArgs() const;

	// Define Flag is the same as instantiating GetSet<type>(section,key) prior to calling flag(cmdflag,section,key)
	template <typename BasicType>
	GetSetCmdLineParser& defineFlag(const std::string& cmdflag, const std::string& section, const std::string& key, bool required=false)
	{
		GetSet<BasicType>(section,key);
		flag(cmdflag,section,key);
		if (required)
			require(section, key);
		return *this;
	}

	// Define an unnamed command line argument.
	template <typename BasicType>
	GetSetCmdLineParser& defineIndex(int idx , const std::string& section, const std::string& key, bool required=false)
	{
		GetSet<BasicType>(section,key);
		flag(toString(idx),section,key);
		if (required)
			require(section, key);
		return *this;
	}
	
	/// Define a command line flag for a section/key pair. flag can be shortflag eg. "-o" or long flag eg. "--output-dir" but should always start with '-'
	void flag(const std::string& flag, const std::string& section, const std::string& key);

	/// Define flag for a section/key pair automatically (example:  "My Section", "Some Key" becomes either --some-key or --my-section-some-key if includeSectionInFlag)
	void flagAuto(const std::string& section, const std::string& key, bool includeSectionInFlag=false);

	/// Define a short flag for a section/key pair.
	void flagAutoSection(const std::string& section, bool includeSectionInFlag=true);

	/// Define a short flag for a section/key pair.
	void flagIndexed(int index, const std::string& section, const std::string& key);


	/// Tells the parser that this property is not optional. If its value is not specified, parse(...) will return false.
	void require(const std::string& section, const std::string& key);

protected:
	/// Stores all Flags (and numbers for indexed params) together with a section/key pair
	MapStrStrPair				flags;

	/// Dictionary that stored the properties
	GetSetDictionary			&dict;
	
	/// Arguments that are indexed but not named (eg. ./myapp unnamedArgs[0] -o valueOfO unnamedArgs[1] unnamedArgs[2] --more-stuff 123)
	std::vector<std::string>	unnamedArgs;
	/// Unhandled command line argument and value such as StringPair("--unknown-flag","valueOfUnhandledArg")
	std::vector<StringPair>		unhandledArgs;
	/// Set of required parameters (removed if found)
	std::map<std::string,std::set<std::string> > required;
};

#endif // __GetSetCmdLineParser_h
