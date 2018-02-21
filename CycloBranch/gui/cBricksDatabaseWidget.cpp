#include "gui/cBricksDatabaseWidget.h"
#include "gui/cMainThread.h"
#include "gui/cEventFilter.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QProgressDialog>
#include <QKeyEvent>
#include <QIcon>
#include <QLineEdit>
#include <QMenuBar>
#include <QMenu>


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

	editorname = "Building Blocks Editor";

	setWindowTitle(editorname);
	setWindowIcon(QIcon(":/images/icons/68.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuEdit = new QMenu(tr("&Edit"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	rowsfilterline = new QLineEdit();
	rowsfilterline->setMinimumWidth(250);
	rowsfilterline->setToolTip("Text to Find");

	rowsfiltercasesensitive = new QCheckBox();
	rowsfiltercasesensitive->setToolTip("Case Sensitive");

	rowsfilterbutton = new QPushButton("Filter");
	rowsfilterbutton->setToolTip("Filter Search Results");
	rowsfilterbutton->setMinimumWidth(50);

	rowsfilterclearbutton = new QPushButton("Reset");
	rowsfilterclearbutton->setToolTip("Reset Search Results");
	rowsfilterclearbutton->setMinimumWidth(50);
	rowsfilterclearbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

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

	database = new QTableView(this);
	databasemodel = new QStandardItemModel(0, 0, this);
	proxymodel = new cBricksDatabaseProxyModel(this);
	proxymodel->setSourceModel(databasemodel);
	proxymodel->setDynamicSortFilter(false);
	database->setModel(proxymodel);
	database->setSortingEnabled(true);

	resetHeader();

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(database);

	mainwidget = new QWidget();
	mainwidget->setLayout(mainlayout);

	toolbarFile = addToolBar(tr("File"));
				
	actionNewDatabase = new QAction(QIcon(":/images/icons/2.png"), tr("&New Database"), this);
	actionNewDatabase->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
	actionNewDatabase->setToolTip("New Database (Ctrl + N)");
	toolbarFile->addAction(actionNewDatabase);
	connect(actionNewDatabase, SIGNAL(triggered()), this, SLOT(createNewDatabase()));

	actionOpenDatabase = new QAction(QIcon(":/images/icons/52.png"), tr("&Open Database"), this);
	actionOpenDatabase->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	actionOpenDatabase->setToolTip("Open Database (Ctrl + O)");
	toolbarFile->addAction(actionOpenDatabase);
	connect(actionOpenDatabase, SIGNAL(triggered()), this, SLOT(openDatabase()));

	actionSaveDatabase = new QAction(QIcon(":/images/icons/22.png"), tr("&Save Database..."), this);
	actionSaveDatabase->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	actionSaveDatabase->setToolTip("Save Database... (Ctrl + S)");
	toolbarFile->addAction(actionSaveDatabase);
	connect(actionSaveDatabase, SIGNAL(triggered()), this, SLOT(saveDatabase()));

	actionSaveDatabaseAs = new QAction(QIcon(":/images/icons/86.png"), tr("Save &Database As..."), this);
	actionSaveDatabaseAs->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	actionSaveDatabaseAs->setToolTip("Save Database As... (Ctrl + D)");
	toolbarFile->addAction(actionSaveDatabaseAs);
	connect(actionSaveDatabaseAs, SIGNAL(triggered()), this, SLOT(saveDatabaseAs()));

	actionImportDatabase = new QAction(QIcon(":/images/icons/63.png"), tr("&Import Database"), this);
	actionImportDatabase->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionImportDatabase->setToolTip("Import Database (Ctrl + I)");
	toolbarFile->addAction(actionImportDatabase);
	connect(actionImportDatabase, SIGNAL(triggered()), this, SLOT(importDatabase()));

	actionCloseWindow = new QAction(QIcon(":/images/icons/33.png"), tr("&Close"), this);
	actionCloseWindow->setShortcut(QKeySequence(Qt::Key_Escape));
	actionCloseWindow->setToolTip("Close (Esc)");
	toolbarFile->addAction(actionCloseWindow);
	connect(actionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));

	toolbarEdit = addToolBar(tr("Edit"));

	actionAddRow = new QAction(QIcon(":/images/icons/13.png"), tr("&Insert Row"), this);
	actionAddRow->setShortcut(QKeySequence(Qt::Key_Insert));
	actionAddRow->setToolTip("Insert Row (Insert)");
	toolbarEdit->addAction(actionAddRow);
	connect(actionAddRow, SIGNAL(triggered()), this, SLOT(addRow()));

	actionRemoveSelectedRows = new QAction(QIcon(":/images/icons/14.png"), tr("&Remove Selected Rows"), this);
	actionRemoveSelectedRows->setShortcut(QKeySequence(Qt::Key_Delete));
	actionRemoveSelectedRows->setToolTip("Remove Selected Rows (Delete)");
	toolbarEdit->addAction(actionRemoveSelectedRows);
	connect(actionRemoveSelectedRows, SIGNAL(triggered()), this, SLOT(removeSelectedRows()));

	actionSelectAll = new QAction(QIcon(":/images/icons/38.png"), tr("Select &All"), this);
	actionSelectAll->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
	actionSelectAll->setToolTip("Select all rows (Ctrl + A)");
	toolbarEdit->addAction(actionSelectAll);
	connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));

	actionUnselectAll = new QAction(QIcon(":/images/icons/38b.png"), tr("&Unselect All"), this);
	actionUnselectAll->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
	actionUnselectAll->setToolTip("Unselect all rows (Ctrl + U)");
	toolbarEdit->addAction(actionUnselectAll);
	connect(actionUnselectAll, SIGNAL(triggered()), this, SLOT(unselectAll()));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));
	
	toolbarFilter = addToolBar(tr("Filter"));
	toolbarFilter->addWidget(rowsfilterwidget);

	menuFile->addAction(actionNewDatabase);
	menuFile->addAction(actionOpenDatabase);
	menuFile->addSeparator();
	menuFile->addAction(actionSaveDatabase);
	menuFile->addAction(actionSaveDatabaseAs);
	menuFile->addSeparator();
	menuFile->addAction(actionImportDatabase);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuEdit->addAction(actionAddRow);
	menuEdit->addAction(actionRemoveSelectedRows);
	menuEdit->addSeparator();
	menuEdit->addAction(actionSelectAll);
	menuEdit->addAction(actionUnselectAll);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuEdit);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	connect(database->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerItemClicked(int)));
	connect(databasemodel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterRows()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	setCentralWidget(mainwidget);

	resize(1280, 770);

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
	deleteTable();

	delete databasemodel;
	delete proxymodel;
	delete database;

	delete rowsfilterline;
	delete rowsfiltercasesensitive;
	delete rowsfilterbutton;
	delete rowsfilterclearbutton;
	delete rowsfilterhbox;
	delete rowsfilterwidget;

	delete mainlayout;
	delete mainwidget;

	delete actionNewDatabase;
	delete actionOpenDatabase;
	delete actionSaveDatabase;
	delete actionSaveDatabaseAs;
	delete actionImportDatabase;
	delete actionCloseWindow;
	delete actionAddRow;
	delete actionRemoveSelectedRows;
	delete actionSelectAll;
	delete actionUnselectAll;
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuEdit;
	delete menuHelp;

	delete menuBar;
}


void cBricksDatabaseWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


void cBricksDatabaseWidget::deleteTable() {
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		if (database->itemDelegateForColumn(i)) {
			delete database->itemDelegateForColumn(i);
		}
	}

	databasemodel->clear();
	databasemodel->setRowCount(0);
}


void cBricksDatabaseWidget::resetHeader() {
	databasemodel->setColumnCount(7);

	databasemodel->setHorizontalHeaderItem(0, new QStandardItem());
	database->setItemDelegateForColumn(0, new cCheckBoxDelegate());
	databasemodel->setHorizontalHeaderItem(1, new QStandardItem("Name(s)"));
	databasemodel->setHorizontalHeaderItem(2, new QStandardItem("Acronym(s)"));
	databasemodel->setHorizontalHeaderItem(3, new QStandardItem("Residue Summary"));
	databasemodel->setHorizontalHeaderItem(4, new QStandardItem("Monoisotopic Residue Mass"));
	databasemodel->setHorizontalHeaderItem(5, new QStandardItem("Reference(s)"));
	databasemodel->setHorizontalHeaderItem(6, new QStandardItem("Preview"));
	database->setItemDelegateForColumn(6, new cMultipleButtonDelegate());

	database->horizontalHeader()->setStretchLastSection(true);
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}
	database->setColumnWidth(1, 400);
	database->setColumnWidth(2, 400);
	database->setColumnWidth(5, 400);
}


