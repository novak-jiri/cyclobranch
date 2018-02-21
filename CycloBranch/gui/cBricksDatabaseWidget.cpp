#include "gui/cBricksDatabaseWidget.h"
#include "gui/cMainThread.h"
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


int numberOfOccurrences(const string& s, char c) {
	int count = 0;
	for (int i = 0; i < (int)s.size(); i++) {
		if (s[i] == c) {
			count++;
		}
	}
	return count;
}


cBricksDatabaseWidget::cBricksDatabaseWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Building Blocks Editor");
	setWindowIcon(QIcon(":/images/icons/68.png"));

	insertrow = new QPushButton(tr("Add Row"));
	insertrow->setToolTip("Add a new row.");
	removechecked = new QPushButton(tr(" Remove Rows "));
	removechecked->setToolTip("Remove selected rows.");

	close = new QPushButton(tr("Close"));
	close->setToolTip("Close the window.");
	load = new QPushButton(tr("Load"));
	load->setToolTip("Load the database of building blocks.");
	save = new QPushButton(QString("Save"));
	save->setToolTip("Save the database of building blocks in the current file. When a file has not been loaded yet, the \"Save As ...\" file dialog is opened.");
	saveas = new QPushButton(tr("Save As..."));
	saveas->setToolTip("Save the database of building blocks into a file.");

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
	database->setColumnCount(7);
	database->setHorizontalHeaderItem(0, new QTableWidgetItem());
	database->setHorizontalHeaderItem(1, new QTableWidgetItem());
	database->horizontalHeaderItem(1)->setText("Name(s)");
	database->setHorizontalHeaderItem(2, new QTableWidgetItem());
	database->horizontalHeaderItem(2)->setText("Acronym(s)");
	database->setHorizontalHeaderItem(3, new QTableWidgetItem());
	database->horizontalHeaderItem(3)->setText("Residue Summary");
	database->setHorizontalHeaderItem(4, new QTableWidgetItem());
	database->horizontalHeaderItem(4)->setText("Monoisotopic Residue Mass");
	database->setHorizontalHeaderItem(5, new QTableWidgetItem());
	database->horizontalHeaderItem(5)->setText("Reference(s)");
	database->setHorizontalHeaderItem(6, new QTableWidgetItem());
	database->horizontalHeaderItem(6)->setText("Preview");

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
		lastdir = "./BrickDatabases/";
	#else
		lastdir = linuxinstalldir + "BrickDatabases/";
	#endif
	
	bricks.clear();
}


cBricksDatabaseWidget::~cBricksDatabaseWidget() {
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


void cBricksDatabaseWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
}


void cBricksDatabaseWidget::deleteTable(bool enableprogress) {
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
		delete database->cellWidget(rowcount - i - 1, 0);
		delete database->cellWidget(rowcount - i - 1, 6);

        if (enableprogress) {
            progress->setValue(i);
            //if (progress->wasCanceled()) {
            //    break;
            //}
        }
    }
	
	database->setRowCount(0);

	if (enableprogress) {
		progress->setValue(rowcount);
		delete progress;
	}
}


void cBricksDatabaseWidget::removeRow(int row) {
   for (int i = 0; i < database->columnCount(); i++) {
        if ((i == 0) || (i == 6)) {
            delete database->cellWidget(row, i);
        }
        else {
			database->takeItem(row, i);
        }
    }
    database->removeRow(row);
}


bool cBricksDatabaseWidget::checkTable() {
	// the fields name, acronym and reference must contain the same numbers of '/'
	int checkslash;
	for (int i = 0; i < database->rowCount(); i++) {
		checkslash = numberOfOccurrences(database->item(i, 1)->text().toStdString(),'/');
		if ((checkslash != numberOfOccurrences(database->item(i, 2)->text().toStdString(),'/')) || (checkslash != numberOfOccurrences(database->item(i, 5)->text().toStdString(),'/'))) {
			QMessageBox msgBox;
			QString errstr = "Syntax error in the row no. ";
			errstr += to_string(i + 1).c_str();
			errstr += ":\nThe number of '/' must be equal in the fields Name(s), Acronym(s) and Reference(s) !\n\nNote: The symbol '/' separates izomers of a building block.";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}

	// check summary formulas
	for (int i = 0; i < database->rowCount(); i++) {
		if (!checkFormula(i, database->item(i, 3)->text().toStdString())) {
			return false;
		}
	}

	return true;
}


bool cBricksDatabaseWidget::checkFormula(int row, const string& summary) {
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
	if (database->item(row, 4)) {
		database->item(row, 4)->setData(Qt::DisplayRole, formula.getMass());
	}
	return true;
}


void cBricksDatabaseWidget::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
		hide();
    }
}


void cBricksDatabaseWidget::closeWindow() {
	hide();
}


