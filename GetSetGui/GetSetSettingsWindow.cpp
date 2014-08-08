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

#include "GetSetSettingsWindow.h"

#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenu>

// Minor leaks. FIXME
void GetSetSettingsWindow::create(GetSetDictionary& dict, const std::vector<std::string>& tabs)
{
	m_tabWidget = new QTabWidget;
	for (int i=0;i<(int)tabs.size(); i++)
	{
		GetSetWidget* tab=new GetSetWidget(dict,tabs[i]);
		tab->setObjectName(tabs[i].c_str());
		m_tabWidget->addTab(tab,tabs[i].c_str());
	}
	m_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_tabWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));

	m_mainLayout = new QVBoxLayout;
	m_mainLayout->addWidget(m_tabWidget);

	setLayout(m_mainLayout);

}

GetSetSettingsWindow::GetSetSettingsWindow(const std::string& path, GetSetDictionary& dict ,const std::string& title, const std::string& listOfTabs, QWidget *parent)
	: QDialog(parent)
	, Access(dict)
{
	setWindowTitle(title.c_str());
	std::vector<std::string> tabs=stringToVector<std::string>(listOfTabs,';');

	if (tabs.empty())
	{
		std::vector<std::string> all;
		GetSetInternal::GetSetSection * section=dynamic_cast<GetSetInternal::GetSetSection *>(Access::getProperty(path));
		if (section)
		{
			for (GetSetInternal::GetSetSection::PropertyByName::const_iterator it=section->getSection().begin();it!=section->getSection().end();++it)
				if (dynamic_cast<GetSetInternal::GetSetSection *>(it->second))
					all.push_back(it->first);
			create(dict,all);
		}
	}
	else
		create(dict,tabs);

	setMinimumWidth(300);
	// fixme... correct size?
}

QPushButton* GetSetSettingsWindow::setButton(const std::string& name, void (*clicked)(const std::string& windowTitle,const std::string& buttonName))
{
	// Find a button that matches name
	for (std::map<QPushButton*, void (*)(const std::string&,const std::string&)>::iterator it=m_buttons.begin(); it!=m_buttons.end();++it)
	{
		std::string n=it->first->objectName().toLatin1();
		if (n==name)
		{
			if (!clicked)
			{
				delete it->first;
				m_buttons.erase(it);
				return 0x0;
			}
			else
				it->second=clicked;
			return it->first;
		}
	}
	// Button does not yet exist
	QPushButton *pb=new QPushButton(name.c_str(),this);
	pb->setObjectName(name.c_str());
	connect(pb, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	m_mainLayout->addWidget(pb);
	m_buttons[pb]=clicked;
	return pb;
}

GetSetSettingsWindow::~GetSetSettingsWindow()
{
	delete m_mainLayout;
}

void GetSetSettingsWindow::ctxMenu(const QPoint &pos)
{
	QMenu menu;
	menu.addAction("Pop out");
	QAction* selectedItem = menu.exec(this->mapToGlobal(pos));
	if (selectedItem)
	{
		GetSetWidget* out=dynamic_cast<GetSetWidget*>(m_tabWidget->currentWidget());
		if (out)
		{
			std::string section=out->objectName().toLatin1().data();
			GetSetWidget* w=new GetSetWidget(out->getDictionary(), section, this);
			w->setAttribute(Qt::WA_DeleteOnClose, true);
			w->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
			w->show();
		}
	}
}

void GetSetSettingsWindow::buttonClicked()
{
	std::string button=sender()->objectName().toLatin1();
	std::string window=windowTitle().toLatin1();
	for (std::map<QPushButton*, void (*)(const std::string&,const std::string&)>::iterator it=m_buttons.begin(); it!=m_buttons.end();++it)
	{
		std::string name=it->first->objectName().toLatin1();
		if (button==name)
			it->second(window,name);
	}
}
