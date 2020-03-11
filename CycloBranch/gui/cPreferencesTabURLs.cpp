#include "gui/cPreferencesTabURLs.h"


cPreferencesTabURLs::cPreferencesTabURLs(cGlobalPreferences& globalpreferences, QWidget *parent) : QWidget(parent) {
	gridlayout = new QGridLayout();

	labelurl1 = new QLabel("Norine : ");
	lineediturl1 = new QLineEdit();
	lineediturl1->setText(globalpreferences.bookmarkurl1);
	gridlayout->addWidget(labelurl1, 0, 0);
	gridlayout->addWidget(lineediturl1, 0, 1);

	labelurl2 = new QLabel("BBDGNC : ");
	lineediturl2 = new QLineEdit();
	lineediturl2->setText(globalpreferences.bookmarkurl2);
	gridlayout->addWidget(labelurl2, 1, 0);
	gridlayout->addWidget(lineediturl2, 1, 1);

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


void cPreferencesTabURLs::setPreferences(cGlobalPreferences& globalpreferences) {
	globalpreferences.bookmarkurl1 = lineediturl1->text();
	globalpreferences.bookmarkurl2 = lineediturl2->text();
}


void cPreferencesTabURLs::resetPreferences(cGlobalPreferences& globalpreferences) {
	lineediturl1->setText(globalpreferences.bookmarkurl1);
	lineediturl2->setText(globalpreferences.bookmarkurl2);
}


cPreferencesTabURLs::~cPreferencesTabURLs() {
	delete labelurl1;
	delete lineediturl1;

	delete labelurl2;
	delete lineediturl2;

	delete defaultbutton;
	delete hlayout;
	delete hwidget;

	delete gridlayout;
	delete gridlayoutwidget;

	delete mainlayout;
}


void cPreferencesTabURLs::defaultButtonReleased() {
	cGlobalPreferences gp;
	lineediturl1->setText(gp.bookmarkurl1);
	lineediturl2->setText(gp.bookmarkurl2);
}

