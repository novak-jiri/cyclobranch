#include "gui/cSummaryPeaksTableWidget.h"
#include "core/cParameters.h"
#include "core/cTheoreticalSpectrumList.h"
#include "gui/cEventFilter.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QProgressDialog>
#include <QCheckBox>
#include <QKeyEvent>
#include <QIcon>
#include <QLineEdit>
#include <QBrush>
#include <QTextStream>
#include <QMenuBar>
#include <QMenu>


cSummaryPeaksTableWidget::cSummaryPeaksTableWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Summary Table of Matched Peaks");
	setWindowIcon(QIcon(":/images/icons/43.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
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
	proxymodel = new cSummaryPeaksTableProxyModel(this);
	proxymodel->setSourceModel(databasemodel);
	proxymodel->setDynamicSortFilter(false);
	database->setModel(proxymodel);
	database->setSortingEnabled(true);
	database->setEditTriggers(QAbstractItemView::NoEditTriggers);
	database->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	database->horizontalHeader()->setSectionsMovable(true);
	database->verticalHeader()->setDefaultSectionSize(25);

	toolbarFile = addToolBar(tr("File"));
				
	actionExportCSV = new QAction(QIcon(":/images/icons/62.png"), tr("&Export to CSV"), this);
	actionExportCSV->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	actionExportCSV->setToolTip("Export to CSV (Ctrl + E)");
	toolbarFile->addAction(actionExportCSV);
	connect(actionExportCSV, SIGNAL(triggered()), this, SLOT(exportToCsv()));

	actionExportStatistics = new QAction(QIcon(":/images/icons/74.png"), tr("Export Stat&istics to CSV"), this);
	actionExportStatistics->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionExportStatistics->setToolTip("Export Statistics to CSV (Ctrl + I)");
	toolbarFile->addAction(actionExportStatistics);
	connect(actionExportStatistics, SIGNAL(triggered()), this, SLOT(exportStatistics()));

	actionCloseWindow = new QAction(QIcon(":/images/icons/33.png"), tr("&Close"), this);
	actionCloseWindow->setShortcut(QKeySequence(Qt::Key_Escape));
	actionCloseWindow->setToolTip("Close (Esc)");
	toolbarFile->addAction(actionCloseWindow);
	connect(actionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	toolbarFilter = addToolBar(tr("Filter"));
	toolbarFilter->addWidget(rowsfilterwidget);

	menuFile->addAction(actionExportCSV);
	menuFile->addAction(actionExportStatistics);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterRows()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(database);

	mainwidget = new QWidget();
	mainwidget->setLayout(mainlayout);

	setCentralWidget(mainwidget);

	resize(1280, 770);

	lastdirexporttocsv = "./";
	lastdirexportstatisticstocsv = "./";

	coordinates.clear();
	coordinates_orig.clear();
}


cSummaryPeaksTableWidget::~cSummaryPeaksTableWidget() {
	deleteTable();
	
	delete rowsfilterline;
	delete rowsfiltercasesensitive;
	delete rowsfilterbutton;
	delete rowsfilterclearbutton;
	delete rowsfilterhbox;
	delete rowsfilterwidget;

	delete databasemodel;
	delete proxymodel;
	delete database;
	delete mainlayout;
	delete mainwidget;

	delete actionExportCSV;
	delete actionExportStatistics;
	delete actionCloseWindow;
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuHelp;

	delete menuBar;
}


void cSummaryPeaksTableWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


bool cSummaryPeaksTableWidget::prepareToShow(QStandardItemModel* resultsstandardmodel, cMainWindowProxyModel* resultsproxymodel, cParameters* parameters, cTheoreticalSpectrumList* spectralist) {
	if (!parameters || !spectralist || (resultsstandardmodel->rowCount() != spectralist->size())) {
		return false;
	}

	int spectracount = spectralist->size();
	int thpeakscount = 0;
	int spectrumindex;
	cPeak* peak;

	for (int i = 0; i < spectracount; i++) {

		if (!resultsproxymodel->mapFromSource(resultsstandardmodel->index(i, 0)).isValid()) {
			continue;
		}

		spectrumindex = resultsstandardmodel->item(i, 1)->data(Qt::DisplayRole).toInt() - 1;

		if ((*spectralist)[spectrumindex].getNumberOfMatchedPeaks() == 0) {
			continue;
		}

		for (int j = 0; j < (int)((*spectralist)[spectrumindex].getTheoreticalPeaks()->size()); j++) {
			peak = &((*((*spectralist)[spectrumindex].getTheoreticalPeaks()))[j]);
			if (peak->matchedmz > 0) {
				thpeakscount++;
			}
		}
	}

	if (thpeakscount > 1000000) {
		QMessageBox::StandardButton reply;
		string msg = "The table contains " + to_string(thpeakscount) + " rows and might not fit into the main memory. Do you with to continue ?\n\nTip: Re-run the search using adjusted search settings to limit the number of rows. You may increase the minimum threshold of relative peak intensity, the minimum m/z ratio, decrease the m/z error tolerance or limit the number of selected ion types in theoretical spectra.";
		reply = QMessageBox::question(this, appname, msg.c_str(), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No) {
			return false;
		}
	}

	this->parameters = parameters;

	resetFilter();
	deleteTable();
	proxymodel->initialize(parameters->mode, parameters->peaklistfileformat, parameters->generateisotopepattern);

	coordinates.clear();
	coordinates_orig.clear();

	// prepare the header
	if (parameters->mode == dereplication) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			if (parameters->generateisotopepattern) {
				databasemodel->setColumnCount(13);
			}
			else {
				databasemodel->setColumnCount(12);
			}
		}
		else {
			if (parameters->generateisotopepattern) {
				databasemodel->setColumnCount(11);
			}
			else {
				databasemodel->setColumnCount(10);
			}
		}
	}
	else if (parameters->mode == denovoengine) {
		databasemodel->setColumnCount(9);
	}
	else {
		databasemodel->setColumnCount(8);
	}
	
	databasemodel->setHorizontalHeaderItem(0, new QStandardItem());
	databasemodel->horizontalHeaderItem(0)->setText("ID");
	database->setItemDelegateForColumn(0, new QItemDelegate());

	int currentcolumn = 1;
	if (parameters->mode == dereplication) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("Coordinate X");
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
			currentcolumn++;

			databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("Coordinate Y");
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
			currentcolumn++;
		}
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Ion Type");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	if (parameters->mode == denovoengine) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Group ID");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	if (parameters->mode != dereplication) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Fragment Type");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical m/z");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	if ((parameters->mode == dereplication) && (parameters->generateisotopepattern)) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical Intensity [%]");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Experimental m/z");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Relative Intensity [%]");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Absolute Intensity");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Error [ppm]");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	if (parameters->mode == dereplication) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Summary Formula");
		if (parameters->generateisotopepattern) {
			database->setItemDelegateForColumn(currentcolumn, new cHTMLDelegate());
		}
		else {
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		}
		currentcolumn++;

		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Name");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;

		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Reference");
		database->setItemDelegateForColumn(currentcolumn, new cViewButtonDelegate());
		currentcolumn++;
	}
	else {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Sequence");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}	

	for (int i = 0; i < databasemodel->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}


	// fill new data
	QProgressDialog* progress;
	cEventFilter filter;

	progress = new QProgressDialog("Preparing the data...", "Cancel", 0, spectracount, this);
	progress->setMinimumWidth(250);
	progress->installEventFilter(&filter);
	progress->setMinimumDuration(0);
	progress->setWindowModality(Qt::WindowModal);
	progress->setValue(0);

	database->setModel(0);
	proxymodel->setSourceModel(0);
	database->setSortingEnabled(false);

	int secondspace, langle, rangle, tmp;
	cPeaksList* thpeaks;
	string stmp;
	int currentrow;

	QBrush brush;
	brush.setColor(QColor(0, 0, 0));

	for (int i = 0; i < spectracount; i++) {

		if (!resultsproxymodel->mapFromSource(resultsstandardmodel->index(i, 0)).isValid()) {
			continue;
		}

		spectrumindex = resultsstandardmodel->item(i, 1)->data(Qt::DisplayRole).toInt() - 1;

		if ((*spectralist)[spectrumindex].getNumberOfMatchedPeaks() == 0) {
			progress->setValue(i);
			continue;
		}

		thpeaks = (*spectralist)[spectrumindex].getTheoreticalPeaks();
		thpeakscount = (*spectralist)[spectrumindex].getTheoreticalPeaks()->size();

		// theoretical peaks
		for (int j = 0; j < thpeakscount; j++) {
			peak = &((*thpeaks)[j]);
			if (peak->descriptionid != -1) {
				peak->description = parameters->peakidtodesc[peak->descriptionid];
			}

			if (peak->matchedmz == 0) {
				continue;
			}

			currentrow = databasemodel->rowCount();
			currentcolumn = 0;

			databasemodel->insertRow(currentrow);

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(spectrumindex + 1), Qt::DisplayRole);
			currentcolumn++;

			if (parameters->mode == dereplication) {
				if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue((*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateX()), Qt::DisplayRole);
					currentcolumn++;
					
					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue((*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateY()), Qt::DisplayRole);
					currentcolumn++;

					coordinates.push_back(cCoordinates((*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateX(), (*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateY(), spectrumindex + 1, cropPrecisionToSixDecimals(peak->matchedmz), cropPrecisionToSixDecimals(peak->matchedrelativeintensity), peak->matchedabsoluteintensity));
				}
				secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(0, secondspace).c_str());
				currentcolumn++;		
			}

			if (parameters->mode == denovoengine) {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue((*spectralist)[spectrumindex].getPathId() + 1), Qt::DisplayRole);
				currentcolumn++;		
			}
		
			if (parameters->mode != dereplication) {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(0, peak->description.find(':')).c_str());
				currentcolumn++;		
			}
		
			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->mzratio)), Qt::DisplayRole);
			currentcolumn++;

			if ((parameters->mode == dereplication) && (parameters->generateisotopepattern)) {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->relativeintensity)), Qt::DisplayRole);
				currentcolumn++;
			}

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedmz)), Qt::DisplayRole);
			currentcolumn++;

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedrelativeintensity)), Qt::DisplayRole);
			currentcolumn++;

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropDecimalsByteArray(peak->matchedabsoluteintensity)), Qt::DisplayRole);
			currentcolumn++;

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedppm)), Qt::DisplayRole);
			currentcolumn++;

			if (parameters->mode == dereplication) {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1).c_str());
				currentcolumn++;		

				langle = (int)peak->description.rfind("</a>");
				rangle = (int)peak->description.find('>');
				if ((langle != string::npos) && (rangle != string::npos)) {
					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(rangle + 1, langle - rangle - 1).c_str());
					if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
						if (databasemodel->item(currentrow, currentcolumn)->text().length() > 40) {
							coordinates.back().name = databasemodel->item(currentrow, currentcolumn)->text().toStdString().substr(0, 37) + "...";
						}
						else {
							coordinates.back().name = databasemodel->item(currentrow, currentcolumn)->text().toStdString();
						}
					}
					currentcolumn++;		

					tmp = (int)peak->description.find('<');
					stmp = peak->description.substr(tmp, rangle - tmp + 1);
					stmp = stmp.substr(stmp.find('\"') + 1);
					stmp = stmp.substr(0, stmp.rfind('\"'));

					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setText(stmp.c_str());
					currentcolumn++;		
				}
				else {
					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2).c_str());
					if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
						if (databasemodel->item(currentrow, currentcolumn)->text().length() > 40) {
							coordinates.back().name = databasemodel->item(currentrow, currentcolumn)->text().toStdString().substr(0, 37) + "...";
						}
						else {
							coordinates.back().name = databasemodel->item(currentrow, currentcolumn)->text().toStdString();
						}
					}
					currentcolumn++;		

					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setText("");
					currentcolumn++;		
				}
			}
			else {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setText("");
				if (peak->description.find(':') + 2 < peak->description.size()) {
					databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(peak->description.find(':') + 2).c_str());
				}
				currentcolumn++;		
			}

		}
		
        progress->setValue(i);
        if (progress->wasCanceled()) {
			deleteTable();
			emit tableCancelled();
			break;
        }

	}

	proxymodel->setSourceModel(databasemodel);
	database->setModel(proxymodel);
	proxymodel->setSortRole(Qt::InitialSortOrderRole);
	database->setSortingEnabled(true);

	if (!progress->wasCanceled()) {
		for (int i = 0; i < databasemodel->columnCount(); i++) {
			database->resizeColumnToContents(i);
		}
	}

	if ((parameters->mode == dereplication) && !progress->wasCanceled()) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			emit resetRegion();
			emit sendFilterOptions(coordinates, rowsfilterline->text().toStdString(), rowsfiltercasesensitive->isChecked());
			coordinates_orig = coordinates;

			if (parameters->generateisotopepattern) {
				database->setColumnWidth(10, min(400, database->columnWidth(10)));
			}
			else {
				database->setColumnWidth(9, min(400, database->columnWidth(9)));
			}
		}
		else {
			if (parameters->generateisotopepattern) {
				database->setColumnWidth(8, min(400, database->columnWidth(8)));
			}
			else {
				database->setColumnWidth(7, min(400, database->columnWidth(7)));
			}
		}
	}

	progress->setValue(spectracount);
	delete progress;

	return true;
}


