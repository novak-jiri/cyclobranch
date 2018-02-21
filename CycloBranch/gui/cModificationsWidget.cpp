#include "gui/cModificationsWidget.h"
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


cModificationsWidget::cModificationsWidget(QWidget* parent) {
	this->parent = parent;

	editorname = "Modifications Editor";

	setWindowTitle(editorname);
	setWindowIcon(QIcon(":/images/icons/61.png"));

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
	proxymodel = new cModificationsProxyModel(this);
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

	resize(1280, 750);

	databasefile = "";

	#if OS_TYPE == WIN
		lastdir = "./Modifications/";
	#else
		lastdir = installdir + "Modifications/";
	#endif
	
	modifications.clear();

	setDataModified(false);
}


cModificationsWidget::~cModificationsWidget() {
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


void cModificationsWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


void cModificationsWidget::deleteTable() {
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		if (database->itemDelegateForColumn(i)) {
			delete database->itemDelegateForColumn(i);
		}
	}

	databasemodel->clear();
	databasemodel->setRowCount(0);
}


void cModificationsWidget::resetHeader() {
	databasemodel->setColumnCount(6);

	databasemodel->setHorizontalHeaderItem(0, new QStandardItem());
	database->setItemDelegateForColumn(0, new cCheckBoxDelegate());
	databasemodel->setHorizontalHeaderItem(1, new QStandardItem("Name"));
	databasemodel->setHorizontalHeaderItem(2, new QStandardItem("Summary Formula"));
	databasemodel->setHorizontalHeaderItem(3, new QStandardItem("Monoisotopic Mass"));
	databasemodel->setHorizontalHeaderItem(4, new QStandardItem("N-terminal"));
	database->setItemDelegateForColumn(4, new cCheckBoxDelegate());
	databasemodel->setHorizontalHeaderItem(5, new QStandardItem("C-terminal"));
	database->setItemDelegateForColumn(5, new cCheckBoxDelegate());

	database->horizontalHeader()->setStretchLastSection(true);
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}
	database->setColumnWidth(1, 400);
	database->setColumnWidth(2, 400);
	database->setColumnWidth(4, (database->horizontalHeader()->sectionSize(4) + database->horizontalHeader()->sectionSize(5)) / 2);
}


bool cModificationsWidget::checkTable() {
	// check summary formulas
	for (int i = 0; i < databasemodel->rowCount(); i++) {
		if (!checkFormula(i, databasemodel->item(i, 2)->text().toStdString())) {
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
	if (databasemodel->item(row, 3)) {
		databasemodel->item(row, 3)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(formula.getMass())), Qt::DisplayRole);
	}
	return true;
}


