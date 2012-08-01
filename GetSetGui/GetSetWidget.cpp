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

#include <QtGui/QApplication>
#include <QtGui/QFormLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QSpinBox>
#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>

#include "GetSet/GetSet.hxx"

#include <iostream>

void GetSetWidget::trigger()
{
	std::string key=sender()->objectName().toAscii().data();
	dict.signalChange(m_section,key);
}

void GetSetWidget::selectFile()
{
	std::string key=sender()->objectName().toAscii().data();
	GetSetGui::File file(m_section,key);
	std::string path;
	QString currentDir=QFileInfo(QFile(file.getString().c_str())).absoluteDir().absolutePath();
//	QString extensions=vectorToString(file.getExtensions(),";;").c_str();
	QString extensions=file.getExtensions().c_str();
	if (file.getMultiple())
	{
		QStringList files = QFileDialog::getOpenFileNames(this, "Select Files", currentDir, extensions);
		if (!files.empty())
			path=files[0].toAscii().data();
		for (int i=0;i<(int)files.size();i++)
			path+=std::string(";")+files[i].toAscii().data();
	}
	else
	{
		if (file.getCreateNew())
			path=QFileDialog::getSaveFileName(this, "Select A File", currentDir, extensions).toAscii().data();
		else
			path=QFileDialog::getOpenFileName(this, "Select A File", currentDir, extensions).toAscii().data();
	}

	if (!path.empty())
		file=path;
}

void GetSetWidget::selectFolder()
{
	std::string key=sender()->objectName().toAscii().data();
	GetSetGui::Directory folder(m_section,key);
	QString path = QFileDialog::getExistingDirectory(this, "Select A Directory",folder.getString().c_str());
	if ( path.isNull())
		return;
	folder=path.toAscii().data();
}

void GetSetWidget::editingFinished()
{
	std::string key=sender()->objectName().toAscii().data();
	QLineEdit* l=dynamic_cast<QLineEdit*>(sender());
	if (!l) return;
	std::string value=l->text().toStdString();
	GetSet<std::string>(m_section,key)=value;
}

void GetSetWidget::sliderMoved(int value)
{
	if (m_expectChange) { m_expectChange=0; return; }
	std::string key=sender()->objectName().toAscii().data();
	GetSetGui::Slider slider(m_section,key);
	double	d=(double)value/1000.;
	slider=d;
}

void GetSetWidget::setValue(int value)
{
	std::string key=sender()->objectName().toAscii().data();
	GetSet<std::string>(m_section,key)=toString(value);
}

void GetSetWidget::setValue(const QString& value)
{
	std::string key=sender()->objectName().toAscii().data();
	GetSet<std::string>(m_section,key)=value.toAscii().data();
}

void GetSetWidget::init()
{
	setWindowTitle(m_section.c_str());

	m_layout = new QFormLayout();

	setLayout(m_layout);

	for (std::map<std::string,GetSetInternal::GetSetDataInterface*>::iterator it=dict.get()[m_section].begin();it!=dict.get()[m_section].end();++it)
	{
		this->notifyCreate(m_section, it->first);
		this->notifyChange(m_section, it->first);
	}

}

void GetSetWidget::destroy()
{
	if (layout())
	{
		QLayoutItem* item;
		while ( ( item = layout()->takeAt( 0 ) ) != NULL )
		{
			delete item->widget();
			delete item;
		}
		delete layout();
	}
	m_owned.clear();
}

GetSetWidget::GetSetWidget(GetSetDictionary& dict, const std::string& section, QWidget *parent)
	: QWidget(parent)
	, GetSetDictionary::Observer(dict)
	, m_section(section)
{
	init();
}

GetSetWidget::~GetSetWidget()
{
	destroy();
}

GetSetDictionary& GetSetWidget::getDictionary()
{
	return dict;
}

