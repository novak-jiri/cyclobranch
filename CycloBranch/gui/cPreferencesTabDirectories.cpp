#include "gui/cPreferencesTabDirectories.h"


cPreferencesTabDirectories::cPreferencesTabDirectories(cGlobalPreferences& globalpreferences, QWidget *parent) : QWidget(parent) {
	gridlayout = new QGridLayout();
	
	labelsettingsdefaultdir = new QLabel("Settings : ");
	lineeditsettingsdefaultdir = new QLineEdit();
	lineeditsettingsdefaultdir->setText(globalpreferences.settingsdefaultdir);
	buttonsettingsdefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelsettingsdefaultdir, 0, 0);
	gridlayout->addWidget(lineeditsettingsdefaultdir, 0, 1);
	gridlayout->addWidget(buttonsettingsdefaultdir, 0, 2);

	labelpeaklistsdefaultdir = new QLabel("Peaklists : ");
	lineeditpeaklistsdefaultdir = new QLineEdit();
	lineeditpeaklistsdefaultdir->setText(globalpreferences.peaklistsdefaultdir);
	buttonpeaklistsdefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelpeaklistsdefaultdir, 1, 0);
	gridlayout->addWidget(lineeditpeaklistsdefaultdir, 1, 1);
	gridlayout->addWidget(buttonpeaklistsdefaultdir, 1, 2);

	labelblocksdefaultdir = new QLabel("Building Blocks : ");
	lineeditblocksdefaultdir = new QLineEdit();
	lineeditblocksdefaultdir->setText(globalpreferences.blocksdefaultdir);
	buttonblocksdefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelblocksdefaultdir, 2, 0);
	gridlayout->addWidget(lineeditblocksdefaultdir, 2, 1);
	gridlayout->addWidget(buttonblocksdefaultdir, 2, 2);

	labelsequencesdefaultdir = new QLabel("Sequences/Compounds : ");
	lineeditsequencesdefaultdir = new QLineEdit();
	lineeditsequencesdefaultdir->setText(globalpreferences.sequencesdefaultdir);
	buttonsequencesdefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelsequencesdefaultdir, 3, 0);
	gridlayout->addWidget(lineeditsequencesdefaultdir, 3, 1);
	gridlayout->addWidget(buttonsequencesdefaultdir, 3, 2);

	labelmodificationsdefaultdir = new QLabel("Modifications : ");
	lineeditmodificationsdefaultdir = new QLineEdit();
	lineeditmodificationsdefaultdir->setText(globalpreferences.modificationsdefaultdir);
	buttonmodificationsdefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelmodificationsdefaultdir, 4, 0);
	gridlayout->addWidget(lineeditmodificationsdefaultdir, 4, 1);
	gridlayout->addWidget(buttonmodificationsdefaultdir, 4, 2);

	labelopenopticalimagedefaultdir = new QLabel("Optical Images : ");
	lineeditopenopticalimagedefaultdir = new QLineEdit();
	lineeditopenopticalimagedefaultdir->setText(globalpreferences.openopticalimagedir);
	buttonopenopticalimagedefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelopenopticalimagedefaultdir, 5, 0);
	gridlayout->addWidget(lineeditopenopticalimagedefaultdir, 5, 1);
	gridlayout->addWidget(buttonopenopticalimagedefaultdir, 5, 2);

	labelopenhistologyimagedefaultdir = new QLabel("Histology Images : ");
	lineeditopenhistologyimagedefaultdir = new QLineEdit();
	lineeditopenhistologyimagedefaultdir->setText(globalpreferences.openhistologyimagedir);
	buttonopenhistologyimagedefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelopenhistologyimagedefaultdir, 6, 0);
	gridlayout->addWidget(lineeditopenhistologyimagedefaultdir, 6, 1);
	gridlayout->addWidget(buttonopenhistologyimagedefaultdir, 6, 2);

	labelopenmicroscopyimagedefaultdir = new QLabel("Microscopy Images : ");
	lineeditopenmicroscopyimagedefaultdir = new QLineEdit();
	lineeditopenmicroscopyimagedefaultdir->setText(globalpreferences.openmicroscopyimagedir);
	buttonopenmicroscopyimagedefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelopenmicroscopyimagedefaultdir, 7, 0);
	gridlayout->addWidget(lineeditopenmicroscopyimagedefaultdir, 7, 1);
	gridlayout->addWidget(buttonopenmicroscopyimagedefaultdir, 7, 2);

	labelresultsdefaultdir = new QLabel("Results : ");
	lineeditresultsdefaultdir = new QLineEdit();
	lineeditresultsdefaultdir->setText(globalpreferences.resultsdefaultdir);
	buttonresultsdefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelresultsdefaultdir, 8, 0);
	gridlayout->addWidget(lineeditresultsdefaultdir, 8, 1);
	gridlayout->addWidget(buttonresultsdefaultdir, 8, 2);

	labelexportcsvdefaultdir = new QLabel("Export CSV : ");
	lineeditexportcsvdefaultdir = new QLineEdit();
	lineeditexportcsvdefaultdir->setText(globalpreferences.exportcsvdefaultdir);
	buttonexportcsvdefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelexportcsvdefaultdir, 9, 0);
	gridlayout->addWidget(lineeditexportcsvdefaultdir, 9, 1);
	gridlayout->addWidget(buttonexportcsvdefaultdir, 9, 2);

	labelexporthtmldefaultdir = new QLabel("Export HTML : ");
	lineeditexporthtmldefaultdir = new QLineEdit();
	lineeditexporthtmldefaultdir->setText(globalpreferences.exporthtmldefaultdir);
	buttonexporthtmldefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelexporthtmldefaultdir, 10, 0);
	gridlayout->addWidget(lineeditexporthtmldefaultdir, 10, 1);
	gridlayout->addWidget(buttonexporthtmldefaultdir, 10, 2);

	labelexportimagedefaultdir = new QLabel("Export Images : ");
	lineeditexportimagedefaultdir = new QLineEdit();
	lineeditexportimagedefaultdir->setText(globalpreferences.exportimagedefaultdir);
	buttonexportimagedefaultdir = new QPushButton("Browse");
	gridlayout->addWidget(labelexportimagedefaultdir, 11, 0);
	gridlayout->addWidget(lineeditexportimagedefaultdir, 11, 1);
	gridlayout->addWidget(buttonexportimagedefaultdir, 11, 2);

	gridlayoutwidget = new QWidget();
	gridlayoutwidget->setLayout(gridlayout);

	defaultbutton = new QPushButton("Default");
	hlayout = new QHBoxLayout();
	hlayout->addWidget(defaultbutton);
	hlayout->addStretch(1);
	hwidget = new QWidget();
	hwidget->setLayout(hlayout);

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(gridlayoutwidget);
	mainlayout->addWidget(hwidget);
	mainlayout->addStretch(1);

	setLayout(mainlayout);

	connect(buttonsettingsdefaultdir, SIGNAL(released()), this, SLOT(settingsButtonReleased()));
	connect(buttonpeaklistsdefaultdir, SIGNAL(released()), this, SLOT(peaklistsButtonReleased()));
	connect(buttonblocksdefaultdir, SIGNAL(released()), this, SLOT(blocksButtonReleased()));
	connect(buttonsequencesdefaultdir, SIGNAL(released()), this, SLOT(sequencesButtonReleased()));
	connect(buttonmodificationsdefaultdir, SIGNAL(released()), this, SLOT(modificationsButtonReleased()));
	connect(buttonopenopticalimagedefaultdir, SIGNAL(released()), this, SLOT(openOpticalImageButtonReleased()));
	connect(buttonopenhistologyimagedefaultdir, SIGNAL(released()), this, SLOT(openHistologyImageButtonReleased()));
	connect(buttonopenmicroscopyimagedefaultdir, SIGNAL(released()), this, SLOT(openMicroscopyImageButtonReleased()));
	connect(buttonresultsdefaultdir, SIGNAL(released()), this, SLOT(resultsButtonReleased()));
	connect(buttonexportcsvdefaultdir, SIGNAL(released()), this, SLOT(exportCSVButtonReleased()));
	connect(buttonexporthtmldefaultdir, SIGNAL(released()), this, SLOT(exportHTMLButtonReleased()));
	connect(buttonexportimagedefaultdir, SIGNAL(released()), this, SLOT(exportImageButtonReleased()));
	connect(defaultbutton, SIGNAL(released()), this, SLOT(defaultButtonReleased()));
}


