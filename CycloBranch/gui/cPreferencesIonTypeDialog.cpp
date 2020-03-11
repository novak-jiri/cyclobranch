#include "gui/cPreferencesIonTypeDialog.h"

#include <QVBoxLayout>


cPreferencesIonTypeDialog::cPreferencesIonTypeDialog(QWidget *parent) : QDialog(parent) {
	gridlayout = new QGridLayout();

	labelname = new QLabel("Name : ");
	lineeditname = new QLineEdit();
	gridlayout->addWidget(labelname, 0, 0);
	gridlayout->addWidget(lineeditname, 0, 1);

	labelformula = new QLabel("Formula : ");
	lineeditformula = new QLineEdit();
	gridlayout->addWidget(labelformula, 1, 0);
	gridlayout->addWidget(lineeditformula, 1, 1);

	labelmode = new QLabel("Mode : ");
	comboboxmode = new QComboBox();
	comboboxmode->addItem("positive");
	comboboxmode->addItem("negative");
	gridlayout->addWidget(labelmode, 2, 0);
	gridlayout->addWidget(comboboxmode, 2, 1);

	labelmultiplier = new QLabel("M : ");
	spinboxmultiplier = new QSpinBox();
	spinboxmultiplier->setRange(1, 100);
	spinboxmultiplier->setSingleStep(1);
	gridlayout->addWidget(labelmultiplier, 3, 0);
	gridlayout->addWidget(spinboxmultiplier, 3, 1);

	gridlayoutwidget = new QWidget();
	gridlayoutwidget->setLayout(gridlayout);

	buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(buttonbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonbox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(gridlayoutwidget);
	mainlayout->addWidget(buttonbox);
	setLayout(mainlayout);

	resize(300, 200);

	setWindowTitle(tr("Define Ion Type"));
	setWindowIcon(QIcon(":/images/icons/preferences.png"));
}


void cPreferencesIonTypeDialog::getValues(cIonType& ion) {
	ion.name = lineeditname->text().toStdString();
	ion.formula = lineeditformula->text().toStdString();
	ion.positive = (comboboxmode->currentIndex() == 0) ? true : false;
	ion.multiplier = spinboxmultiplier->value();

	cSummaryFormula formula;
	formula.setFormula(ion.formula);
	ion.massdifference = formula.getMass();
}


void cPreferencesIonTypeDialog::setValues(cIonType& ion) {
	lineeditname->setText(ion.name.c_str());
	lineeditformula->setText(ion.formula.c_str());
	ion.positive ? comboboxmode->setCurrentIndex(0) : comboboxmode->setCurrentIndex(1);
	spinboxmultiplier->setValue(ion.multiplier);
}


cPreferencesIonTypeDialog::~cPreferencesIonTypeDialog() {
	delete labelname;
	delete lineeditname;

	delete labelformula;
	delete lineeditformula;

	delete labelmode;
	delete comboboxmode;

	delete labelmultiplier;
	delete spinboxmultiplier;

	delete gridlayout;
	delete gridlayoutwidget;

	delete buttonbox;

	delete mainlayout;
}

