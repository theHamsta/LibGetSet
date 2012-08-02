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

#ifndef __StringUtil_hxx
#define __StringUtil_hxx

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

// Type general conversion to string
template <typename T> inline std::string toString(const T& in)
{
	std::ostringstream strstr;
	strstr << in;
	return strstr.str();
}

// General conversion from string to another type
template <typename T> inline T stringTo(const std::string& in)
{
	T value;
	std::istringstream strstr(in);
	strstr >> value;
	return value;
}

template <> inline std::string toString<>(const std::string& in) { return in; }
template <> inline std::string stringTo<>(const std::string& in) { return in; }
template <> inline std::string toString<>(const bool& in) { return in ? "true" : "false"; }
template <> inline bool stringTo<>(const std::string& in)
{
	if (in=="true" || in=="yes") return true;
	if (in=="false" || in=="no") return false;
	return stringTo<int>(in)>0;
}

// Conversion from vector of any type to string
template <typename T> inline std::string vectorToString(const std::vector<T>& in, const std::string& delim=" ")
{
	if (in.empty()) return std::string();
	std::vector<T>::const_iterator it=in.begin();
	std::string ret=toString(*it);
	for (++it;it!=in.end();++it)
		ret+=delim+toString(*it);
	return ret;
}

// Conversion of a string to a vector of any type
template <typename T> inline std::vector<T> stringToVector(const std::string& in, const char delim=' ')
{
	std::string item;
	std::vector<T> ret;
	std::istringstream str(in);
	for (;std::getline(str,item,delim);str&&!str.eof())
		ret.push_back(stringTo<T>(item));
	if (item.empty()) ret.pop_back();
	return ret;
}

// Specializations of toString and stringTo for select vector-types assuming seperating semicolon
#define STRING_UTIL_VECTOR_SPECIALIZATION(TYPE) \
	template <> inline std::string toString<>(const std::vector<TYPE>& in) {return vectorToString(in,";");} \
	template <> inline std::vector<TYPE> stringTo<>(const std::string& in) {return stringToVector<TYPE>(in,';');}

STRING_UTIL_VECTOR_SPECIALIZATION(bool)
STRING_UTIL_VECTOR_SPECIALIZATION(int)
STRING_UTIL_VECTOR_SPECIALIZATION(double)
STRING_UTIL_VECTOR_SPECIALIZATION(std::string)

/// Right trim
inline void rtrim(std::string &str , const std::string& t = " \t")
{
	str.erase(str.find_last_not_of(t)+1);
}

/// Left trim
inline void ltrim(std::string& str, const std::string& t = " \t")
{
	str.erase(0,str.find_first_not_of(t));
}

/// Trim
inline void trim(std::string& str, const std::string& t = " \t")
{
	ltrim(str,t);
	rtrim(str,t);
} 

/// Remove the part right of last occurence of delim and return it
inline std::string splitRight(std::string& str, const std::string& delim)
{
	std::string::size_type loc=str.find_last_of(delim);
	std::string right;
	if (loc!=std::string::npos)
	{
		right=str.substr(loc+1,std::string::npos);
		str=str.substr(0,loc); // left
	}
	else
	{
		right=str;
		str.clear();
	}
	return right;
}

/// Remove filename from an absolute path and return it
inline std::string splitNameFromPath(std::string& path)
{
	std::string name=splitRight(path,"/\\");
	if (path.empty()) path=".";
	return name;
}

/// Overload of string conversion for specific lengths
template <typename T> inline std::string toString(T in, int width, char fill='0')
{
	std::ostringstream strstr;
	strstr << std::setfill(fill) << std::setw(width) << in;
	return strstr.str();
}

/// Scan for files matching path pattern prefix###postfix, where ### is a number between 0 and max
std::vector<std::pair<int,std::string> > inline
	listFileNameEnum(std::string prefix, std::string postfix, int width, int minidx=0, int maxidx=0)
{
	if (maxidx==0)
	{
		maxidx=1;
		for (int i=0;i<width;i++) maxidx*=10;
		maxidx--;
	}
	std::vector<std::pair<int,std::string> > ret;
	for (int i=minidx;i<=maxidx;i++)
	{
		std::string filename=prefix+toString(i,width)+postfix;
		std::ifstream test(filename.c_str());
		if (test.is_open())
		{
			test.close();
			ret.push_back(std::pair<int,std::string>(i,filename));
		}
	}
	return ret;
}

#endif // __StringUtil_hxx