bool cBricksDatabaseWidget::checkTable() {
	// the fields name, acronym and reference must contain the same numbers of '/'
	int checkslash;
	for (int i = 0; i < databasemodel->rowCount(); i++) {
		checkslash = numberOfOccurrences(databasemodel->item(i, 1)->text().toStdString(),'/');
		if ((checkslash != numberOfOccurrences(databasemodel->item(i, 2)->text().toStdString(),'/')) || (checkslash != numberOfOccurrences(databasemodel->item(i, 5)->text().toStdString(),'/'))) {
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
	for (int i = 0; i < databasemodel->rowCount(); i++) {
		if (!checkFormula(i, databasemodel->item(i, 3)->text().toStdString())) {
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
	if (databasemodel->item(row, 4)) {
		databasemodel->item(row, 4)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(formula.getMass())), Qt::DisplayRole);
	}
	return true;
}


void cBricksDatabaseWidget::setDataModified(bool datamodified) {
	if (datamodified == this->datamodified) {
		return;
	}

	this->datamodified = datamodified;

	if (datamodified) {
		setWindowTitle("*" + windowTitle());
	}
	else {
		if ((windowTitle().size() > 0) && (windowTitle().at(0) == '*')) {
			setWindowTitle(windowTitle().toStdString().substr(1).c_str());
		}
	}
}


void cBricksDatabaseWidget::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (rowsfilterline->hasFocus()) {
			filterRows();
		}
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_F)) {
		rowsfilterline->setFocus();
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_T)) {
		rowsfiltercasesensitive->setChecked(!rowsfiltercasesensitive->isChecked());
	}

	event->accept();
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


void cBricksDatabaseWidget::openDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Database"), lastdir, tr("Database of Building Blocks (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
		string errormessage;
		
		databasefile = filename;
		setWindowTitle(editorname + QString(" - ") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()));
		
		inputstream.open(filename.toStdString().c_str());

		if (!inputstream.good()) {
			QMessageBox msgBox;
			msgBox.setText("Cannot open the file '" + databasefile + "'.");
			msgBox.exec();
		}
		else {

			deleteTable();
			resetHeader();
			resetFilter();

			bricks.clear();
			bricks.loadFromPlainTextStream(inputstream, errormessage, true, false);

			QProgressDialog progress("Loading the Database of Building Blocks...", "Cancel", 0, bricks.size(), this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

			database->setModel(0);
			proxymodel->setSourceModel(0);
			database->setSortingEnabled(false);

			databasemodel->setRowCount(bricks.size());
			for (int i = 0; i < bricks.size(); i++) {

				databasemodel->setItem(i, 0, new QStandardItem());
				databasemodel->item(i, 0)->setFlags(databasemodel->item(i, 0)->flags() & ~Qt::ItemIsEditable);

				databasemodel->setItem(i, 1, new QStandardItem());
				databasemodel->item(i, 1)->setText(bricks[i].getName().c_str());

				databasemodel->setItem(i, 2, new QStandardItem());
				databasemodel->item(i, 2)->setText(bricks[i].getAcronymsAsString().c_str());

				databasemodel->setItem(i, 3, new QStandardItem());
				databasemodel->item(i, 3)->setText(bricks[i].getSummary().c_str());

				databasemodel->setItem(i, 4, new QStandardItem());
				databasemodel->item(i, 4)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(bricks[i].getMass())), Qt::DisplayRole);
				
				databasemodel->setItem(i, 5, new QStandardItem());
				databasemodel->item(i, 5)->setText(bricks[i].getReferencesAsString().c_str());
				
                databasemodel->setItem(i, 6, new QStandardItem());
				databasemodel->item(i, 6)->setText(bricks[i].getAcronymsWithReferencesAsHTMLString().c_str());

				progress.setValue(i);
				if (progress.wasCanceled()) {
					deleteTable();
					resetHeader();
					bricks.clear();
					databasefile = "";
					setWindowTitle(editorname);
					break;
				}
			
			}

			proxymodel->setSourceModel(databasemodel);
			database->setModel(proxymodel);
			proxymodel->setSortRole(Qt::InitialSortOrderRole);
			database->setSortingEnabled(true);
			
			for (int i = 0; i < databasemodel->columnCount(); i++) {
				database->resizeColumnToContents(i);
			}
			database->setColumnWidth(1, 400);
			database->setColumnWidth(2, 400);
			database->setColumnWidth(5, 400);

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

		QProgressDialog progress("Saving the Database of Building Blocks...", 0, 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		cBrick b;
		bricks.clear();
		string s;

		for (int i = 0; i < proxymodel->rowCount(); i++) {

			b.clear();
			for (int j = 0; j < proxymodel->columnCount(); j++) {
				switch (j)
				{
				case 0:
					// nothing to do
					break;
				case 1:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					b.setName(removeWhiteSpacesExceptSpaces(s));
					break;
				case 2:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					b.setAcronyms(removeWhiteSpacesExceptSpaces(s));
					break;
				case 3:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					b.setSummary(removeWhiteSpacesExceptSpaces(s));
					break;
				case 4:
					b.setMass(proxymodel->index(i, j).data(Qt::DisplayRole).toDouble());
					break;
				case 5:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					b.setReferences(removeWhiteSpacesExceptSpaces(s));
					break;
				default:
					break;
				}
			}
			bricks.push_back(b);

			progress.setValue(i);
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

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Database As..."), lastdir, tr("Database of Building Blocks (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
	
		databasefile = filename;
		setWindowTitle(editorname + QString(" - ") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()));
		return saveDatabase();
	}

	return false;
}


