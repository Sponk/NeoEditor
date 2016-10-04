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

#ifndef WIN32
static const std::pair<std::string, std::string> configs[] = {
	{"Linux NPK", "./NeoPlayer2"},
	{"Windows NPK", "./Arch/Win32/NeoPlayer2.exe"},
	{"Web NPK", "./Arch/Web/NeoWeb.html"},
	{"NPK", ""}
};

#else

static const std::pair<std::string, std::string> configs[] = {
	{"Windows NPK", ".\\NeoPlayer2.exe"},
	{"Linux NPK", ".\\Arch\\Linux\\NeoPlayer2"},
	{"Web NPK", ".\\Arch\\Web\\NeoWeb.html"},
	{"NPK", ""}
};
#endif

PublisherDialog::PublisherDialog(Project& project, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PublisherDialog),
	project(project)
{
	ui->setupUi(this);
	for(auto& g : configs)
		ui->generatorsComboBox->addItem(g.first.c_str());
}

PublisherDialog::~PublisherDialog()
{
	delete ui;
}

void PublisherDialog::publish()
{
	QProcess process;
	int idx = ui->generatorsComboBox->currentIndex();
	
	if(idx < 0)
		return;
	
	process.setReadChannel(QProcess::StandardOutput);
	process.start(PUBLISHER_EXEC, QStringList()  << "-i" << project.getFilePath().c_str()
							<< "-o" << ui->outputEdit->text()
							<< "-g \"" << ui->generatorsComboBox->currentText() << "\""
							<< "-p \"" <<  configs[idx].second.c_str() << "\" -v");
	
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
