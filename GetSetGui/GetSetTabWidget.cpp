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

#include "GetSetTabWidget.h"
#include "GetSetScriptEdit.h"
#include "../GetSet/GetSetScripting.h"

#include "../GetSet/GetSet.hxx"

#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStyle>

#include <QApplication>

// Minor leaks. FIXME

/// Same as GetSetHandler, but keeps track of state changes
class GetSetScriptRecorder : public GetSetDictionary::Observer
{
public:
	GetSetScriptRecorder(GetSetDictionary& subject)
		: GetSetDictionary::Observer(subject) {}
	std::string log;

protected:
	virtual void notifyChange(const std::string& section, const std::string& key)
	{
		std::string path=section+"/"+key;
		if (GetSet<>(path).getType()=="Button")
			log=log+"set trigger \""+ path + "\"\n";
		else
			log=log+"set key \""+ path + "\" to value \"" + GetSet<>(path).getString() + "\"\n"; 
	}

};

namespace GetSetGui
{

	void GetSetTabWidget::create(GetSetDictionary& dict, const std::string & path, const std::vector<std::string>& tabs)
	{
		if (tabs.empty())
		{
			std::vector<std::string> all;
			GetSetInternal::Section * section=dynamic_cast<GetSetInternal::Section *>(Access::getProperty(path));
			if (section)
			{
				for (GetSetInternal::Section::PropertyByName::const_iterator it=section->getSection().begin();it!=section->getSection().end();++it)
					if (dynamic_cast<GetSetInternal::Section *>(it->second))
						all.push_back(it->first);
				if (!all.empty())
				{
					create(dict,path,all);
					m_tabs.clear();
					return;
				}
			}
			else return;
		}

		m_path=path;
		m_tabs=tabs;

		if (!m_mainLayout)
		{
			m_mainLayout = new QVBoxLayout;
			setLayout(m_mainLayout);
		}

		if (m_tabWidget)
		{
			int n=m_tabWidget->count();
			for (int i=0;i<n;i++)
				delete m_tabWidget->widget(0);
			m_tabWidget->clear();
		}
		else m_tabWidget = new QTabWidget(this);
		for (int i=0;i<(int)tabs.size(); i++)
		{
			GetSetWidget* tab=new GetSetWidget(GetSetSection(path,dict).subsection(tabs[i]),m_tabWidget);
			tab->setObjectName(tabs[i].c_str());
			m_tabWidget->addTab(tab,tabs[i].c_str());
		}
		m_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(m_tabWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));

		m_mainLayout->insertWidget(0,m_tabWidget);

