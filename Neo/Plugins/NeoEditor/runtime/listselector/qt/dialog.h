#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
	Q_OBJECT

public:
	explicit Dialog(const QStringList& entries, QWidget *parent = 0);
	~Dialog();

	QString getSelection() const { return selection; }

private slots:
	void selectionChanged(QString str) { selection = str; }

private:
	Ui::Dialog *ui;

	QString selection;
};

#endif // DIALOG_H
