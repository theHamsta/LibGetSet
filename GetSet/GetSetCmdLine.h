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

#ifndef __GetSetCmdLine_h
#define __GetSetCmdLine_h

#include "GetSet.hxx"

namespace GetSetIO {
	/// Set properties via command line.
	class GetSetCmdLineParser : public GetSetInternal::Access {
	public:
		typedef std::map<std::string, std::string>	MapStrStr;

		/// Longformat: GetSet("Super.Section.Some Key") becomes --super-section-some-key instead of just --some-key
		GetSetCmdLineParser(GetSetDictionary& d=GetSetDictionary::global(), bool useLongFlags=false)
			: GetSetInternal::Access(d)
			, longFormat(useLongFlags)
		{}

		/// Parse command line arguments.
		bool parse(int argc, char **argv);

		/// A brief text listing all command line flags
		std::string synopsis() const
		{
			std::ostringstream str;
			for (MapStrStr::const_iterator it=flags.begin();it!=flags.end();++it)
				str << it->first << std::endl;
			return str.str();
		}

		/// Unhandled command line arguments
		const std::map<int,std::string>& getUnhandledArgs() const {return unhandledArgs;}

		/// Define Flag is the same as instantiating GetSet&lt;type&gt;(key) prior to calling flag(cmdflag,key)
		template <typename BasicType>
		void declareKey(std::string& key, bool required=false)
		{
			GetSet<BasicType>(key);
			flag(key);
			if (required) require(key);
			return *this;
		}

		/// Define an unnamed command line argument.
		template <typename BasicType>
		void declareIndexedKey(int idx ,const std::string& key, bool required=false)
		{
			GetSet<BasicType>(key);
			flag(toString(idx),key);
			if (required) require(key);
			return *this;
		}

		/// Tells the parser that this property is not optional. If its value is not specified, parse(...) will return false.
		void require(const std::string& key) {required.insert(key);}

		/// Declare flag(s) automatically. If longFormat is false, the keys must be unambigous.
		void flag(const std::string& path="");

	protected:
		/// Stores all Flags (and numbers for indexed params) together with a key
		MapStrStr flags;

		/// Control whether GetSet("Super/Section/Some Key") becomes --super-section-some-key or just --some-key
		bool longFormat;

		/// Unhandled command line arguments by its index in argv[]
		std::map<int,std::string> unhandledArgs;
	
		/// Set of required parameters (removed if found)
		std::set<std::string> required;
	};


	void GetSetCmdLineParser::flag(const std::string& path)
	{
		GetSetInternal::GetSetNode *p=getProperty(path);
		if (!p || p->getType()!="Section")
		{
			// Path points to a key
			std::string cmd=path;
			if (!longFormat)
			{
				cmd=path;
				cmd=splitRight(cmd,"/");
			}
			normalize(cmd);
		}
		else
		{
			using namespace GetSetInternal;
			GetSetSection* s=dynamic_cast<GetSetSection*>(p);
			if (!s) return; // never happens
			typedef  std::map<std::string,GetSetNode*> MapStrParam;
			const MapStrParam& m=s->getSection();
			for (MapStrParam::const_iterator it=m.begin();it!=m.end();++it)
				if (path.empty()) flag(it->first);
				else flag(path+"/"+it->first);
		}
	}

	bool GetSetCmdLineParser::parse(int argc, char **argv)
	{
		int unnamed=0;
		for (int i=0;i<argc;i++)
		{
			std::string arg=argv[i];
			if (arg[0]=='-')
			{
				MapStrStr::iterator it=flags.find(arg);
				if (it!=flags.end() && ++i<argc)
				{
					getProperty(it->second)->setString(argv[i]);
					required.erase(it->second);
					continue;
				}
			}
			else
			{
				MapStrStr::iterator it=flags.find(toString(unnamed));
				if (it!=flags.end() && ++i<argc)
				{
					getProperty(it->second)->setString(argv[i]);
					required.erase(it->second);
				}
				unnamed++;
				continue;
			}
			unhandledArgs[i]=arg;
		}
		return unhandledArgs.empty() && required.empty();
	}

} // namespace GetSetIO

#endif // __GetSetCmdLine_h
