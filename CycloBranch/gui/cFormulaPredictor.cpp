#include "gui/cFormulaPredictor.h"

#include <QTextBrowser>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QScrollBar>
#include <QCheckBox>
#include <QPushButton>
#include <QSplitter>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QKeyEvent>
#include <QAction>
#include <QLabel>
#include <QList>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QClipboard>
#include <QApplication>

#include "core/cParameters.h"


cFormulaPredictor::cFormulaPredictor(cGlobalPreferences* globalpreferences, QWidget *parent) {
	this->parent = parent;
	this->globalpreferences = globalpreferences;

	formulapredictorthread = 0;
	chargevalue = 0;
	maximumcombinedelements = 0;
	basicformulacheck = false;
	advancedformulacheck = false;
	noratiocheck = false;
	mzvalue = 0;
	mzerror = 0;

	selectedions.clear();
	selectedelements.clear();
	ionitems.clear();

	success = false;
	terminatecomputation = false;
	errmsg = "";

	pchemdata.clear();

	setWindowTitle("Calculate Molecular Formula");
	setWindowIcon(QIcon(":/images/icons/formula.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuSearch = new QMenu(tr("&Search"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	toolbarFile = addToolBar(tr("File"));

	actionExportTable = new QAction(QIcon(":/images/icons/csv.png"), tr("&Export Table to CSV"), this);
	actionExportTable->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	actionExportTable->setToolTip("Export Table to CSV (Ctrl + E)");
	toolbarFile->addAction(actionExportTable);
	connect(actionExportTable, SIGNAL(triggered()), this, SLOT(exportTableToCSV()));

	actionCloseWindow = new QAction(QIcon(":/images/icons/33.png"), tr("&Close"), this);
	actionCloseWindow->setShortcut(QKeySequence(Qt::Key_Escape));
	actionCloseWindow->setToolTip("Close (Esc)");
	toolbarFile->addAction(actionCloseWindow);
	connect(actionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));

	toolbarSearch = addToolBar(tr("Search"));

	actionSearchPubChem = new QAction(QIcon(":/images/icons/search.png"), tr("Search &PubChem..."), this);
	actionSearchPubChem->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	actionSearchPubChem->setToolTip("Search PubChem... (Ctrl + P)");
	toolbarSearch->addAction(actionSearchPubChem);
	connect(actionSearchPubChem, SIGNAL(triggered()), this, SLOT(searchPubChem()));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	const int formwidth = 350;

	formlayout = new QFormLayout();

	doublespinboxmz = new QDoubleSpinBox();
	doublespinboxmz->setToolTip("Enter the value of m/z.");
	doublespinboxmz->setFixedWidth(formwidth);
	doublespinboxmz->setDecimals(6);
	doublespinboxmz->setRange(0, 1000000);
	doublespinboxmz->setSingleStep(1);
	doublespinboxmz->setValue(500.0);
	formlayout->addRow(tr("m/z: "), doublespinboxmz);

	spinboxcharge = new QSpinBox();
	spinboxcharge->setToolTip("Enter the value of charge. Use negative values in negative ion mode (e.g., -1).");
	spinboxcharge->setFixedWidth(formwidth);
	spinboxcharge->setRange(-100, 100);
	spinboxcharge->setSingleStep(1);
	spinboxcharge->setValue(1);
	formlayout->addRow(tr("Charge: "), spinboxcharge);

	doublespinboxmzerror = new QDoubleSpinBox();
	doublespinboxmzerror->setToolTip("Enter the m/z error tolerance in ppm.");
	doublespinboxmzerror->setFixedWidth(formwidth);
	doublespinboxmzerror->setDecimals(6);
	doublespinboxmzerror->setRange(0, 10000);
	doublespinboxmzerror->setSingleStep(1);
	doublespinboxmzerror->setValue(5.0);
	doublespinboxmzerror->setSuffix(" ppm");
	formlayout->addRow(tr("m/z Error Tolerance: "), doublespinboxmzerror);

	widgetiontypes = new cFragmentIonsListWidget(this);
	widgetiontypes->setToolTip("Select the types of ions to be generated.");
	widgetiontypes->setFixedWidth(formwidth);
	formlayout->addRow(tr("Ion Types: "), widgetiontypes);

	widgetelements = new cNeutralLossesListWidget(false, true, this);
	widgetelements->setToolTip("Define and select the chemical elements. Use the colon to limit the maximum number of occurrences of an element (e.g., S:2).");
	widgetelements->setFixedWidth(formwidth);
	formlayout->addRow(tr("Chemical Elements: "), widgetelements);

	spinboxmaximumcombinedelements = new QSpinBox();
	spinboxmaximumcombinedelements->setToolTip("Enter the maximum number of chemical elements in a neutral molecule (i.e., M).");
	spinboxmaximumcombinedelements->setRange(1, 10000);
	spinboxmaximumcombinedelements->setSingleStep(1);
	spinboxmaximumcombinedelements->setFixedWidth(formwidth);
	spinboxmaximumcombinedelements->setValue(200);
	formlayout->addRow(tr("Maximum Number of Combined Elements "), spinboxmaximumcombinedelements);

	checkboxbasicformula = new QCheckBox();
	checkboxbasicformula->setToolTip("Apply Senior's filtering rules.");
	checkboxbasicformula->setFixedWidth(formwidth);
	checkboxbasicformula->setChecked(true);
	formlayout->addRow(tr("Basic Formula Check: "), checkboxbasicformula);

	checkboxadvancedformula = new QCheckBox();
	checkboxadvancedformula->setToolTip("Apply advanced filtering rules.");
	checkboxadvancedformula->setFixedWidth(formwidth);
	checkboxadvancedformula->setChecked(true);
	formlayout->addRow(tr("Advanced Formula Check: "), checkboxadvancedformula);

	checkboxnoratio = new QCheckBox();
	checkboxnoratio->setToolTip("Check if the number of nitrogen atoms is less or equal to the number of oxygen atoms.");
	checkboxnoratio->setFixedWidth(formwidth);
	checkboxnoratio->setChecked(true);
	formlayout->addRow(tr("N/O Ratio Check: "), checkboxnoratio);

	predictbutton = new QPushButton("Calculate");
	predictbutton->setFixedWidth(100);
	//formlayout->addRow("", predictbutton);
	formlayout->addRow(predictbutton);
	connect(predictbutton, SIGNAL(released()), this, SLOT(predictButtonReleased()));

	cancelbutton = new QPushButton("Cancel");
	cancelbutton->setFixedWidth(100);
	//formlayout->addRow("", cancelbutton);
	formlayout->addRow(cancelbutton);
	connect(cancelbutton, SIGNAL(released()), this, SLOT(cancelButtonReleased()));

	formwidget = new QWidget();
	formwidget->setLayout(formlayout);

	peakstable = new QTableView(this);
	peakstablemodel = new QStandardItemModel(0, 0, this);
	proxymodel = new cSpectrumDetailProxyModel(this);
	proxymodel->setSourceModel(peakstablemodel);
	proxymodel->setDynamicSortFilter(false);
	peakstable->setModel(proxymodel);
	peakstable->setSortingEnabled(true);

	hsplitter = new QSplitter();
	hsplitter->setOrientation(Qt::Horizontal);

	vsplitter1 = new QSplitter();
	vsplitter1->setOrientation(Qt::Vertical);

	vsplitter2 = new QSplitter();
	vsplitter2->setOrientation(Qt::Vertical);

	vsplitter1->addWidget(peakstable);
	vsplitter2->addWidget(formwidget);

	QList<int> sizes;
	sizes.push_back(1900);
	sizes.push_back(100);

	hsplitter->addWidget(vsplitter1);
	hsplitter->addWidget(vsplitter2);

	hsplitter->setSizes(sizes);

	setCentralWidget(hsplitter);
	centralWidget()->setContentsMargins(5, 5, 5, 5);

	menuFile->addAction(actionExportTable);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuSearch->addAction(actionSearchPubChem);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuSearch);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	connect(widgetiontypes, SIGNAL(resetReleased()), this, SLOT(resetIonTypes()));

	resize(defaultwinsizex, defaultwinsizey);

	applyGlobalPreferences();
}


