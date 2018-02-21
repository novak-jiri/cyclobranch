#include "gui/cFragmentIonsListWidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


cFragmentIonsListWidget::cFragmentIonsListWidget(QObject* parent) {
	this->parent = parent;

	vbox = new QVBoxLayout();
	hbox = new QHBoxLayout();

	list = new QListWidget();
	list->setMinimumHeight(210);
	list->setMaximumHeight(210);
	list->setSelectionMode(QAbstractItemView::MultiSelection);

	selectall = new QPushButton("Select All");
	selectall->setToolTip("Select all fragment ion types in the list.");
	clearall = new QPushButton("Clear All");
	clearall->setToolTip("Unselect all fragment ion types in the list.");
	reset = new QPushButton("Reset");
	reset->setToolTip("Reset to a default selection of fragment ions considering selected \"Peptide Type\".");

	hbox->addWidget(selectall);
	hbox->addWidget(clearall);
	hbox->addWidget(reset);
	hbox->addStretch(1);

	vbox->setMargin(0);
	vbox->addWidget(list);
	vbox->addLayout(hbox);

	connect(selectall, SIGNAL(released()), this, SLOT(selectAllItems()));
	connect(clearall, SIGNAL(released()), this, SLOT(clearAllItems()));
	connect(reset, SIGNAL(released()), this, SLOT(sendResetReleased()));

	setLayout(vbox);
}


cFragmentIonsListWidget::~cFragmentIonsListWidget() {
	delete list;
	delete selectall;
	delete clearall;
	delete reset;
	
	delete hbox;
	delete vbox;
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

