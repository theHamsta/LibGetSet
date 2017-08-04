// Created by A. Aichert on Sun Mar 3rd 2013
// 2do clean up

#ifndef __getset_progress_window_h
#define __getset_progress_window_h

#include <QDialog>

#include "ProgressInterface.hxx"

class QLayout;
class QLabel;
class QProgressBar;
class QPushButton;

namespace GetSetGui
{
	/// A simple window for a prograss bar with a cancel button.
	class GetSetProgressWindow : public QDialog, public ProgressInterface
	{
		Q_OBJECT
	private slots:
		void trigger();

	public:
		GetSetProgressWindow(void (*handler)(const std::string&, const std::string&)=0x0);

		virtual void progressStart(const std::string& progress, const std::string& info, int maximum, bool *cancel_clicked);
		virtual void progressUpdate(int i);
		virtual void progressEnd();

		virtual void info(const std::string& who, const std::string& what, bool show_dialog=false);
		virtual void warn(const std::string& who, const std::string& what, bool only_inormative=true);

	private:
		QLayout*			layout;
		QLabel*				label;
		QProgressBar*		progress_bar;
		QPushButton*		button;
		bool*				cancel_clicked;

		void (*callback)(const std::string&, const std::string&);
	};

} // namespace GetSetGui

#endif // __getset_progress_window_h
