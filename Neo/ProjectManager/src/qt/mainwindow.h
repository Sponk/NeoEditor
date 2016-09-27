#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../ProjectBackend.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	ProjectBackend m_backend;
	QString m_configFile;

public slots:
	void createProject();
	void importProject();
	void openSelectedProject();
	void publishSelectedProject();
	void removeProject();
	void importTemplate();
	void removeTemplate();
	void updateTemplateData(QString name);
};

#endif // MAINWINDOW_H