		setWindowIcon(style()->standardIcon(QStyle::SP_TitleBarMenuButton));
	}

	GetSetTabWidget::GetSetTabWidget(QWidget *parent, GetSetDictionary& _dict)
		: QWidget(parent)
		, GetSetDictionary::Observer(_dict)
		, m_menuBar(0x0)
		, m_mainLayout(0x0)
		, m_tabWidget(0x0)
		, m_script_recorder(0x0)
	{
		setWindowTitle("Settings");
		create(_dict,"",std::vector<std::string>());
	}

	GetSetTabWidget::GetSetTabWidget(const std::string& path, GetSetDictionary& dict ,const std::string& title, const std::string& listOfTabs, QWidget *parent)
		: QWidget(parent)
		, GetSetDictionary::Observer(dict)
		, m_menuBar(0x0)
		, m_mainLayout(0x0)
		, m_tabWidget(0x0)
		, m_script_recorder(0x0)
	{
		setWindowTitle(title.c_str());
		std::vector<std::string> tabs=stringToVector<std::string>(listOfTabs,';');
		create(dict,path,tabs);
	}

	void GetSetTabWidget::setCallBack(void (*gui)(const std::string& sender, const std::string& action))
	{
		callback=gui;
	}

	QAction* GetSetTabWidget::addMenuItem(const std::string& menu, const std::string& action, const std::string& shortcut)
	{
		if (!m_menuBar)
		{
			m_menuBar=new QMenuBar();
			m_mainLayout->setMenuBar(m_menuBar);
		}
		// First create menu structure
		if (m_menus.find(menu)==m_menus.end())
		{
			std::vector<std::string> submenus=stringToVector<std::string>(menu,'/');
			std::string this_menu=submenus.back();
			submenus.pop_back();
			m_menus[menu]=new QMenu(this_menu.c_str(),this);
			if (submenus.empty())
			{
				m_menuBar->addMenu(m_menus[menu]);
			}
			else
			{
				std::string super_menu=vectorToString(submenus,"/");
				if (m_menus.find(super_menu)==m_menus.end())
					addMenuItem(super_menu,"");
				m_menus[super_menu]->addMenu(m_menus[menu]);
			}
		}
		// Then create menu item
		if (action=="-")
			m_menus[menu]->addSeparator();
		else if (!action.empty())
		{
			QAction* item=m_menus[menu]->addAction(action.c_str(), this, SLOT(handle_action()), QKeySequence(shortcut.c_str()));
			item->setObjectName(action.c_str());
			return item;
		}
		return 0x0;
	}

	QPushButton* GetSetTabWidget::addButton(const std::string& action)
	{
		if (m_push_buttons.find(action)==m_push_buttons.end())
		{
			QPushButton *pb=new QPushButton(action.c_str(),this);
			pb->setObjectName(action.c_str());
			connect(pb, SIGNAL(clicked()), this, SLOT(handle_action()) );
			m_mainLayout->addWidget(pb);
		}
		return m_push_buttons[action];
	}

	void GetSetTabWidget::addDefaultFileMenu()
	{
		// File menu
		addMenuItem("File","");
		m_menus["File"]->addAction(tr("&About"), this, SLOT(about()));
		auto *scripting_menu=m_menus["File"]->addMenu("Scripting");
		m_menus["File"]->addSeparator();
		m_menus["File"]->addAction(tr("&Quit"), QApplication::instance(), SLOT(quit()), QKeySequence::Quit);
		// Scripting menu
		scripting_menu->addAction(tr("(Re-)Start Recording"), this, SLOT(rec_start()) );
		scripting_menu->addAction(tr("Stop Recording"), this, SLOT(rec_stop()) );
		scripting_menu->addSeparator();
		scripting_menu->addAction(tr("Run default script."), this, SLOT(script_run_default()), QKeySequence(Qt::CTRL + Qt::SHIFT+ Qt::Key_D));
		scripting_menu->addAction(tr("Show Script &Editor..."), this, SLOT(script_editor()), QKeySequence(Qt::CTRL + Qt::Key_E));
	}

	void GetSetTabWidget::handle_action()
	{
		std::string who=windowTitle().toStdString();
		std::string what=sender()->objectName().toStdString();
		if (callback) callback(who,what);

	}

	void GetSetTabWidget::about()
	{
		QMessageBox::about( this, GetSet<>("Application").getString().c_str(), aboutText().c_str());
	}

	std::string& GetSetTabWidget::aboutText() 
	{
		if (m_about.empty())
			m_about=
				"<h4>Load/saving *typed* and *named* properties and automatic GUI</h4>\n\n"
				"Copyright 2011-2016 by <a href=\"mailto:aaichert@gmail.com?Subject=GetSet\">Andre Aichert</a> <br><br> See slso: "
				"<a href=\"https://sourceforge.net/projects/getset/\">SourceForge Project Page</a> <br><br>"
				"<h4>Licensed under the Apache License, Version 2.0 (the \"License\")</h4>\n\n"
				"You may not use this file except in compliance with the License. You may obtain a copy of the License at "
				"<a href=\"http://www.apache.org/licenses/LICENSE-2.0\">http://www.apache.org/licenses/LICENSE-2.0</a><br>"
				;
		return m_about;
	}

	void GetSetTabWidget::rec_start()
	{
		if (m_script_recorder) delete m_script_recorder;
		m_script_recorder=new GetSetScriptRecorder(dictionary);
	}

	void GetSetTabWidget::rec_stop()
	{
		if (!m_script_recorder) return;
		GetSetScriptEdit *script_editor=new GetSetScriptEdit(this,dictionary);
		script_editor->setAttribute(Qt::WA_DeleteOnClose);
		script_editor->setText(m_script_recorder->log.c_str());
		script_editor->show();
		delete m_script_recorder;
		m_script_recorder=0x0;
	}

	void GetSetTabWidget::script_editor()
	{
		std::string appname=GetSet<>("Application");
		GetSetScriptEdit *script_editor=new GetSetScriptEdit();
		script_editor->setAttribute(Qt::WA_DeleteOnClose);
		script_editor->openFile((appname+".getset").c_str());
		script_editor->show();
	}
	
	void GetSetTabWidget::script_run_default()
	{
		std::string appname=GetSet<>("Application");
		std::cout << "Running script " << appname << ".getset" << std::endl;
		GetSetScriptParser::global().parse(std::string("file run ")+appname+".getset");
	}

	GetSetTabWidget::~GetSetTabWidget()
	{
		delete m_mainLayout;
	}

	void GetSetTabWidget::ctxMenu(const QPoint &pos)
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
				GetSetWidget* w=new GetSetWidget(GetSetSection(section, out->getDictionary()), this);
				w->setAttribute(Qt::WA_DeleteOnClose, true);
				w->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
				w->show();
			}
		}
	}

	void GetSetTabWidget::notifyCreate(const std::string& list, const std::string& key)
	{
		if (m_tabs.empty()) // only if this window shows *all* tabs, we have to make sure that a new one was not created
			create(dictionary,m_path,m_tabs);
	}

	void GetSetTabWidget::notifyDestroy(const std::string& list, const std::string& key)
	{
		if (m_tabs.empty()) // only if this window shows *all* tabs, we have to make sure that a new one was not created
			create(dictionary,m_path,m_tabs);
	}

	void GetSetTabWidget::notifyChange(const std::string &,const std::string &) {} // ignore

} // namespace GetSetGui
