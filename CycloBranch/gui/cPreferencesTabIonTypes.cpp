#include "gui/cPreferencesTabIonTypes.h"

#include "core/cFragmentIons.h"


cPreferencesTabIonTypes::cPreferencesTabIonTypes(cGlobalPreferences& globalpreferences, QWidget *parent) : QWidget(parent) {
	list = new QListWidget();
	resetPreferences(globalpreferences);

	addbutton = new QPushButton("Add");
	addbutton->setToolTip("Add a new item.");

	editbutton = new QPushButton("Edit");
	editbutton->setToolTip("Edit selected item.");

	removebutton = new QPushButton("Remove");
	removebutton->setToolTip("Remove selected item.");
	
	defaultbutton = new QPushButton("Default");
	defaultbutton->setToolTip("Set default items.");

	hlayout = new QHBoxLayout();
	hlayout->addWidget(addbutton);
	hlayout->addWidget(editbutton);
	hlayout->addWidget(removebutton);
	hlayout->addWidget(defaultbutton);
	hlayout->addStretch(1);

	hwidget = new QWidget();
	hwidget->setLayout(hlayout);

	vlayout = new QVBoxLayout();
	vlayout->addWidget(list);
	vlayout->addWidget(hwidget);

	setLayout(vlayout);

	connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listItemDoubleClicked(QListWidgetItem*)));
	connect(addbutton, SIGNAL(released()), this, SLOT(addButtonReleased()));
	connect(editbutton, SIGNAL(released()), this, SLOT(editButtonReleased()));
	connect(removebutton, SIGNAL(released()), this, SLOT(removeButtonReleased()));
	connect(defaultbutton, SIGNAL(released()), this, SLOT(defaultButtonReleased()));

	iontypedialog = new cPreferencesIonTypeDialog(this);
}


void cPreferencesTabIonTypes::setPreferences(cGlobalPreferences& globalpreferences) {
	globalpreferences.customions = localcustomions;
}


void cPreferencesTabIonTypes::resetPreferences(cGlobalPreferences& globalpreferences) {
	localcustomions = globalpreferences.customions;

	list->clear();
	for (auto& it : localcustomions) {
		list->addItem(it.name.c_str());
	}
}


cPreferencesTabIonTypes::~cPreferencesTabIonTypes() {
	delete addbutton;
	delete editbutton;
	delete removebutton;
	delete defaultbutton;

	delete hlayout;
	delete hwidget;

	delete vlayout;

	delete iontypedialog;
}


bool cPreferencesTabIonTypes::isValid(cIonType& ion, int row, string& errormessage) {
	errormessage = "";

	if (ion.name.empty()) {
		errormessage = "The name of ion is empty !";
		return false;
	}

	if (ion.formula.empty()) {
		errormessage = "The formula is empty !";
		return false;
	}

	if (ion.name[0] != '[') {
		errormessage = "The name of ion must begin with '[' !";
		return false;
	}

	if ((ion.name.substr(ion.name.size() - 2).compare("]+") != 0) && (ion.name.substr(ion.name.size() - 2).compare("]-") != 0)) {
		errormessage = "The name of ion must end with ']+' or ']-' !";
		return false;
	}

	if ((ion.name.substr(ion.name.size() - 2).compare("]+") == 0) && !ion.positive) {
		errormessage = "This ion cannot be defined in negative mode !";
		return false;
	}

	if ((ion.name.substr(ion.name.size() - 2).compare("]-") == 0) && ion.positive) {
		errormessage = "This ion cannot be defined in positive mode !";
		return false;
	}

	cSummaryFormula formula;
	formula.setFormula(ion.formula);
	if (!formula.isValid(errormessage)) {
		return false;
	}
	
	for (int i = 0; i < (int)localcustomions.size(); i++) {
		if (row != -1) {
			if (i == row) {
				continue;
			}
		}

		if (localcustomions[i].name.compare(ion.name) == 0) {
			errormessage = "A ion with the same name ";
			errormessage += ion.name.c_str();
			errormessage += " is already defined !";
			return false;
		}
	}

	return true;
}


void cPreferencesTabIonTypes::listItemDoubleClicked(QListWidgetItem* item) {
	editButtonReleased();
}


void cPreferencesTabIonTypes::addButtonReleased() {
	cIonType ion;
	iontypedialog->setValues(ion);

	string errormessage;
	bool valid;

	do {
		if (iontypedialog->exec() != QDialog::Accepted) {
			return;
		}
		iontypedialog->getValues(ion);
		valid = isValid(ion, -1, errormessage);
		if (!valid) {
			QMessageBox msgBox;
			msgBox.setText(errormessage.c_str());
			msgBox.exec();
		}
	} while (!valid);

	list->addItem(ion.name.c_str());
	localcustomions.push_back(ion);
}


void cPreferencesTabIonTypes::editButtonReleased() {
	cIonType ion;

	string errormessage;
	bool valid;

	int row = list->currentRow();

	if ((list->count() > 0) && (row != -1)) {
		iontypedialog->setValues(localcustomions[row]);

		do {
			if (iontypedialog->exec() != QDialog::Accepted) {
				return;
			}
			iontypedialog->getValues(ion);
			valid = isValid(ion, row, errormessage);
			if (!valid) {
				QMessageBox msgBox;
				msgBox.setText(errormessage.c_str());
				msgBox.exec();
			}
		} while (!valid);

		localcustomions[row] = ion;
		list->item(row)->setText(localcustomions[row].name.c_str());
	}
}


void cPreferencesTabIonTypes::removeButtonReleased() {
	int row = list->currentRow();
	if ((list->count() > 0) && (row != -1)) {
		delete list->takeItem(row);
		localcustomions.erase(localcustomions.begin() + row);
	}
}


void cPreferencesTabIonTypes::defaultButtonReleased() {
	cGlobalPreferences gp;
	resetPreferences(gp);
}