void cSummaryPeaksTableWidget::deleteTable() {
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		if (database->itemDelegateForColumn(i)) {
			delete database->itemDelegateForColumn(i);
		}
	}
	databasemodel->clear();
}


void cSummaryPeaksTableWidget::updateFilterBySelectedRegion(int xmin, int xmax, int ymin, int ymax) {
	proxymodel->setRegion(xmin, xmax, ymin, ymax);
	filterRows();
}


void cSummaryPeaksTableWidget::keyPressEvent(QKeyEvent *event) {
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


void cSummaryPeaksTableWidget::closeWindow() {
	hide();
}


void cSummaryPeaksTableWidget::filterRows() {
	Qt::CaseSensitivity casesensitive = rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;
	QString str = rowsfilterline->text();
	int rowcount = databasemodel->rowCount();
	bool match;
	int i, j, x, y;
	int iontypecol, mzcol, relintcol, absintcol, namecol;

	if (parameters->mode == dereplication) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			iontypecol = 3;
			if (parameters->generateisotopepattern) {
				mzcol = 6;
				relintcol = 7;
				absintcol = 8;
				namecol = 11;
			}
			else {
				mzcol = 5;
				relintcol = 6;
				absintcol = 7;
				namecol = 10;
			}
		}
	}

	QProgressDialog progress("Updating...", "Cancel", 0, rowcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	coordinates.clear();

	for (i = 0; i < rowcount; i++) {
		x = databasemodel->item(i, 1)->data(Qt::DisplayRole).toInt();
		y = databasemodel->item(i, 2)->data(Qt::DisplayRole).toInt();

		match = false;
		for (j = 0; j < databasemodel->columnCount(); j++) {
			// ignore non-text fields
			if (parameters->mode == dereplication) {
				if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
					if (parameters->generateisotopepattern) {
						if (j == 12) {
							continue;
						}
					}
					else {
						if (j == 11) {
							continue;
						}
					}
				}
				else {
					if (parameters->generateisotopepattern) {
						if (j == 10) {
							continue;
						}
					}
					else {
						if (j == 9) {
							continue;
						}
					}
				}
			}

			if (databasemodel->item(i, j) && databasemodel->item(i, j)->text().contains(str, casesensitive)) {
				if (parameters->mode == dereplication) {
					if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
						if (((proxymodel->xmin == -1) && (proxymodel->xmax == -1) && (proxymodel->ymin == -1) && (proxymodel->ymax == -1)) || ((x >= proxymodel->xmin) && (x <= proxymodel->xmax) && (y >= proxymodel->ymin) && (y <= proxymodel->ymax))) {
							coordinates.push_back(cCoordinates(databasemodel->item(i, 1)->text().toInt(), databasemodel->item(i, 2)->text().toInt(), databasemodel->item(i, 0)->text().toInt(), databasemodel->item(i, mzcol)->data(Qt::DisplayRole).toDouble(), databasemodel->item(i, relintcol)->data(Qt::DisplayRole).toDouble(), databasemodel->item(i, absintcol)->data(Qt::DisplayRole).toDouble()));
							coordinates.back().name = databasemodel->item(i, iontypecol)->text().toStdString() + " ";
							if (databasemodel->item(i, namecol)->text().length() > 40) {
								coordinates.back().name += databasemodel->item(i, namecol)->text().toStdString().substr(0, 37) + "...";
							}
							else {
								coordinates.back().name += databasemodel->item(i, namecol)->text().toStdString();
							}
						}
					}
				}

				match = true;
				break;
			}
		}
		progress.setValue(i);

		if (progress.wasCanceled()) {
			resetFilter();
			break;
		}

	}

	if ((parameters->mode == dereplication) && !progress.wasCanceled()) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			emit sendFilterOptions(coordinates, rowsfilterline->text().toStdString(), rowsfiltercasesensitive->isChecked());
		}
	}

	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterCaseSensitivity(casesensitive);
	proxymodel->setFilterFixedString(str);

	progress.setValue(rowcount);
}