void cBricksDatabaseWidget::loadDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load the Database of Building Blocks"), lastdir, tr("Database of Building Blocks (*.txt)"));

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

			bricks.clear();
			bricks.loadFromPlainTextStream(inputstream, errormessage, true);

			deleteTable(true);

			QProgressDialog progress("Loading the Databatase of Building Blocks...", /*"Cancel"*/0, 0, bricks.size(), this);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

			database->setRowCount(bricks.size());
			for (int i = 0; i < bricks.size(); i++) {
				QCheckBox* checkbox = new QCheckBox();
				database->setCellWidget(i, 0, checkbox);

				database->setItem(i, 1, widgetitemallocator.getNewItem());
				database->item(i, 1)->setText(bricks[i].getName().c_str());

				database->setItem(i, 2, widgetitemallocator.getNewItem());
				database->item(i, 2)->setText(bricks[i].getAcronymsAsString().c_str());

				database->setItem(i, 3, widgetitemallocator.getNewItem());
				database->item(i, 3)->setText(bricks[i].getSummary().c_str());

				database->setItem(i, 4, widgetitemallocator.getNewItem());
				database->item(i, 4)->setData(Qt::DisplayRole, bricks[i].getMass());
				
				database->setItem(i, 5, widgetitemallocator.getNewItem());
				database->item(i, 5)->setText(bricks[i].getReferencesAsString().c_str());
				
				database->setCellWidget(i, 6, new QLabel(bricks[i].getAcronymsWithReferencesAsHTMLString().c_str()));
				((QLabel *)database->cellWidget(i, 6))->setTextFormat(Qt::RichText);
				((QLabel *)database->cellWidget(i, 6))->setTextInteractionFlags(Qt::TextBrowserInteraction);
				((QLabel *)database->cellWidget(i, 6))->setOpenExternalLinks(true);

				progress.setValue(i);
				//if (progress.wasCanceled()) {
				//	break;
				//}
			}

			for (int i = 0; i < database->columnCount(); i++) {
				database->resizeColumnToContents(i);
			}

			progress.setValue(bricks.size());

		}

		inputstream.close();

	}
}


void cBricksDatabaseWidget::saveDatabase() {

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

		QProgressDialog progress("Saving the Databatase of Building Blocks...", /*"Cancel"*/0, 0, database->rowCount(), this);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		cBrick b;
		bricks.clear();
		string s;

		removeEmptyRows();

		for (int i = 0; i < database->rowCount(); i++) {
			b.clear();
			for (int j = 0; j < database->columnCount(); j++) {
				switch (j)
				{
				case 0:
					// nothing to do
					break;
				case 1:
					s = database->item(i,j)->text().toStdString();
					b.setName(removeWhiteSpacesExceptSpaces(s));
					break;
				case 2:
					s = database->item(i,j)->text().toStdString();
					b.setAcronyms(removeWhiteSpacesExceptSpaces(s));
					break;
				case 3:
					s = database->item(i,j)->text().toStdString();
					b.setSummary(removeWhiteSpacesExceptSpaces(s));
					break;
				case 4:
					b.setMass(database->item(i,j)->data(Qt::DisplayRole).toDouble());
					break;
				case 5:
					s = database->item(i,j)->text().toStdString();
					b.setReferences(removeWhiteSpacesExceptSpaces(s));
					break;
				default:
					break;
				}
			}
			bricks.push_back(b);

			progress.setValue(i);
			//if (progress.wasCanceled()) {
			//	break;
			//}
		}

		bricks.storeToPlainTextStream(outputstream);

		progress.setValue(progress.maximum());

	}
	outputstream.close();
}


void cBricksDatabaseWidget::saveDatabaseAs() {

	if (!checkTable()) {
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save the Database of Building Blocks As..."), lastdir, tr("Database of Building Blocks (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
	
		databasefile = filename;
		save->setText(QString("  Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("'  "));
		saveDatabase();
	}
}


void cBricksDatabaseWidget::addRow() {
	int row = database->rowCount();
	database->insertRow(row);

	QCheckBox* checkbox = new QCheckBox();
	database->setCellWidget(row, 0, checkbox);

	database->setItem(row, 1, widgetitemallocator.getNewItem());
	database->setItem(row, 2, widgetitemallocator.getNewItem());
	database->setItem(row, 3, widgetitemallocator.getNewItem());
	database->setItem(row, 4, widgetitemallocator.getNewItem());
	database->setItem(row, 5, widgetitemallocator.getNewItem());

	database->setCellWidget(row, 6, new QLabel());
	((QLabel *)database->cellWidget(row, 6))->setTextFormat(Qt::RichText);
	((QLabel *)database->cellWidget(row, 6))->setTextInteractionFlags(Qt::TextBrowserInteraction);
	((QLabel *)database->cellWidget(row, 6))->setOpenExternalLinks(true);
}


void cBricksDatabaseWidget::removeEmptyRows() {
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


void cBricksDatabaseWidget::itemChanged(QTableWidgetItem* item) {
	// recalculate mass when formula is changed
	if (item->column() == 3) {
		checkFormula(item->row(), item->text().toStdString());
	}

	// update references preview
	if (((item->column() == 2) || (item->column() == 5)) && database->cellWidget(item->row(), 6)) {
		cBrick b;
		b.setAcronyms(database->item(item->row(), 2)->text().toStdString());
		b.setReferences(database->item(item->row(), 5)->text().toStdString());
		((QLabel *)database->cellWidget(item->row(), 6))->setText(b.getAcronymsWithReferencesAsHTMLString().c_str());
	}
}


void cBricksDatabaseWidget::headerItemDoubleClicked(int index) {
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

