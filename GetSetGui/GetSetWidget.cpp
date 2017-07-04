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


namespace GetSetGui {

	void GetSetWidget::trigger()
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetGui::Button(key,GetSetSection(m_section,dictionary)).trigger(); // signalChange(m_section,key);
	}

	void GetSetWidget::selectFile()
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetGui::File file(key,GetSetSection(m_section,dictionary));
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
		GetSetGui::Directory folder(key,GetSetSection(m_section,dictionary));
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
		std::string section=m_section.empty()?key:m_section+"/"+key;
		GetSetWidget* w=new GetSetWidget(GetSetSection(section,dictionary));
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
		GetSet<std::string> property(key,GetSetSection(m_section,dictionary));
		if (property.getString()!=value)
			property=value;
	}

	void GetSetWidget::sliderMoved(int value)
	{
		if (m_expectChange) { m_expectChange=0; return; }
		std::string key=sender()->objectName().toLatin1().data();
		GetSetGui::Slider slider(key,GetSetSection(m_section,dictionary));
		double	d=(double)value/1000.;
		slider=d;
	}

	void GetSetWidget::setValue(int value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSet<std::string>(key,GetSetSection(m_section,dictionary))=toString(value);
	}

	void GetSetWidget::setRangeValue(double value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSetGui::RangedDouble item(key,GetSetSection(m_section,dictionary));
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
		item.setValue(value);
	}

	void GetSetWidget::setRangeValue(int value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		std::string type=GetSetSection(m_section,dictionary).getTypeOfKey(key);
		if (type=="RangedInt")
		{
			RangedInt item(key,GetSetSection(m_section,dictionary));
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
			item.setValue(value);
		}
		else GetSet<int>(key,GetSetSection(m_section,dictionary))=value;
	}

	void GetSetWidget::setValue(const QString& value)
	{
		std::string key=sender()->objectName().toLatin1().data();
		GetSet<std::string>(key,GetSetSection(m_section,dictionary))=value.toLatin1().data();
	}

	void GetSetWidget::init()
	{
		setWindowTitle(m_section.c_str());
		setFrameShape(QFrame::NoFrame);

		m_content=new QWidget(this);
		m_layout = new QFormLayout();
		m_content->setLayout(m_layout);

		GetSetInternal::Section * section=(GetSetInternal::Section *)Access::getProperty(m_section);

		for (GetSetInternal::Section::PropertyByName::const_iterator it=section->getSection().begin();it!=section->getSection().end();++it)
		{
			this->notifyCreate(m_section, it->first);
			this->notifyChange(m_section, it->first);
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

	GetSetWidget::GetSetWidget(const GetSetSection& section, QWidget *parent)
		: QScrollArea(parent)
		, GetSetDictionary::Observer(section.dict())
		, m_section(section.path())
	{
		init();
	}

	GetSetWidget::~GetSetWidget()
	{
		destroy();
	}

	GetSetDictionary& GetSetWidget::getDictionary()
	{
		return dictionary;
	}

	void GetSetWidget::keyPressEvent(QKeyEvent *e)
	{
		if (e->key() == Qt::Key_Escape && parentWidget()==0x0)
			close();
	}

	void GetSetWidget::notifyCreate(const std::string& section, const std::string& key)
	{
		std::string path_to_key=section.empty() ? key : section+"/"+key;
		if (section!=m_section) return; // wrong section.

		if (m_owned.find(key)!=m_owned.end()) return; // already exists

		QObject* anything = m_layout->findChild<QObject*>(key.c_str());
		if (anything) return;

		using namespace GetSetGui;
		using namespace GetSetInternal;
		Node*	p=getProperty(path_to_key);

		auto& attribs(p->attributes);

		if (dynamic_cast<Section*>(p)!=0x0)
		{
			// Section is not shown in GUI at all
			bool is_hidden      =attribs.end()==attribs.find("Hidden"  )?false:stringTo<bool>(attribs["Hidden"  ]);
			// Key in this section are gayed-out in GUI
			bool is_disabled    =attribs.end()==attribs.find("Disabled")?false:stringTo<bool>(attribs["Disabled"]);
			// This section is shown in a group box, rather than an independent widget
			bool is_grouped     =attribs.end()==attribs.find("Grouped" )?false:stringTo<bool>(attribs["Grouped" ]);
			// This section is shown in a group box, rather than an independent widget
			bool is_collapsible =attribs.end()!=attribs.find("Collapsed" );
			if (is_hidden) return;
			if (is_grouped)
			{
				bool is_collapsed=is_collapsible?stringTo<bool>(attribs["Collapsed"]):false;
				std::string section=m_section.empty()?key:m_section+"/"+key;
				GetSetWidget *widget=new GetSetWidget(GetSetSection(section,dictionary), this);
				widget->setObjectName("Collapsible");
				widget->setFixedHeight( widget->sizeHint().height()+2); 
				widget->setMinimumWidth( widget->sizeHint().width()+2); 
				widget->setStyleSheet("#Collapsible {border: 1px solid gray}");
				widget->setVisible(!is_collapsed);
				widget->setEnabled(!is_disabled);
				QPushButton *label=new QPushButton(is_collapsible?(std::string("+ ")+key).c_str():key.c_str(),this);
				label->setEnabled(is_collapsible);
				label->setFlat(true);
				label->setObjectName(key.c_str());
				label->setStyleSheet(
					"QPushButton{text-align:left;}"
					"QPushButton:hover:!pressed{color: blue;}"
					"QPushButton:disabled{color: black}"
					);
				m_owned[key]=widget;
				connect(label, SIGNAL(clicked()), this, SLOT(collapseGroupedSection()));
				m_layout->addRow(label,widget);
			}
			else
			{
				QWidget		*item = new QWidget(this);
				QLineEdit	*editfield=new QLineEdit(p->getString().c_str(),item);
				editfield->setEnabled(false);
				QPushButton *button=new QPushButton("...",item);
				button->setFixedWidth(50);
				button->setEnabled(!is_disabled);
				QHBoxLayout *hlayout = new QHBoxLayout();
				hlayout->setContentsMargins(0,0,0,0);
				hlayout->addWidget(editfield);
				hlayout->addWidget(button);
				item->setLayout(hlayout);
				button->setObjectName(key.c_str());
				item->setObjectName(key.c_str());
				m_owned[key]=item;
				item->setObjectName(key.c_str());
				connect(button, SIGNAL(clicked()), this, SLOT(openSubSection()));
				m_layout->addRow(key.c_str(),item);
			}
		}
		else if (dynamic_cast<GetSetKeyButton*>(p)!=0x0)
		{
			QPushButton* item=new QPushButton(p->getString().c_str(),this);
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			connect(item, SIGNAL(clicked()), this, SLOT(trigger()) );
			m_layout->addRow(key.c_str(),item);
		}
		else if (dynamic_cast<GetSetKey<bool>*>(p)!=0x0)
		{
			QCheckBox* item = new QCheckBox(this);
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			connect(item, SIGNAL(stateChanged(int)), this, SLOT(setValue(int)));
			m_layout->addRow(key.c_str(),item);
		}
		else if (dynamic_cast<GetSetKeyEnum*>(p)!=0x0)
		{
			QComboBox* item = new QComboBox(this);
			std::vector<std::string> enumerator=Enum(key,GetSetSection(section,dictionary)).getChoices();
			for (std::vector<std::string>::iterator it=enumerator.begin(); it!=enumerator.end(); ++it)
				item->addItem(it->c_str());
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			connect(item, SIGNAL(currentIndexChanged(int)), this, SLOT(setValue(int)));		
			m_layout->addRow(key.c_str(),item);
		}
		else if (dynamic_cast<GetSetKeyStaticText*>(p)!=0x0) // 2do implement differently
		{
			QLabel* item = new QLabel(this);
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			m_layout->addRow(item);
		}
		else if (dynamic_cast<GetSetKey<int>*>(p)!=0x0)
		{
			QSpinBox* item = new QSpinBox(this);
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			connect(item, SIGNAL(valueChanged(int)), this, SLOT(setRangeValue(int)));
			m_layout->addRow(key.c_str(),item);
		}
		else if (dynamic_cast<GetSetKeySlider*>(p)!=0x0)
		{
			QSlider* item = new QSlider(Qt::Horizontal,this);
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			connect(item, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
			m_layout->addRow(key.c_str(),item);
		}
		else if (dynamic_cast<GetSetKeyRangedDouble*>(p)!=0x0)
		{
			QDoubleSpinBox* item = new QDoubleSpinBox(this);
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			connect(item, SIGNAL(valueChanged(double)), this, SLOT(setRangeValue(double)));
			m_layout->addRow(key.c_str(),item);
		}
		else if (dynamic_cast<GetSetKeyDirectory*>(p)!=0x0 || dynamic_cast<GetSetKeyFile*>(p)!=0x0)
		{
			QWidget *item = new QWidget(this);
			QLineEdit	*editfield=new QLineEdit(p->getString().c_str(),item);
			QPushButton *button=new QPushButton("...",item);
			button->setFixedWidth(50);
			QHBoxLayout *hlayout = new QHBoxLayout();
			hlayout->setContentsMargins(0,0,0,0);
			hlayout->addWidget(editfield);
			hlayout->addWidget(button);
			item->setLayout(hlayout);
			editfield->setObjectName(key.c_str());
			button->setObjectName(key.c_str());
			item->setObjectName(key.c_str());
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			connect(editfield, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
			if (dynamic_cast<GetSetKeyFile*>(p))
				connect(button, SIGNAL(clicked()), this, SLOT(selectFile()));
			else
				connect(button, SIGNAL(clicked()), this, SLOT(selectFolder()));
			m_layout->addRow(key.c_str(),item);
		}
		else if (dynamic_cast<GetSetKeyReadOnlyText*>(p)!=0x0)
		{
			QLineEdit* item=new QLineEdit(p->getString().c_str(),this);
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			item->setEnabled(false);
			m_layout->addRow(key.c_str(),item);
		}
		else if (p)
		{
			QLineEdit* item=new QLineEdit(p->getString().c_str(),this);
			m_owned[key]=item;
			item->setObjectName(key.c_str());
			connect(item, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
			m_layout->addRow(key.c_str(),item);
		}
		else
		{
			QLabel* item=new QLabel(key.c_str(),this);
			m_layout->addWidget(item);
		}
		notifyUpdateAttrib(section,key);
	}

	void GetSetWidget::notifyChange(const std::string& section, const std::string& key)
	{
		if (section!=m_section) return; // wrong section.
		if (m_owned.find(key)==m_owned.end()) return; // doesn't exist anyway
		QWidget* w=m_owned[key];

		if (dynamic_cast<QSlider*>(w))
		{
			// put the slider first in line since sliders tend to create lots of events
			QSlider* item=dynamic_cast<QSlider*>(w);
			item->blockSignals(true);
			item->setValue((int)(1000.*GetSet<double>(key,GetSetSection(section,dictionary))));
			item->blockSignals(false);
		}
		if (dynamic_cast<QDoubleSpinBox*>(w))
		{
			QDoubleSpinBox* item=dynamic_cast<QDoubleSpinBox*>(w);
			item->blockSignals(true);
			item->setValue(GetSet<double>(key,GetSetSection(section,dictionary)));
			item->blockSignals(false);
		}
		else if (dynamic_cast<QPushButton*>(w))
		{
			dynamic_cast<QPushButton*>(w)->setText(GetSet<std::string>(key,GetSetSection(section,dictionary)).getString().c_str());
		}
		else if (dynamic_cast<QCheckBox*>(w))
		{
			QCheckBox* item=dynamic_cast<QCheckBox*>(w);
			item->blockSignals(true);
			item->setChecked(GetSet<bool>(key,GetSetSection(section,dictionary)));		
			item->blockSignals(false);
		}
		else if (dynamic_cast<QComboBox*>(w))
		{
			QComboBox* item=dynamic_cast<QComboBox*>(w);
			item->blockSignals(true);
			item->setCurrentIndex(GetSet<int>(key,GetSetSection(section,dictionary)));
			item->blockSignals(false);
		}
		else if (dynamic_cast<QSpinBox*>(w))
		{
			QSpinBox* item=dynamic_cast<QSpinBox*>(w);
			item->blockSignals(true);
			item->setValue(GetSet<int>(key,GetSetSection(section,dictionary)));
			item->blockSignals(false);
		}
		else if (dynamic_cast<QLabel*>(w))
		{
			QLabel* item=dynamic_cast<QLabel*>(w);
			item->setText(GetSet<std::string>(key,GetSetSection(section,dictionary)).getString().c_str());
		}
		else if (dynamic_cast<QLineEdit*>(w))
		{
			QLineEdit* item=dynamic_cast<QLineEdit*>(w);
			item->blockSignals(true);
			item->setText(GetSet<std::string>(key,GetSetSection(section,dictionary)).getString().c_str());
			item->blockSignals(false);
		}
		else if (w->layout() && w->layout()->count()) // true only for file, folder and subsection (editfield+pushbutton)
		{
			QWidget* subwidget=w->layout()->itemAt(0)->widget();
			QLineEdit* item=dynamic_cast<QLineEdit*>(subwidget);
			if (item)
			{
				item->blockSignals(true);
				item->setText(GetSet<std::string>(key,GetSetSection(section,dictionary)).getString().c_str());
				item->blockSignals(false);
			}
		}
	}

	void GetSetWidget::notifyDestroy(const std::string& section, const std::string& key)
	{
		if (section!=m_section) return; // wrong section.
		if (m_owned.find(key)==m_owned.end()) return; // doesn't exist anyway
		destroy();
		init();
	}
	
	void GetSetWidget::notifyUpdateAttrib(const std::string& section, const std::string& key)
	{
		std::string path_to_key=section.empty() ? key : section+"/"+key;
		if (path_to_key==m_section) // This concerns myself!
		{
			GetSetSection myself(path_to_key,dictionary);
			if (!myself.exists() || myself.isHidden()) close();
			setEnabled(!myself.isDisabled());
			return;
		}
		else if (section!=m_section) return; // wrong section.

		// Get widget
		if (m_owned.find(key)==m_owned.end()) return; // doesn't exist anyway
		QWidget* w=m_owned[key];
		// Get attribs
		using namespace GetSetGui;
		using namespace GetSetInternal;
		Node*	p=getProperty(path_to_key);
		if (!p) return;
		auto& attrib=p->attributes;

		// Set tool tip
		if (attrib.end()!=attrib.find("Description"))
			w->setToolTip(attrib["Description"].c_str());
		w->blockSignals(true);
		if (dynamic_cast<GetSetInternal::Section*>(p))
		{
			auto gw=dynamic_cast<GetSetWidget*>(w);
			if (gw)
			{
				bool is_hidden      =attrib.end()==attrib.find("Hidden"   )?false:stringTo<bool>(attrib["Hidden"   ]);
				bool is_disabled    =attrib.end()==attrib.find("Disabled" )?false:stringTo<bool>(attrib["Disabled" ]);
				bool is_grouped     =attrib.end()==attrib.find("Grouped"  )?false:stringTo<bool>(attrib["Grouped"  ]);
				bool is_collapsed   =attrib.end()==attrib.find("Collapsed")?false:stringTo<bool>(attrib["Collapsed"]);
				gw->setVisible(!is_collapsed);
				gw->setEnabled(!is_disabled);
				/*if (!is_grouped || is_hidden)
				{
					delete w;
					this->notifyCreate(section,key);
					return;
				}*/
			}
		}
		else if (dynamic_cast<QSlider*>(w))
		{
			QSlider* item=dynamic_cast<QSlider*>(w);
			if (attrib["Min"]=="") attrib["Min"]="0";
			if (attrib["Max"]=="") attrib["Max"]="1";
			item->setMaximum(Slider(key,GetSetSection(section,dictionary)).getMax()*1000);
			item->setMinimum(Slider(key,GetSetSection(section,dictionary)).getMin()*1000);
		}
		if (dynamic_cast<QDoubleSpinBox*>(w))
		{
			QDoubleSpinBox* item=dynamic_cast<QDoubleSpinBox*>(w);
			if (attrib["Min"]=="") attrib["Min"]="0";
			if (attrib["Max"]=="") attrib["Max"]="1";
			if (attrib["Step"]=="") attrib["Step"]="0.05";
			double minv=stringTo<double>(attrib["Min"] );
			double maxv=stringTo<double>(attrib["Max"] );
			double step=stringTo<double>(attrib["Step"]);
			bool is_periodic=stringTo<bool>(attrib["Periodic"]);
			item->setMaximum(maxv);
			if (is_periodic) item->setMinimum(minv-step);
			else item->setMinimum(minv);
			item->setSingleStep(step);
		}
		else if (dynamic_cast<QSpinBox*>(w))
		{
			QSpinBox* item=dynamic_cast<QSpinBox*>(w);
			if (attrib["Min"]=="") attrib["Min"]="-2147483648";
			if (attrib["Max"]=="") attrib["Max"]="2147483647";
			double minv=stringTo<double>(attrib["Min"] );
			double maxv=stringTo<double>(attrib["Max"] );
			double step=stringTo<double>(attrib["Step"]);
			bool is_periodic=stringTo<bool>(attrib["Periodic"]);
			item->setMinimum(is_periodic?minv-1:minv);
			item->setMaximum(is_periodic?maxv+1:maxv);
		}
		w->blockSignals(false);

	}


} // namespace GetSetGui
