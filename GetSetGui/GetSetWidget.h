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

#ifndef __GetSetWidget_h
#define __GetSetWidget_h

#include "../GetSet/GetSetDictionary.h"

#include <string>

#include <QtGui/QWidget>

class QFormLayout;

/// A QWidget based representation of a GetSetDictionary's section
class GetSetWidget : public QWidget, protected GetSetDictionary::Observer
{
	Q_OBJECT

protected:
	/// string identifying the GetSet section represented by this widget
	std::string		m_section;
	
	/// The list of properties
	QFormLayout*	m_layout;

	/// Keep track of QObjects created in this window.
	std::map<std::string,QWidget*> m_owned;

	/// suppresses slots (since there was no way to prevent an emission)
	bool m_expectChange;

	/// (re-)build widged contents
	void init();

	/// clean up
	void destroy();

private slots:
	void trigger();
	void selectFile();
	void selectFolder();
	void openSubSection();
	void editingFinished();
	void sliderMoved(int value);
	void setValue(int value);
	void setValue(const QString&);

public:
	GetSetWidget(GetSetDictionary& dict, const std::string& list, QWidget *parent=0x0);
	virtual ~GetSetWidget();

	/// Access to dictionary this section resides in
	GetSetDictionary& getDictionary();

	/// Close window on ESC key (if we have no parent)
	void keyPressEvent(QKeyEvent *e);

	// GetSetDictionary::Observer
	virtual void notifyCreate(const std::string& list, const std::string& key);
	virtual void notifyChange(const std::string& list, const std::string& key);
	virtual void notifyDestroy(const std::string& list, const std::string& key);
};

#endif // __GetSetWidget_h
