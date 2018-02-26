#include "gui/cNeutralLossesListWidget.h"
#include "core/cFragmentIons.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


cNeutralLossesListWidget::cNeutralLossesListWidget(QObject* parent) {
	this->parent = parent;

	vbox = new QVBoxLayout();
	hbox = new QHBoxLayout();

	list = new QListWidget();
	list->setMinimumHeight(240);
	list->setMaximumHeight(240);
	list->setSelectionMode(QAbstractItemView::MultiSelection);

	selectallbutton = new QPushButton("Select All");
	selectallbutton->setToolTip("Select all items.");
	clearallbutton = new QPushButton("Clear All");
	clearallbutton->setToolTip("Unselect all items.");
	addbutton = new QPushButton("Add");
	addbutton->setToolTip("Add a new item.");
	removebutton = new QPushButton("Remove");
	removebutton->setToolTip("Remove selected items.");
	defaultbutton = new QPushButton("Default");
	defaultbutton->setToolTip("Set default items.");

	vbox->addWidget(selectallbutton);
	vbox->addWidget(clearallbutton);
	vbox->addWidget(addbutton);
	vbox->addWidget(removebutton);
	vbox->addWidget(defaultbutton);
	vbox->addStretch(1);

	hbox->setMargin(0);
	hbox->addWidget(list);
	hbox->addLayout(vbox);

	connect(selectallbutton, SIGNAL(released()), this, SLOT(selectAllItems()));
	connect(clearallbutton, SIGNAL(released()), this, SLOT(clearAllItems()));
	connect(addbutton, SIGNAL(released()), this, SLOT(addEmptyItem()));
	connect(removebutton, SIGNAL(released()), this, SLOT(removeItem()));
	connect(defaultbutton, SIGNAL(released()), this, SLOT(setDefaultItems()));

	setLayout(hbox);

	setDefaultItems();
}


cNeutralLossesListWidget::~cNeutralLossesListWidget() {
	delete list;
	delete selectallbutton;
	delete clearallbutton;
	delete addbutton;
	delete removebutton;
	delete defaultbutton;
	
	delete vbox;
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
	addItem("type a formula here");
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
}

