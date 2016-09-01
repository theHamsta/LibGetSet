// Created by A. Aichert on Sun Mar 3rd 2013
// 2do clean up

#ifndef __getset_progress_window_h
#define __getset_progress_window_h

#include <QDialog>

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

#include <QVBoxLayout>

class GetSetProgressWindow : public QDialog
{
	Q_OBJECT
private slots:
	void trigger();

public:
	GetSetProgressWindow(void (*handler)(const std::string&, const std::string&)=0x0);

	QLayout*			layout;
	QLabel*				info;
	QProgressBar*		progress_bar;
	QPushButton*		button;
	bool*				cancel_clicked;
private:
	void (*callback)(const std::string&, const std::string&);
};

#endif // __getset_progress_window_h
