#include "gui/cPreferencesTabURLs.h"


cPreferencesTabURLs::cPreferencesTabURLs(cGlobalPreferences& globalpreferences, QWidget *parent) : QWidget(parent) {
	gridlayout = new QGridLayout();

	labelurl1 = new QLabel("Bookmark 1 : ");
	lineeditname1 = new QLineEdit();
	lineeditname1->setText(globalpreferences.bookmarkname1);
	lineediturl1 = new QLineEdit();
	lineediturl1->setText(globalpreferences.bookmarkurl1);
	gridlayout->addWidget(labelurl1, 0, 0);
	gridlayout->addWidget(lineeditname1, 0, 1);
	gridlayout->addWidget(lineediturl1, 0, 2);

	labelurl2 = new QLabel("Bookmark 2 : ");
	lineeditname2 = new QLineEdit();
	lineeditname2->setText(globalpreferences.bookmarkname2);
	lineediturl2 = new QLineEdit();
	lineediturl2->setText(globalpreferences.bookmarkurl2);
	gridlayout->addWidget(labelurl2, 1, 0);
	gridlayout->addWidget(lineeditname2, 1, 1);
	gridlayout->addWidget(lineediturl2, 1, 2);

	labelurl3 = new QLabel("Bookmark 3 : ");
	lineeditname3 = new QLineEdit();
	lineeditname3->setText(globalpreferences.bookmarkname3);
	lineediturl3 = new QLineEdit();
	lineediturl3->setText(globalpreferences.bookmarkurl3);
	gridlayout->addWidget(labelurl3, 2, 0);
	gridlayout->addWidget(lineeditname3, 2, 1);
	gridlayout->addWidget(lineediturl3, 2, 2);

	labelurl4 = new QLabel("Bookmark 4 : ");
	lineeditname4 = new QLineEdit();
	lineeditname4->setText(globalpreferences.bookmarkname4);
	lineediturl4 = new QLineEdit();
	lineediturl4->setText(globalpreferences.bookmarkurl4);
	gridlayout->addWidget(labelurl4, 3, 0);
	gridlayout->addWidget(lineeditname4, 3, 1);
	gridlayout->addWidget(lineediturl4, 3, 2);

	labelurl5 = new QLabel("Bookmark 5 : ");
	lineeditname5 = new QLineEdit();
	lineeditname5->setText(globalpreferences.bookmarkname5);
	lineediturl5 = new QLineEdit();
	lineediturl5->setText(globalpreferences.bookmarkurl5);
	gridlayout->addWidget(labelurl5, 4, 0);
	gridlayout->addWidget(lineeditname5, 4, 1);
	gridlayout->addWidget(lineediturl5, 4, 2);

	labelurl6 = new QLabel("Bookmark 6 : ");
	lineeditname6 = new QLineEdit();
	lineeditname6->setText(globalpreferences.bookmarkname6);
	lineediturl6 = new QLineEdit();
	lineediturl6->setText(globalpreferences.bookmarkurl6);
	gridlayout->addWidget(labelurl6, 5, 0);
	gridlayout->addWidget(lineeditname6, 5, 1);
	gridlayout->addWidget(lineediturl6, 5, 2);

	labelurl7 = new QLabel("Bookmark 7 : ");
	lineeditname7 = new QLineEdit();
	lineeditname7->setText(globalpreferences.bookmarkname7);
	lineediturl7 = new QLineEdit();
	lineediturl7->setText(globalpreferences.bookmarkurl7);
	gridlayout->addWidget(labelurl7, 6, 0);
	gridlayout->addWidget(lineeditname7, 6, 1);
	gridlayout->addWidget(lineediturl7, 6, 2);

	labelurl8 = new QLabel("Bookmark 8 : ");
	lineeditname8 = new QLineEdit();
	lineeditname8->setText(globalpreferences.bookmarkname8);
	lineediturl8 = new QLineEdit();
	lineediturl8->setText(globalpreferences.bookmarkurl8);
	gridlayout->addWidget(labelurl8, 7, 0);
	gridlayout->addWidget(lineeditname8, 7, 1);
	gridlayout->addWidget(lineediturl8, 7, 2);

	labelurl9 = new QLabel("Bookmark 9 : ");
	lineeditname9 = new QLineEdit();
	lineeditname9->setText(globalpreferences.bookmarkname9);
	lineediturl9 = new QLineEdit();
	lineediturl9->setText(globalpreferences.bookmarkurl9);
	gridlayout->addWidget(labelurl9, 8, 0);
	gridlayout->addWidget(lineeditname9, 8, 1);
	gridlayout->addWidget(lineediturl9, 8, 2);

	labelurl10 = new QLabel("Bookmark 10 : ");
	lineeditname10 = new QLineEdit();
	lineeditname10->setText(globalpreferences.bookmarkname10);
	lineediturl10 = new QLineEdit();
	lineediturl10->setText(globalpreferences.bookmarkurl10);
	gridlayout->addWidget(labelurl10, 9, 0);
	gridlayout->addWidget(lineeditname10, 9, 1);
	gridlayout->addWidget(lineediturl10, 9, 2);

	labelviewbookmarks = new QLabel("Show Toolbar");
	checkboxviewbookmarks = new QCheckBox();
	checkboxviewbookmarks->setChecked(globalpreferences.viewbookmarks);
	gridlayout->addWidget(labelviewbookmarks, 10, 0);
	gridlayout->addWidget(checkboxviewbookmarks, 10, 1);

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
	globalpreferences.bookmarkname1 = lineeditname1->text();
	globalpreferences.bookmarkurl1 = lineediturl1->text();

	globalpreferences.bookmarkname2 = lineeditname2->text();
	globalpreferences.bookmarkurl2 = lineediturl2->text();

	globalpreferences.bookmarkname3 = lineeditname3->text();
	globalpreferences.bookmarkurl3 = lineediturl3->text();

	globalpreferences.bookmarkname4 = lineeditname4->text();
	globalpreferences.bookmarkurl4 = lineediturl4->text();

	globalpreferences.bookmarkname5 = lineeditname5->text();
	globalpreferences.bookmarkurl5 = lineediturl5->text();

	globalpreferences.bookmarkname6 = lineeditname6->text();
	globalpreferences.bookmarkurl6 = lineediturl6->text();

	globalpreferences.bookmarkname7 = lineeditname7->text();
	globalpreferences.bookmarkurl7 = lineediturl7->text();

	globalpreferences.bookmarkname8 = lineeditname8->text();
	globalpreferences.bookmarkurl8 = lineediturl8->text();

	globalpreferences.bookmarkname9 = lineeditname9->text();
	globalpreferences.bookmarkurl9 = lineediturl9->text();

	globalpreferences.bookmarkname10 = lineeditname10->text();
	globalpreferences.bookmarkurl10 = lineediturl10->text();

	globalpreferences.viewbookmarks = checkboxviewbookmarks->isChecked();
}


