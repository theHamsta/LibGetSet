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

#include "GetSetScriptEdit.h"

#include <sstream>

#include <QtWidgets>
#include <QToolBar>
#include <QStyle>

#include "../GetSet/GetSet.hxx"

#include <fstream>

#include "GetSetProgressWindow.h"

namespace GetSetGui
{
	void GetSetScriptEdit::setOutputPane(const std::string& text, void* instance)
	{
		using GetSetGui::GetSetScriptEdit;
		GetSetScriptEdit* se=static_cast<GetSetScriptEdit*>(instance);
		if (se) se->m_outputMsg->setText(se->m_outputMsg->toPlainText() +"\n"+ text.c_str());
	}

	void GetSetScriptEdit::setStatusPane(const std::string& text, void* instance)
	{
		using GetSetGui::GetSetScriptEdit;
		GetSetScriptEdit* se=static_cast<GetSetScriptEdit*>(instance);
		if (!se || text.empty()) return;
		if (text.front()=='@') se->m_location->setText(text.c_str());
		else se->m_statusMsg->setText(se->m_statusMsg->toPlainText() +"\n"+ text.c_str());
		QApplication::processEvents();
	}

	GetSetScriptEdit::GetSetScriptEdit(QWidget *parent, GetSetInternal::Dictionary& _dict)
		: QMainWindow(parent)
		, parser(_dict)
	{
		setupToolBarsAndMenus();
		setupEditor();

		setCentralWidget(editor);
		setWindowTitle(tr("GetSet Script Editor"));

		setWindowIcon(style()->standardIcon(QStyle::SP_FileIcon));

		resize(600,480);

		parser.addOutputCallback((void*)this,GetSetScriptEdit::setOutputPane);
		parser.addErrorCallback((void*)this,GetSetScriptEdit::setStatusPane);

		m_progress=new QProgressBar(this);
		m_progress->setFixedWidth(300);
		this->statusBar()->addWidget(m_progress);
		m_location=new QLabel(this);
		this->statusBar()->addWidget(m_location);
		m_progress->setVisible(false);

		QDockWidget *outputMsg = new QDockWidget(this);
		outputMsg->setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
		outputMsg->setWindowTitle("Output");
		m_outputMsg=new QTextEdit(this);
		m_outputMsg->setReadOnly(true);
		outputMsg->setWidget(m_outputMsg);
		addDockWidget(Qt::BottomDockWidgetArea, outputMsg);
		QDockWidget *statusMsg = new QDockWidget(this);
		statusMsg->setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
		statusMsg->setWindowTitle("Info & Errors");
		m_statusMsg=new QTextEdit(this);
		m_statusMsg->setReadOnly(true);
		statusMsg->setWidget(m_statusMsg);
		addDockWidget(Qt::BottomDockWidgetArea, statusMsg);
//		tabifyDockWidget(outputMsg, statusMsg);
//		setDockNestingEnabled(true);
	}

	void GetSetScriptEdit::highlightCurrentLine()
	{
		extraSelections.clear();
		if (!editor->isReadOnly()) {
			QTextEdit::ExtraSelection selection;

			QColor lineColor = QColor(Qt::yellow).lighter(160);

			selection.format.setBackground(lineColor);
			selection.format.setProperty(QTextFormat::FullWidthSelection, true);
			selection.cursor = editor->textCursor();
			selection.cursor.clearSelection();
			extraSelections.append(selection);
		}
		// This is reidiculously slow. whatever: scripts are not supoosed to be very long...
		std::istringstream file(editor->toPlainText().toStdString());
		file.seekg(editor->textCursor().position());
		std::pair<int,std::string> location=GetSetScriptParser::location(file);

		editor->setExtraSelections(extraSelections);
		m_location->setText(location.second.c_str());
	}

