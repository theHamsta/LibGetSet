// Created by A. Aichert on Sun Mar 3rd 2013
// 2do clean up.

#ifndef __q_client_window_
#define __q_client_window_

#include <QtGui/QDialog>

#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>

#include <QtGui/QVBoxLayout>

class QClientWindow : public QDialog
{
	Q_OBJECT
private slots:
	void trigger();

public:
	QClientWindow(void (*handler)(const std::string&, const std::string&));

	QLayout*			layout;
	QLabel*				info;
	QProgressBar*		progress_bar;
private:
	QPushButton*		button;
	void (*callback)(const std::string&, const std::string&);
};

#endif // __q_client_window_
