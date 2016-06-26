#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(const QStringList& entries,
			   QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Dialog)
{
	ui->setupUi(this);

	ui->listWidget->addItems(entries);
}

Dialog::~Dialog()
{
	delete ui;
}
