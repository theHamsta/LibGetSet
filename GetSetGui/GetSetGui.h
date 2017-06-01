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

#include "../GetSet/GetSetCmdLine.hxx"

#include "ProgressInterface.hxx"

class QApplication;
class GetSetLog;

namespace GetSetGui
{
	class GetSetTabWidget;
	class GetSetProgressWindow;


	class GetSetApplication : public ProgressInterface
	{
	protected:
		GetSetDictionary&		dict;
		GetSetIO::CmdLineParser	cmd;
		QApplication			*qt_app;
		GetSetHandler			*callback;
		GetSetTabWidget			*main_window;
		GetSetProgressWindow	*progress_window;
		GetSetLog				*log;

	public:
		GetSetApplication(std::string _appname, GetSetDictionary& _dict = GetSetDictionary::global());
		~GetSetApplication();

		/// Define how to handle command line arguments
		GetSetIO::CmdLineParser& commandLine();

		/// Parse command line and load settings. Must be called before exec!
		bool init(int &argc, char **argv, void (*gui)(const std::string&, const std::string&));

		/// Ignores notifications temporarily
		void ignoreNotifications(bool ignore);
		
		/// Access to main window
		GetSetTabWidget& window();
		
		/// Access to progress window
		GetSetProgressWindow& progress();

		/// 
		operator GetSetDictionary&() {return dict;}

		/// Show progress bar and hide main window (optionally includes a "cancel" button)
		virtual void progressStart(const std::string& progress, const std::string& info, int maximum=100, bool *cancel_clicked=0x0);

		/// Show progress bar and hide main window
		virtual void progressUpdate(int i);

		/// Hide progress bar and show main window
		virtual void progressEnd();

		/// Inform the user of something important. (optional: show modal dialog)
		virtual void info(const std::string& who, const std::string& what, bool show_dialog=false);

		/// Inform the user of a problem. (optional: show modal dialog)
		virtual void warn(const std::string& who, const std::string& what, bool only_inormative=true);

		//
		//
		//

		/// Save ini-file
		void saveSettings() const;

		/// Load ini-file
		void loadSettings();

		/// Parse a GetSet script (see GetSetScriptParser)
		bool parseScript(const std::string& script);

		/// Run application (shows gui and enters Qt's event loop)
		int exec();

	};

} // namespace GetSetGui

#endif // __GetSetGui_h
