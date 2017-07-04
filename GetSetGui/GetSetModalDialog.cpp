#include "GetSetModalDialog.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include "GetSetWidget.h"

namespace GetSetGui
{

	 GetSetModalDialog::operator GetSetSection() const { return GetSetSection(my_dict); }

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
