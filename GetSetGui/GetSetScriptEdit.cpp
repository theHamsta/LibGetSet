
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
		std::string fileName = path.toStdString();

		if (fileName.empty())
			fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "GetSet scripts (*.getset);;All Files (*)").toStdString();

		if (!fileName.empty())
			fileWriteString(fileName, editor->toPlainText().toStdString());
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
		fileMenu->addAction(tr("&Save..."), this, SLOT(saveFile()), QKeySequence::Save);
		fileMenu->addAction(tr("E&xecute"), this, SLOT(execute()), QKeySequence(QKeySequence(Qt::CTRL + Qt::Key_X)));
	}

	void GetSetScriptEdit::setScript(const std::string& script)
	{
		editor->setPlainText(script.c_str());
	}

	GetSetScriptSyntaxHighlighter::GetSetScriptSyntaxHighlighter(QTextDocument *parent)
		: QSyntaxHighlighter(parent)
	{
		std::string separator="\\b";
		auto keyWordsClass1=stringToVector<std::string>(
			"exit;help;who;set;file;input;echo;eval;call;with;"
			"function;endfunction;if;endif;while;endwhile;for;endfor"
			,';');
		auto keyWordsClass2=stringToVector<std::string>(
			"each;in;from;to;step;as;load;save;run"
			,';');
		auto keyWordsClass3=stringToVector<std::string>(
			"not;strequal;numequal;gequal;lequal;greater;less;plus;minus;times"
			,';');

		QTextCharFormat formatClass1;
		formatClass1.setForeground(Qt::darkBlue);
		formatClass1.setFontWeight(QFont::Bold);
		QTextCharFormat formatClass2;
		formatClass2.setForeground(Qt::darkBlue);
		QTextCharFormat formatClass3;
		formatClass3.setFontWeight(QFont::Bold);

		QTextCharFormat varkeyvalue;
		varkeyvalue.setFontWeight(QFont::Bold);
		varkeyvalue.setForeground(Qt::darkYellow);

		QTextCharFormat quotedstring;
		quotedstring.setFontWeight(QFont::Bold);
		quotedstring.setForeground(Qt::darkGray);

		QTextCharFormat comment;
		comment.setForeground(Qt::darkGreen);
		
		// FIXME is it not more efficient to create one regex for all keywords in a class?
		for (auto it=keyWordsClass1.begin();it!=keyWordsClass1.end();++it)
			highlightingRules.push_back( std::pair<std::string,QTextCharFormat>
				(separator+*it+separator, formatClass1));
		for (auto it=keyWordsClass2.begin();it!=keyWordsClass2.end();++it)
			highlightingRules.push_back( std::pair<std::string,QTextCharFormat>
				(separator+*it+separator, formatClass2));
		for (auto it=keyWordsClass3.begin();it!=keyWordsClass3.end();++it)
			highlightingRules.push_back( std::pair<std::string,QTextCharFormat>
				(separator+*it+separator, formatClass3));

		highlightingRules.push_back (std::pair<std::string,QTextCharFormat >
			("\"[^\"]*\"", quotedstring)
			);
		highlightingRules.push_back (std::pair<std::string,QTextCharFormat >
			("\\bkey[ \t]+\"[^\"]*\"", varkeyvalue)
			);
		highlightingRules.push_back (std::pair<std::string,QTextCharFormat >
			("\\bvar[ \t]+\"[^\"]*\"", varkeyvalue)
			);
		highlightingRules.push_back (std::pair<std::string,QTextCharFormat >
			("\\bvalue[ \t]+\"[^\"]*\"", varkeyvalue)
			);


		highlightingRules.push_back (std::pair<std::string,QTextCharFormat >
			("#[^\n]*", comment)
			);

		// multi-line comment QRegExp("\".*\"");
		// function call rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
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
		setCurrentBlockState(0);



		//auto commentStartExpression = QRegExp("/\\*");
		//auto commentEndExpression = QRegExp("\\*/");
		//int startIndex = 0;
		//if (previousBlockState() != 1)
		//	startIndex = commentStartExpression.indexIn(text);

		//while (startIndex >= 0) {
		//	int endIndex = commentEndExpression.indexIn(text, startIndex);
		//	int commentLength;
		//	if (endIndex == -1) {
		//		setCurrentBlockState(1);
		//		commentLength = text.length() - startIndex;
		//	} else {
		//		commentLength = endIndex - startIndex
		//						+ commentEndExpression.matchedLength();
		//	}
		//	setFormat(startIndex, commentLength, multiLineCommentFormat);
		//	startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
		//}
	}
} // namespace GetSetGui
