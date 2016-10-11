#include "GetSetProgressWindow.h"


#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

#include <QVBoxLayout>

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
		, info(0x0)
		, progress_bar(0x0)
		, button(0x0)
		, callback(handler)
		, cancel_clicked(0x0)
	{
		layout=new QVBoxLayout(this);
		setLayout(layout);
		info=new QLabel(this);
		progress_bar=new QProgressBar(this);
		button=new QPushButton(this);
		button->setText("Cancel");
		button->setDefault(true);
		connect(button, SIGNAL(clicked()), this, SLOT(trigger()));
		layout->addWidget(info);
		layout->addWidget(progress_bar);
		layout->addWidget(button);
		progress_bar->hide();
		info->hide();
	}

} // namespace GetSetGui

