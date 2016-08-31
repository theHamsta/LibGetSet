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

#ifndef __GetSetGui_h
#define __GetSetGui_h

#include "GetSetSettingsWindow.h"
#include "../GetSet/GetSetScripting.h"
#include <QApplication>

namespace GetSetGui
{
	class GetSetApplication
	{
	protected:
		const std::string appname;
		QApplication  qt_app;
		GetSetHandler callback;
		GetSetSettingsWindow *main_window;
		GetSetScriptParser parser;
	public:
			
		GetSetApplication(std::string _appname, void (*gui)(const std::string&, const std::string&), int argc, char **argv);

		void showSettings();

		void saveSettings() const;

		void loadSettings();

		void setIgnoreNotifications(bool ignore);

		bool parse(const std::string& script);

		int exec();

	};

} // namespace GetSetGui

#endif // __GetSetGui_h
