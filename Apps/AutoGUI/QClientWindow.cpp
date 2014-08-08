#include "QClientWindow.h"

void QClientWindow::trigger()
{
	std::string window=windowTitle().toLatin1();
	std::string button=sender()->objectName().toLatin1();
	callback(window,button);
}

QClientWindow::QClientWindow(void (*handler)(const std::string&, const std::string&))
	: QDialog()
	, layout(0x0)
	, info(0x0)
	, progress_bar(0x0)
	, button(0x0)
	, callback(handler)
{
	layout=new QVBoxLayout(this);
	setLayout(layout);
	info=new QLabel(this);
	progress_bar=new QProgressBar(this);
	button=new QPushButton(this);
	button->setText("Kill");
	button->setDefault(true);
	connect(button, SIGNAL(clicked()), this, SLOT(trigger()));
	layout->addWidget(info);
	layout->addWidget(progress_bar);
	layout->addWidget(button);
	progress_bar->hide();
	info->hide();
}