void cSummaryPeaksTableWidget::resetFilter() {
	rowsfilterline->setText("");

	int rowcount = databasemodel->rowCount();
	int i, x, y;
	int iontypecol, mzcol, relintcol, absintcol, namecol;

	if (parameters->mode == dereplication) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			iontypecol = 3;
			if (parameters->generateisotopepattern) {
				mzcol = 6;
				relintcol = 7;
				absintcol = 8;
				namecol = 11;
			}
			else {
				mzcol = 5;
				relintcol = 6;
				absintcol = 7;
				namecol = 10;
			}
		}
	}

	QProgressDialog progress("Updating...", 0, 0, rowcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	if (parameters->mode == dereplication) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			coordinates.clear();
			for (i = 0; i < rowcount; i++) {
				x = databasemodel->item(i, 1)->data(Qt::DisplayRole).toInt();
				y = databasemodel->item(i, 2)->data(Qt::DisplayRole).toInt();

				if (((proxymodel->xmin == -1) && (proxymodel->xmax == -1) && (proxymodel->ymin == -1) && (proxymodel->ymax == -1)) || ((x >= proxymodel->xmin) && (x <= proxymodel->xmax) && (y >= proxymodel->ymin) && (y <= proxymodel->ymax))) {
					coordinates.push_back(cCoordinates(x, y, databasemodel->item(i, 0)->text().toInt(), databasemodel->item(i, mzcol)->data(Qt::DisplayRole).toDouble(), databasemodel->item(i, relintcol)->data(Qt::DisplayRole).toDouble(), databasemodel->item(i, absintcol)->data(Qt::DisplayRole).toDouble()));
					coordinates.back().name = databasemodel->item(i, iontypecol)->text().toStdString() + " ";
					if (databasemodel->item(i, namecol)->text().length() > 40) {
						coordinates.back().name += databasemodel->item(i, namecol)->text().toStdString().substr(0, 37) + "...";
					}
					else {
						coordinates.back().name += databasemodel->item(i, namecol)->text().toStdString();
					}
				}

				progress.setValue(i);
			}
			emit sendFilterOptions(coordinates, rowsfilterline->text().toStdString(), rowsfiltercasesensitive->isChecked());
		}
	}

	database->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
	proxymodel->sort(-1);

	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterFixedString("");

	progress.setValue(rowcount);
}


