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

#include "GetSetModalDialog.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include "GetSetWidget.h"

namespace GetSetGui
{

	 GetSetModalDialog::operator GetSetGui::Section() { return my_dict; }

	bool GetSetModalDialog::exec(const std::string& title)
	{
		QDialog *diag=new QDialog();
		QVBoxLayout *layout=new QVBoxLayout();
		GetSetWidget *widget=new GetSetWidget(my_dict,diag);
		layout->addWidget(widget);
		QDialogButtonBox *buttonBox=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		layout->addWidget(buttonBox);
		diag->setLayout(layout);
			
		QPushButton *ok=buttonBox->button(QDialogButtonBox::Ok);
		QPushButton *cancel=buttonBox->button(QDialogButtonBox::Cancel);
		diag->connect(ok, SIGNAL(clicked()), diag, SLOT(accept()) );
		diag->connect(cancel, SIGNAL(clicked()), diag, SLOT(reject()) );
			
		diag->setWindowTitle(title.c_str());
		diag->setAttribute(Qt::WA_DeleteOnClose);
		return diag->exec()==QDialog::Accepted;
	}

} // namespace GetSetGui
