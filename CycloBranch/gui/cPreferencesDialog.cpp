#include "gui/cPreferencesDialog.h"
#include "gui/cPreferencesTabIonTypes.h"
#include "gui/cPreferencesTabDataProcessing.h"
#include "gui/cPreferencesTabDirectories.h"
#include "gui/cPreferencesTabURLs.h"

#include <QVBoxLayout>


cPreferencesDialog::cPreferencesDialog(cGlobalPreferences& globalpreferences, QWidget *parent) : QDialog(parent) {
	tabiontypes = new cPreferencesTabIonTypes(globalpreferences);
	tabdataprocessing = new cPreferencesTabDataProcessing(globalpreferences);
	tabdirectories = new cPreferencesTabDirectories(globalpreferences);
	taburls = new cPreferencesTabURLs(globalpreferences);

	tabwidget = new QTabWidget();
	tabwidget->addTab(tabiontypes, tr("Ion Types"));
	//tabwidget->addTab(tabdataprocessing, tr("Data Processing"));
	tabwidget->addTab(tabdirectories, tr("Directories"));
	tabwidget->addTab(taburls, tr("URLs"));

	buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(buttonbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonbox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(tabwidget);
	mainlayout->addWidget(buttonbox);
	setLayout(mainlayout);

	resize(750, 600);

	setWindowTitle(tr("Preferences"));
	setWindowIcon(QIcon(":/images/icons/preferences.png"));
}


void cPreferencesDialog::setPreferences(cGlobalPreferences& globalpreferences) {
	tabiontypes->setPreferences(globalpreferences);
	tabdataprocessing->setPreferences(globalpreferences);
	tabdirectories->setPreferences(globalpreferences);
	taburls->setPreferences(globalpreferences);
}


void cPreferencesDialog::resetPreferences(cGlobalPreferences& globalpreferences) {
	tabiontypes->resetPreferences(globalpreferences);
	tabdataprocessing->resetPreferences(globalpreferences);
	tabdirectories->resetPreferences(globalpreferences);
	taburls->resetPreferences(globalpreferences);
}


cPreferencesDialog::~cPreferencesDialog() {
	delete tabiontypes;
	delete tabdataprocessing;
	delete tabdirectories;
	delete taburls;
	
	delete tabwidget;
	delete buttonbox;

	delete mainlayout;
}


void cPreferencesDialog::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/globalpreferences.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/globalpreferences.html").absoluteFilePath()));
		#endif
	}

	event->accept();
}