void cSummaryPeaksTableWidget::exportToCsv() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), lastdirexporttocsv, tr("Files (*.csv)"));

	if (!filename.isEmpty()) {
		lastdirexporttocsv = filename;

		QProgressDialog progress("Exporting CSV file...", "Cancel", 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		bool removefile = false;
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		QTextStream out(&file);

		for (int i = 0; i < databasemodel->columnCount(); i++) {
			out << "\"" << databasemodel->horizontalHeaderItem(i)->text() << "\"";
			if (i < databasemodel->columnCount() - 1) {
				out << ",";
			}
		}
		out << endl;

		QStandardItem* item;
		string str;
		for (int i = 0; i < proxymodel->rowCount(); i++) {

			for (int j = 0; j < databasemodel->columnCount(); j++) {
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, j)));
				if (item) {
					str = item->data(Qt::DisplayRole).toString().toStdString();
					out << "\"" << stripHTML(str).c_str() << "\"";
					if (j < databasemodel->columnCount() - 1) {
						out << ",";
					}
				}
				else {
					if (j < databasemodel->columnCount() - 1) {
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


void cSummaryPeaksTableWidget::exportStatistics() {

	if (!parameters) {
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Export Statistics"), lastdirexportstatisticstocsv, tr("Files (*.csv)"));

	if (!filename.isEmpty()) {
		lastdirexportstatisticstocsv = filename;

		QProgressDialog progress("Exporting statistics to CSV file...", "Cancel", 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		bool removefile = false;
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		QTextStream out(&file);

		int iontypecol;
		int theoreticalmzcol;
		int summaryformulacol;
		int namecol;
		int referencecol;

		int idcol;
		int xcol, ycol;
		int relintcol, absintcol;

		int experimentalmzcol;
		int ppmcol;

		if (parameters->mode == dereplication) {

			if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
				if (parameters->generateisotopepattern) {
					iontypecol = 3;
					theoreticalmzcol = 4;
					summaryformulacol = 10;
					namecol = 11;
					referencecol = 12;

					idcol = 0;
					xcol = 1;
					ycol = 2;
					relintcol = 7;
					absintcol = 8;
				}
				else {
					iontypecol = 3;
					theoreticalmzcol = 4;
					summaryformulacol = 9;
					namecol = 10;
					referencecol = 11;

					idcol = 0;
					xcol = 1;
					ycol = 2;
					relintcol = 6;
					absintcol = 7;
				}
			}
			else {
				if (parameters->generateisotopepattern) {
					iontypecol = 1;
					theoreticalmzcol = 2;
					summaryformulacol = 8;
					namecol = 9;
					referencecol = 10;

					idcol = 0;
					relintcol = 5;
					absintcol = 6;
				}
				else {
					iontypecol = 1;
					theoreticalmzcol = 2;
					summaryformulacol = 7;
					namecol = 8;
					referencecol = 9;

					idcol = 0;
					relintcol = 4;
					absintcol = 5;
				}
			}

		}
		else if (parameters->mode == denovoengine) {
			theoreticalmzcol = 3;
			experimentalmzcol = 4;
			relintcol = 5;
			absintcol = 6;
			ppmcol = 7;
		}
		else {
			theoreticalmzcol = 2;
			experimentalmzcol = 3;
			relintcol = 4;
			absintcol = 5;
			ppmcol = 6;
		}

		map<cSummaryTableKeyMS, vector<int>, cSummaryTableKeyMS_comp> mapms;
		cSummaryTableKeyMS keyms;

		map<cSummaryTableKeyMSMS, vector<int>, cSummaryTableKeyMSMS_comp> mapmsms;
		cSummaryTableKeyMSMS keymsms;

		QStandardItem* item;
		for (int i = 0; i < proxymodel->rowCount(); i++) {

			if (parameters->mode == dereplication) {

				keyms.clear();

				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, iontypecol)));
				if (item) {
					keyms.iontype = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, theoreticalmzcol)));
				if (item) {
					keyms.theoreticalmz = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, summaryformulacol)));
				if (item) {
					keyms.summaryformula = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, namecol)));
				if (item) {
					keyms.name = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, referencecol)));
				if (item) {
					keyms.reference = item->text().toStdString();
				}

				auto obj = mapms.find(keyms);
				if (obj != mapms.end()) {
					obj->second.push_back(i);
				}
				else {
					vector<int> v;
					v.push_back(i);
					mapms.insert(std::pair< cSummaryTableKeyMS, vector<int> >(keyms, v));
				}

			}
			else {

				keymsms.clear();

				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, theoreticalmzcol)));
				if (item) {
					keymsms.theoreticalmz = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, experimentalmzcol)));
				if (item) {
					keymsms.experimentalmz = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, relintcol)));
				if (item) {
					keymsms.relint = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, absintcol)));
				if (item) {
					keymsms.absint = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, ppmcol)));
				if (item) {
					keymsms.ppmerror = item->text().toStdString();
				}

				auto obj = mapmsms.find(keymsms);
				if (obj != mapmsms.end()) {
					obj->second.push_back(i);
				}
				else {
					vector<int> v;
					v.push_back(i);
					mapmsms.insert(std::pair< cSummaryTableKeyMSMS, vector<int> >(keymsms, v));
				}

			}

			progress.setValue(i);
			if (progress.wasCanceled()) {
				removefile = true;
				break;
			}

		}
		
		int idmin, idmax;
		int xmin, xmax, ymin, ymax;
		double relintmax, relintavg, relintmed, relintcur;
		double absintmax, absintavg, absintmed, absintcur;
		vector<double> relintmedianvector;
		vector<double> absintmedianvector;

		if (parameters->mode == dereplication) {

			out << "Number of Points, ID (min), ID (max), ";
			if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
				out << "Coordinate X (min), Coordinate X (max), Coordinate Y (min), Coordinate Y (max), ";
			}
			out << "Ion Type, Theoretical m/z, Average Relative Intensity [%], Median Relative Intensity [%], Maximum Relative Intensity [%], ";
			out << "Average Absolute Intensity, Median Absolute Intensity, Maximum Absolute Intensity, ";
			out << "Summary Formula, Name, Reference" << endl;

			vector< pair<cSummaryTableKeyMS, vector<int> > > statisticsms;

			for (auto it = mapms.begin(); it != mapms.end(); ++it) {
				statisticsms.push_back(*it);
			}

			auto cmpms = [](pair<cSummaryTableKeyMS, vector<int> > const &a, pair<cSummaryTableKeyMS, vector<int> > const &b) {
				return a.second.size() > b.second.size();
			};
			sort(statisticsms.begin(), statisticsms.end(), cmpms);

			for (auto it = statisticsms.begin(); it != statisticsms.end(); ++it) {

				idmin = INT32_MAX;
				idmax = 0;

				xmin = INT32_MAX;
				xmax = 0;

				ymin = INT32_MAX;
				ymax = 0;

				relintmax = 0;
				relintavg = 0;
				relintmedianvector.clear();

				absintmax = 0;
				absintavg = 0;
				absintmedianvector.clear();

				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
					item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, idcol)));
					if (item) {
						if (item->data(Qt::DisplayRole).toInt() < idmin) {
							idmin = item->data(Qt::DisplayRole).toInt();
						}
						if (item->data(Qt::DisplayRole).toInt() > idmax) {
							idmax = item->data(Qt::DisplayRole).toInt();
						}
					}

					if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {

						item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, xcol)));
						if (item) {
							if (item->data(Qt::DisplayRole).toInt() < xmin) {
								xmin = item->data(Qt::DisplayRole).toInt();
							}
							if (item->data(Qt::DisplayRole).toInt() > xmax) {
								xmax = item->data(Qt::DisplayRole).toInt();
							}
						}

						item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, ycol)));
						if (item) {
							if (item->data(Qt::DisplayRole).toInt() < ymin) {
								ymin = item->data(Qt::DisplayRole).toInt();
							}
							if (item->data(Qt::DisplayRole).toInt() > ymax) {
								ymax = item->data(Qt::DisplayRole).toInt();
							}
						}

					}

					item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, relintcol)));
					if (item) {
						relintcur = item->data(Qt::DisplayRole).toDouble();
						if (relintcur > relintmax) {
							relintmax = relintcur;
						}
						relintavg += relintcur;
						relintmedianvector.push_back(relintcur);
					}

					item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, absintcol)));
					if (item) {
						absintcur = item->data(Qt::DisplayRole).toDouble();
						if (absintcur > absintmax) {
							absintmax = absintcur;
						}
						absintavg += absintcur;
						absintmedianvector.push_back(absintcur);
					}
				}

				relintavg /= (double)it->second.size();
				absintavg /= (double)it->second.size();

				relintmed = median(relintmedianvector);
				absintmed = median(absintmedianvector);

				out << it->second.size() << ", " << idmin << ", " << idmax << ", ";
				if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
					out << xmin << ", " << xmax << ", " << ymin << ", " << ymax << ", ";
				}
				out << it->first.iontype.c_str() << ", " << it->first.theoreticalmz.c_str() << ", ";

				stringstream ss;
				ss << std::fixed << std::setprecision(2) << relintavg << ", " << relintmed << ", " << relintmax << ", ";
				ss << std::fixed << std::setprecision(0) << absintavg << ", " << absintmed << ", " << absintmax << ", ";
				out << ss.str().c_str();

				out << stripHTML(it->first.summaryformula).c_str() << ", " << it->first.name.c_str() << ", " << it->first.reference.c_str() << endl;

			}

		}
		else {

			out << "Number of Matches, Theoretical m/z, Experimental m/z, Relative Intensity [%], Absolute Intensity, Error [ppm]" << endl;

			vector< pair<cSummaryTableKeyMSMS, vector<int> > > statisticsmsms;

			for (auto it = mapmsms.begin(); it != mapmsms.end(); ++it) {
				statisticsmsms.push_back(*it);
			}

			auto cmpmsms = [](pair<cSummaryTableKeyMSMS, vector<int> > const &a, pair<cSummaryTableKeyMSMS, vector<int> > const &b) {
				return a.second.size() > b.second.size();
			};
			sort(statisticsmsms.begin(), statisticsmsms.end(), cmpmsms);

			for (auto it = statisticsmsms.begin(); it != statisticsmsms.end(); ++it) {
				out << it->second.size() << ", " << it->first.theoreticalmz.c_str() << ", " << it->first.experimentalmz.c_str() << ", " << it->first.relint.c_str() << ", " << it->first.absint.c_str() << ", " << it->first.ppmerror.c_str() << endl;
			}

		}
	
		file.close();

		if (removefile) {
			file.remove();
		}

		progress.setValue(proxymodel->rowCount());
	}
}


void cSummaryPeaksTableWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/summarypeakstable.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/summarypeakstable.html").absoluteFilePath()));
	#endif
}