	void GetSetScriptEdit::setupEditor()
	{
		QFont font;
		font.setFamily("Courier");
		font.setFixedPitch(true);
		font.setPointSize(12);

		editor = new QTextEdit;
		editor->setFont(font);
		editor->setLineWrapMode(QTextEdit::NoWrap);

		highlighter = new GetSetScriptSyntaxHighlighter(editor->document());

		std::string appname=GetSet<>("Application");

		if (!appname.empty())
		{
			QFile file((appname+".getset").c_str());
			if (file.open(QFile::ReadOnly | QFile::Text))
				editor->setPlainText(file.readAll());
		}
		connect(editor, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
	}

	void GetSetScriptEdit::setupToolBarsAndMenus()
	{
		// File Menu:
		QMenu *fileMenu = new QMenu(tr("&File"), this);
		menuBar()->addMenu(fileMenu);
		QAction *aopen =fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogOpenButton), tr("&Open..."), this, SLOT(openFile()), QKeySequence::Open);
		QAction *asave =fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Save As..."), this, SLOT(saveFile()), QKeySequence::Save);
		fileMenu->addSeparator();
		QAction *ahelp =fileMenu->addAction(style()->standardIcon(QStyle::SP_MessageBoxQuestion), tr("Syntax Help (selection)"), this, SLOT(help()), QKeySequence(QKeySequence(Qt::Key_F1)));
		fileMenu->addSeparator();
		QAction *aexec =fileMenu->addAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("E&xecute"), this, SLOT(execute()), QKeySequence(QKeySequence(Qt::Key_F5)));
		QAction *aexel =fileMenu->addAction(style()->standardIcon(QStyle::SP_MediaSkipForward), tr("Execute line/selection"), this, SLOT(executeSelected()), QKeySequence(QKeySequence(Qt::Key_F10)));
		QAction *astop =fileMenu->addAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Force Stop"), this, SLOT(force_stop()));

		// Tool Bar
		QToolBar *fileToolBar = addToolBar(tr("Script"));
		fileToolBar->addAction(ahelp);
		fileToolBar->addSeparator();
		fileToolBar->addAction(aexec);
		fileToolBar->addAction(aexel);
		fileToolBar->addAction(astop);
	}

	void GetSetScriptEdit::setText(const QString &text)
	{
		editor->setPlainText(text);
	}
	
	void GetSetScriptEdit::openFile(const QString &path)
	{
		QString fileName = path;

		if (fileName.isNull())
			fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "GetSet scripts (*.getset);;All Files (*)");

		if (!fileName.isEmpty()) {
			QFile file(fileName);
			if (file.open(QFile::ReadOnly | QFile::Text))
				editor->setPlainText(file.readAll());
		}
	}

	void GetSetScriptEdit::saveFile(const QString &path)
	{
		std::string appname=GetSet<>("Application");
		std::string fileName = path.toStdString();

		if (fileName.empty())
			fileName = QFileDialog::getSaveFileName(this, tr("Save File"), (appname+".getset").c_str(), "GetSet scripts (*.getset);;All Files (*)").toStdString();

		if (!fileName.empty())
		{
			std::ofstream file(fileName);
			file << editor->toPlainText().toStdString() << std::endl;
		}
	}

	void GetSetScriptEdit::closeEvent(QCloseEvent *event)
	{
		// FIXME if (anything_has_changes)
		if(QMessageBox::question(NULL, "Save Changes?", "Any changes will be discarded. Would you like to save before closing?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
			saveFile();
	}

	// Some magic to make the little progress bar work.
	void GetSetScriptEdit::progressStart(const std::string& progress, const std::string& info, int maximum, bool *cancel_clicked) { m_progress->setRange(0,maximum);  m_progress->setValue(-1); m_progress->setVisible(true);}
	void GetSetScriptEdit::progressUpdate(int i) {m_progress->setValue(i);}
	void GetSetScriptEdit::progressEnd() {m_progress->setVisible(false);}
	void GetSetScriptEdit::info(const std::string& who, const std::string& what, bool show_dialog)     {}
	void GetSetScriptEdit::warn(const std::string& who, const std::string& what, bool only_inormative) {}

	void GetSetScriptEdit::help()
	{
		m_statusMsg->setText(parser.synopsis(editor->textCursor().selectedText().toStdString(),true).c_str());
	}

	void GetSetScriptEdit::execute()
	{
		m_outputMsg->setText("");
		m_statusMsg->setText("");
		std::string script=editor->toPlainText().toStdString();
		parser.parse(script,"Script",this);
		editor->setExtraSelections(QList<QTextEdit::ExtraSelection>());
	}

	void GetSetScriptEdit::force_stop()
	{
		parser.force_stop();
	}

	void GetSetScriptEdit::executeSelected()
	{
		m_outputMsg->setText("");
		m_statusMsg->setText("");
		// Find out which part of the script to execute
		auto tc=editor->textCursor();
		bool execline=!tc.hasSelection();
		if (execline)
			tc.select(QTextCursor::LineUnderCursor);

		// editor->textCursor().selectedText() <- Qt Bug??
		int from=tc.selectionStart();
		int to=tc.selectionEnd();
		std::string all=editor->toPlainText().toStdString();

		// Special case: line execution does not work for if/for/while/function blocks
		if (execline)
		{
			std::istringstream block(all);
			block.seekg(from, block.beg);
			std::string command;
			block >> std::ws;
			int no_ws=block.tellg();
			block >> command;
			if (command == "if"||command == "for"||command == "while"||command == "define")
			{
				std::string endcommand=std::string("end"+command);
				int i=1;
				while (i>0||block.eof() && block.good())
				{
					std::string input;
					block >> input;
					if (input==command) i++;
					else if (input==endcommand) i--;
				}
				// Set selection to block
				from=no_ws;
				to=block.tellg();
				tc.setPosition(no_ws);
				tc.setPosition(block.tellg(), QTextCursor::KeepAnchor);
			}
		}

		// Try to execute
		std::string selection=all.substr(from,to-from);

		// Execute script block
		bool sucess=parser.parse(selection,"selection");

		// Highlight executed code
		QTextEdit::ExtraSelection highlight_executed;
		highlight_executed.format.setBackground(QColor(sucess?Qt::green:Qt::red).lighter(175));
		highlight_executed.cursor=tc;
		highlight_executed.format.setProperty(QTextFormat::FullWidthSelection, true);
		// If we evecute things line-byline, move cursor
		if (execline)
		{
			tc.setPosition(highlight_executed.cursor.selectionEnd()+1);
			editor->setTextCursor(tc);
		}
		extraSelections.append(highlight_executed);
		editor->setExtraSelections(extraSelections);
	}

	
	void GetSetScriptEdit::setScript(const std::string& script)
	{
		editor->setPlainText(script.c_str());
	}

	void GetSetScriptSyntaxHighlighter::addHighlightingRule(const std::string& pattern, unsigned char r, unsigned char g, unsigned char b, bool bold, bool italic, bool underline)
	{
		QTextCharFormat format;
		format.setFontWeight((bold?QFont::Bold : QFont::Normal) );
		format.setFontItalic(italic);
		format.setFontUnderline(underline);
		format.setForeground(QColor(r,g,b));
		highlightingRules.push_back(std::pair<std::string,QTextCharFormat >(pattern.c_str(),format));
	}

	GetSetScriptSyntaxHighlighter::GetSetScriptSyntaxHighlighter(QTextDocument *parent)
		: QSyntaxHighlighter(parent)
	{
		std::string keywords="\\b(?:help|call|concat|define|discard|print|eval|exit|file|for|if|input|on|set|while|who|with|enddefine|endfor|endif|endwhile)\\b";
		std::string keywords_cosub="\\b(?:each|in|from|to|than|step|output|replace|append|and|ini\\s+(?:|load|save|get\\s+var|set\\s+key|remove\\s+key)|run|trigger|change|do nothing)\\b";
		std::string keywords_cmpar="\\b(?:not|strequal|numequal|gequal|lequal|greater|less)\\b";
		std::string keywords_cmpop="\\b(?:plus|minus|times|over)\\b";

		addHighlightingRule(keywords,0,0,128,true);
		addHighlightingRule(keywords_cosub,0,0,128,false);
		addHighlightingRule(keywords_cmpar,0,0,0,true);
		addHighlightingRule(keywords_cmpop,128,0,0,false,false,false);

		addHighlightingRule("\\bvalue\\s+(?:\"[^\"]*\"|\\S+\\b)",128,0,0,false,false,false); 
		addHighlightingRule("\\b(:?var|function)\\s+(?:\"[^\"]*\"|\\S+\\b)",128,0,0,false,false,true); 
		addHighlightingRule("\\b(?:key|trigger|section)\\s+(?:\"[^\"]*\"|\\S+\\b)",200,128,0,false,false,true); 

		addHighlightingRule("#[^\n]*",0,128,0,false);

		QTextCharFormat format;
		format.setFontWeight(QFont::Bold);
		format.setForeground(QColor(0,0,0));

		overridingRules.push_back(std::pair<std::string,QTextCharFormat >("\\b(key|value|var|trigger|section|function)\\s+",format));
	}

	void GetSetScriptSyntaxHighlighter::highlightBlock(const QString &text)
	{

		for (auto rule=highlightingRules.begin();rule!=highlightingRules.end();++rule)
		{
			QRegExp expression(rule->first.c_str());
			int index = expression.indexIn(text);
			while (index >= 0) {
				int length = expression.matchedLength();
				setFormat(index, length, rule->second);
				index = expression.indexIn(text, index + length);
			}
		}

		for (auto rule=overridingRules.begin();rule!=overridingRules.end();++rule)
		{
			QRegExp expression(rule->first.c_str());
			int index = expression.indexIn(text);
			while (index >= 0) {
				int length = expression.matchedLength();
				setFormat(index, length, rule->second);
				index = expression.indexIn(text, index + length);
			}
		}

		setCurrentBlockState(0);

	}
} // namespace GetSetGui
