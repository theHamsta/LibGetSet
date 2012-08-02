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

#ifndef __GetSetSettingsWindow_h
#define __GetSetSettingsWindow_h

#include "GetSetWidget.h"

#include <string>

//#include <QtGui/QDialog>
#include <QtGui/QWidget>

class QTabWidget;
class QPushButton;
class QVBoxLayout;

class GetSetSettingsWindow : public QWidget
{
	Q_OBJECT

protected slots:
	void ctxMenu(const QPoint &pos);

protected:
	QTabWidget	*m_tabWidget;
	QPushButton	*m_ok;
	QVBoxLayout *m_mainLayout;

	/// (Re-)Create the tabs and GetSetWidgets
	void create(GetSetDictionary& dict, const std::vector<std::string>& tabs);

public:
	/// Settings dialog with a selection of sections from a dictionary
	GetSetSettingsWindow(GetSetDictionary& dict, const std::vector<std::string>& tabs=std::vector<std::string>(), const std::string& title="Settings", QWidget *parent=0x0);

	/// Settings dialog with a selection of sections from the global dictionary
	GetSetSettingsWindow(const std::vector<std::string>& tabs=std::vector<std::string>(), const std::string& title="Settings", QWidget *parent=0x0);

	virtual ~GetSetSettingsWindow();
};

#endif // __GetSetSettingsWindow.h