void cPreferencesTabDirectories::setPreferences(cGlobalPreferences& globalpreferences) {
	globalpreferences.settingsdefaultdir = lineeditsettingsdefaultdir->text();
	globalpreferences.peaklistsdefaultdir = lineeditpeaklistsdefaultdir->text();
	globalpreferences.blocksdefaultdir = lineeditblocksdefaultdir->text();
	globalpreferences.sequencesdefaultdir = lineeditsequencesdefaultdir->text();
	globalpreferences.modificationsdefaultdir = lineeditmodificationsdefaultdir->text();
	globalpreferences.openopticalimagedir = lineeditopenopticalimagedefaultdir->text();
	globalpreferences.openhistologyimagedir = lineeditopenhistologyimagedefaultdir->text();
	globalpreferences.openmicroscopyimagedir = lineeditopenmicroscopyimagedefaultdir->text();
	globalpreferences.resultsdefaultdir = lineeditresultsdefaultdir->text();
	globalpreferences.exportcsvdefaultdir = lineeditexportcsvdefaultdir->text();
	globalpreferences.exporthtmldefaultdir = lineeditexporthtmldefaultdir->text();
	globalpreferences.exportimagedefaultdir = lineeditexportimagedefaultdir->text();
}


void cPreferencesTabDirectories::resetPreferences(cGlobalPreferences& globalpreferences) {
	lineeditsettingsdefaultdir->setText(globalpreferences.settingsdefaultdir);
	lineeditpeaklistsdefaultdir->setText(globalpreferences.peaklistsdefaultdir);
	lineeditblocksdefaultdir->setText(globalpreferences.blocksdefaultdir);
	lineeditsequencesdefaultdir->setText(globalpreferences.sequencesdefaultdir);
	lineeditmodificationsdefaultdir->setText(globalpreferences.modificationsdefaultdir);
	lineeditopenopticalimagedefaultdir->setText(globalpreferences.openopticalimagedir);
	lineeditopenhistologyimagedefaultdir->setText(globalpreferences.openhistologyimagedir);
	lineeditopenmicroscopyimagedefaultdir->setText(globalpreferences.openmicroscopyimagedir);
	lineeditresultsdefaultdir->setText(globalpreferences.resultsdefaultdir);
	lineeditexportcsvdefaultdir->setText(globalpreferences.exportcsvdefaultdir);
	lineeditexporthtmldefaultdir->setText(globalpreferences.exporthtmldefaultdir);
	lineeditexportimagedefaultdir->setText(globalpreferences.exportimagedefaultdir);
}


