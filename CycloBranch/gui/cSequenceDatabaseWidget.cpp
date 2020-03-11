#include "gui/cSequenceDatabaseWidget.h"
#include "gui/cMainThread.h"
#include "gui/cEventFilter.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QComboBox>
#include <QProgressDialog>
#include <QKeyEvent>
#include <QIcon>
#include <QLineEdit>
#include <QMenuBar>
#include <QMenu>


cSequenceDatabaseWidget::cSequenceDatabaseWidget(cGlobalPreferences* globalpreferences, QWidget* parent) {
	this->globalpreferences = globalpreferences;
	this->parent = parent;

	editorname = "Sequence/Compound Database Editor";

	setWindowTitle(editorname);
	setWindowIcon(QIcon(":/images/icons/26.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuEdit = new QMenu(tr("&Edit"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	rowsfilteroperator = new QComboBox();
	rowsfilteroperator->setToolTip("OR = any condition must be met; AND = all conditions must be met.");
	rowsfilteroperator->addItem("OR");
	rowsfilteroperator->addItem("AND");
	rowsfilteroperator->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfiltercombobox1 = new QComboBox();
	rowsfiltercombobox1->setToolTip("Column to be Searched");
	rowsfiltercombobox1->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfiltercomparatorcombobox1 = new QComboBox();
	rowsfiltercomparatorcombobox1->setToolTip("Type of Comparison");
	rowsfiltercomparatorcombobox1->addItem("=");
	rowsfiltercomparatorcombobox1->addItem("<");
	rowsfiltercomparatorcombobox1->addItem("<=");
	rowsfiltercomparatorcombobox1->addItem(">");
	rowsfiltercomparatorcombobox1->addItem(">=");
	rowsfiltercomparatorcombobox1->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfilterline1 = new QLineEdit();
	rowsfilterline1->setMinimumWidth(150);
	rowsfilterline1->setToolTip("Text to Find");

	rowsfiltercombobox2 = new QComboBox();
	rowsfiltercombobox2->setToolTip("Column to be Searched");
	rowsfiltercombobox2->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfiltercomparatorcombobox2 = new QComboBox();
	rowsfiltercomparatorcombobox2->setToolTip("Type of Comparison");
	rowsfiltercomparatorcombobox2->addItem("=");
	rowsfiltercomparatorcombobox2->addItem("<");
	rowsfiltercomparatorcombobox2->addItem("<=");
	rowsfiltercomparatorcombobox2->addItem(">");
	rowsfiltercomparatorcombobox2->addItem(">=");
	rowsfiltercomparatorcombobox2->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfilterline2 = new QLineEdit();
	rowsfilterline2->setMinimumWidth(150);
	rowsfilterline2->setToolTip("Text to Find");

	rowsfiltercasesensitive = new QCheckBox();
	rowsfiltercasesensitive->setToolTip("Case Sensitive");

	rowsfilterwholeword = new QCheckBox();
	rowsfilterwholeword->setToolTip("Whole Words Only");

	rowsfilterbutton = new QPushButton("Filter");
	rowsfilterbutton->setToolTip("Filter Search Results");
	rowsfilterbutton->setMinimumWidth(75);

	rowsfilterclearbutton = new QPushButton("Reset");
	rowsfilterclearbutton->setToolTip("Reset Search Results");
	rowsfilterclearbutton->setMinimumWidth(75);
	rowsfilterclearbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

	rowsfilterhbox = new QHBoxLayout();
	rowsfilterhbox->addWidget(rowsfiltercombobox1);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercomparatorcombobox1);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterline1);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilteroperator);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercombobox2);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercomparatorcombobox2);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterline2);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercasesensitive);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterwholeword);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterbutton);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterclearbutton);

	rowsfilterwidget = new QWidget();
	rowsfilterwidget->setLayout(rowsfilterhbox);

	database = new QTableView(this);
	databasemodel = new QStandardItemModel(0, 0, this);
	proxymodel = new cSequenceDatabaseProxyModel(this);
	proxymodel->setSourceModel(databasemodel);
	proxymodel->setDynamicSortFilter(false);
	proxymodel->initialize(rowsfilteroperator, rowsfiltercombobox1, rowsfiltercomparatorcombobox1, rowsfilterline1, rowsfiltercombobox2, rowsfiltercomparatorcombobox2, rowsfilterline2);
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
	connect(actionSaveDatabase, SIGNAL(triggered()), this, SLOT(saveDatabaseCheck()));

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

	addToolBarBreak();
	
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

	database->setMouseTracking(true);
	connect(database, SIGNAL(entered(const QModelIndex&)), this, SLOT(editItem(const QModelIndex&)));

	connect(database->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerItemClicked(int)));
	connect(databasemodel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterRows()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	setCentralWidget(mainwidget);

	resize(defaultwinsizex, defaultwinsizey);

	databasefile = "";

	applyGlobalPreferences(globalpreferences);

	sequences.clear();

	setDataModified(false);
}


