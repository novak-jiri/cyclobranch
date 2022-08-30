#include "gui/cPreferencesTabDataProcessing.h"


cPreferencesTabDataProcessing::cPreferencesTabDataProcessing(cGlobalPreferences& globalpreferences, QWidget *parent) : QWidget(parent) {
	gridlayout = new QGridLayout();

	labellinebafprocessingmethod = new QLabel("Line Data Converter (*.baf) : ");
	comboboxlinebafprocessingmethod = new QComboBox();
	comboboxlinebafprocessingmethod->addItem("CompassXport");
	comboboxlinebafprocessingmethod->addItem("CompassXtract");
	comboboxlinebafprocessingmethod->setCurrentIndex(globalpreferences.linebafprocessingmethod);
	gridlayout->addWidget(labellinebafprocessingmethod, 0, 0);
	gridlayout->addWidget(comboboxlinebafprocessingmethod, 0, 1);

	labelprofilebafprocessingmethod = new QLabel("Profile Data Converter (*.baf) : ");
	comboboxprofilebafprocessingmethod = new QComboBox();
	comboboxprofilebafprocessingmethod->addItem("CompassXport");
	comboboxprofilebafprocessingmethod->addItem("CompassXtract");
	comboboxprofilebafprocessingmethod->setCurrentIndex(globalpreferences.profilebafprocessingmethod);
	gridlayout->addWidget(labelprofilebafprocessingmethod, 1, 0);
	gridlayout->addWidget(comboboxprofilebafprocessingmethod, 1, 1);

	labelrawdataprocessingmethod = new QLabel("Peak Picking (non-vendor's file formats) : ");
	comboboxrawdataprocessingmethod = new QComboBox();
	comboboxrawdataprocessingmethod->addItem("OpenMS");
	comboboxrawdataprocessingmethod->addItem("Custom");
	//comboboxrawdataprocessingmethod->setCurrentIndex(globalpreferences.rawdataprocessingmethod);
	//gridlayout->addWidget(labelrawdataprocessingmethod, 2, 0);
	//gridlayout->addWidget(comboboxrawdataprocessingmethod, 2, 1);

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

	connect(defaultbutton, SIGNAL(released()), this, SLOT(defaultButtonReleased()));
}


void cPreferencesTabDataProcessing::setPreferences(cGlobalPreferences& globalpreferences) {
	globalpreferences.linebafprocessingmethod = comboboxlinebafprocessingmethod->currentIndex();
	globalpreferences.profilebafprocessingmethod = comboboxprofilebafprocessingmethod->currentIndex();

	globalpreferences.rawdataprocessingmethod = comboboxrawdataprocessingmethod->currentIndex();
}


void cPreferencesTabDataProcessing::resetPreferences(cGlobalPreferences& globalpreferences) {
	comboboxlinebafprocessingmethod->setCurrentIndex(globalpreferences.linebafprocessingmethod);
	comboboxprofilebafprocessingmethod->setCurrentIndex(globalpreferences.profilebafprocessingmethod);

	comboboxrawdataprocessingmethod->setCurrentIndex(globalpreferences.rawdataprocessingmethod);
}


cPreferencesTabDataProcessing::~cPreferencesTabDataProcessing() {
	delete labellinebafprocessingmethod;
	delete comboboxlinebafprocessingmethod;

	delete labelprofilebafprocessingmethod;
	delete comboboxprofilebafprocessingmethod;

	delete labelrawdataprocessingmethod;
	delete comboboxrawdataprocessingmethod;

	delete defaultbutton;
	delete hlayout;
	delete hwidget;

	delete gridlayout;
	delete gridlayoutwidget;

	delete mainlayout;
}


void cPreferencesTabDataProcessing::defaultButtonReleased() {
	cGlobalPreferences gp;

	comboboxlinebafprocessingmethod->setCurrentIndex(gp.linebafprocessingmethod);
	comboboxprofilebafprocessingmethod->setCurrentIndex(gp.profilebafprocessingmethod);

	comboboxrawdataprocessingmethod->setCurrentIndex(gp.rawdataprocessingmethod);
}

