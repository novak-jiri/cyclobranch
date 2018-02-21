#include "gui/cFindDialog.h"

#include "gui/cSpectrumDetailWidget.h"


cFindDialog::cFindDialog(QWidget* parent) {
	this->parent = parent;

	searchline = new QLineEdit();
	searchbutton = new QPushButton("Find");
	connect(searchbutton, SIGNAL(released()), this, SLOT(findAll()));

	matchcase = new QCheckBox();
	matchwholeword = new QCheckBox();

	searchbox = new QHBoxLayout();
	searchbox->addWidget(searchline);
	searchbox->addWidget(searchbutton);

	formlayout = new QFormLayout();
	formlayout->addRow("Find what: ", searchbox);
	formlayout->addRow("Match case: ", matchcase);
	formlayout->addRow("Match whole word: ", matchwholeword);
	
	setLayout(formlayout);
	setWindowTitle("Find Text");
	setMinimumSize(400, 50);
}


cFindDialog::~cFindDialog() {
	delete searchline;
	delete searchbutton;
	delete searchbox;

	delete matchcase;
	delete matchwholeword;

	delete formlayout;
}


void cFindDialog::findAll() {
	close();

	QTextDocument::FindFlags flags = 0;

	if (matchcase->isChecked()) {
		flags |= QTextDocument::FindCaseSensitively;
	}

	if (matchwholeword->isChecked()) {
		flags |= QTextDocument::FindWholeWords;
	}

	((cSpectrumDetailWidget *)parent)->findAll(searchline->text(), flags);
}