cSequenceDatabaseWidget::~cSequenceDatabaseWidget() {	
	deleteTable();

	delete databasemodel;
	delete proxymodel;
	delete database;

	delete rowsfilteroperator;
	delete rowsfiltercombobox1;
	delete rowsfiltercomparatorcombobox1;
	delete rowsfilterline1;
	delete rowsfiltercombobox2;
	delete rowsfiltercomparatorcombobox2;
	delete rowsfilterline2;
	delete rowsfiltercasesensitive;
	delete rowsfilterwholeword;
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


void cSequenceDatabaseWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


void cSequenceDatabaseWidget::importSequence(int peptidetypeindex, QString sequence) {
	addRow();
	databasemodel->item(databasemodel->rowCount() - 1, 1)->setData(QVariant::fromValue(peptidetypeindex), Qt::DisplayRole);
	databasemodel->item(databasemodel->rowCount() - 1, 5)->setText(sequence);

	for (int i = 0; i < databasemodel->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}
	database->setColumnWidth(1, 150);
	database->setColumnWidth(2, 400);
	database->setColumnWidth(5, 400);
}


void cSequenceDatabaseWidget::applyGlobalPreferences(cGlobalPreferences* globalpreferences) {
	if (globalpreferences) {
		if (lastdir.right(4).compare(".txt", Qt::CaseInsensitive) != 0) {
			lastdir = globalpreferences->sequencesdefaultdir;
		}
	}
}


void cSequenceDatabaseWidget::deleteTable() {
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		if (database->itemDelegateForColumn(i)) {
			delete database->itemDelegateForColumn(i);
		}
	}

	databasemodel->clear();
	databasemodel->setRowCount(0);
}


void cSequenceDatabaseWidget::resetHeader() {
	databasemodel->setColumnCount(11);

	databasemodel->setHorizontalHeaderItem(0, new QStandardItem());
	database->setItemDelegateForColumn(0, new cCheckBoxDelegate());
	databasemodel->setHorizontalHeaderItem(1, new QStandardItem("Type"));
	database->setItemDelegateForColumn(1, new cComboBoxDelegate());
	databasemodel->setHorizontalHeaderItem(2, new QStandardItem("Name"));
	databasemodel->setHorizontalHeaderItem(3, new QStandardItem("Summary Formula"));
	databasemodel->setHorizontalHeaderItem(4, new QStandardItem("Monoisotopic Mass"));
	databasemodel->setHorizontalHeaderItem(5, new QStandardItem("Sequence"));
	databasemodel->setHorizontalHeaderItem(6, new QStandardItem("N-terminal Modification"));
	databasemodel->setHorizontalHeaderItem(7, new QStandardItem("C-terminal Modification"));
	databasemodel->setHorizontalHeaderItem(8, new QStandardItem("Branch Modification"));
	databasemodel->setHorizontalHeaderItem(9, new QStandardItem("Reference"));
	databasemodel->setHorizontalHeaderItem(10, new QStandardItem("View"));
	database->setItemDelegateForColumn(10, new cViewButtonDelegate());

	rowsfiltercombobox1->clear();
	rowsfiltercombobox2->clear();
	for (int i = 1; i < databasemodel->columnCount() - 1; i++) {
		rowsfiltercombobox1->addItem(databasemodel->horizontalHeaderItem(i)->text());
		rowsfiltercombobox2->addItem(databasemodel->horizontalHeaderItem(i)->text());
	}

	database->horizontalHeader()->setStretchLastSection(true);
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}
	database->setColumnWidth(1, 150);
	database->setColumnWidth(2, 400);
	database->setColumnWidth(5, 400);
}


