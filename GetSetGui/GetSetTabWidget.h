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

#include "../GetSet/GetSetInternal.h"

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

	class GetSetTabWidget : public QWidget, public GetSetInternal::Dictionary::Observer
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
		GetSetInternal::Dictionary&			m_dict;
		std::string							m_path;
		std::map<std::string,QMenu*>		m_menus;
		std::map<std::string,QPushButton*>	m_push_buttons;

		void (*m_menu_callback)(const std::string& sender, const std::string& action);

	protected:
		QVBoxLayout					*m_mainLayout;
		QMenuBar					*m_menuBar;
		QTabWidget					*m_tabWidget;
		GetSetScriptRecorder		*m_script_recorder;

		/// (Re-)Create the tabs and GetSetWidgets
		void create(GetSetInternal::Section& section);
	public:
	
		/// Settings dialog with a selection of sections from a dictionary
		GetSetTabWidget(QWidget *parent=0x0, GetSetInternal::Section& section=GetSetGui::Section());

		/// Shortcut for example "Ctrl+O", If no action is supplied, a seperator will be added.
		QAction* addMenuItem(const std::string& menu, const std::string& action="-", const std::string& shortcut="");
		QPushButton* addButton(const std::string& action);
		void setMenuCallBack(void (*gui)(const std::string& sender, const std::string& action));

		void addDefaultFileMenu();
		std::string& aboutText();

		virtual ~GetSetTabWidget();

		// GetSetInternal::Dictionary::Observer
		virtual void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal);
	};

} // namespace GetSetGui

#endif // __GetSetTabWidget.h
