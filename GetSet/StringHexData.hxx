//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andr� Aichert (aaichert@gmail.com)
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

#ifndef __string_hex_data_hxx
#define __string_hex_data_hxx

#include "StringConversion.hxx"
#include "StringType.hxx"

/// A special type which can be used to store binary data as string
class HexData : public std::vector<unsigned char> {
public:

	template <typename T=unsigned char>
	T * raw() { return (T*)&this->front(); }

	template <typename T=unsigned char>
	const T * raw() const { return (const T*)&this->front(); }

	template <typename T> T* copy_array(T* data, int N)
	{
		resize(N*sizeof(T));
		T* this_data=(T*)raw();
		for (int i=0;i<N;i++)
			this_data[i]=data[i];
		return this_data;
	}

	template <typename T> T& copy(const T& object)
	{
		resize(sizeof(T));
		T* this_data=(T*)raw();
		*this_data=object;
		return *this_data;
	}

	template <typename T> T& cast() { return *((T*)raw()); }

	template <typename T> const T& cast() const { return *((const T*)raw()); }

};

template<> inline std::string typeName<HexData>() {return "HexData";}

template <> inline std::string toString<>(const HexData& in)
{
	char const hex[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	std::string ret;
	ret.resize(in.size()*2);
	for (int i=0;i<(int)in.size();i++)
	{
		char c=in[(int)in.size()-i-1];
		ret[i*2+0]=hex[(c&0xF0)>>4];
		ret[i*2+1]=hex[(c&0x0F)>>0];
	}
	return ret;
}

template <> inline HexData stringTo<>(const std::string& in)
{
	int N=in.size()/2;
	HexData ret;
	ret.resize(N);
	char *raw=ret.raw<char>();
	for (int i=0;i<N;i++)
	{
		raw[N-i-1]=0;
		const char& hi(in[2*i+0]);
		const char& lo(in[2*i+1]);
		if (hi>='0'||hi<='9')      raw[i]+=(hi-'0')>>4;
		else if (hi>='A'||hi<='F') raw[i]+=(hi-'9')>>4;
		else return HexData();
		if (lo>='0'||lo<='9')      raw[i]+=(lo-'0');
		else if (lo>='A'||lo<='F') raw[i]+=(lo-'9');
		else return HexData();
	}

	return ret;
}

#endif // __string_hex_data_hxx
