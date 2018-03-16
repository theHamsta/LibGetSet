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

#include "GetSetProgressWindow.h"


#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

#include <QVBoxLayout>
#include <QCoreApplication>
#include <QMessageBox>

namespace GetSetGui {

	void GetSetProgressWindow::trigger()
	{
		progress_bar->setRange(0,0);
		button->setEnabled(false);
		std::string window=windowTitle().toStdString();
		std::string text_on_button=sender()->objectName().toStdString();
		if (callback) callback(window,text_on_button);
		if (cancel_clicked) *cancel_clicked=true;
	}

	GetSetProgressWindow::GetSetProgressWindow(void (*handler)(const std::string&, const std::string&))
		: QDialog()
		, layout(0x0)
		, label(0x0)
		, progress_bar(0x0)
		, button(0x0)
		, callback(handler)
		, cancel_clicked(0x0)
	{
		layout=new QVBoxLayout(this);
		setLayout(layout);
		label=new QLabel(this);
		progress_bar=new QProgressBar(this);
		button=new QPushButton(this);
		button->setText("Cancel");
		button->setDefault(true);
		connect(button, SIGNAL(clicked()), this, SLOT(trigger()));
		layout->addWidget(label);
		layout->addWidget(progress_bar);
		layout->addWidget(button);
		progress_bar->hide();
		label->hide();
		setWindowFlags(Qt::WindowStaysOnTopHint);
	}

	void GetSetProgressWindow::progressStart(const std::string& title, const std::string& info, int maximum, bool *_cancel_clicked)
	{
		setWindowTitle(title.c_str());
		cancel_clicked=_cancel_clicked;
		if (info.empty())
			label->hide();
		else
		{
			label->setText(info.c_str());
			label->show();
		}
		progress_bar->show();
		if (maximum<0)
			progress_bar->setRange(0,0);
		else
			progress_bar->setRange(0,maximum);
		if (_cancel_clicked)
		{
			button->show();
			button->setEnabled(true);
		}
		else
			button->hide();
		progress_bar->setValue(-1);
		show();
	}

	void GetSetProgressWindow::progressUpdate(int i) {
		progress_bar->setValue(i);
		QCoreApplication::processEvents();
	}

	void GetSetProgressWindow::progressEnd() {
		close();
	}

	void GetSetProgressWindow::info(const std::string& who, const std::string& what, bool show_dialog) {
		if (what.empty())
			label->hide();
		else
		{
			if (show_dialog)
				QMessageBox::information(0x0,who.c_str(),what.c_str(),QMessageBox::Ok);
			else
			{ 
				label->show();
				label->setText(what.c_str());
			}
		}
	}

	void GetSetProgressWindow::warn(const std::string& who, const std::string& what, bool only_inormative) {
		QMessageBox::warning(0x0,who.c_str(),what.c_str(),QMessageBox::Ok);
		if (!only_inormative) {
			std::cerr << who << ": " << what << std::endl << "Exitting...\n";
			exit(1);
		}

	}

} // namespace GetSetGui