cPreferencesTabDirectories::~cPreferencesTabDirectories() {
	delete labelsettingsdefaultdir;
	delete lineeditsettingsdefaultdir;
	delete buttonsettingsdefaultdir;

	delete labelpeaklistsdefaultdir;
	delete lineeditpeaklistsdefaultdir;
	delete buttonpeaklistsdefaultdir;

	delete labelblocksdefaultdir;
	delete lineeditblocksdefaultdir;
	delete buttonblocksdefaultdir;

	delete labelsequencesdefaultdir;
	delete lineeditsequencesdefaultdir;
	delete buttonsequencesdefaultdir;

	delete labelmodificationsdefaultdir;
	delete lineeditmodificationsdefaultdir;
	delete buttonmodificationsdefaultdir;

	delete labelopenopticalimagedefaultdir;
	delete lineeditopenopticalimagedefaultdir;
	delete buttonopenopticalimagedefaultdir;

	delete labelopenhistologyimagedefaultdir;
	delete lineeditopenhistologyimagedefaultdir;
	delete buttonopenhistologyimagedefaultdir;

	delete labelopenmicroscopyimagedefaultdir;
	delete lineeditopenmicroscopyimagedefaultdir;
	delete buttonopenmicroscopyimagedefaultdir;

	delete labelresultsdefaultdir;
	delete lineeditresultsdefaultdir;
	delete buttonresultsdefaultdir;

	delete labelexportcsvdefaultdir;
	delete lineeditexportcsvdefaultdir;
	delete buttonexportcsvdefaultdir;

	delete labelexporthtmldefaultdir;
	delete lineeditexporthtmldefaultdir;
	delete buttonexporthtmldefaultdir;

	delete labelexportimagedefaultdir;
	delete lineeditexportimagedefaultdir;
	delete buttonexportimagedefaultdir;

	delete defaultbutton;
	delete hlayout;
	delete hwidget;

	delete gridlayout;
	delete gridlayoutwidget;

	delete mainlayout;
}


