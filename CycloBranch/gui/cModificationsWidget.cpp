#include "gui/cModificationsWidget.h"
#include "gui/cEventFilter.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QProgressDialog>
#include <QCheckBox>
#include <QKeyEvent>
#include <QIcon>


cModificationsWidget::cModificationsWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Modifications Editor");
	setWindowIcon(QIcon(":/images/icons/61.png"));

	insertrow = new QPushButton(tr("Add Row"));
	insertrow->setToolTip("Add a new row.");
	removechecked = new QPushButton(tr(" Remove Rows "));
	removechecked->setToolTip("Remove selected rows.");

	close = new QPushButton(tr("Close"));
	close->setToolTip("Close the window.");
	load = new QPushButton(tr("Load"));
	load->setToolTip("Load modifications.");
	save = new QPushButton(QString("Save"));
	save->setToolTip("Save modifications in the current file. When a file has not been loaded yet, the \"Save As ...\" file dialog is opened.");
	saveas = new QPushButton(tr("Save As..."));
	saveas->setToolTip("Save modifications into a file.");

	buttons = new QHBoxLayout();
	buttons->addWidget(close);
	buttons->addStretch(1);
	buttons->addWidget(insertrow);
	buttons->addWidget(removechecked);
	buttons->addStretch(10);
	buttons->addWidget(load);
	buttons->addWidget(save);
	buttons->addWidget(saveas);

	database = new QTableWidget(0, 0, this);
	database->setColumnCount(6);
	database->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
	database->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
	database->setHorizontalHeaderItem(2, new QTableWidgetItem("Summary Formula"));
	database->setHorizontalHeaderItem(3, new QTableWidgetItem("Monoisotopic Mass"));
	database->setHorizontalHeaderItem(4, new QTableWidgetItem("N-terminal"));
	database->setHorizontalHeaderItem(5, new QTableWidgetItem("C-terminal"));
	database->horizontalHeader()->setStretchLastSection(true);
	for (int i = 0; i < database->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}

	headersort.resize(database->columnCount());
	for (int i = 0; i < database->columnCount(); i++) {
		headersort[i] = -1;
	}

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(database);
	mainlayout->addLayout(buttons);

	connect(database->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(headerItemDoubleClicked(int)));
	connect(database, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged(QTableWidgetItem *)));
	connect(insertrow, SIGNAL(released()), this, SLOT(addRow()));
	connect(removechecked, SIGNAL(released()), this, SLOT(removeEmptyRows()));
	connect(close, SIGNAL(released()), this, SLOT(closeWindow()));
	connect(load, SIGNAL(released()), this, SLOT(loadDatabase()));
	connect(save, SIGNAL(released()), this, SLOT(saveDatabase()));
	connect(saveas, SIGNAL(released()), this, SLOT(saveDatabaseAs()));

	setLayout(mainlayout);

	resize(1280, 700);

	databasefile = "";

	#if OS_TYPE == WIN
		lastdir = "./Modifications/";
	#else
		lastdir = linuxinstalldir + "Modifications/";
	#endif
	
	modifications.clear();
}


cModificationsWidget::~cModificationsWidget() {
	deleteTable(false);

	for (int i = 0; i < database->columnCount(); i++) {
		delete database->horizontalHeaderItem(i);
	}

	database->setColumnCount(0);

	delete insertrow;
	delete removechecked;
	delete close;
	delete load;
	delete save;
	delete saveas;
	delete database;
	delete buttons;
	delete mainlayout;
}


void cModificationsWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
}


void cModificationsWidget::deleteTable(bool enableprogress) {
	QProgressDialog* progress;
	cEventFilter filter;
	int rowcount = database->rowCount();

	if (enableprogress) {
		progress = new QProgressDialog("Clearing the table...", /*"Cancel"*/0, 0, rowcount, this);
		progress->installEventFilter(&filter);
		progress->setMinimumDuration(0);
		progress->setWindowModality(Qt::WindowModal);
		progress->setValue(0);
	}

	widgetitemallocator.reset();

	for (int i = 0; i < rowcount; i++) {
		delete database->cellWidget(rowcount - i - 1, 4);
		delete database->cellWidget(rowcount - i - 1, 5);

		if (enableprogress) {
			progress->setValue(i);
			//if (progress->wasCanceled()) {
			//	break;
			//}
		}
	}
	
	database->setRowCount(0);

	if (enableprogress) {
		progress->setValue(rowcount);
		delete progress;
	}
}


void cModificationsWidget::removeRow(int row) {
	for (int i = 0; i < database->columnCount(); i++) {
		if ((i == 4) || (i == 5)) {
			delete database->cellWidget(row, i);
		}
		else {
			database->takeItem(row, i);
		}
	}
	database->removeRow(row);
}


bool cModificationsWidget::checkTable() {
	// check summary formulas
	for (int i = 0; i < database->rowCount(); i++) {
		if (!checkFormula(i, database->item(i, 2)->text().toStdString())) {
			return false;
		}
	}

	return true;
}


bool cModificationsWidget::checkFormula(int row, const string& summary) {
	cSummaryFormula formula;
	string errmsg;
	formula.setFormula(summary);
	if (!formula.isValid(errmsg)) {
		QMessageBox msgBox;
		QString errstr = "Syntax error in the row no. ";
		errstr += to_string(row + 1).c_str();
		errstr += ": ";
		errstr += errmsg.c_str();
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}
	if (database->item(row, 3)) {
		database->item(row, 3)->setData(Qt::DisplayRole, formula.getMass());
	}
	return true;
}


