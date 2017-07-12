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

#ifndef __GetSetFile_h
#define __GetSetFile_h

#include "GetSet.hxx"

namespace GetSetInternal {
	/// A simple representation of all the information in a property (sub-)tree
	struct InputOutput{
		typedef std::map<std::string, std::string> MapStrStr;
		typedef std::map<std::string, std::map<std::string, std::string> > MapStrMapStrStr;
		std::map<std::string, std::map<std::string, std::string> > contents;

		/// Retreive information in this object from a Section or Dictionary. path_prefix is used for recursion and empty by default.
		void retreive(const Section& section=Dictionary::global(), const std::string& path_prefix="");

		/// Create Section from information in this object
		void restore(Section& section=Dictionary::global()) const;

		virtual void loadStream(std::istream& istr) = 0;
		virtual void saveStream(std::ostream& ostr) const = 0;
	};

} // namespace GetSetInternal

namespace GetSetIO {

	template <typename InputOutputType=IniFile>
	inline bool load(const std::string& path, GetSetInternal::Section& section=GetSetGui::Section())
	{
		std::ifstream file(path);
		if (!file) return false;
		InputOutputType io;
		io.loadStream(file);
		io.restore(section);
		return true;
	}

	template <typename InputOutputType=IniFile>
	inline bool save(const std::string& path, const GetSetInternal::Section& section=GetSetGui::Section())
	{
		std::ofstream file(path);
		if (!file) return false;
		InputOutputType io;
		io.retreive(section);
		io.saveStream(file);
		return true;
	}

	/// A simple text file with one property per line in "section/key=value" format
	struct TxtKeyValue : public GetSetInternal::InputOutput {
		virtual void loadStream(std::istream& istr);
		virtual void saveStream(std::ostream& ostr) const ;
	};

	/// An ini-File in "[Section.Subsection] Key=Value" format
	struct IniFile : public GetSetInternal::InputOutput {
		virtual void loadStream(std::istream& istr);
		virtual void saveStream(std::ostream& ostr) const ;
	};

	/// A text file with one property per line containing all information (Key, Value, Type and additional info) in attribute="value" format
	struct TxtDetailed : public GetSetInternal::InputOutput {
		virtual void loadStream(std::istream& istr);
		virtual void saveStream(std::ostream& ostr) const ;
	};

} // namespace GetSetIO

#endif // __GetSetFile_h


