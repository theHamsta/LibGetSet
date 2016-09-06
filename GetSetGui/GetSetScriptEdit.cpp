
#include <QtWidgets>

#include "../GetSet/GetSet.hxx"
#include "GetSetScriptEdit.h"
#include "../GetSet/GetSetScripting.h"

namespace GetSetGui
{

	GetSetScriptEdit::GetSetScriptEdit(QWidget *parent)
		: QMainWindow(parent)
	{
		setupFileMenu();
		setupEditor();

		setCentralWidget(editor);
		setWindowTitle(tr("GetSet Script Editor"));

		resize(600,480);
	}

	void GetSetScriptEdit::newFile()
	{
		editor->clear();
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
		{
			saveFile();
		}
	}

	void GetSetScriptEdit::execute()
	{
		std::string script=editor->toPlainText().toStdString();
		GetSetScriptParser::global().parse(script);
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

	void GetSetScriptEdit::setupFileMenu()
	{
		QMenu *fileMenu = new QMenu(tr("&File"), this);
		menuBar()->addMenu(fileMenu);

		fileMenu->addAction(tr("&New"), this, SLOT(newFile()), QKeySequence::New);
		fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()), QKeySequence::Open);
		fileMenu->addAction(tr("&Save As..."), this, SLOT(saveFile()), QKeySequence::Save);
		fileMenu->addSeparator();
		fileMenu->addAction(tr("E&xecute"), this, SLOT(execute()), QKeySequence(QKeySequence(Qt::Key_F5)));
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
		std::string keywords="\\b(?:exit|help|who|set|file|input|echo|eval|call|with|function|if|while|for|endfunction|endif|endwhile|endfor)\\b";
		std::string keywords_cosub="\\b(?:each|in|from|to|step|as|load|save|run)\\b";
		std::string keywords_cmpar="\\b(?:not|strequal|numequal|gequal|lequal|greater|less)\\b";
		std::string keywords_cmpop="\\b(?:plus|minus|times|over)\\b";

		addHighlightingRule(keywords,0,0,128,true);
		addHighlightingRule(keywords_cosub,0,0,128,false);
		addHighlightingRule(keywords_cmpar,0,0,0,true);
		addHighlightingRule(keywords_cmpar,false,false,false,128,0,0);

		addHighlightingRule("\\bvalue\\s+(?:\"[^\"]*\"|\\S+\\b)",128,0,0,false,false,false); 
		addHighlightingRule("\\bkey\\s+(?:\"[^\"]*\"|\\S+\\b)",200,128,0,false,false,true); 
		addHighlightingRule("\\bvar\\s+(?:\"[^\"]*\"|\\S+\\b)",200,128,0,false,false,true); 
//		addHighlightingRule("\\b(?:key|value|var)\\s+(?:\"[^\"]*\"|\\S+\\b)",128,128,0,false); 
		
		addHighlightingRule("#[^\n]*",0,128,0,false);

		QTextCharFormat format;
		format.setFontWeight(QFont::Bold);
		format.setForeground(QColor(0,0,0));

	//	overridingRules.push_back(std::pair<std::string,QTextCharFormat >("\\bkey\\b)",format));
		overridingRules.push_back(std::pair<std::string,QTextCharFormat >("\\b(key|value|var)\\s+",format));
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
