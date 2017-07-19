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

#include "GetSetWidget.h"

#include <QApplication>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QGroupBox>
#include <QStyle>

#include "../GetSet/GetSet.hxx"

#include <iostream>

#define GETSET_GUI_IGNORE_SIGNAL_CHANGE(CMD) m_expectChange=true; CMD; m_expectChange=false; 

namespace GetSetGui {

	void GetSetWidget::trigger()
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetGui::Button(key,getSection()).trigger();
	}

	void GetSetWidget::selectFile()
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetGui::File file(key,getSection());
		std::string path;
		if (file.getMultiple())
		{
			std::vector<std::string> files=stringToVector<std::string>(file,';');
			if (!files.empty())
				path=files[0];
		}
		else
			path=file;
		QString currentDir=path.c_str();
		if (!path.empty())
			currentDir=QFileInfo(QFile(currentDir)).absoluteDir().absolutePath().toLatin1();
	//	QString extensions=vectorToString(file.getExtensions(),";;").c_str();
		QString extensions=file.getExtensions().c_str();
		if (file.getMultiple())
		{
			QStringList files = QFileDialog::getOpenFileNames(this, "Select Files", currentDir, extensions);
			if (!files.empty())
				path=files[0].toLatin1().data();
			for (int i=1;i<(int)files.size();i++)
				path+=std::string(";")+files[i].toLatin1().data();
		}
		else
		{
			if (file.getCreateNew())
				path=QFileDialog::getSaveFileName(this, "Select A File", currentDir , extensions).toStdString();
			else
				path=QFileDialog::getOpenFileName(this, "Select A File", currentDir, extensions).toStdString();
		}

		if (!path.empty())
			file=path;
	}

	void GetSetWidget::selectFolder()
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetGui::Directory folder(key,getSection());
		QString path = QFileDialog::getExistingDirectory(this, "Select A Directory",folder.getString().c_str());
		if ( path.isNull())
			return;
		folder=path.toLatin1().data();
	}

	void GetSetWidget::collapseGroupedSection()
	{
		std::string key=sender()->objectName().toLatin1().data();
		if (m_owned.find(key)==m_owned.end()) return;
		QPushButton* l=dynamic_cast<QPushButton*>(sender());
		bool is_visible=!m_owned[key]->isVisible();
		if (is_visible)
		{
			m_owned[key]->setVisible(true);
			l->setText((std::string("- ")+key).c_str());
		}
		else
		{
			m_owned[key]->setVisible(false);
			l->setText((std::string("+ ")+key).c_str());
		}
	}

	void GetSetWidget::openSubSection()
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetWidget* w=new GetSetWidget(GetSetGui::Section(key,getSection()));
		w->setAttribute(Qt::WA_DeleteOnClose, true);
		w->setWindowFlags(Qt::Tool);
		w->show();
	}

	void GetSetWidget::editingFinished()
	{
		std::string key=sender()->objectName().toLatin1().data();
		QLineEdit* l=dynamic_cast<QLineEdit*>(sender());
		if (!l) return;
		std::string value=l->text().toStdString();
		GetSet<std::string> property(key,getSection());
		if (property.getString()!=value)
			property=value;
	}

	void GetSetWidget::sliderMoved(int value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		double	d=(double)value/1000.;
		GETSET_GUI_IGNORE_SIGNAL_CHANGE(GetSet<double>(key,getSection())=d;);
	}

	void GetSetWidget::setValue(int value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		GETSET_GUI_IGNORE_SIGNAL_CHANGE(GetSet<int>(key,getSection())=value);
	}

	void GetSetWidget::setRangeValue(double value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetGui::RangedDouble item(key,getSection());
		double minv=item.getMin();
		double maxv=item.getMax();
		double step=item.getStep();
		if (step<=0) step=0.01;
		if (item.getPeriodic())
		{
			if (value>=maxv)
				value=minv;
			if (value<minv)
				value=maxv-step;
		}
		if (item.getValue()!=value)
			item.setValue(value);
	}

	void GetSetWidget::setRangeValue(int value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetInternal::Node* node=getSection().nodeAt(key);
		if (!node) return;
		if (node->getType()=="RangedInt")
		{
			RangedInt item(key,getSection());
			int minv=item.getMin();
			int maxv=item.getMax();
			if (item.getPeriodic())
			{
				if (value>maxv) value=minv;
				if (value<minv) value=maxv;
			}
			else
			{
				if (value>maxv) value=maxv;
				if (value<minv) value=minv;
			}
			if (item.getValue()!=value)
				item.setValue(value);
		}
		else
		{
			GETSET_GUI_IGNORE_SIGNAL_CHANGE(GetSet<int>(key,getSection())=value;);
		}
	}

	void GetSetWidget::setValue(const QString& value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSet<std::string>(key,getSection())=value.toLatin1().data();
	}

	void GetSetWidget::init()
	{
		setWindowTitle(m_section.c_str());
		setFrameShape(QFrame::NoFrame);

		m_content=new QWidget(this);
		m_layout = new QFormLayout();
		m_content->setLayout(m_layout);

		GetSetInternal::Section &inernal_section=getSection();
		auto contents=inernal_section.getChildren();
		for (auto it=contents.begin();it!=contents.end();++it)
		{
			GetSetInternal::Node *node=getSection().nodeAt(it->first);
			if (!node) continue;
			this->notifyCreate(*node);
			this->notifyChange(*node);
		}

		setWidget(m_content);
		setWidgetResizable(true);
		setMinimumHeight(sizeHint().height());
		setMinimumWidth(sizeHint().width()+100);
	}

	void GetSetWidget::destroy()
	{
		if (layout())
			delete layout();
		m_owned.clear();
	}

	GetSetWidget::GetSetWidget(const GetSetInternal::Section& section, QWidget *parent)
		: QScrollArea(parent)
		, GetSetInternal::Dictionary::Observer(section.dictionary)
		, m_dictionary(section.dictionary)
		, m_section(section.path())
		, m_expectChange(false)
	{
		init();
	}

	GetSetWidget::~GetSetWidget()
	{
		destroy();
	}

	void GetSetWidget::keyPressEvent(QKeyEvent *e)
	{
		if (e->key() == Qt::Key_Escape && parentWidget()==0x0)
			close();
	}

	 void GetSetWidget::notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
	 {
		 // Ignore some expected change signals.
		 if (m_expectChange && signal == GetSetInternal::Dictionary::Signal::Change) return;
		 // Matters pertaining to my existence are handled immediately.
		if (node.path()==m_section) {
			GetSetGui::Section myself(getSection());
			if (myself.getHidden()||signal==GetSetInternal::Dictionary::Signal::Destroy)
				close();
			else setEnabled(!myself.getDisabled());
		}
		if (node.super_section!=m_section) return; // Wrong section. Do nothing.
		switch (signal) {
			case GetSetInternal::Dictionary::Signal::Create:  notifyCreate  (node);
			case GetSetInternal::Dictionary::Signal::Change:  notifyChange  (node);
			case GetSetInternal::Dictionary::Signal::Destroy: notifyDestroy (node);
			default: notifyUpdateAttrib(node);
		}
	}

	void GetSetWidget::notifyCreate(const GetSetInternal::Node& node)
	{
		if (m_owned.find(node.name)!=m_owned.end()) return; // already exists

		QObject* anything = m_layout->findChild<QObject*>(node.name.c_str());
		if (anything) return;

		using namespace GetSetInternal;

		if (node.getType()=="Section")
		{
			GetSetGui::Section section(node.name,getSection());
			if (section.getHidden()) return;
			if (section.getGrouped()||section.isCollapsible())
			{
				GetSetWidget *widget=new GetSetWidget(section, this);
				widget->setObjectName("Collapsible");
				widget->setFixedHeight( widget->sizeHint().height()+2); 
				widget->setMinimumWidth( widget->sizeHint().width()+2); 
				widget->setStyleSheet("#Collapsible {border: 1px solid gray}");
				widget->setVisible(!section.getCollapsed());
				widget->setEnabled(!section.getDisabled());
				QPushButton *label=new QPushButton(section.isCollapsible()?(std::string("+ ")+node.name).c_str():node.name.c_str(),this);
				label->setEnabled(section.isCollapsible());
				label->setFlat(true);
				label->setObjectName(node.name.c_str());
				label->setStyleSheet(
					"QPushButton{text-align:left;}"
					"QPushButton:hover:!pressed{color: blue;}"
					"QPushButton:disabled{color: black}"
					);
				m_owned[node.name]=widget;
				connect(label, SIGNAL(clicked()), this, SLOT(collapseGroupedSection()));
				m_layout->addRow(label,widget);
			}
			else
			{
				QWidget		*item = new QWidget(this);
				QLineEdit	*editfield=new QLineEdit(node.getString().c_str(),item);
				editfield->setEnabled(false);
				QPushButton *button=new QPushButton("...",item);
				button->setFixedWidth(50);
				button->setEnabled(!section.getDisabled());
				QHBoxLayout *hlayout = new QHBoxLayout();
				hlayout->setContentsMargins(0,0,0,0);
				hlayout->addWidget(editfield);
				hlayout->addWidget(button);
				item->setLayout(hlayout);
				button->setObjectName(node.name.c_str());
				item->setObjectName(node.name.c_str());
				m_owned[node.name]=item;
				item->setObjectName(node.name.c_str());
				connect(button, SIGNAL(clicked()), this, SLOT(openSubSection()));
				m_layout->addRow(node.name.c_str(),item);
			}
		}
		else if (node.getType()=="Button")
		{
			QPushButton* item=new QPushButton(node.getString().c_str(),this);
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			connect(item, SIGNAL(clicked()), this, SLOT(trigger()) );
			m_layout->addRow(node.name.c_str(),item);
		}
		else if (node.getType()=="bool")
		{
			QCheckBox* item = new QCheckBox(this);
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			connect(item, SIGNAL(stateChanged(int)), this, SLOT(setValue(int)));
			m_layout->addRow(node.name.c_str(),item);
		}
		else if (node.getType()=="Enum")
		{
			QComboBox* item = new QComboBox(this);
			std::vector<std::string> enumerator=Enum(node.name,getSection()).getChoices();
			for (std::vector<std::string>::iterator it=enumerator.begin(); it!=enumerator.end(); ++it)
				item->addItem(it->c_str());
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			connect(item, SIGNAL(currentIndexChanged(int)), this, SLOT(setValue(int)));		
			m_layout->addRow(node.name.c_str(),item);
		}
		else if (node.getType()=="StaticText")
		{
			QLabel* item = new QLabel(this);
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			m_layout->addRow(item);
		}
		else if (node.getType()=="int")
		{
			QSpinBox* item = new QSpinBox(this);
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			connect(item, SIGNAL(valueChanged(int)), this, SLOT(setRangeValue(int)));
			m_layout->addRow(node.name.c_str(),item);
		}
		else if (node.getType()=="Slider")
		{
			QSlider* item = new QSlider(Qt::Horizontal,this);
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			connect(item, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
			m_layout->addRow(node.name.c_str(),item);
		}
		else if (node.getType()=="RangedDouble")
		{
			QDoubleSpinBox* item = new QDoubleSpinBox(this);
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			connect(item, SIGNAL(valueChanged(double)), this, SLOT(setRangeValue(double)));
			m_layout->addRow(node.name.c_str(),item);
		}
		else if (node.getType()=="Directory" || node.getType()=="File")
		{
			QWidget *item = new QWidget(this);
			QLineEdit	*editfield=new QLineEdit(node.getString().c_str(),item);
			QPushButton *button=new QPushButton("...",item);
			button->setFixedWidth(50);
			QHBoxLayout *hlayout = new QHBoxLayout();
			hlayout->setContentsMargins(0,0,0,0);
			hlayout->addWidget(editfield);
			hlayout->addWidget(button);
			item->setLayout(hlayout);
			editfield->setObjectName(node.name.c_str());
			button->setObjectName(node.name.c_str());
			item->setObjectName(node.name.c_str());
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			connect(editfield, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
			if (node.getType()=="File")
				connect(button, SIGNAL(clicked()), this, SLOT(selectFile()));
			else
				connect(button, SIGNAL(clicked()), this, SLOT(selectFolder()));
			m_layout->addRow(node.name.c_str(),item);
		}
		else if (node.getType()=="ReadOnlyText")
		{
			QLineEdit* item=new QLineEdit(node.getString().c_str(),this);
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			item->setEnabled(false);
			m_layout->addRow(node.name.c_str(),item);
		}
		else
		{
			QLineEdit* item=new QLineEdit(node.getString().c_str(),this);
			m_owned[node.name]=item;
			item->setObjectName(node.name.c_str());
			connect(item, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
			m_layout->addRow(node.name.c_str(),item);
		}
		notifyUpdateAttrib(node);
	}

	void GetSetWidget::notifyChange(const GetSetInternal::Node& node)
	{
		if (m_owned.find(node.name)==m_owned.end()) return; // doesn't exist anyway
		QWidget* w=m_owned[node.name];

		if (dynamic_cast<QSlider*>(w))
		{
			// put the slider first in line since sliders tend to create lots of events
			QSlider* item=dynamic_cast<QSlider*>(w);
			item->blockSignals(true);
			item->setValue((int)(1000.*GetSet<double>(node.name,getSection())));
			item->blockSignals(false);
		}
		if (dynamic_cast<QDoubleSpinBox*>(w))
		{
			QDoubleSpinBox* item=dynamic_cast<QDoubleSpinBox*>(w);
			item->blockSignals(true);
			item->setValue(GetSet<double>(node.name,getSection()));
			item->blockSignals(false);
		}
		else if (dynamic_cast<QPushButton*>(w))
		{
			dynamic_cast<QPushButton*>(w)->setText(GetSet<std::string>(node.name,getSection()).getString().c_str());
		}
		else if (dynamic_cast<QCheckBox*>(w))
		{
			QCheckBox* item=dynamic_cast<QCheckBox*>(w);
			item->blockSignals(true);
			item->setChecked(GetSet<bool>(node.name,getSection()));		
			item->blockSignals(false);
		}
		else if (dynamic_cast<QComboBox*>(w))
		{
			QComboBox* item=dynamic_cast<QComboBox*>(w);
			item->blockSignals(true);
			item->setCurrentIndex(GetSet<int>(node.name,getSection()));
			item->blockSignals(false);
		}
		else if (dynamic_cast<QSpinBox*>(w))
		{
			QSpinBox* item=dynamic_cast<QSpinBox*>(w);
			item->blockSignals(true);
			item->setValue(GetSet<int>(node.name,getSection()));
			item->blockSignals(false);
		}
		else if (dynamic_cast<QLabel*>(w))
		{
			QLabel* item=dynamic_cast<QLabel*>(w);
			item->setText(GetSet<std::string>(node.name,getSection()).getString().c_str());
		}
		else if (dynamic_cast<QLineEdit*>(w))
		{
			QLineEdit* item=dynamic_cast<QLineEdit*>(w);
			item->blockSignals(true);
			item->setText(GetSet<std::string>(node.name,getSection()).getString().c_str());
			item->blockSignals(false);
		}
		else if (w->layout() && w->layout()->count()) // true only for file, folder and subsection (editfield+pushbutton)
		{
			QWidget* subwidget=w->layout()->itemAt(0)->widget();
			QLineEdit* item=dynamic_cast<QLineEdit*>(subwidget);
			if (item)
			{
				item->blockSignals(true);
				item->setText(node.getString().c_str());
				item->blockSignals(false);
			}
		}
	}

	void GetSetWidget::notifyDestroy(const GetSetInternal::Node& node)
	{
		if (m_owned.find(node.name)==m_owned.end()) return; // doesn't exist anyway
		destroy();
		init();
	}
	
	void GetSetWidget::notifyUpdateAttrib(const GetSetInternal::Node& node)
	{
		// Get widget
		if (m_owned.find(node.name)==m_owned.end()) return; // doesn't exist anyway
		QWidget* w=m_owned[node.name];

		using namespace GetSetInternal;

		// Set tool tip
		w->setToolTip(node.getAttribute<>("Description").c_str());
		w->blockSignals(true);

		// Most numeric types have these tags:
		double minv=node.getAttribute<double>("Min");
		double maxv=node.getAttribute<>("Max")=="" ?1:node.getAttribute<double>("Max");
		double step=node.getAttribute<double>("Step");
		bool is_periodic=node.getAttribute<bool>("Periodic");
		
		if (dynamic_cast<const GetSetInternal::Section*>(&node))
		{
			auto gw=dynamic_cast<GetSetWidget*>(w);
			if (gw) {
				gw->setVisible(!node.getAttribute<bool>("Collapsed"));
				gw->setEnabled(!node.getAttribute<bool>("Disabled"));
			}
		}
		else if (dynamic_cast<QSlider*>(w))
		{
			QSlider* item=dynamic_cast<QSlider*>(w);
			item->setMaximum(maxv*1000);
			item->setMinimum(minv*1000);
		}
		if (dynamic_cast<QDoubleSpinBox*>(w))
		{
			QDoubleSpinBox* item=dynamic_cast<QDoubleSpinBox*>(w);
			item->setMaximum(maxv);
			if (is_periodic) item->setMinimum(minv-step);
			else item->setMinimum(minv);
			item->setSingleStep(step);
		}
		else if (dynamic_cast<QSpinBox*>(w))
		{
			QSpinBox* item=dynamic_cast<QSpinBox*>(w);
			// For QSpinBox the default Min and Max values are differrnt
			int minv=node.getAttribute<>("Min")=="" ?-2147483647:node.getAttribute<int>("Min");
			int maxv=node.getAttribute<>("Max")=="" ?+2147483647:node.getAttribute<int>("Max");
			item->setMinimum(is_periodic?minv-1:minv);
			item->setMaximum(is_periodic?maxv+1:maxv);
		}
		w->blockSignals(false);

	}


} // namespace GetSetGui