bool cSequenceDatabaseWidget::checkTable() {

	// check summary formulas
	for (int i = 0; i < databasemodel->rowCount(); i++) {
		if (!checkFormula(i, databasemodel->item(i, 3)->text().toStdString())) {
			return false;
		}
	}

	// check sequences
	for (int i = 0; i < databasemodel->rowCount(); i++) {
		if (!checkSequence(i)) {
			return false;
		}
	}

	return true;
}


bool cSequenceDatabaseWidget::checkFormula(int row, const string& summary) {
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


bool cSequenceDatabaseWidget::checkSequence(int row) {

	if (databasemodel->item(row, 5)->text().toStdString().compare("") == 0) {
		return true;
	}

	regex rx;
	// [^\\[\\]]+ is used instead of .+ to prevent from a too complex regex error
	switch ((ePeptideType)(databasemodel->item(row, 1)->data(Qt::DisplayRole).toInt())) {
		case linear:
		case linearpolyketide:
			rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
			break;
		case cyclic:
		case cyclicpolyketide:
			rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+$";
			break;
		case branched:
			rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
			break;
		case branchcyclic:
			rx = "(^(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$|^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?$)";
			break;
		case other:
		default:
			rx = ".*";
			break;
	}

	try {
		if (!(regex_search(databasemodel->item(row, 5)->text().toStdString(), rx))) {
			QMessageBox msgBox;
			QString errstr = "Row no. ";
			errstr += to_string(row + 1).c_str();
			errstr += ". The format of sequence '";
			errstr += databasemodel->item(row, 5)->text().toStdString().c_str();
			errstr += "' does not correspond to the sequence type '";
			errstr += getStringFromPeptideType((ePeptideType)(databasemodel->item(row, 1)->data(Qt::DisplayRole).toInt())).c_str();
			errstr += "'.";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}
	catch (regex_error& e) {
		QMessageBox msgBox;
		QString errstr = "Row no. ";
		errstr += to_string(row + 1).c_str();
		errstr += ". cSequenceDatabaseWidget::checkSequence: regex_search failed, error no. ";
		errstr += to_string(e.code()).c_str();
		errstr += ". Please, check the sequence: '";
		errstr += databasemodel->item(row, 5)->text().toStdString().c_str();
		errstr += "'.";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	return true;
}


void cSequenceDatabaseWidget::setDataModified(bool datamodified) {
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


void cSequenceDatabaseWidget::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (rowsfilterline1->hasFocus() || rowsfilterline2->hasFocus()) {
			filterRows();
		}
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_F)) {
		rowsfilterline1->setFocus();
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_T)) {
		rowsfiltercasesensitive->setChecked(!rowsfiltercasesensitive->isChecked());
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_W)) {
		rowsfilterwholeword->setChecked(!rowsfilterwholeword->isChecked());
	}

	event->accept();
}