cFormulaPredictor::~cFormulaPredictor() {
	delete doublespinboxmz;
	delete spinboxcharge;
	delete doublespinboxmzerror;
	delete widgetiontypes;
	delete widgetelements;
	delete spinboxmaximumcombinedelements;
	delete checkboxbasicformula;
	delete checkboxadvancedformula;
	delete checkboxnoratio;
	delete predictbutton;
	delete cancelbutton;

	delete formlayout;
	delete formwidget;

	deleteTable();

	delete peakstablemodel;
	delete proxymodel;
	delete peakstable;

	delete vsplitter1;
	delete vsplitter2;
		
	delete hsplitter;

	delete actionExportTable;
	delete actionCloseWindow;
	delete actionSearchPubChem;
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuSearch;
	delete menuHelp;

	delete menuBar;
}


void cFormulaPredictor::closeEvent(QCloseEvent *event) {
	hide();
	event->accept();
}


void cFormulaPredictor::applyGlobalPreferences() {
	if (globalpreferences) {
		resetIonTypes();
	}
}


void cFormulaPredictor::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		predictButtonReleased();
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C)) {
		QModelIndexList selectedindexes = peakstable->selectionModel()->selectedIndexes();
		QStandardItem* item;

		QString selectedtext;
		string itemtext;

		int selectedcount = selectedindexes.count();
		for (int i = 0; i < selectedcount; i++) {
			if (i > 0) {
				if (proxymodel->mapToSource(selectedindexes[i - 1]).row() != proxymodel->mapToSource(selectedindexes[i]).row()) {
					selectedtext += "\n";
				}
				else {
					selectedtext += "\t";
				}
			}

			item = peakstablemodel->itemFromIndex(proxymodel->mapToSource(selectedindexes[i]));
			if (item) {
				itemtext = item->text().toStdString();
				selectedtext += stripHTML(itemtext).c_str();
			}
		}
		selectedtext += "\n";
		QApplication::clipboard()->setText(selectedtext);
	}

	event->accept();
}


