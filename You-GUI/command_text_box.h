//@author A0094446X
#pragma once
#ifndef YOU_GUI_COMMAND_TEXT_BOX_H_
#define YOU_GUI_COMMAND_TEXT_BOX_H_
#include <QTextEdit>
#include "syntax_highlighter.h"
namespace You {
namespace GUI {

class CommandTextBox : public QTextEdit {
	Q_OBJECT

public:
	explicit CommandTextBox(QWidget *parent = 0);
	~CommandTextBox();

	void setup();
	void setCompleter();
	QCompleter *completer;

signals:
	void enterKey();

protected:
	void keyPressEvent(QKeyEvent *e);
	void focusInEvent(QFocusEvent *e);
	void wheelEvent(QWheelEvent *e);
	void dropEvent(QDropEvent * e);
	private slots:
	void insertCompletion(const QString &completion);

private:
	std::unique_ptr<SyntaxHighlighter> syntaxHighlighter;
	std::list<std::wstring> commandHistory;
	std::list<std::wstring>::iterator historyIndex;
	QString textUnderCursor() const;
	void defaultKeyHandling(QKeyEvent *e);
	void completerKeyHandling(QKeyEvent *e);
};
}  // namespace GUI
}  // namespace You
#endif  // YOU_GUI_COMMAND_TEXT_BOX_H_
