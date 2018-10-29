#include "gui/cFragmentIonsListWidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


cFragmentIonsListWidget::cFragmentIonsListWidget(QObject* parent) {
	this->parent = parent;

	vbox1 = new QVBoxLayout();
	vbox2 = new QVBoxLayout();
	hbox = new QHBoxLayout();

	list = new QListWidget();
	list->setMinimumHeight(110);
	list->setMaximumHeight(110);
	list->setSelectionMode(QAbstractItemView::MultiSelection);

	selectallbutton = new QPushButton("Select All");
	selectallbutton->setToolTip("Select all ion types in the list.");
	clearallbutton = new QPushButton("Clear All");
	clearallbutton->setToolTip("Unselect all ion types in the list.");
	resetbutton = new QPushButton("Reset");
	resetbutton->setToolTip("Reset to the default selection of ions.");

	vbox1->addWidget(list);
	vbox1->addStretch(1);

	vbox2->addWidget(selectallbutton);
	vbox2->addWidget(clearallbutton);
	vbox2->addWidget(resetbutton);
	vbox2->addStretch(1);

	hbox->setMargin(0);
	hbox->addLayout(vbox1);
	hbox->addLayout(vbox2);

	connect(selectallbutton, SIGNAL(released()), this, SLOT(selectAllItems()));
	connect(clearallbutton, SIGNAL(released()), this, SLOT(clearAllItems()));
	connect(resetbutton, SIGNAL(released()), this, SLOT(sendResetReleased()));

	setLayout(hbox);
}


cFragmentIonsListWidget::~cFragmentIonsListWidget() {
	delete list;
	delete selectallbutton;
	delete clearallbutton;
	delete resetbutton;
	
	delete vbox1;
	delete vbox2;
	delete hbox;
}


QListWidget* cFragmentIonsListWidget::getList() {
	return list;
}


void cFragmentIonsListWidget::selectAllItems() {
	for (int i = 0; i < list->count(); i++) {
		list->item(i)->setSelected(true);
	}
}


void cFragmentIonsListWidget::clearAllItems() {
	for (int i = 0; i < list->count(); i++) {
		list->item(i)->setSelected(false);
	}
}


void cFragmentIonsListWidget::sendResetReleased() {
	emit resetReleased();
}

