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

	resize(1280, 750);

	lastdirexporttocsv = "./";

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


void cSummaryPeaksTableWidget::prepareToShow(QStandardItemModel* resultsstandardmodel, cMainWindowProxyModel* resultsproxymodel, cParameters* parameters, cTheoreticalSpectrumList* spectralist) {
	if (!parameters || !spectralist || (resultsstandardmodel->rowCount() != spectralist->size())) {
		return;
	}


	this->parameters = parameters;

	resetFilter();
	deleteTable();
	proxymodel->initialize(parameters->mode, parameters->peaklistfileformat);


	coordinates.clear();
	coordinates_orig.clear();

	// prepare the header
	if (parameters->mode == dereplication) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			databasemodel->setColumnCount(11);
		}
		else {
			databasemodel->setColumnCount(9);
		}
	}
	else if (parameters->mode == denovoengine) {
		databasemodel->setColumnCount(8);
	}
	else {
		databasemodel->setColumnCount(7);
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

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Experimental m/z");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Intensity [%]");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Error [ppm]");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	if (parameters->mode == dereplication) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Summary Formula");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
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
	int spectracount = spectralist->size();

	progress = new QProgressDialog("Preparing the data...", "Cancel", 0, spectracount, this);
	progress->setMinimumWidth(250);
	progress->installEventFilter(&filter);
	progress->setMinimumDuration(0);
	progress->setWindowModality(Qt::WindowModal);
	progress->setValue(0);

	database->setModel(0);
	proxymodel->setSourceModel(0);
	database->setSortingEnabled(false);

	int thpeakscount;
	int secondspace, langle, rangle, tmp;
	cPeaksList* thpeaks;
	cPeak* peak;
	bool isred;
	string stmp;
	int currentrow;
	int spectrumindex;

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

		if (parameters->mode == dereplication) {
			thpeaks = new cPeaksList();
			for (int j = 0; j < (int)((*spectralist)[spectrumindex].getTheoreticalPeaks()->size()); j++) {
				peak = &((*((*spectralist)[spectrumindex].getTheoreticalPeaks()))[j]);
				if (peak->matchedmz > 0) {
					thpeaks->add(*peak);
				}
			}
			thpeakscount = thpeaks->size();
		}
		else {
			thpeaks = (*spectralist)[spectrumindex].getTheoreticalPeaks();
			thpeakscount = (*spectralist)[spectrumindex].getTheoreticalPeaks()->size();
		}

		// theoretical peaks
		for (int j = 0; j < thpeakscount; j++) {
			peak = &((*thpeaks)[j]);

			if (peak->matchedmz > 0) {
				isred = true;
			}
			else {
				isred = false;
			}

			if (!isred) {
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

					coordinates.push_back(cCoordinates((*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateX(), (*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateY(), spectrumindex + 1, cropPrecisionToSixDecimalsByteArray(peak->matchedmz).toDouble(), cropPrecisionToSixDecimalsByteArray(peak->matchedintensity).toDouble()));
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

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedmz)), Qt::DisplayRole);
			currentcolumn++;

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedintensity)), Qt::DisplayRole);
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

				langle = (int)peak->description.rfind('<');
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

		if (parameters->mode == dereplication) {
			delete thpeaks;
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
			emit sendCoordinates(coordinates);
			coordinates_orig = coordinates;
		}
	}

	progress->setValue(spectracount);
	delete progress;
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
					if (j == 10) {
						continue;
					}
				}
				else {
					if (j == 8) {
						continue;
					}
				}
			}

			if (databasemodel->item(i, j) && databasemodel->item(i, j)->text().contains(str, casesensitive)) {
				if (parameters->mode == dereplication) {
					if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
						if (((proxymodel->xmin == -1) && (proxymodel->xmax == -1) && (proxymodel->ymin == -1) && (proxymodel->ymax == -1)) || ((x >= proxymodel->xmin) && (x <= proxymodel->xmax) && (y >= proxymodel->ymin) && (y <= proxymodel->ymax))) {
							coordinates.push_back(cCoordinates(databasemodel->item(i, 1)->text().toInt(), databasemodel->item(i, 2)->text().toInt(), databasemodel->item(i, 0)->text().toInt(), databasemodel->item(i, 5)->data(Qt::DisplayRole).toDouble(), databasemodel->item(i, 6)->data(Qt::DisplayRole).toDouble()));
							if (databasemodel->item(i, 9)->text().length() > 40) {
								coordinates.back().name = databasemodel->item(i, 9)->text().toStdString().substr(0, 37) + "...";
							}
							else {
								coordinates.back().name = databasemodel->item(i, 9)->text().toStdString();
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
			emit sendCoordinates(coordinates);
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
					coordinates.push_back(cCoordinates(x, y, databasemodel->item(i, 0)->text().toInt(), databasemodel->item(i, 5)->data(Qt::DisplayRole).toDouble(), databasemodel->item(i, 6)->data(Qt::DisplayRole).toDouble()));
					if (databasemodel->item(i, 9)->text().length() > 40) {
						coordinates.back().name = databasemodel->item(i, 9)->text().toStdString().substr(0, 37) + "...";
					}
					else {
						coordinates.back().name = databasemodel->item(i, 9)->text().toStdString();
					}
				}

				progress.setValue(i);
			}
			emit sendCoordinates(coordinates);
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
		for (int i = 0; i < proxymodel->rowCount(); i++) {

			for (int j = 0; j < databasemodel->columnCount(); j++) {
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, j)));
				if (item) {
					out << "\"" << item->data(Qt::DisplayRole).toString() << "\"";
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


void cSummaryPeaksTableWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/summarypeakstable.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/summarypeakstable.html").absoluteFilePath()));
	#endif
}

