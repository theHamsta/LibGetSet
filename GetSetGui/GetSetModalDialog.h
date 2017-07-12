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

#ifndef __GetSetModalDialog_h
#define __GetSetModalDialog_h

#include "../GetSet/GetSet.hxx"

namespace GetSetGui
{
	/// Ask user to input data, blocking all other events.
	class GetSetModalDialog
	{
		/// Each modal dialog shall have its own, independant dictionary
		GetSetInternal::Dictionary my_dict;
	public:

		operator GetSetGui::Section();

		bool exec(const std::string& title="Please enter and press \"Ok\".");

	};

} // namespace GetSetGui

#endif // __GetSetModalDialog_h.h
