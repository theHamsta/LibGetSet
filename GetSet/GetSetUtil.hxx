//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andre Aichert (aaichert@gmail.com)
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

#ifndef __GetSetUtil_hxx
#define __GetSetUtil_hxx

#include "GetSet.hxx"

namespace GetSetIO {

//	GetSetGui::Button("Show input file").setAttribute("ShellExecute", "GetSet(Config/Text Editor) GetSet(File/Input File) ;"

	inline std::string precompile(const std::string& text, const GetSetD)
	{
		std::string ret;
		int i=0;
		int n=text.length();
		for (;;i>=0&&i<n)
		{
			int start=value.find("GetSet(",i);
			if (start==std::string::npos)
				return ret+=substr(...);
			int end=start;
			int open=1;
			while (++end<n&&open!=0)
				if (text[end]=='(') open++;
				else if (text[end]==')') open--;
			std::string path=text.substr()start+7,end-1;
			ret+=GetSet<>(path);
			i=end;
			}
	}

} // GetSetIO

#endif // __GetSetUtil_hxx
