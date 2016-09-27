#include <QtWidgets/QFileDialog>
#include "NewProjectWizard.h"
#include "ui_NewProjectWizard.h"

NewProjectWizard::NewProjectWizard(QWidget *parent) :
	QWizard(parent),
	ui(new Ui::NewProjectWizard),
	m_accepted(false)
{
	ui->setupUi(this);
}

NewProjectWizard::~NewProjectWizard()
{
	delete ui;
}

void NewProjectWizard::accept()
{
	m_projectName = ui->projectNameEdit->text();
	m_projectPath = ui->projectFileEdit->text();
	m_projectTemplate = ui->templateCombo->currentText();
	m_description = ui->descriptionEdit->toPlainText();
	m_accepted = true;
	close();
}

void NewProjectWizard::setTemplates(std::map<std::string, Project>& templates)
{
	for(auto p : templates)
		ui->templateCombo->addItem(p.first.c_str());
}

void NewProjectWizard::findProjectFile()
{
	QString directory = QFileDialog::getExistingDirectory(this, tr("Find Project Directory"));
	ui->projectFileEdit->setText(directory);
}