void cFormulaPredictor::preparePeaksTable() {
	deleteTable();

	pchemdata.clear();

	peakstable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	peakstable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	peakstable->horizontalHeader()->setSectionsMovable(true);
	//peakstable->verticalHeader()->setDefaultSectionSize(30);

	peakstablemodel->setColumnCount(7);

	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		peakstablemodel->setHorizontalHeaderItem(i, new QStandardItem());
	}

	int currentcolumn = 0;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Compound");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical m/z");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Error [ppm]");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Selected Ion");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Charge");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("PubChem Results");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Reference");
	peakstable->setItemDelegateForColumn(currentcolumn, new cViewButtonDelegate());

	int formulascount = (int)ionitems.size();
	peakstablemodel->setRowCount(formulascount);

	peakstable->setModel(0);
	proxymodel->setSourceModel(0);
	peakstable->setSortingEnabled(false);

	string link;
	for (int i = 0; i < formulascount; i++) {
		currentcolumn = 0;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setText(ionitems[i].ionformula.c_str());
		currentcolumn++;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(ionitems[i].theoreticalmz)), Qt::DisplayRole);
		currentcolumn++;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(ppmError(mzvalue, ionitems[i].theoreticalmz))), Qt::DisplayRole);
		currentcolumn++;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setText(ionitems[i].iontype.c_str());
		currentcolumn++;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(ionitems[i].charge), Qt::DisplayRole);
		currentcolumn++;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		currentcolumn++;

		link = "https://pubchem.ncbi.nlm.nih.gov/#query=" + ionitems[i].ionformula;
		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setText(link.c_str());
	}

	proxymodel->setSourceModel(peakstablemodel);
	peakstable->setModel(proxymodel);
	peakstable->setSortingEnabled(true);

	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		peakstable->resizeColumnToContents(i);
	}

	//peakstable->setColumnWidth(4, min(1200, peakstable->columnWidth(4)));
}


void cFormulaPredictor::deleteTable() {
	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		if (peakstable->itemDelegateForColumn(i)) {
			delete peakstable->itemDelegateForColumn(i);
		}
	}
	peakstablemodel->clear();
}


