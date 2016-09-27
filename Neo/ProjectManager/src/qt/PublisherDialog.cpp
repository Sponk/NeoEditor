#include "PublisherDialog.h"
#include "ui_PublisherDialog.h"
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>

#ifdef WIN32
#define PUBLISHER_EXEC ".\\neo-publisher.exe"
#define PLAYER_EXEC ".\\NeoPlayer2.exe"
#else
#define PUBLISHER_EXEC "./neo-publisher"
#define PLAYER_EXEC "./NeoPlayer2"
#endif

PublisherDialog::PublisherDialog(Project& project, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PublisherDialog),
	project(project)
{
	ui->setupUi(this);
	
	// Find all generators
	QProcess process;
	process.start(PUBLISHER_EXEC " -hg");
	process.waitForFinished(-1); // FIXME: Wait forever or timeout?
	
	QString out = process.readAllStandardOutput();
	out.remove('\t');
	
	QStringList lines = out.split("\n");
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
	process.start(PUBLISHER_EXEC, QStringList()  << "-i" << project.getFilePath().c_str()
							<< "-o" << ui->outputEdit->text()
							<< "-g" << ui->generatorsComboBox->currentText()
							<< "-p" << PLAYER_EXEC << "-v");
	
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