void cBricksDatabaseWidget::addRow() {
	resetFilter();

	int row = databasemodel->rowCount();
	databasemodel->insertRow(row);

	databasemodel->setItem(row, 0, new QStandardItem());
	databasemodel->item(row, 0)->setFlags(databasemodel->item(row, 0)->flags() & ~Qt::ItemIsEditable);

	databasemodel->setItem(row, 1, new QStandardItem());
	databasemodel->setItem(row, 2, new QStandardItem());
	databasemodel->setItem(row, 3, new QStandardItem());
	databasemodel->setItem(row, 4, new QStandardItem());
	databasemodel->setItem(row, 5, new QStandardItem());
	databasemodel->setItem(row, 6, new QStandardItem());

	database->scrollToBottom();
}


void cBricksDatabaseWidget::removeSelectedRows() {
	int i = 0;
	while (i < proxymodel->rowCount()) {
		if (proxymodel->index(i, 0).data(Qt::DisplayRole).toBool()) {
			proxymodel->removeRow(i);
			setDataModified(true);
		}
		else {
			i++;
		}
	}
}


void cBricksDatabaseWidget::itemChanged(QStandardItem* item) {
	if (!item) {
		return;
	}

	// recalculate mass when formula is changed
	if (item->column() == 3) {
		checkFormula(item->row(), item->text().toStdString());
	}

	// update references preview
	if (((item->column() == 2) || (item->column() == 5)) && databasemodel->item(item->row(), 6)) {
		cBrick b;

		b.setAcronyms(databasemodel->item(item->row(), 2)->text().toStdString());
		b.setReferences(databasemodel->item(item->row(), 5)->text().toStdString());

		databasemodel->item(item->row(), 6)->setText(b.getAcronymsWithReferencesAsHTMLString().c_str());
	}

	if (item->column() > 0) {
		setDataModified(true);
	}
}


void cBricksDatabaseWidget::headerItemClicked(int index) {
	setDataModified(true);
}


void cBricksDatabaseWidget::filterRows() {
	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterCaseSensitivity(rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive);
	proxymodel->setFilterFixedString(rowsfilterline->text());
}


void cBricksDatabaseWidget::resetFilter() {
	rowsfilterline->setText("");

	database->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
	proxymodel->sort(-1);

	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterFixedString("");
}


