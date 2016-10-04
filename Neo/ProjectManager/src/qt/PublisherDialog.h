#ifndef PUBLISHERDIALOG_H
#define PUBLISHERDIALOG_H

#include <QDialog>
#include "../ProjectBackend.h"

namespace Ui {
class PublisherDialog;
}

class PublisherDialog : public QDialog
{
	Q_OBJECT
	Project& project;
public:
	PublisherDialog(Project& project, QWidget* parent = 0);
	~PublisherDialog();

public slots:
	void publish();
	void findOutputDirectory();
	
private:
	Ui::PublisherDialog *ui;
};

#endif // PUBLISHERDIALOG_H
