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
#include <QLineEdit>


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
	save = new QPushButton(QString(" Save "));
	save->setToolTip("Save the database of building blocks in the current file. When a file has not been loaded yet, the \"Save As ...\" file dialog is opened.");
	saveas = new QPushButton(tr("Save As..."));
	saveas->setToolTip("Save the database of building blocks into a file.");

	rowsfilterline = new QLineEdit();
	rowsfilterline->setMinimumWidth(250);
	rowsfilterline->setToolTip("Text to Find");
	rowsfiltercasesensitive = new QCheckBox();
	rowsfiltercasesensitive->setToolTip("Case Sensitive");
	rowsfilterbutton = new QPushButton("Filter");
	rowsfilterbutton->setToolTip("Filter Search Results");
	rowsfilterbutton->setMinimumWidth(50);
	rowsfilterclearbutton = new QPushButton("Clear");
	rowsfilterclearbutton->setToolTip("Clear Form and Reset Search Results");
	rowsfilterclearbutton->setMinimumWidth(50);

	rowsfilterhbox = new QHBoxLayout();
	rowsfilterhbox->addWidget(rowsfilterline);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfiltercasesensitive);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfilterbutton);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfilterclearbutton);

	rowsfilterwidget = new QWidget();
	rowsfilterwidget->setLayout(rowsfilterhbox);
	rowsfilterwidget->setMaximumWidth(420);

	buttons = new QHBoxLayout();
	buttons->addWidget(close);
	buttons->addStretch();
	buttons->addWidget(insertrow);
	buttons->addWidget(removechecked);
	buttons->addStretch();
	buttons->addWidget(rowsfilterwidget);
	buttons->addStretch();
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
	connect(removechecked, SIGNAL(released()), this, SLOT(removeCheckedRows()));
	connect(close, SIGNAL(released()), this, SLOT(closeWindow()));
	connect(load, SIGNAL(released()), this, SLOT(loadDatabase()));
	connect(save, SIGNAL(released()), this, SLOT(saveDatabase()));
	connect(saveas, SIGNAL(released()), this, SLOT(saveDatabaseAs()));
	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterRows()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	setLayout(mainlayout);

	resize(1280, 700);

	databasefile = "";

	#if OS_TYPE == WIN
		lastdir = "./BrickDatabases/";
	#else
		lastdir = installdir + "BrickDatabases/";
	#endif
	
	bricks.clear();

	setDataModified(false);
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

	delete rowsfilterline;
	delete rowsfiltercasesensitive;
	delete rowsfilterbutton;
	delete rowsfilterclearbutton;
	delete rowsfilterhbox;
	delete rowsfilterwidget;

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
		database->item(row, 4)->setData(Qt::DisplayRole, to_string(formula.getMass()).c_str());
	}
	return true;
}


void cBricksDatabaseWidget::setDataModified(bool datamodified) {
	if (datamodified == this->datamodified) {
		return;
	}

	this->datamodified = datamodified;

	if (datamodified) {
		save->setText("*" + save->text());
	}
	else {
		if ((save->text().size() > 0) && (save->text().at(0) == '*')) {
			save->setText(save->text().toStdString().substr(1).c_str());
		}
	}
}


void cBricksDatabaseWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		closeWindow();
    }

	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (rowsfilterline->hasFocus()) {
			filterRows();
		}
    }
}


void cBricksDatabaseWidget::closeWindow() {
	if (datamodified) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, appname, "Save changes ?", QMessageBox::Yes|QMessageBox::No);
	
		if (reply == QMessageBox::Yes) {
			if (!saveDatabase()) {
				return;
			}
		}
	}

	hide();
}


void cBricksDatabaseWidget::loadDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load the Database of Building Blocks"), lastdir, tr("Database of Building Blocks (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
		string errormessage;
		
		databasefile = filename;
		save->setText(QString(" Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("' "));
		
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
				database->item(i, 4)->setData(Qt::DisplayRole, to_string(bricks[i].getMass()).c_str());
				
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

			setDataModified(false);

		}

		inputstream.close();

	}
}


bool cBricksDatabaseWidget::saveDatabase() {
	bool saved = false;

	if (!checkTable()) {
		return saved;
	}

	if (databasefile.compare("") == 0) {
		return saveDatabaseAs();
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

		removeCheckedRows();

		for (int i = 0; i < database->rowCount(); i++) {

			if (database->isRowHidden(i)) {
				progress.setValue(i);
				continue;
			}

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

		setDataModified(false);
			
		saved = true;

	}
	outputstream.close();

	return saved;
}


bool cBricksDatabaseWidget::saveDatabaseAs() {
	if (!checkTable()) {
		return false;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save the Database of Building Blocks As..."), lastdir, tr("Database of Building Blocks (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
	
		databasefile = filename;
		save->setText(QString(" Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("' "));
		return saveDatabase();
	}

	return false;
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


void cBricksDatabaseWidget::removeCheckedRows() {
	int i = 0;
	while (i < database->rowCount()) {
		if (((QCheckBox *)(database->cellWidget(i, 0)))->isChecked()) {
			removeRow(i);
			setDataModified(true);
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

	setDataModified(true);
}


void cBricksDatabaseWidget::headerItemDoubleClicked(int index) {
	setDataModified(true);

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


void cBricksDatabaseWidget::filterRows() {
	Qt::CaseSensitivity casesensitive = rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;
	QString str = rowsfilterline->text();
	int rowcount = database->rowCount();
	bool match;
	int i, j;

	QProgressDialog progress("Updating...", /*"Cancel"*/0, 0, rowcount, this);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	for (i = 0; i < rowcount; i++) {
		match = false;
		for (j = 0; j < database->columnCount(); j++) {
			// ignore non-text fields
			if ((j == 0) || (j == 6)) {
				continue;
			}

			if (database->item(i, j)->text().contains(str, casesensitive)) {
				match = true;
				break;
			}
		}
		database->setRowHidden(i, !match);
		progress.setValue(i);
	}

	progress.setValue(rowcount);
}


void cBricksDatabaseWidget::resetFilter() {
	rowsfilterline->setText("");
	int rowcount = database->rowCount();

	QProgressDialog progress("Updating...", /*"Cancel"*/0, 0, rowcount, this);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	for (int i = 0; i < rowcount; i++) {
		database->setRowHidden(i, false);
		progress.setValue(i);
	}

	progress.setValue(rowcount);
}