void GetSetWidget::notifyCreate(const std::string& section, const std::string& key)
{
	if (section!=m_section) return; // wrong section.

	QObject* anything = m_layout->findChild<QObject*>(key.c_str());
	if (anything) return;

	GetSetInternal::GetSetDataInterface* p=dict.getDatainterface(section,key);

	using namespace GetSetGui;
	using namespace GetSetInternal;

	if (dynamic_cast<GetSetDataTrigger*>(p)!=0x0)
	{
		QPushButton* item=new QPushButton(p->getString().c_str());
		m_owned[key]=item;
		item->setObjectName(key.c_str());
		connect(item, SIGNAL(clicked()), this, SLOT(trigger()) );
		m_layout->addRow(key.c_str(),item);
	}
	else if (dynamic_cast<GetSetData<bool>*>(p)!=0x0)
	{
		QCheckBox* item = new QCheckBox();
		m_owned[key]=item;
		item->setObjectName(key.c_str());
		connect(item, SIGNAL(stateChanged(int)), this, SLOT(setValue(int)));
		m_layout->addRow(key.c_str(),item);
	}
	else if (dynamic_cast<GetSetDataEnum*>(p)!=0x0)
	{
		QComboBox* item = new QComboBox();
		std::vector<std::string> enumerator=stringToVector<std::string>(p->attributes[""],';');
		for (std::vector<std::string>::iterator it=enumerator.begin(); it!=enumerator.end(); ++it)
			item->addItem(it->c_str());
		m_owned[key]=item;
		item->setObjectName(key.c_str());
		connect(item, SIGNAL(currentIndexChanged(int)), this, SLOT(setValue(int)));		
		m_layout->addRow(key.c_str(),item);
	}
	else if (dynamic_cast<GetSetDataStaticText*>(p)!=0x0) // 2do implement differently
	{
		QLabel* item = new QLabel();
		m_owned[key]=item;
		item->setObjectName(key.c_str());
		m_layout->addRow(item);
	}
	else if (dynamic_cast<GetSetData<int>*>(p)!=0x0)
	{
		QSpinBox* item = new QSpinBox();
		item->setMinimum(-32768);
		item->setMaximum(32767);
		m_owned[key]=item;
		item->setObjectName(key.c_str());
		connect(item, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
		m_layout->addRow(key.c_str(),item);
	}
	else if (dynamic_cast<GetSetDataSlider*>(p)!=0x0)
	{
		QSlider* item = new QSlider(Qt::Horizontal);
		item->setMaximum(Slider(section,key).getMax()*1000);
		item->setMinimum(Slider(section,key).getMin()*1000);
		m_owned[key]=item;
		item->setObjectName(key.c_str());
		connect(item, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
		m_layout->addRow(key.c_str(),item);
	}
	else if (dynamic_cast<GetSetDataDirectory*>(p)!=0x0 || dynamic_cast<GetSetDataFile*>(p)!=0x0)
	{
		QWidget *item = new QWidget();
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
		if (dynamic_cast<GetSetDataFile*>(p))
			connect(button, SIGNAL(clicked()), this, SLOT(selectFile()));
		else
			connect(button, SIGNAL(clicked()), this, SLOT(selectFolder()));
		m_layout->addRow(key.c_str(),item);
	}
	else if (p)
	{
		QLineEdit* item=new QLineEdit(p->getString().c_str());
		m_owned[key]=item;
		item->setObjectName(key.c_str());
		connect(item, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
		m_layout->addRow(key.c_str(),item);
	}
	else
		m_layout->addWidget(new QLabel(key.c_str()));
		
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
		item->setValue((int)(1000.*GetSet<double>(section,key)));
		item->blockSignals(false);
	}
	else if (dynamic_cast<QPushButton*>(w))
	{
		dynamic_cast<QPushButton*>(w)->setText(GetSet<std::string>(section,key).getString().c_str());
	}
	else if (dynamic_cast<QCheckBox*>(w))
	{
		QCheckBox* item=dynamic_cast<QCheckBox*>(w);
		item->blockSignals(true);
		item->setChecked(GetSet<bool>(section,key));		
		item->blockSignals(false);
	}
	else if (dynamic_cast<QComboBox*>(w))
	{
		QComboBox* item=dynamic_cast<QComboBox*>(w);
		item->blockSignals(true);
		item->setCurrentIndex(GetSet<int>(section,key));
		item->blockSignals(false);
	}
	else if (dynamic_cast<QSpinBox*>(w))
	{
		QSpinBox* item=dynamic_cast<QSpinBox*>(w);
		item->blockSignals(true);
		item->setValue(GetSet<int>(section,key));
		item->blockSignals(false);
	}
	else if (dynamic_cast<QLabel*>(w))
	{
		QLabel* item=dynamic_cast<QLabel*>(w);
		item->setText(GetSet<std::string>(section,key).getString().c_str());
	}
	else if (dynamic_cast<QLineEdit*>(w))
	{
		QLineEdit* item=dynamic_cast<QLineEdit*>(w);
		item->blockSignals(true);
		item->setText(GetSet<std::string>(section,key).getString().c_str());
		item->blockSignals(false);
	}
	else if (w->layout() && w->layout()->count()) // true only for file and folder (editfield+pushbutton)
	{
		QWidget* subwidget=w->layout()->itemAt(0)->widget();
		QLineEdit* item=dynamic_cast<QLineEdit*>(subwidget);
		if (item)
		{
			item->blockSignals(true);
			item->setText(GetSet<std::string>(section,key).getString().c_str());
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