void cModificationsWidget::setDataModified(bool datamodified) {
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


void cModificationsWidget::keyPressEvent(QKeyEvent *event) {
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


void cModificationsWidget::closeWindow() {
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


void cModificationsWidget::openDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Database"), lastdir, tr("Database of Modifications (*.txt)"));

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

			modifications.clear();
			loadModificationsFromPlainTextStream(inputstream, modifications, errormessage, true);

			QProgressDialog progress("Loading Modifications...", "Cancel", 0, (int)modifications.size(), this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

			database->setModel(0);
			proxymodel->setSourceModel(0);
			database->setSortingEnabled(false);

			databasemodel->setRowCount((int)modifications.size());
			for (int i = 0; i < (int)modifications.size(); i++) {

				databasemodel->setItem(i, 0, new QStandardItem());
				databasemodel->item(i, 0)->setFlags(databasemodel->item(i, 0)->flags() & ~Qt::ItemIsEditable);

				databasemodel->setItem(i, 1, new QStandardItem());
				databasemodel->item(i, 1)->setText(modifications[i].name.c_str());

				databasemodel->setItem(i, 2, new QStandardItem());
				databasemodel->item(i, 2)->setText(modifications[i].summary.c_str());

				databasemodel->setItem(i, 3, new QStandardItem());
				databasemodel->item(i, 3)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(modifications[i].massdifference)), Qt::DisplayRole);

				databasemodel->setItem(i, 4, new QStandardItem());
				databasemodel->item(i, 4)->setFlags(databasemodel->item(i, 4)->flags() & ~Qt::ItemIsEditable);
				if (modifications[i].nterminal) {
					databasemodel->item(i, 4)->setData(QVariant::fromValue(true), Qt::DisplayRole);
				}

				databasemodel->setItem(i, 5, new QStandardItem());
				databasemodel->item(i, 5)->setFlags(databasemodel->item(i, 5)->flags() & ~Qt::ItemIsEditable);
				if (modifications[i].cterminal) {
					databasemodel->item(i, 5)->setData(QVariant::fromValue(true), Qt::DisplayRole);
				}

				progress.setValue(i);
				if (progress.wasCanceled()) {
					deleteTable();
					resetHeader();
					modifications.clear();
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
			database->setColumnWidth(4, (database->horizontalHeader()->sectionSize(4) + database->horizontalHeader()->sectionSize(5)) / 2);

			progress.setValue((int)modifications.size());

			setDataModified(false);

		}

		inputstream.close();

	}
}


bool cModificationsWidget::saveDatabase() {
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

		QProgressDialog progress("Saving Modifications...", 0, 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		fragmentDescription modification;
		modifications.clear();

		for (int i = 0; i < proxymodel->rowCount(); i++) {

			modification.clear();
			for (int j = 0; j < proxymodel->columnCount(); j++) {
				switch (j)
				{
				case 0:
					// nothing to do
					break;
				case 1:
					modification.name = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					modification.name = removeWhiteSpacesExceptSpaces(modification.name);
					break;
				case 2:
					modification.summary = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					modification.summary = removeWhiteSpacesExceptSpaces(modification.summary);
					break;
				case 3:
					modification.massdifference = proxymodel->index(i, j).data(Qt::DisplayRole).toDouble();
					break;
				case 4:
					modification.nterminal = proxymodel->index(i, j).data(Qt::DisplayRole).toBool();
					break;
				case 5:
					modification.cterminal = proxymodel->index(i, j).data(Qt::DisplayRole).toBool();
					break;
				default:
					break;
				}
			}
			modifications.push_back(modification);

			progress.setValue(i);
		}

		storeModificationsToPlainTextStream(outputstream, modifications);

		progress.setValue(progress.maximum());

		setDataModified(false);

		saved = true;

	}
	outputstream.close();

	return saved;
}


bool cModificationsWidget::saveDatabaseAs() {
	if (!checkTable()) {
		return false;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Database As..."), lastdir, tr("Database of Modifications (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
	
		databasefile = filename;
		setWindowTitle(editorname + QString(" - ") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()));
		return saveDatabase();
	}

	return false;
}


void cModificationsWidget::addRow() {
	resetFilter();

	int row = databasemodel->rowCount();
	databasemodel->insertRow(row);

	databasemodel->setItem(row, 0, new QStandardItem());
	databasemodel->item(row, 0)->setFlags(databasemodel->item(row, 0)->flags() & ~Qt::ItemIsEditable);

	databasemodel->setItem(row, 1, new QStandardItem());
	databasemodel->setItem(row, 2, new QStandardItem());
	databasemodel->setItem(row, 3, new QStandardItem());

	databasemodel->setItem(row, 4, new QStandardItem());
	databasemodel->item(row, 4)->setFlags(databasemodel->item(row, 4)->flags() & ~Qt::ItemIsEditable);

	databasemodel->setItem(row, 5, new QStandardItem());
	databasemodel->item(row, 5)->setFlags(databasemodel->item(row, 5)->flags() & ~Qt::ItemIsEditable);
	
	database->scrollToBottom();
}


void cModificationsWidget::removeSelectedRows() {
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


void cModificationsWidget::itemChanged(QStandardItem* item) {
	if (!item) {
		return;
	}

	// recalculate mass when formula is changed
	if (item->column() == 2) {
		checkFormula(item->row(), item->text().toStdString());
	}
	
	if (item->column() > 0) {
		setDataModified(true);
	}
}


void cModificationsWidget::headerItemClicked(int index) {
	setDataModified(true);
}


void cModificationsWidget::filterRows() {
	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterCaseSensitivity(rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive);
	proxymodel->setFilterFixedString(rowsfilterline->text());
}


void cModificationsWidget::resetFilter() {
	rowsfilterline->setText("");

	database->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
	proxymodel->sort(-1);

	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterFixedString("");
}


void cModificationsWidget::createNewDatabase() {
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
	modifications.clear();
	databasefile = "";
	setWindowTitle(editorname);

	setDataModified(false);
}


void cModificationsWidget::importDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Import Database"), lastdir, tr("Database of Modifications (*.txt)"));

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

			vector<fragmentDescription> importedmodifications;
			loadModificationsFromPlainTextStream(inputstream, importedmodifications, errormessage, true);

			QProgressDialog progress("Importing the Database of Modifications...", "Cancel", 0, (int)importedmodifications.size(), this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

			database->setModel(0);
			proxymodel->setSourceModel(0);
			database->setSortingEnabled(false);

			int originalrowcount = databasemodel->rowCount();
			databasemodel->setRowCount(originalrowcount + (int)importedmodifications.size());
			for (int i = 0; i < (int)importedmodifications.size(); i++) {

				databasemodel->setItem(i + originalrowcount, 0, new QStandardItem());
				databasemodel->item(i + originalrowcount, 0)->setFlags(databasemodel->item(i + originalrowcount, 0)->flags() & ~Qt::ItemIsEditable);

				databasemodel->setItem(i + originalrowcount, 1, new QStandardItem());
				databasemodel->item(i + originalrowcount, 1)->setText(importedmodifications[i].name.c_str());

				databasemodel->setItem(i + originalrowcount, 2, new QStandardItem());
				databasemodel->item(i + originalrowcount, 2)->setText(importedmodifications[i].summary.c_str());

				databasemodel->setItem(i + originalrowcount, 3, new QStandardItem());
				databasemodel->item(i + originalrowcount, 3)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(importedmodifications[i].massdifference)), Qt::DisplayRole);

				databasemodel->setItem(i + originalrowcount, 4, new QStandardItem());
				databasemodel->item(i + originalrowcount, 4)->setFlags(databasemodel->item(i + originalrowcount, 4)->flags() & ~Qt::ItemIsEditable);
				if (importedmodifications[i].nterminal) {
					databasemodel->item(i + originalrowcount, 4)->setData(QVariant::fromValue(true), Qt::DisplayRole);
				}

				databasemodel->setItem(i + originalrowcount, 5, new QStandardItem());
				databasemodel->item(i + originalrowcount, 5)->setFlags(databasemodel->item(i + originalrowcount, 5)->flags() & ~Qt::ItemIsEditable);
				if (importedmodifications[i].cterminal) {
					databasemodel->item(i + originalrowcount, 5)->setData(QVariant::fromValue(true), Qt::DisplayRole);
				}

				modifications.push_back(importedmodifications[i]);

				progress.setValue(i);
				if (progress.wasCanceled()) {
					deleteTable();
					resetHeader();
					modifications.clear();
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
			database->setColumnWidth(4, (database->horizontalHeader()->sectionSize(4) + database->horizontalHeader()->sectionSize(5)) / 2);

			if (progress.wasCanceled()) {
				setDataModified(false);
			}
			else {
				setDataModified(true);
			}

			progress.setValue((int)importedmodifications.size());

		}

		inputstream.close();

	}
}


void cModificationsWidget::selectAll() {
	for (int i = 0; i < proxymodel->rowCount(); i++) {
		databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 0)))->setData(QVariant::fromValue(true), Qt::DisplayRole);
	}
}


void cModificationsWidget::unselectAll() {
	for (int i = 0; i < proxymodel->rowCount(); i++) {
		databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 0)))->setData(QVariant::fromValue(false), Qt::DisplayRole);
	}
}


void cModificationsWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/modificationseditor.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/modificationseditor.html").absoluteFilePath()));
	#endif
}

