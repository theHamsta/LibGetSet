#include <sstream>

#include <QtWidgets>
#include <QToolBar>
#include <QStyle>

#include "../GetSet/GetSet.hxx"
#include "GetSetScriptEdit.h"
#include "../GetSet/GetSetScripting.h"

namespace GetSetGui
{

	GetSetScriptEdit::GetSetScriptEdit(QWidget *parent)
		: QMainWindow(parent)
	{
		setupToolBarsAndMenus();
		setupEditor();

		setCentralWidget(editor);
		setWindowTitle(tr("GetSet Script Editor"));

		setWindowIcon(style()->standardIcon(QStyle::SP_FileIcon));

		resize(600,480);
	}

	void GetSetScriptEdit::setupEditor()
	{
		QFont font;
		font.setFamily("Courier");
		font.setFixedPitch(true);
		font.setPointSize(12);

		editor = new QTextEdit;
		editor->setFont(font);

		highlighter = new GetSetScriptSyntaxHighlighter(editor->document());

		std::string appname=GetSet<>("Application");

		if (!appname.empty())
		{
			QFile file((appname+".getset").c_str());
			if (file.open(QFile::ReadOnly | QFile::Text))
				editor->setPlainText(file.readAll());
		}
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
			fileWriteString(fileName, editor->toPlainText().toStdString());
	}

	void GetSetScriptEdit::closeEvent(QCloseEvent *event)
	{
		if(QMessageBox::question(NULL, "Save Changes?", "Any changes will be discarded. Would you like to save before closing?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
			saveFile();
	}

	void GetSetScriptEdit::help()
	{
		std::cout << GetSetScriptParser::global().synopsis(editor->textCursor().selectedText().toStdString(),true) << std::endl;
	}

	void GetSetScriptEdit::execute()
	{
		std::string script=editor->toPlainText().toStdString();
		GetSetScriptParser::global().parse(script);
		editor->setExtraSelections(QList<QTextEdit::ExtraSelection>());
	}

	void GetSetScriptEdit::force_stop()
	{
		GetSetScriptParser::global().force_stop();
	}

	void GetSetScriptEdit::executeSelected()
	{
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
				while (i>0||block.eof())
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
		bool sucess=GetSetScriptParser::global().parse(selection);

		// Highlight executed code
		QList<QTextEdit::ExtraSelection> extraSelections;
		QTextEdit::ExtraSelection highlight_executed;
		highlight_executed.format.setBackground(QColor(sucess?Qt::green:Qt::red).lighter(175));
		highlight_executed.cursor=tc;
		highlight_executed.format.setProperty(QTextFormat::FullWidthSelection, true);
		extraSelections.append(highlight_executed);
		editor->setExtraSelections(extraSelections);

		// If we evecute things line-byline, move cursor
		if (execline)
		{
			tc.setPosition(highlight_executed.cursor.selectionEnd()+1);
			editor->setTextCursor(tc);
		}

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
		std::string keywords="\\b(?:help|call|concat|define|discard|echo|eval|exit|file|for|if|input|set|while|who|with|enddefine|endfor|endif|endwhile)\\b";
		std::string keywords_cosub="\\b(?:each|in|from|to|than|step|and|ini\\s+(?:|load|save|get\\s+var|set\\s+key|remove\\s+key)|run|trigger)\\b";
		std::string keywords_cmpar="\\b(?:not|strequal|numequal|gequal|lequal|greater|less)\\b";
		std::string keywords_cmpop="\\b(?:plus|minus|times|over)\\b";

		addHighlightingRule(keywords,0,0,128,true);
		addHighlightingRule(keywords_cosub,0,0,128,false);
		addHighlightingRule(keywords_cmpar,0,0,0,true);
		addHighlightingRule(keywords_cmpop,128,0,0,false,false,false);

		addHighlightingRule("\\bvalue\\s+(?:\"[^\"]*\"|\\S+\\b)",128,0,0,false,false,false); 
		addHighlightingRule("\\b(:?var|function)\\s+(?:\"[^\"]*\"|\\S+\\b)",128,0,0,false,false,true); 
		addHighlightingRule("\\b(?:key|trigger|section)\\s+(?:\"[^\"]*\"|\\S+\\b)",200,128,0,false,false,true); 
//		addHighlightingRule("\\b(?:key|value|var)\\s+(?:\"[^\"]*\"|\\S+\\b)",128,128,0,false); 
		
		addHighlightingRule("#[^\n]*",0,128,0,false);

		QTextCharFormat format;
		format.setFontWeight(QFont::Bold);
		format.setForeground(QColor(0,0,0));

	//	overridingRules.push_back(std::pair<std::string,QTextCharFormat >("\\bkey\\b)",format));
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
