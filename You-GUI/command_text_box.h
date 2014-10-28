//@author A0094446X
#pragma once
#ifndef YOU_GUI_COMMAND_TEXT_BOX_H_
#define YOU_GUI_COMMAND_TEXT_BOX_H_
#include <QTextEdit>

namespace You {
namespace GUI {

class CommandTextBox : public QTextEdit {
	Q_OBJECT

public:
	explicit CommandTextBox(QWidget *parent = 0);
	~CommandTextBox();

	void setCompleter();
	QCompleter *completer;

protected:
	void keyPressEvent(QKeyEvent *e);
	void focusInEvent(QFocusEvent *e);

	private slots:
	void insertCompletion(const QString &completion);

private:
	QString textUnderCursor() const;
};
}  // namespace GUI
}  // namespace You
#endif  // YOU_GUI_COMMAND_TEXT_BOX_H_