void cSequenceDatabaseWidget::closeWindow() {
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


void cSequenceDatabaseWidget::openDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Database"), lastdir, tr("Database of Sequences (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;

		databasefile = filename;
		setWindowTitle(editorname + QString(" - ") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()));
		
		inputstream.open(filename.toStdString().c_str());

		if (!inputstream.good()) {
			QMessageBox msgBox;
			msgBox.setText("Cannot open the file '" + databasefile + "'.");
			msgBox.exec();
		}
		else {

			string stmp;
			size_t pos;

			deleteTable();
			resetHeader();
			resetFilter();

			sequences.clear();
			sequences.loadFromPlainTextStream(inputstream);

			QProgressDialog progress("Loading the Sequence Database...", "Cancel", 0, sequences.size(), this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::ApplicationModal);

			database->setModel(0);
			proxymodel->setSourceModel(0);
			database->setSortingEnabled(false);

            cSummaryFormula formula;
            databasemodel->setRowCount(sequences.size());
            for (int i = 0; i < sequences.size(); i++) {

				databasemodel->setItem(i, 0, new QStandardItem());
				databasemodel->item(i, 0)->setFlags(databasemodel->item(i, 0)->flags() & ~Qt::ItemIsEditable);

				databasemodel->setItem(i, 1, new QStandardItem());
				databasemodel->item(i, 1)->setData(QVariant::fromValue((int)sequences[i].getPeptideType()), Qt::DisplayRole);

                databasemodel->setItem(i, 2, new QStandardItem());
				databasemodel->item(i, 2)->setText(sequences[i].getName().c_str());

                databasemodel->setItem(i, 3, new QStandardItem());
				databasemodel->item(i, 3)->setText(sequences[i].getSummaryFormula().c_str());

				formula.setFormula(sequences[i].getSummaryFormula());
                databasemodel->setItem(i, 4, new QStandardItem());
				databasemodel->item(i, 4)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(formula.getMass())), Qt::DisplayRole);

				databasemodel->setItem(i, 5, new QStandardItem());
				databasemodel->item(i, 5)->setText(sequences[i].getSequence().c_str());

				databasemodel->setItem(i, 6, new QStandardItem());
				databasemodel->item(i, 6)->setText(sequences[i].getNTterminalModification().c_str());

				databasemodel->setItem(i, 7, new QStandardItem());
				databasemodel->item(i, 7)->setText(sequences[i].getCTterminalModification().c_str());

				databasemodel->setItem(i, 8, new QStandardItem());
				databasemodel->item(i, 8)->setText(sequences[i].getBranchModification().c_str());

				databasemodel->setItem(i, 9, new QStandardItem());
				databasemodel->item(i, 9)->setText(sequences[i].getReference().c_str());

                databasemodel->setItem(i, 10, new QStandardItem());
				databasemodel->item(i, 10)->setText("");
				stmp = sequences[i].getNameWithReferenceAsHTMLString();
				pos = stmp.find('\"');
				if (pos + 1 != string::npos) {
					stmp = stmp.substr(pos + 1);
					pos = stmp.rfind('\"');
					if (pos != string::npos) {
						stmp = stmp.substr(0, pos);
						databasemodel->item(i, 10)->setText(stmp.c_str());
					}
				}

				progress.setValue(i);
				if (progress.wasCanceled()) {
					deleteTable();
					resetHeader();
					sequences.clear();
					databasefile = "";
					setWindowTitle(editorname);
					break;
				}
			}

			proxymodel->setSourceModel(databasemodel);
			database->setModel(proxymodel);
			database->setSortingEnabled(true);
			proxymodel->sort(-1);
			
			for (int i = 0; i < databasemodel->columnCount(); i++) {
				database->resizeColumnToContents(i);
			}
			database->setColumnWidth(1, 150);
			database->setColumnWidth(2, 400);
			database->setColumnWidth(5, 400);

			progress.setValue(sequences.size());

			setDataModified(false);
						
		}

		inputstream.close();

	}
}


bool cSequenceDatabaseWidget::saveDatabase() {
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

		QProgressDialog progress("Saving the Sequence Database...", 0, 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);

		// close an editor of a combobox
		database->setDisabled(true);
		database->setDisabled(false);
		database->setFocus();

		cSequence seq;
		sequences.clear();
		string s;

		for (int i = 0; i < proxymodel->rowCount(); i++) {

			seq.clear();
			for (int j = 0; j < proxymodel->columnCount(); j++) {
				switch (j)
				{
				case 0:
					// nothing to do
					break;
				case 1:
					seq.setPeptideType((ePeptideType)(proxymodel->index(i, j).data(Qt::DisplayRole).toInt()));
					break;
				case 2:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					seq.setName(removeWhiteSpacesExceptSpaces(s));
					break;
				case 3:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					seq.setSummaryFormula(removeWhiteSpacesExceptSpaces(s));
					break;
				case 4:
					// nothing to do
					break;
				case 5:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					seq.setSequence(removeWhiteSpacesExceptSpaces(s));
					break;
				case 6:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					seq.setNTterminalModification(removeWhiteSpacesExceptSpaces(s));
					break;
				case 7:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					seq.setCTterminalModification(removeWhiteSpacesExceptSpaces(s));
					break;
				case 8:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					seq.setBranchModification(removeWhiteSpacesExceptSpaces(s));
					break;
				case 9:
					s = proxymodel->index(i, j).data(Qt::DisplayRole).toString().toStdString();
					seq.setReference(removeWhiteSpacesExceptSpaces(s));
					break;
				default:
					break;
				}
			}
			sequences.push_back(seq);
			
			progress.setValue(i);
		}

		sequences.storeToPlainTextStream(outputstream);

		progress.setValue(progress.maximum());
		
		setDataModified(false);
		
		saved = true;

	}
	outputstream.close();

	return saved;
}