void cModificationsWidget::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
		hide();
    }
}


void cModificationsWidget::closeWindow() {
	hide();
}


void cModificationsWidget::loadDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Modifications"), lastdir, tr("Modifications (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
		string errormessage;
		
		databasefile = filename;
		save->setText(QString("  Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("'  "));
		
		inputstream.open(filename.toStdString().c_str());

		if (!inputstream.good()) {
			QMessageBox msgBox;
			msgBox.setText("Cannot open the file '" + databasefile + "'.");
			msgBox.exec();
		}
		else {

			modifications.clear();
			loadModificationsFromPlainTextStream(inputstream, modifications, errormessage, true);

			deleteTable(true);

			QProgressDialog progress("Loading Modifications...", /*"Cancel"*/0, 0, (int)modifications.size(), this);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

			database->setRowCount((int)modifications.size());
			for (int i = 0; i < (int)modifications.size(); i++) {
				QCheckBox* checkbox = new QCheckBox();
				database->setCellWidget(i, 0, checkbox);

				database->setItem(i, 1, widgetitemallocator.getNewItem());
				database->item(i, 1)->setText(modifications[i].name.c_str());

				database->setItem(i, 2, widgetitemallocator.getNewItem());
				database->item(i, 2)->setText(modifications[i].summary.c_str());

				database->setItem(i, 3, widgetitemallocator.getNewItem());
				database->item(i, 3)->setData(Qt::DisplayRole, modifications[i].massdifference);

				checkbox = new QCheckBox();
				if (modifications[i].nterminal) {
					checkbox->setChecked(true);
				}
				database->setCellWidget(i, 4, checkbox);

				checkbox = new QCheckBox();
				if (modifications[i].cterminal) {
					checkbox->setChecked(true);
				}
				database->setCellWidget(i, 5, checkbox);

				progress.setValue(i);
				//if (progress.wasCanceled()) {
				//	break;
				//}
			}

			for (int i = 0; i < database->columnCount(); i++) {
				database->resizeColumnToContents(i);
			}

			progress.setValue((int)modifications.size());

		}

		inputstream.close();

	}
}


void cModificationsWidget::saveDatabase() {

	if (!checkTable()) {
		return;
	}

	if (databasefile.compare("") == 0) {
		saveDatabaseAs();
		return;
	}

	outputstream.open(databasefile.toStdString().c_str());
	if (!outputstream.good()) {
		QMessageBox msgBox;
		msgBox.setText("Cannot open the file '" + databasefile + "'.");
		msgBox.exec();
	}
	else {

		QProgressDialog progress("Saving Modifications...", /*"Cancel"*/0, 0, database->rowCount(), this);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		fragmentDescription modification;
		modifications.clear();

		removeEmptyRows();

		for (int i = 0; i < database->rowCount(); i++) {
			modification.clear();
			for (int j = 0; j < database->columnCount(); j++) {
				switch (j)
				{
				case 0:
					// nothing to do
					break;
				case 1:
					modification.name = database->item(i,j)->text().toStdString();
					break;
				case 2:
					modification.summary = database->item(i,j)->text().toStdString();
					break;
				case 3:
					modification.massdifference = database->item(i,j)->data(Qt::DisplayRole).toDouble();
					break;
				case 4:
					modification.nterminal = ((QCheckBox *)database->cellWidget(i, j))->isChecked();
					break;
				case 5:
					modification.cterminal = ((QCheckBox *)database->cellWidget(i, j))->isChecked();
					break;
				default:
					break;
				}
			}
			modifications.push_back(modification);

			progress.setValue(i);
			//if (progress.wasCanceled()) {
			//	break;
			//}
		}

		storeModificationsToPlainTextStream(outputstream, modifications);

		progress.setValue(progress.maximum());

	}
	outputstream.close();
}


void cModificationsWidget::saveDatabaseAs() {

	if (!checkTable()) {
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Modifications As..."), lastdir, tr("Modifications (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
	
		databasefile = filename;
		save->setText(QString("  Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("'  "));
		saveDatabase();
	}
}


void cModificationsWidget::addRow() {
	int row = database->rowCount();
	database->insertRow(row);

	database->setCellWidget(row, 0, new QCheckBox());
	database->setItem(row, 1, widgetitemallocator.getNewItem());
	database->setItem(row, 2, widgetitemallocator.getNewItem());
	database->setItem(row, 3, widgetitemallocator.getNewItem());
	database->setCellWidget(row, 4, new QCheckBox());
	database->setCellWidget(row, 5, new QCheckBox());
}


void cModificationsWidget::removeEmptyRows() {
	int i = 0;
	while (i < database->rowCount()) {
		if (((QCheckBox *)(database->cellWidget(i, 0)))->isChecked()) {
			removeRow(i);
		}
		else {
			i++;
		}
	}
}


void cModificationsWidget::itemChanged(QTableWidgetItem* item) {
	// recalculate mass when formula is changed
	if (item->column() == 2) {
		checkFormula(item->row(), item->text().toStdString());
	}
}


void cModificationsWidget::headerItemDoubleClicked(int index) {
	if (headersort[index] == -1) {
		database->sortByColumn(index, Qt::AscendingOrder);
		headersort[index] = 1;
		return;
	}

	if (headersort[index] == 0) {
		database->sortByColumn(index, Qt::AscendingOrder);
		headersort[index] = 1;
	}
	else {
		database->sortByColumn(index, Qt::DescendingOrder);
		headersort[index] = 0;
	}
}