void cFormulaPredictor::predictButtonReleased() {
	if (!globalpreferences) {
		return;
	}

	chargevalue = spinboxcharge->value();
	maximumcombinedelements = spinboxmaximumcombinedelements->value();

	basicformulacheck = checkboxbasicformula->isChecked();
	advancedformulacheck = checkboxadvancedformula->isChecked();
	noratiocheck = checkboxnoratio->isChecked();

	mzvalue = doublespinboxmz->value();
	mzerror = doublespinboxmzerror->value();

	selectedions.clear();
	selectedelements.clear();
	ionitems.clear();

	QMessageBox msgbox;
	errmsg = "";

	if (chargevalue == 0) {
		errmsg = "The charge cannot be zero !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	for (int i = 0; i < widgetiontypes->getList()->count(); i++) {
		if (widgetiontypes->getList()->item(i)->isSelected()) {
			if ((globalpreferences->customions[i].positive && (chargevalue > 0)) || (!globalpreferences->customions[i].positive && (chargevalue < 0))) {
				selectedions.push_back(globalpreferences->customions[i]);
			}
		}
	}

	if (selectedions.empty()) {
		errmsg = "Please, select at least one valid ion type.\n\nIf charge > 0, select positive ions only.\nIf charge < 0, select negative ions only.";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	for (int i = 0; i < widgetelements->getList()->count(); i++) {
		if (widgetelements->getList()->item(i)->isSelected()) {
			selectedelements.push_back(widgetelements->getList()->item(i)->text().toStdString());
		}
	}

	if (selectedelements.empty()) {
		errmsg = "Please, select some chemical elements first.";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	predictbutton->setDisabled(true);
	predictbutton->setText("Calculating...");

	success = false;
	terminatecomputation = false;

	//cout << "Starting thread..." << endl;

	formulapredictorthread = new cFormulaPredictorThread();
	formulapredictorthread->initialize(chargevalue, maximumcombinedelements, basicformulacheck, advancedformulacheck, noratiocheck, mzvalue, mzerror, selectedions, selectedelements, ionitems, success, terminatecomputation, errmsg);
	connect(formulapredictorthread, SIGNAL(finished()), this, SLOT(formulaPredictorThreadFinished()));
	formulapredictorthread->start();
}


void cFormulaPredictor::cancelButtonReleased() {
	terminatecomputation = true;
}


void cFormulaPredictor::exportTableToCSV() {
	QString currentdir = "./";
	if (globalpreferences) {
		currentdir = globalpreferences->exportcsvdefaultdir;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Export Table to CSV..."), currentdir, "Files (*.csv)");
	
	if (!filename.isEmpty()) {

		QProgressDialog progress("Exporting the CSV file...", "Cancel", 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);
		progress.setValue(0);
		
		bool removefile = false;
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		QTextStream out(&file);

		for (int i = 0; i < peakstablemodel->columnCount(); i++) {
			out << "\"" << peakstablemodel->horizontalHeaderItem(i)->text() << "\"";
			if (i < peakstablemodel->columnCount() - 1) {
				out << ",";
			}
		}
		out << endl;

		QStandardItem* item;
		string str;
		for (int i = 0; i < proxymodel->rowCount(); i++) {

			for (int j = 0; j < peakstablemodel->columnCount(); j++) {
				item = peakstablemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, j)));
				if (item) {
					str = item->data(Qt::DisplayRole).toString().toStdString();
					out << "\"" << stripHTML(str).c_str() << "\"";
					if (j < peakstablemodel->columnCount() - 1) {
						out << ",";
					}
				}
				else {
					if (j < peakstablemodel->columnCount() - 1) {
						out << ",";
					}
				}
			}
			out << endl;

			progress.setValue(i);
			if (progress.wasCanceled()) {
				removefile = true;
				break;
			}
		}

		file.close();

		if (removefile) {
			file.remove();
		}

		progress.setValue(proxymodel->rowCount());

	}
}


void cFormulaPredictor::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/calculatemolecularformula.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/calculatemolecularformula.html").absoluteFilePath()));
	#endif
}


void cFormulaPredictor::closeWindow() {
	hide();
}


void cFormulaPredictor::resetIonTypes() {
	if (globalpreferences) {
		widgetiontypes->getList()->clear();
		for (int i = 0; i < (int)globalpreferences->customions.size(); i++) {
			widgetiontypes->getList()->addItem(globalpreferences->customions[i].name.c_str());
			if (globalpreferences->customions[i].name.compare("[M+H]+") == 0) {
				widgetiontypes->getList()->item(i)->setSelected(true);
			}
		}
	}
}


