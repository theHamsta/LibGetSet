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

#include "../GetSet/GetSetObjects.h"

#include "../GetSet/GetSetCmdLine.hxx"

#include "../Getset/ProgressInterface.hxx"

class QApplication;
class GetSetLog;

namespace GetSetGui
{
	class GetSetTabWidget;
	class GetSetProgressWindow;

	/// The "default" application with one settings window.
	class Application : public ProgressInterface
	{
	protected:
		GetSetInternal::Dictionary&	dict;
		GetSetIO::CmdLineParser		cmd;
		QApplication				*qt_app;
		GetSetHandler				*callback;
		GetSetTabWidget				*main_window;
		GetSetProgressWindow		*progress_window;
		GetSetLog					*log;

	public:

		/// Running a stand-alone application from a GetSet Object.
		template <class GetSetObject>
		static int exec() {
			using namespace std::placeholders;
			std::string type_name=typeName<GetSetObject>();
			int argc=1;
			char *argv[]={&type_name[0],0x0};
			Application  app(type_name);
			GetSetObject obj(GetSetGui::Section(type_name),&app);
			obj.gui_init();
			app.init(argc,argv,std::bind(&Application::saveSettings, &app));
			return app.exec();
		}

		Application(std::string _appname, GetSetInternal::Dictionary& _dict = GetSetInternal::Dictionary::global());
		~Application();

		/// Define how to handle command line arguments
		GetSetIO::CmdLineParser& commandLine();

		//
		// main(...) should call init(...) and then exec().
		//

		/// Run application (shows gui and enters Qt's event loop)
		int exec();

		/// Parse command line and load settings. Must be called before exec!
		bool init(int &argc, char **argv, std::function<void(const GetSetInternal::Node&)> gui);

		/// Access underlying dictionary
		const GetSetInternal::Dictionary& dictionary() {return dict;}

		/// Access to main window
		GetSetTabWidget& window();
		
		//
		// Showing model progress bar
		//

		/// Access to progress window
		GetSetProgressWindow& progress();

		/// Show progress bar and hide main window (optionally includes a "cancel" button)
		virtual void progressStart(const std::string& progress, const std::string& info, int maximum=100, bool *cancel_clicked=0x0);

		/// Show progress bar and hide main window
		virtual void progressUpdate(int i);

		/// Hide progress bar and show main window
		virtual void progressEnd();

		//
		// Informative modal dialog
		//

		/// Inform the user of something important. (optional: show modal dialog)
		virtual void info(const std::string& who, const std::string& what, bool show_dialog=false);

		/// Inform the user of a problem. (optional: show modal dialog)
		virtual void warn(const std::string& who, const std::string& what, bool only_inormative=true);

		//
		// Ini-File and Scripting
		//

		/// Save ini-file
		void saveSettings() const;

		/// Load ini-file
		void loadSettings();

		/// Parse a GetSet script (see GetSetScriptParser)
		bool parseScript(const std::string& script);

		//
		// Silently alter the applications state using ignoreNotifications(true);
		//

		/// Ignores notifications temporarily
		void ignoreNotifications(bool ignore);

	};

} // namespace GetSetGui

#endif // __GetSetGui_h
