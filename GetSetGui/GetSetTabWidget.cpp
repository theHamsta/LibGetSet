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
class GetSetScriptRecorder : public GetSetInternal::Dictionary::Observer
{
public:
	GetSetScriptRecorder(GetSetInternal::Dictionary& subject)
		: GetSetInternal::Dictionary::Observer(subject) {}
	std::string log;

protected:
	virtual void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
	{
		if (signal==GetSetInternal::Dictionary::Signal::Change)
		if (node.getType()=="Button")
			log=log+"set trigger \""+ node.path() + "\"\n";
		else
			log=log+"set key \""+ node.path() + "\" to value \"" + node.getString() + "\"\n"; 
	}

};

namespace GetSetGui
{

	void GetSetTabWidget::create(GetSetInternal::Section& section)
	{
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
		auto children=section.getChildren();
		for (auto it=children.begin();it!=children.end();++it)
		{
			// Is this a section and is it even visible?
			if (!it->second
				|| it->second->getType()!="Section"
				|| it->second->getAttribute<bool>("Hidden"))
				continue;

			GetSetWidget* tab=new GetSetWidget(GetSetGui::Section(it->second->path(), it->second->dictionary),m_tabWidget);
			tab->setObjectName(it->first.c_str());
			m_tabWidget->addTab(tab,it->first.c_str());
		}
		m_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(m_tabWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));

		m_mainLayout->insertWidget(0,m_tabWidget);

		setWindowIcon(style()->standardIcon(QStyle::SP_TitleBarMenuButton));
	}

	GetSetTabWidget::GetSetTabWidget(QWidget *parent, GetSetInternal::Section& section)
		: QWidget(parent)
		, GetSetInternal::Dictionary::Observer(section.dictionary)
		, m_dict(section.dictionary)
		, m_path(section.path())
		, m_menuBar(0x0)
		, m_mainLayout(0x0)
		, m_tabWidget(0x0)
		, m_script_recorder(0x0)
		, m_menu_callback(0x0)
	{
		setWindowTitle(section.name.empty()?"Settings":section.name.c_str());
		create(section);
	}

	void GetSetTabWidget::setMenuCallBack(void (*gui)(const std::string& sender, const std::string& action))
	{
		m_menu_callback=gui;
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
		if (m_menu_callback) m_menu_callback(who,what);
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
		m_script_recorder=new GetSetScriptRecorder(m_dict);
	}

	void GetSetTabWidget::rec_stop()
	{
		if (!m_script_recorder) return;
		GetSetScriptEdit *script_editor=new GetSetScriptEdit(this,m_dict);
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
				GetSetWidget* w=new GetSetWidget(out->getSection(), this);
				w->setAttribute(Qt::WA_DeleteOnClose, true);
				w->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
				w->show();
			}
		}
	}

	void GetSetTabWidget::notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
	{
		if (signal==GetSetInternal::Dictionary::Signal::Change) return;
		if (node.super_section!=m_path) return;
		if (dynamic_cast<const GetSetInternal::Section*>(&node)==0x0) return;
		// Update our tabs.
		create(GetSetGui::Section(node.path(),node.dictionary));
	}


} // namespace GetSetGui
