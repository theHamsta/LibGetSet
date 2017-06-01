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

#include "GetSetDictionary.h"

namespace GetSetIO {

	/// Saving a file
	template <typename GetSetIO_Object>
	bool save(const std::string& path, GetSetDictionary& dict = GetSetDictionary::global())
	{
		std::ofstream file(path.c_str());
		if (!file.good()) return false;
		GetSetIO_Object io(std::cin, file);
		dict.save(io);
		return true;
	}

	/// Loading a file
	template <typename GetSetIO_Object>
	bool load(const std::string& path, GetSetDictionary& dict = GetSetDictionary::global())
	{
		std::ifstream file(path.c_str());
		if (!file || !file.good()) return false;
		GetSetIO_Object io(file, std::cout);
		dict.load(io);	
		return true;
	}

	/// An ini-File in "[Section.Subsection] Key=Value" format
	class IniFile : public GetSetInternal::InputOutput
	{
	public:
		IniFile(std::istream&, std::ostream&);
	protected:
		virtual void write() const;
		virtual void read();
	};

	/// A simple text file with one property per line in "section/key=value" format
	class TxtFileKeyValue : public GetSetInternal::InputOutput
	{
	public:
		TxtFileKeyValue(std::istream&, std::ostream&);
	protected:
		virtual void write() const;
		virtual void read();
	};

	/// A text file with one property per line containing all information (Key, Value, Type and additional info) in attribute="value" format
	class TxtFileDescription : public GetSetInternal::InputOutput
	{
	public:
		TxtFileDescription(std::istream&, std::ostream&);
	protected:
		virtual void write() const;
		virtual void read();
	};

} // namespace GetSetIO

#endif // __GetSetFile_h
