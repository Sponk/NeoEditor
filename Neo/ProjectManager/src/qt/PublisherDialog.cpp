#include "PublisherDialog.h"
#include "ui_PublisherDialog.h"
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>

PublisherDialog::PublisherDialog(Project& project, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PublisherDialog),
	project(project)
{
	ui->setupUi(this);
	
	// Find all generators
	QProcess process;
	process.start("./neo-publisher -hg");
	process.waitForFinished(-1); // FIXME: Wait forever or timeout?
	
	QString stdout = process.readAllStandardOutput();
	stdout.remove('\t');
	
	QStringList lines = stdout.split("\n");
	lines.removeFirst();
	lines.removeLast();
	
	ui->generatorsComboBox->addItems(lines);
}

PublisherDialog::~PublisherDialog()
{
	delete ui;
}

void PublisherDialog::publish()
{
	QProcess process;
	process.setReadChannel(QProcess::StandardOutput);
	process.start("./neo-publisher", QStringList()  << "-i" << project.getFilePath().c_str()
							<< "-o" << ui->outputEdit->text()
							<< "-g" << ui->generatorsComboBox->currentText()
							<< "-p" << "./NeoPlayer2" << "-v");
	
	ui->commandTextEdit->clear();
	process.waitForStarted();
	while(process.state() != QProcess::NotRunning)
	{
		process.waitForReadyRead();
		ui->commandTextEdit->appendPlainText(process.readLine());
		
		QApplication::processEvents();
	}
	
	if(process.exitCode() != 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("Could not publish project!"));
	}
	else
	{
		QMessageBox::information(this, tr("Success"), tr("Successfully published project!"));
	}
	ui->commandTextEdit->appendPlainText("Finished");
}

void PublisherDialog::findOutputDirectory()
{
	ui->outputEdit->setText(QFileDialog::getExistingDirectory(this, tr("Find Project Directory")));
}
