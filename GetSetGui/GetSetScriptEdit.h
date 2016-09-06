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
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QTextEdit;
class QTextDocument;

namespace GetSetGui
{
	class GetSetScriptSyntaxHighlighter;

	class GetSetScriptEdit : public QMainWindow
	{
		Q_OBJECT

	public:
		GetSetScriptEdit(QWidget *parent = 0);

		void setScript(const std::string& script);

	public slots:
		void newFile();
		void openFile(const QString &path = QString());
		void saveFile(const QString &path = QString());
		void execute();

	private:
		void setupEditor();
		void setupFileMenu();

		void closeEvent(QCloseEvent *event);

		QTextEdit *editor;
		GetSetScriptSyntaxHighlighter *highlighter;
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
