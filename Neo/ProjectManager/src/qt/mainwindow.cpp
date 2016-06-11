#include <QtWidgets/QFileDialog>
#include <QtCore/QProcess>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NewProjectWizard.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_configFile = QDir::homePath() + QDir::separator() + ".neo-project-manager";

	QDir qdir = QDir(m_configFile);
	if(!qdir.exists())
		qdir.mkpath(m_configFile);

	m_configFile.append(QString() + QDir::separator() + "backend.cfg");
	m_backend.loadConfig(m_configFile.toStdString().c_str());

	// Default templates
	QString defaultTemplate = QDir::currentPath() + QDir::separator() + "templates" + QDir::separator();
	//m_backend.importTemplate((defaultTemplate + "cpp" + QDir::separator() + "cpp.nproj").toStdString().c_str());
	m_backend.importTemplate((defaultTemplate + QDir::separator() + "NeoSharpTemplate"
		+ QDir::separator() + "NeoSharpTemplate.nproj").toStdString().c_str());

	for(auto p : m_backend.getProjects())
		ui->listWidget->addItem(p.getName().c_str());

	for(auto p : m_backend.getTemplates())
		ui->templatesListWidget->addItem(p.first.c_str());
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::createProject()
{
	NewProjectWizard wizard;

	wizard.setTemplates(m_backend.getTemplates());
	wizard.exec();

	if(wizard.accepted())
	{
        bool result = m_backend.createProject(wizard.getProjectName().toStdString().c_str(),
                                              wizard.getProjectPath().toStdString().c_str(),
                                              wizard.getProjectTemplate().toStdString().c_str());

        if(!result)
        {
            QMessageBox::information(this, tr("Error"), tr("Could not create project!"));
            return;
        }

		ui->listWidget->addItem(m_backend.getProjects().back().getName().c_str());
		m_backend.saveConfig(m_configFile.toStdString().c_str());
	}
}

void MainWindow::importProject()
{
	QString project = QFileDialog::getOpenFileName(this, tr("Open Project"), "", "Projects (*.nproj)");

	if(project.isEmpty())
		return;

	size_t id = m_backend.importProject(project.toStdString().c_str());
	ui->listWidget->addItem(m_backend.getProjects()[id].getName().c_str());
	m_backend.saveConfig(m_configFile.toStdString().c_str());
}

void MainWindow::openSelectedProject()
{
	int selected = ui->listWidget->currentIndex().row();

	if(selected == -1)
		return;

	std::string file = m_backend.getProjects()[selected].getFilePath();

#ifndef WIN32
	QProcess::startDetached(QApplication::applicationDirPath() + QDir::separator() + "NeoPlayer2", QStringList() << "-p" << file.c_str());
#else
	QProcess::startDetached(QApplication::applicationDirPath() + QDir::separator() + "NeoPlayer2.exe", QStringList() << "-p" << file.c_str());
#endif
}

void MainWindow::removeProject()
{
	int selected = ui->listWidget->currentIndex().row();
	if(selected == -1)
		return;

	ui->listWidget->takeItem(selected);
	m_backend.removeProject(selected, false);
	m_backend.saveConfig(m_configFile.toStdString().c_str());
}

void MainWindow::importTemplate()
{
	QString project = QFileDialog::getOpenFileName(this, tr("Open Template"), "", "Projects (*.nproj)");

	if(project.isEmpty())
		return;

	std::string name = m_backend.importTemplate(project.toStdString().c_str());
	ui->templatesListWidget->addItem(name.c_str());
	m_backend.saveConfig(m_configFile.toStdString().c_str());
}

void MainWindow::removeTemplate()
{
	std::string selected = ui->templatesListWidget->currentItem()->text().toStdString();
	if(selected.empty())
		return;

	ui->templatesListWidget->takeItem(ui->templatesListWidget->currentRow());
	m_backend.getTemplates().erase(selected);
	m_backend.saveConfig(m_configFile.toStdString().c_str());
}

void MainWindow::updateTemplateData(QString name)
{
	const Project& p = m_backend.getTemplates()[name.toStdString()];
	ui->templateVersionEdit->setText(p.getVersion().c_str());
	ui->templateAuthorEdit->setText(p.getAuthor().c_str());
	ui->templateDescriptionEdit->setPlainText(p.getDescription().c_str());
}