void cBricksDatabaseWidget::createNewDatabase() {
	if (datamodified) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, appname, "Save changes ?", QMessageBox::Yes|QMessageBox::No);
	
		if (reply == QMessageBox::Yes) {
			if (!saveDatabase()) {
				return;
			}
		}
	}

	deleteTable();
	resetHeader();
	resetFilter();
	bricks.clear();
	databasefile = "";
	setWindowTitle(editorname);

	setDataModified(false);
}


void cBricksDatabaseWidget::importDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Import Database"), lastdir, tr("Database of Building Blocks (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
		string errormessage;
		
		inputstream.open(filename.toStdString().c_str());

		if (!inputstream.good()) {
			QMessageBox msgBox;
			msgBox.setText("Cannot open the file '" + filename + "'.");
			msgBox.exec();
		}
		else {
			resetFilter();

			cBricksDatabase importedbricks;
			importedbricks.loadFromPlainTextStream(inputstream, errormessage, true, false);

			QProgressDialog progress("Importing the Database of Building Blocks...", "Cancel", 0, importedbricks.size(), this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

			database->setModel(0);
			proxymodel->setSourceModel(0);
			database->setSortingEnabled(false);
			
			int originalrowcount = databasemodel->rowCount();
			databasemodel->setRowCount(originalrowcount + importedbricks.size());
			for (int i = 0; i < importedbricks.size(); i++) {

				databasemodel->setItem(i + originalrowcount, 0, new QStandardItem());
				databasemodel->item(i + originalrowcount, 0)->setFlags(databasemodel->item(i + originalrowcount, 0)->flags() & ~Qt::ItemIsEditable);

				databasemodel->setItem(i + originalrowcount, 1, new QStandardItem());
				databasemodel->item(i + originalrowcount, 1)->setText(importedbricks[i].getName().c_str());

				databasemodel->setItem(i + originalrowcount, 2, new QStandardItem());
				databasemodel->item(i + originalrowcount, 2)->setText(importedbricks[i].getAcronymsAsString().c_str());

				databasemodel->setItem(i + originalrowcount, 3, new QStandardItem());
				databasemodel->item(i + originalrowcount, 3)->setText(importedbricks[i].getSummary().c_str());

				databasemodel->setItem(i + originalrowcount, 4, new QStandardItem());
				databasemodel->item(i + originalrowcount, 4)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(importedbricks[i].getMass())), Qt::DisplayRole);
				
				databasemodel->setItem(i + originalrowcount, 5, new QStandardItem());
				databasemodel->item(i + originalrowcount, 5)->setText(importedbricks[i].getReferencesAsString().c_str());
				
                databasemodel->setItem(i + originalrowcount, 6, new QStandardItem());
				databasemodel->item(i + originalrowcount, 6)->setText(importedbricks[i].getAcronymsWithReferencesAsHTMLString().c_str());

				bricks.push_back(importedbricks[i]);

				progress.setValue(i);
				if (progress.wasCanceled()) {
					deleteTable();
					resetHeader();
					bricks.clear();
					databasefile = "";
					setWindowTitle(editorname);
					break;
				}
			
			}

			proxymodel->setSourceModel(databasemodel);
			database->setModel(proxymodel);
			proxymodel->setSortRole(Qt::InitialSortOrderRole);
			database->setSortingEnabled(true);

			for (int i = 0; i < databasemodel->columnCount(); i++) {
				database->resizeColumnToContents(i);
			}
			database->setColumnWidth(1, 400);
			database->setColumnWidth(2, 400);
			database->setColumnWidth(5, 400);

			if (progress.wasCanceled()) {
				setDataModified(false);
			}
			else {
				setDataModified(true);
			}

			progress.setValue(importedbricks.size());

		}

		inputstream.close();

	}
}


void cBricksDatabaseWidget::selectAll() {
	for (int i = 0; i < proxymodel->rowCount(); i++) {
		databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 0)))->setData(QVariant::fromValue(true), Qt::DisplayRole);
	}
}


void cBricksDatabaseWidget::unselectAll() {
	for (int i = 0; i < proxymodel->rowCount(); i++) {
		databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 0)))->setData(QVariant::fromValue(false), Qt::DisplayRole);
	}
}


void cBricksDatabaseWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/blockseditor.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/blockseditor.html").absoluteFilePath()));
	#endif
}