void cPreferencesTabURLs::resetPreferences(cGlobalPreferences& globalpreferences) {
	lineeditname1->setText(globalpreferences.bookmarkname1);
	lineediturl1->setText(globalpreferences.bookmarkurl1);

	lineeditname2->setText(globalpreferences.bookmarkname2);
	lineediturl2->setText(globalpreferences.bookmarkurl2);

	lineeditname3->setText(globalpreferences.bookmarkname3);
	lineediturl3->setText(globalpreferences.bookmarkurl3);

	lineeditname4->setText(globalpreferences.bookmarkname4);
	lineediturl4->setText(globalpreferences.bookmarkurl4);

	lineeditname5->setText(globalpreferences.bookmarkname5);
	lineediturl5->setText(globalpreferences.bookmarkurl5);

	lineeditname6->setText(globalpreferences.bookmarkname6);
	lineediturl6->setText(globalpreferences.bookmarkurl6);

	lineeditname7->setText(globalpreferences.bookmarkname7);
	lineediturl7->setText(globalpreferences.bookmarkurl7);

	lineeditname8->setText(globalpreferences.bookmarkname8);
	lineediturl8->setText(globalpreferences.bookmarkurl8);

	lineeditname9->setText(globalpreferences.bookmarkname9);
	lineediturl9->setText(globalpreferences.bookmarkurl9);

	lineeditname10->setText(globalpreferences.bookmarkname10);
	lineediturl10->setText(globalpreferences.bookmarkurl10);

	checkboxviewbookmarks->setChecked(globalpreferences.viewbookmarks);
}


cPreferencesTabURLs::~cPreferencesTabURLs() {
	delete labelurl1;
	delete lineeditname1;
	delete lineediturl1;

	delete labelurl2;
	delete lineeditname2;
	delete lineediturl2;

	delete labelurl3;
	delete lineeditname3;
	delete lineediturl3;

	delete labelurl4;
	delete lineeditname4;
	delete lineediturl4;

	delete labelurl5;
	delete lineeditname5;
	delete lineediturl5;

	delete labelurl6;
	delete lineeditname6;
	delete lineediturl6;

	delete labelurl7;
	delete lineeditname7;
	delete lineediturl7;

	delete labelurl8;
	delete lineeditname8;
	delete lineediturl8;

	delete labelurl9;
	delete lineeditname9;
	delete lineediturl9;

	delete labelurl10;
	delete lineeditname10;
	delete lineediturl10;

	delete labelviewbookmarks;
	delete checkboxviewbookmarks;

	delete defaultbutton;
	delete hlayout;
	delete hwidget;

	delete gridlayout;
	delete gridlayoutwidget;

	delete mainlayout;
}


void cPreferencesTabURLs::defaultButtonReleased() {
	cGlobalPreferences gp;

	lineeditname1->setText(gp.bookmarkname1);
	lineediturl1->setText(gp.bookmarkurl1);

	lineeditname2->setText(gp.bookmarkname2);
	lineediturl2->setText(gp.bookmarkurl2);

	lineeditname3->setText(gp.bookmarkname3);
	lineediturl3->setText(gp.bookmarkurl3);

	lineeditname4->setText(gp.bookmarkname4);
	lineediturl4->setText(gp.bookmarkurl4);

	lineeditname5->setText(gp.bookmarkname5);
	lineediturl5->setText(gp.bookmarkurl5);

	lineeditname6->setText(gp.bookmarkname6);
	lineediturl6->setText(gp.bookmarkurl6);

	lineeditname7->setText(gp.bookmarkname7);
	lineediturl7->setText(gp.bookmarkurl7);

	lineeditname8->setText(gp.bookmarkname8);
	lineediturl8->setText(gp.bookmarkurl8);

	lineeditname9->setText(gp.bookmarkname9);
	lineediturl9->setText(gp.bookmarkurl9);

	lineeditname10->setText(gp.bookmarkname10);
	lineediturl10->setText(gp.bookmarkurl10);

	checkboxviewbookmarks->setChecked(gp.viewbookmarks);
}