void cFormulaPredictor::formulaPredictorThreadFinished() {
	QMessageBox msgbox;
	
	if (success) {
		preparePeaksTable();
	}
	else {
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
	}

	predictbutton->setText("Calculate");
	predictbutton->setDisabled(false);
}


void cFormulaPredictor::searchPubChem() {
	bool repeatsearch = false;
	if (pchemdata.size() > 0) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Search PubChem...", "Search again ?", QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes) {
			repeatsearch = true;
		}
	}

	if (repeatsearch || (pchemdata.size() == 0)) {

		int formulacol = 0;
		int pchemcol = 5;

		set<string> formulas;
		QStandardItem* itemname;
		QStandardItem* pchemres;

		pchemdata.clear();

		for (int i = 0; i < proxymodel->rowCount(); i++) {
			itemname = peakstablemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, formulacol)));
			if (itemname) {
				formulas.insert(itemname->text().toStdString());
			}
		}

		if (formulas.size() == 0) {
			return;
		}

		const int limit = 500;

		if (formulas.size() > limit) {
			QMessageBox msgBox;
			QString errstr = "The maximum number of formulas to be searched is " + QVariant(limit).toString() + ". You generated " + QVariant((int)formulas.size()).toString() + " formulas. ";
			errstr += "Please, optimize the search settings.";
			msgBox.setText(errstr);
			msgBox.exec();
			return;
		}

		int maxthreadsbackup = QThreadPool::globalInstance()->maxThreadCount();
		int maxparallelqueries = 2;

		QThreadPool::globalInstance()->setMaxThreadCount(maxparallelqueries);

		vector<string> vformulas;
		for (auto& it : formulas) {
			vformulas.push_back(it);
		}

		QProgressDialog firstdialog("Searching compounds...", "Cancel", 0, (int)formulas.size(), this);
		firstdialog.setMinimumWidth(250);
		cEventFilter firstfilter;
		firstdialog.installEventFilter(&firstfilter);
		firstdialog.setMinimumDuration(0);
		firstdialog.setWindowModality(Qt::ApplicationModal);
		firstdialog.setValue(0);

		QFutureWatcher<string> firstfuturewatcher;
		QObject::connect(&firstfuturewatcher, SIGNAL(finished()), &firstdialog, SLOT(reset()));
		QObject::connect(&firstdialog, SIGNAL(canceled()), &firstfuturewatcher, SLOT(cancel()));
		QObject::connect(&firstfuturewatcher, SIGNAL(progressRangeChanged(int, int)), &firstdialog, SLOT(setRange(int, int)));
		QObject::connect(&firstfuturewatcher, SIGNAL(progressValueChanged(int)), &firstdialog, SLOT(setValue(int)));

		QFuture<string> firstfuture = QtConcurrent::mapped(vformulas.begin(), vformulas.end(), cPubchemCIDReader());

		firstfuturewatcher.setFuture(firstfuture);

		firstdialog.exec();

		if (firstfuturewatcher.isCanceled()) {
			QThreadPool::globalInstance()->clear();
		}

		firstfuturewatcher.waitForFinished();

		if (!firstfuturewatcher.isCanceled()) {

			cPubchemCIDReader pubchemformulareader;
			string firstfutureresult;
			for (int i = 0; i < (int)vformulas.size(); i++) {
				firstfutureresult = firstfuture.resultAt(i);

				if (firstfutureresult.substr(0, 6).compare("@ERROR") == 0) {
					firstfutureresult.clear();
				}

				pchemdata[vformulas[i]] = pubchemformulareader.getNumberOfCIDs(firstfutureresult);
			}

		}

		QThreadPool::globalInstance()->setMaxThreadCount(maxthreadsbackup);

		for (int i = 0; i < proxymodel->rowCount(); i++) {
			itemname = peakstablemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, formulacol)));
			pchemres = peakstablemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, pchemcol)));

			if (itemname && pchemres) {
				pchemres->setData(QVariant::fromValue(pchemdata[itemname->text().toStdString()]), Qt::DisplayRole);
			}
		}

	}

}

