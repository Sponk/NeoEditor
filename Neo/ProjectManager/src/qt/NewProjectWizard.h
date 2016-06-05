#ifndef NEWPROJECTWIZARD_H
#define NEWPROJECTWIZARD_H

#include <QWizard>
#include <QString>
#include "../ProjectBackend.h"

namespace Ui {
class NewProjectWizard;
}

class NewProjectWizard : public QWizard
{
	Q_OBJECT

public:
	explicit NewProjectWizard(QWidget *parent = 0);
	~NewProjectWizard();

private:
	Ui::NewProjectWizard *ui;

public:
	QString getProjectName() const { return m_projectName; }
	QString getProjectPath() const { return m_projectPath; }
	QString getProjectTemplate() const { return m_projectTemplate; }
	bool accepted() const { return m_accepted; }
	void setTemplates(std::map<std::string, Project>& templates);

private:
	QString m_projectName, m_projectPath, m_projectTemplate;
	bool m_accepted;

public slots:
	void accept();
	void findProjectFile();
};

#endif // NEWPROJECTWIZARD_H