void cPreferencesTabDirectories::settingsButtonReleased() {
	QString currentdir = lineeditsettingsdefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditsettingsdefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::peaklistsButtonReleased() {
	QString currentdir = lineeditpeaklistsdefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditpeaklistsdefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::blocksButtonReleased() {
	QString currentdir = lineeditblocksdefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditblocksdefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::sequencesButtonReleased() {
	QString currentdir = lineeditsequencesdefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditsequencesdefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::modificationsButtonReleased() {
	QString currentdir = lineeditmodificationsdefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditmodificationsdefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::openOpticalImageButtonReleased() {
	QString currentdir = lineeditopenopticalimagedefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditopenopticalimagedefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::openHistologyImageButtonReleased() {
	QString currentdir = lineeditopenhistologyimagedefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditopenhistologyimagedefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::openMicroscopyImageButtonReleased() {
	QString currentdir = lineeditopenmicroscopyimagedefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditopenmicroscopyimagedefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::resultsButtonReleased() {
	QString currentdir = lineeditresultsdefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditresultsdefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::exportCSVButtonReleased() {
	QString currentdir = lineeditexportcsvdefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditexportcsvdefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::exportHTMLButtonReleased() {
	QString currentdir = lineeditexporthtmldefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditexporthtmldefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::exportImageButtonReleased() {
	QString currentdir = lineeditexportimagedefaultdir->text();
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select directory ..."), currentdir, QFileDialog::ShowDirsOnly);
	if (!newdir.isEmpty()) {
		lineeditexportimagedefaultdir->setText(newdir);
	}
}


void cPreferencesTabDirectories::defaultButtonReleased() {
	cGlobalPreferences gp;
	lineeditsettingsdefaultdir->setText(gp.settingsdefaultdir);
	lineeditpeaklistsdefaultdir->setText(gp.peaklistsdefaultdir);
	lineeditblocksdefaultdir->setText(gp.blocksdefaultdir);
	lineeditsequencesdefaultdir->setText(gp.sequencesdefaultdir);
	lineeditmodificationsdefaultdir->setText(gp.modificationsdefaultdir);
	lineeditopenopticalimagedefaultdir->setText(gp.openopticalimagedir);
	lineeditopenhistologyimagedefaultdir->setText(gp.openhistologyimagedir);
	lineeditopenmicroscopyimagedefaultdir->setText(gp.openmicroscopyimagedir);
	lineeditresultsdefaultdir->setText(gp.resultsdefaultdir);
	lineeditexportcsvdefaultdir->setText(gp.exportcsvdefaultdir);
	lineeditexporthtmldefaultdir->setText(gp.exporthtmldefaultdir);
	lineeditexportimagedefaultdir->setText(gp.exportimagedefaultdir);
}

