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

#include "GetSetInternal.h"

namespace GetSetInternal
{
	/// Intermediate interface for storage and retreival of values ONLY (no types). Subclasses MUST call save in destructor!
	class GetSetInOutValuesOnly : public GetSetInOut
	{
	public:
		GetSetInOutValuesOnly(const std::string file);

		virtual void store(const std::string& section, const std::string& key, GetSetInternal::GetSetNode* value);
		virtual std::string retreive(const std::string& section, const std::string& key);
		virtual void retreiveAll(GetSetDictionary& dictionary);
	};

	class GetSetInOutFullDescription : public GetSetInOut {
	public:
		GetSetInOutFullDescription(const std::string file);

		virtual void store(const std::string& section, const std::string& key, GetSetInternal::GetSetNode* value);
		virtual std::string retreive(const std::string& section, const std::string& key);
		virtual void retreiveAll(GetSetDictionary& dictionary);
	};

} // namespace GetSetInternal

namespace GetSetIO {

	/// Saving a file
	void save(GetSetInternal::GetSetInOut& file, GetSetDictionary& dict=GetSetDictionary::global())
	{
		dict.save(file);
	}

	/// Loading a file
	void load(GetSetInternal::GetSetInOut& file, GetSetDictionary& dict=GetSetDictionary::global())
	{
		dict.load(file);
	}

	/// An ini-File in "[Section.Subsection] Key=Value" format
	class IniFile : public GetSetInternal::GetSetInOutValuesOnly
	{
	public:
		IniFile(const std::string& file);
		virtual ~IniFile();

	protected:
		virtual void save() const;
		virtual void load();
	};

	/// A simple text file with one property per line in "section/key=value" format
	class TxtFileKeyValue : public GetSetInternal::GetSetInOutValuesOnly
	{
	public:
		TxtFileKeyValue(const std::string file);
		virtual ~TxtFileKeyValue();

	protected:
		virtual void save() const;
		virtual void load();
	};

	/// A text file with one property per line containing all information (Key, Value, Type and additional info) in attribute="value" format
	class TxtFileDescription : public GetSetInternal::GetSetInOutFullDescription
	{
	public:
		TxtFileDescription(const std::string file);
		virtual ~TxtFileDescription();

	protected:
		virtual void save() const;
		virtual void load();
	};

} // namespace GetSetIO

#endif // __GetSetFile_h