bool cSequenceDatabaseWidget::saveDatabaseAs() {
	if (!checkTable()) {
		return false;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Database As..."), lastdir, tr("Database of Sequences (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
	
		databasefile = filename;
		setWindowTitle(editorname + QString(" - ") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()));
		return saveDatabase();
	}

	return false;
}


void cSequenceDatabaseWidget::saveDatabaseCheck() {
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, appname, "Save changes ?", QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::No) {
		return;
	}

	saveDatabase();
}


void cSequenceDatabaseWidget::addRow() {
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
	databasemodel->setItem(row, 7, new QStandardItem());
	databasemodel->setItem(row, 8, new QStandardItem());
	databasemodel->setItem(row, 9, new QStandardItem());
	databasemodel->setItem(row, 10, new QStandardItem());

	database->scrollToBottom();
}


void cSequenceDatabaseWidget::removeSelectedRows() {
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


void cSequenceDatabaseWidget::itemChanged(QStandardItem* item) {
	if (!item) {
		return;
	}

	// recalculate mass when formula is changed
	if (item->column() == 3) {
		checkFormula(item->row(), item->data(Qt::DisplayRole).toString().toStdString());
	}

	// check sequence
	if (item->column() == 5) {
		checkSequence(item->row());
	}

	// update references preview
	if (((item->column() == 2) || (item->column() == 9)) && databasemodel->item(item->row(), 10)) {
		cSequence seq;
		string stmp;
		size_t pos;

		seq.setName(databasemodel->index(item->row(), 2).data(Qt::DisplayRole).toString().toStdString());
		seq.setReference(databasemodel->index(item->row(), 9).data(Qt::DisplayRole).toString().toStdString());
		
		stmp = seq.getNameWithReferenceAsHTMLString();
		pos = stmp.find('\"');
		if (pos + 1 != string::npos) {
			stmp = stmp.substr(pos + 1);
			pos = stmp.rfind('\"');
			if (pos != string::npos) {
				stmp = stmp.substr(0, pos);
				databasemodel->item(item->row(), 10)->setText(stmp.c_str());
			}
		}

	}

	if (item->column() > 0) {
		setDataModified(true);
	}
}


void cSequenceDatabaseWidget::headerItemClicked(int index) {
	setDataModified(true);
}


void cSequenceDatabaseWidget::filterRows() {
	proxymodel->setWholeWord(rowsfilterwholeword->isChecked());
	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterCaseSensitivity(rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive);
	proxymodel->setFilterFixedString("");
}


void cSequenceDatabaseWidget::resetFilter() {
	rowsfilterline1->setText("");
	rowsfilterline2->setText("");

	database->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
	proxymodel->sort(-1);

	proxymodel->setWholeWord(false);
	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterFixedString("");
}


void cSequenceDatabaseWidget::createNewDatabase() {
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
	sequences.clear();
	databasefile = "";
	setWindowTitle(editorname);

	setDataModified(false);
}


void cSequenceDatabaseWidget::importDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Import Database"), lastdir, tr("Database of Sequences (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;

		inputstream.open(filename.toStdString().c_str());

		if (!inputstream.good()) {
			QMessageBox msgBox;
			msgBox.setText("Cannot open the file '" + filename + "'.");
			msgBox.exec();
		}
		else {
			resetFilter();

			string stmp;
			size_t pos;

			cSequenceDatabase importedsequences;
			importedsequences.loadFromPlainTextStream(inputstream);

			QProgressDialog progress("Importing the Sequence Database...", "Cancel", 0, importedsequences.size(), this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::ApplicationModal);

			database->setModel(0);
			proxymodel->setSourceModel(0);
			database->setSortingEnabled(false);

            cSummaryFormula formula;
			int originalrowcount = databasemodel->rowCount();
			databasemodel->setRowCount(originalrowcount + importedsequences.size());
			for (int i = 0; i < importedsequences.size(); i++) {

				databasemodel->setItem(i + originalrowcount, 0, new QStandardItem());
				databasemodel->item(i + originalrowcount, 0)->setFlags(databasemodel->item(i + originalrowcount, 0)->flags() & ~Qt::ItemIsEditable);

				databasemodel->setItem(i + originalrowcount, 1, new QStandardItem());
				databasemodel->item(i + originalrowcount, 1)->setData(QVariant::fromValue((int)importedsequences[i].getPeptideType()), Qt::DisplayRole);

                databasemodel->setItem(i + originalrowcount, 2, new QStandardItem());
				databasemodel->item(i + originalrowcount, 2)->setText(importedsequences[i].getName().c_str());

                databasemodel->setItem(i + originalrowcount, 3, new QStandardItem());
				databasemodel->item(i + originalrowcount, 3)->setText(importedsequences[i].getSummaryFormula().c_str());

				formula.setFormula(importedsequences[i].getSummaryFormula());
                databasemodel->setItem(i + originalrowcount, 4, new QStandardItem());
				databasemodel->item(i + originalrowcount, 4)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(formula.getMass())), Qt::DisplayRole);

				databasemodel->setItem(i + originalrowcount, 5, new QStandardItem());
				databasemodel->item(i + originalrowcount, 5)->setText(importedsequences[i].getSequence().c_str());

				databasemodel->setItem(i + originalrowcount, 6, new QStandardItem());
				databasemodel->item(i + originalrowcount, 6)->setText(importedsequences[i].getNTterminalModification().c_str());

				databasemodel->setItem(i + originalrowcount, 7, new QStandardItem());
				databasemodel->item(i + originalrowcount, 7)->setText(importedsequences[i].getCTterminalModification().c_str());

				databasemodel->setItem(i + originalrowcount, 8, new QStandardItem());
				databasemodel->item(i + originalrowcount, 8)->setText(importedsequences[i].getBranchModification().c_str());

				databasemodel->setItem(i + originalrowcount, 9, new QStandardItem());
				databasemodel->item(i + originalrowcount, 9)->setText(importedsequences[i].getReference().c_str());

				databasemodel->setItem(i + originalrowcount, 10, new QStandardItem());
				databasemodel->item(i + originalrowcount, 10)->setText("");
				stmp = importedsequences[i].getNameWithReferenceAsHTMLString();
				pos = stmp.find('\"');
				if (pos + 1 != string::npos) {
					stmp = stmp.substr(pos + 1);
					pos = stmp.rfind('\"');
					if (pos != string::npos) {
						stmp = stmp.substr(0, pos);
						databasemodel->item(i + originalrowcount, 10)->setText(stmp.c_str());
					}
				}

				sequences.push_back(importedsequences[i]);

				progress.setValue(i);
				if (progress.wasCanceled()) {
					deleteTable();
					resetHeader();
					sequences.clear();
					databasefile = "";
					setWindowTitle(editorname);
					break;
				}
			}

			proxymodel->setSourceModel(databasemodel);
			database->setModel(proxymodel);
			database->setSortingEnabled(true);
			proxymodel->sort(-1);

			for (int i = 0; i < databasemodel->columnCount(); i++) {
				database->resizeColumnToContents(i);
			}
			database->setColumnWidth(1, 150);
			database->setColumnWidth(2, 400);
			database->setColumnWidth(5, 400);

			if (progress.wasCanceled()) {
				setDataModified(false);
			}
			else {
				setDataModified(true);
			}

			progress.setValue(importedsequences.size());
			
		}

		inputstream.close();

	}
}


void cSequenceDatabaseWidget::selectAll() {
	for (int i = 0; i < proxymodel->rowCount(); i++) {
		databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 0)))->setData(QVariant::fromValue(true), Qt::DisplayRole);
	}
}


void cSequenceDatabaseWidget::unselectAll() {
	for (int i = 0; i < proxymodel->rowCount(); i++) {
		databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 0)))->setData(QVariant::fromValue(false), Qt::DisplayRole);
	}
}


void cSequenceDatabaseWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/sequenceeditor.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/sequenceeditor.html").absoluteFilePath()));
	#endif
}


void cSequenceDatabaseWidget::editItem(const QModelIndex& index) {
	if (isActiveWindow()) {
		database->setDisabled(true);
		database->setDisabled(false);
		database->setFocus();
		if (index.column() == 1) {
			database->edit(index);
		}
	}
}

