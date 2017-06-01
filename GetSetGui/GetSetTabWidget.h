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

#ifndef __GetSetTabWidget_h
#define __GetSetTabWidget_h

#include "../GetSet/GetSetDictionary.h"

#include "GetSetWidget.h"

#include <string>

class QVBoxLayout;
class QMenu;
class QMenuBar;
class QTabWidget;
class QPushButton;

class GetSetScriptRecorder;

namespace GetSetGui
{
	class GetSetScriptEdit;

	class GetSetTabWidget : public QWidget, public GetSetDictionary::Observer
	{
		Q_OBJECT

	protected slots:
		void ctxMenu(const QPoint &pos);
		void handle_action();
		void about();
		void rec_start();
		void rec_stop();
		void script_editor();
		void script_run_default();


	protected:
		std::string							m_about;
		std::string							m_path;
		std::vector<std::string>			m_tabs;
		std::map<std::string,QMenu*>		m_menus;
		std::map<std::string,QPushButton*>	m_push_buttons;

		void (*callback)(const std::string& sender, const std::string& action);

	protected:
		QVBoxLayout					*m_mainLayout;
		QMenuBar					*m_menuBar;
		QTabWidget					*m_tabWidget;
		GetSetScriptRecorder		*m_script_recorder;

		/// (Re-)Create the tabs and GetSetWidgets
		void create(GetSetDictionary& dict, const std::string& path, const std::vector<std::string>& tabs);
	public:
	
		/// Settings dialog with a selection of sections from a dictionary
		GetSetTabWidget(QWidget *parent, GetSetDictionary& _dict = GetSetDictionary::global());

		/// Settings dialog with a selection of sections from a dictionary
		GetSetTabWidget(const std::string& path="", GetSetDictionary& dict = GetSetDictionary::global() ,const std::string& title="Settings", const std::string& listOfTabs="", QWidget *parent=0x0);

		void setCallBack(void (*gui)(const std::string& sender, const std::string& action));

		/// Shortcut for example "Ctrl+O", If no action is supplied, a seperator will be added.
		QAction* addMenuItem(const std::string& menu, const std::string& action="-", const std::string& shortcut="");
		QPushButton* addButton(const std::string& action);

		void addDefaultFileMenu();
		std::string& aboutText();

		virtual ~GetSetTabWidget();

		// GetSetDictionary::Observer
		virtual void notifyCreate(const std::string& list, const std::string& key);
		virtual void notifyDestroy(const std::string& list, const std::string& key);
		virtual void notifyChange(const std::string &,const std::string &);
	};

} // namespace GetSetGui

#endif // __GetSetTabWidget.h
