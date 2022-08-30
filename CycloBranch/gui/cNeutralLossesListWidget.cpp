#include "gui/cNeutralLossesListWidget.h"
#include "core/cFragmentIons.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


cNeutralLossesListWidget::cNeutralLossesListWidget(bool showdefaultbutton, bool showhconbutton, QObject* parent) {
	this->showdefaultbutton = showdefaultbutton;
	this->showhconbutton = showhconbutton;
	this->parent = parent;

	vbox1 = new QVBoxLayout();
	vbox2 = new QVBoxLayout();
	hbox = new QHBoxLayout();

	list = new QListWidget();
	list->setMinimumHeight(175);
	list->setMaximumHeight(175);
	list->setSelectionMode(QAbstractItemView::MultiSelection);

	selectallbutton = new QPushButton("Select All");
	selectallbutton->setToolTip("Select all items.");
	clearallbutton = new QPushButton("Clear All");
	clearallbutton->setToolTip("Unselect all items.");
	addbutton = new QPushButton("Add");
	addbutton->setToolTip("Add a new item.");
	removebutton = new QPushButton("Remove");
	removebutton->setToolTip("Remove selected items.");
	if (showdefaultbutton) {
		defaultbutton = new QPushButton("Default");
		defaultbutton->setToolTip("Set a list of default neutral losses.");
	}
	if (showhconbutton) {
		hconbutton = new QPushButton("HCON");
		hconbutton->setToolTip("Set a list of default chemical elements (H, C, O, N, S, and P).");
	}

	vbox1->addWidget(list);
	vbox1->addStretch(1);

	vbox2->addWidget(selectallbutton);
	vbox2->addWidget(clearallbutton);
	vbox2->addWidget(addbutton);
	vbox2->addWidget(removebutton);
	if (showdefaultbutton) {
		vbox2->addWidget(defaultbutton);
	}
	if (showhconbutton) {
		vbox2->addWidget(hconbutton);
	}
	vbox2->addStretch(1);

	hbox->setMargin(0);
	hbox->addLayout(vbox1);
	hbox->addLayout(vbox2);

	connect(selectallbutton, SIGNAL(released()), this, SLOT(selectAllItems()));
	connect(clearallbutton, SIGNAL(released()), this, SLOT(clearAllItems()));
	connect(addbutton, SIGNAL(released()), this, SLOT(addEmptyItem()));
	connect(removebutton, SIGNAL(released()), this, SLOT(removeItem()));
	if (showdefaultbutton) {
		connect(defaultbutton, SIGNAL(released()), this, SLOT(setDefaultItems()));
	}
	if (showhconbutton) {
		connect(hconbutton, SIGNAL(released()), this, SLOT(setHCON()));
	}

	setLayout(hbox);

	if (showdefaultbutton) {
		setDefaultItems();
	}
	else {
		setHCON();
	}
}


cNeutralLossesListWidget::~cNeutralLossesListWidget() {
	delete list;
	delete selectallbutton;
	delete clearallbutton;
	delete addbutton;
	delete removebutton;
	if (showdefaultbutton) {
		delete defaultbutton;
	}
	if (showhconbutton) {
		delete hconbutton;
	}
	
	delete vbox1;
	delete vbox2;
	delete hbox;
}


QListWidget* cNeutralLossesListWidget::getList() {
	return list;
}


void cNeutralLossesListWidget::addItem(QString formula) {
	list->addItem(formula);
	QListWidgetItem* item = list->item(list->count() - 1);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
}


void cNeutralLossesListWidget::selectAllItems() {
	for (int i = 0; i < list->count(); i++) {
		list->item(i)->setSelected(true);
	}
}


void cNeutralLossesListWidget::clearAllItems() {
	for (int i = 0; i < list->count(); i++) {
		list->item(i)->setSelected(false);
	}
}


void cNeutralLossesListWidget::addEmptyItem() {
	if (showdefaultbutton) {
		addItem("type a formula here");
	}
	else {
		addItem("type an element here");
	}
	list->setCurrentRow(list->count() - 1); 
	QListWidgetItem* item = list->item(list->count() - 1);
	list->editItem(item);
}


void cNeutralLossesListWidget::removeItem() {
	int i = 0;
	while (i < list->count()) {
		if (list->item(i)->isSelected()) {
			delete list->takeItem(i);
		}
		else {
			i++;
		}
	}
}


void cNeutralLossesListWidget::setDefaultItems() {
	list->clear();
	for (int i = 0; i < (int)defaultneutrallosses.getNeutralLosses().size(); i++) {
		addItem(defaultneutrallosses.getNeutralLosses()[i].c_str());
	}
}


void cNeutralLossesListWidget::setHCON() {
	list->clear();
	addItem("H");
	addItem("C");
	addItem("O");
	addItem("N");

	selectAllItems();

	addItem("S:1");
	addItem("P:1");
}

