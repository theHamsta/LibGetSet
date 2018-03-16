// NOTE. This file temporarily includes adapted example source code of the The Qt Company Ltd.
// It is distributed under a different license than the rest of the GetSet sources.

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

#ifndef __GetSetScriptEdit_h
#define __GetSetScriptEdit_h

#include <QMainWindow>
#include <QTextEdit>
#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QProgressBar>
#include <QLabel>

#include "../GetSet/GetSetScripting.h"


class QTextDocument;

namespace GetSetGui
{
	class GetSetScriptSyntaxHighlighter;

	class GetSetScriptEdit : public QMainWindow, public ProgressInterface
	{
		Q_OBJECT

	public:
		GetSetScriptEdit(QWidget *parent = 0, GetSetInternal::Dictionary& _dict = GetSetInternal::Dictionary::global());

		void setScript(const std::string& script);

		static void setOutputPane(const std::string& text, void* instance);
		static void setStatusPane(const std::string& text, void* instance);

	public slots:
		void setText(const QString &text);
		void openFile(const QString &path = QString());
		void saveFile(const QString &path = QString());
		void highlightCurrentLine();
		void help();
		void execute();
		void force_stop();
		void executeSelected();

	private:
		void setupEditor();
		void setupToolBarsAndMenus();

		void closeEvent(QCloseEvent *event);

		QTextEdit			*editor;
		GetSetScriptParser	parser;
		QLabel				*m_location;
		QTextEdit			*m_statusMsg;
		QTextEdit			*m_outputMsg;
		QProgressBar		*m_progress;
		GetSetScriptSyntaxHighlighter *highlighter;
		QList<QTextEdit::ExtraSelection> extraSelections;

		// A little progressbar in the status bar.
		virtual void progressStart(const std::string& progress, const std::string& info, int maximum, bool *cancel_clicked);
		virtual void progressUpdate(int i);
		virtual void progressEnd();
		virtual void info(const std::string& who, const std::string& what, bool show_dialog=false);
		virtual void warn(const std::string& who, const std::string& what, bool only_inormative=true);

	};

	class GetSetScriptSyntaxHighlighter : public QSyntaxHighlighter
	{
		Q_OBJECT

	public:
		GetSetScriptSyntaxHighlighter(QTextDocument *parent = 0);

		/// Define syntax highlighting. Default format is for commands.
		void addHighlightingRule(const std::string& pattern, unsigned char r, unsigned char g, unsigned char b, bool bold=false, bool italic=false, bool underline=false);

	protected:
		void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

	private:
		std::vector<std::pair<std::string,QTextCharFormat > > highlightingRules;
		std::vector<std::pair<std::string,QTextCharFormat > > overridingRules;
	};

} // namespace GetSetGui

#endif // __GetSetScriptEdit_h
