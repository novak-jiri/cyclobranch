#include "gui/cSummaryPeaksTableWidget.h"
#include "core/cParameters.h"
#include "core/cTheoreticalSpectrumList.h"
#include "gui/cEventFilter.h"

#include <QTableWidget>
#include <QTableWidgetItem>
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


cSummaryPeaksTableWidget::cSummaryPeaksTableWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Summary Table of Matched Peaks");
	setWindowIcon(QIcon(":/images/icons/43.png"));

	close = new QPushButton(tr("Close"));
	close->setToolTip("Close the window.");

	exportcsv = new QPushButton(tr("Export"));
	exportcsv->setToolTip("Export the table to CSV.");
	exportcsv->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));

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

	buttons = new QHBoxLayout();
	buttons->addWidget(close);
	buttons->addStretch();
	buttons->addWidget(rowsfilterwidget);
	buttons->addStretch();
	buttons->addWidget(exportcsv);

	database = new QTableWidget(0, 0, this);
	database->setEditTriggers(QAbstractItemView::NoEditTriggers);
	database->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	database->horizontalHeader()->setSectionsMovable(true);

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(database);
	mainlayout->addLayout(buttons);

	connect(database->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(headerItemDoubleClicked(int)));
	connect(close, SIGNAL(released()), this, SLOT(closeWindow()));
	connect(exportcsv, SIGNAL(released()), this, SLOT(exportToCsv()));
	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterRows()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	setLayout(mainlayout);

	resize(1280, 700);

	lastdirexporttocsv = "./";
}


cSummaryPeaksTableWidget::~cSummaryPeaksTableWidget() {
	deleteTable(false);
	
	for (int i = 0; i < database->columnCount(); i++) {
		delete database->horizontalHeaderItem(i);
	}
	
	database->setColumnCount(0);

	delete close;
	delete exportcsv;

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


void cSummaryPeaksTableWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
}


void cSummaryPeaksTableWidget::prepareToShow(QTableWidget* tablewidget, cParameters* parameters, cTheoreticalSpectrumList* spectralist) {
	if (!parameters || !spectralist || ((int)tablewidget->rowCount() != spectralist->size())) {
		return;
	}


	this->parameters = parameters;


	// delete old data
	deleteTable(true);
	for (int i = 0; i < database->columnCount(); i++) {
		database->setItemDelegateForColumn(i, 0);
		delete database->horizontalHeaderItem(i);
	}
	database->setColumnCount(0);


	// prepare the header
	if (parameters->mode == dereplication) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			database->setColumnCount(11);
		}
		else {
			database->setColumnCount(9);
		}
	}
	else {
		database->setColumnCount(7);
	}
	
	database->setHorizontalHeaderItem(0, new QTableWidgetItem());
	database->horizontalHeaderItem(0)->setText("ID");

	int currentcolumn = 1;
	if (parameters->mode == dereplication) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
			database->horizontalHeaderItem(currentcolumn)->setText("Coordinate X");
			currentcolumn++;
			database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
			database->horizontalHeaderItem(currentcolumn)->setText("Coordinate Y");
			currentcolumn++;
		}
		database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
		database->horizontalHeaderItem(currentcolumn)->setText("Ion Type");
		currentcolumn++;
	}
	else {
		database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
		database->horizontalHeaderItem(currentcolumn)->setText("Fragment Type");
		currentcolumn++;
	}

	database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
	database->horizontalHeaderItem(currentcolumn)->setText("Theoretical m/z");
	database->setItemDelegateForColumn(currentcolumn, &columndelegate);
	currentcolumn++;

	database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
	database->horizontalHeaderItem(currentcolumn)->setText("Experimental m/z");
	database->setItemDelegateForColumn(currentcolumn, &columndelegate);
	currentcolumn++;

	database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
	database->horizontalHeaderItem(currentcolumn)->setText("Intensity [%]");
	database->setItemDelegateForColumn(currentcolumn, &columndelegate);
	currentcolumn++;

	database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
	database->horizontalHeaderItem(currentcolumn)->setText("Error [ppm]");
	database->setItemDelegateForColumn(currentcolumn, &columndelegate);
	currentcolumn++;

	if (parameters->mode == dereplication) {
		database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
		database->horizontalHeaderItem(currentcolumn)->setText("Summary Formula");
		currentcolumn++;

		database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
		database->horizontalHeaderItem(currentcolumn)->setText("Name");
		currentcolumn++;

		database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
		database->horizontalHeaderItem(currentcolumn)->setText("Reference");
		currentcolumn++;
	}
	else {
		database->setHorizontalHeaderItem(currentcolumn, new QTableWidgetItem());
		database->horizontalHeaderItem(currentcolumn)->setText("Sequence");
		currentcolumn++;
	}	

	database->horizontalHeader()->setStretchLastSection(true);
	for (int i = 0; i < database->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}

	headersort.resize(database->columnCount());
	for (int i = 0; i < database->columnCount(); i++) {
		headersort[i] = -1;
	}


	// fill new data
	QProgressDialog* progress;
	cEventFilter filter;
	int spectracount = spectralist->size();

	progress = new QProgressDialog("Preparing the table...", "Cancel", 0, spectracount, this);
	progress->setMinimumWidth(250);
	progress->installEventFilter(&filter);
	progress->setMinimumDuration(0);
	progress->setWindowModality(Qt::WindowModal);
	progress->setValue(0);

	int thpeakscount;
	int secondspace, langle, rangle, tmp1, tmp2;
	cPeaksList* thpeaks;
	cPeak* peak;
	bool isred;
	string stmp;
	int currentrow;
	int spectrumindex;

	QBrush brush;
	brush.setColor(QColor(0, 0, 0));

	for (int i = 0; i < spectracount; i++) {

		if (tablewidget->isRowHidden(i)) {
			progress->setValue(i);
			continue;
		}

		spectrumindex = tablewidget->item(i, 1)->data(Qt::DisplayRole).toInt() - 1;

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

			currentrow = database->rowCount();
			currentcolumn = 0;

			database->insertRow(currentrow);
			database->setRowHeight(currentrow, 20);

			database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
			database->item(currentrow, currentcolumn)->setForeground(brush);
			database->item(currentrow, currentcolumn)->setData(Qt::DisplayRole, spectrumindex + 1);
			currentcolumn++;

			if (parameters->mode == dereplication) {
				if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
					database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
					database->item(currentrow, currentcolumn)->setForeground(brush);
					database->item(currentrow, currentcolumn)->setData(Qt::DisplayRole, (*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateX());
					currentcolumn++;
					database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
					database->item(currentrow, currentcolumn)->setForeground(brush);
					database->item(currentrow, currentcolumn)->setData(Qt::DisplayRole, (*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateY());
					currentcolumn++;		
				}
				secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);
				database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
				database->item(currentrow, currentcolumn)->setForeground(brush);
				database->item(currentrow, currentcolumn)->setText(peak->description.substr(0, secondspace).c_str());
				currentcolumn++;		
			}
			else {
				database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
				database->item(currentrow, currentcolumn)->setForeground(brush);
				database->item(currentrow, currentcolumn)->setText(peak->description.substr(0, peak->description.find(':')).c_str());
				currentcolumn++;		
			}
		
			database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
			database->item(currentrow, currentcolumn)->setForeground(brush);
			database->item(currentrow, currentcolumn)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(peak->mzratio));
			currentcolumn++;

			database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
			database->item(currentrow, currentcolumn)->setForeground(brush);
			database->item(currentrow, currentcolumn)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(peak->matchedmz));
			currentcolumn++;

			database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
			database->item(currentrow, currentcolumn)->setForeground(brush);
			database->item(currentrow, currentcolumn)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(peak->matchedintensity));
			currentcolumn++;

			database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
			database->item(currentrow, currentcolumn)->setForeground(brush);
			database->item(currentrow, currentcolumn)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(peak->matchedppm));
			currentcolumn++;

			if (parameters->mode == dereplication) {
				database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
				database->item(currentrow, currentcolumn)->setForeground(brush);
				database->item(currentrow, currentcolumn)->setText(peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1).c_str());
				currentcolumn++;		

				langle = (int)peak->description.rfind('<');
				rangle = (int)peak->description.find('>');
				if ((langle != string::npos) && (rangle != string::npos)) {
					database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
					database->item(currentrow, currentcolumn)->setForeground(brush);
					database->item(currentrow, currentcolumn)->setText(peak->description.substr(rangle + 1, langle - rangle - 1).c_str());
					currentcolumn++;		

					tmp1 = (int)peak->description.find('<');
					tmp2 = (int)peak->description.rfind('>');
					stmp = peak->description.substr(tmp1, rangle - tmp1 + 1) + "view" + peak->description.substr(langle, tmp2 - langle + 1);
					database->setCellWidget(currentrow, currentcolumn, new QLabel(stmp.c_str()));
					((QLabel *)database->cellWidget(currentrow, currentcolumn))->setTextFormat(Qt::RichText);
					((QLabel *)database->cellWidget(currentrow, currentcolumn))->setTextInteractionFlags(Qt::TextBrowserInteraction);
					((QLabel *)database->cellWidget(currentrow, currentcolumn))->setOpenExternalLinks(true);
					currentcolumn++;		
				}
				else {
					database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
					database->item(currentrow, currentcolumn)->setForeground(brush);
					database->item(currentrow, currentcolumn)->setText(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2).c_str());
					currentcolumn++;		
				}
			}
			else {
				if (peak->description.find(':') + 2 < peak->description.size()) {
					database->setItem(currentrow, currentcolumn, widgetitemallocator.getNewItem());
					database->item(currentrow, currentcolumn)->setForeground(brush);
					database->item(currentrow, currentcolumn)->setText(peak->description.substr(peak->description.find(':') + 2).c_str());
					currentcolumn++;		
				}
			}

		}

		if (parameters->mode == dereplication) {
			delete thpeaks;
		}
				
        progress->setValue(i);
        if (progress->wasCanceled()) {
			deleteTable(true);
			emit tableCancelled();
			break;
        }

	}

	for (int i = 0; i < database->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}

	progress->setValue(spectracount);
	delete progress;
}


void cSummaryPeaksTableWidget::deleteTable(bool enableprogress) {
	QProgressDialog* progress;
	cEventFilter filter;
	int rowcount = database->rowCount();

	if (enableprogress) {
		progress = new QProgressDialog("Clearing the table...", 0, 0, rowcount, this);
		progress->setMinimumWidth(250);
		progress->installEventFilter(&filter);
		progress->setMinimumDuration(0);
		progress->setWindowModality(Qt::WindowModal);
		progress->setValue(0);
	}

	widgetitemallocator.reset();

	for (int i = 0; i < rowcount; i++) {
		if (parameters->mode == dereplication) {
			if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
				delete database->cellWidget(rowcount - i - 1, 10);
			}
			else {
				delete database->cellWidget(rowcount - i - 1, 8);
			}
		}

        if (enableprogress) {
            progress->setValue(i);
        }
    }
	
	database->setRowCount(0);

	if (enableprogress) {
		progress->setValue(rowcount);
		delete progress;
	}
}


void cSummaryPeaksTableWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		closeWindow();
    }

	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (rowsfilterline->hasFocus()) {
			filterRows();
		}
    }

	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/summarypeakstable.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/summarypeakstable.html").absoluteFilePath()));
		#endif
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_F)) {
		rowsfilterline->setFocus();
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_N)) {
		rowsfiltercasesensitive->setChecked(!rowsfiltercasesensitive->isChecked());
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_E)) {
		exportToCsv();
	}
}


void cSummaryPeaksTableWidget::closeWindow() {
	hide();
}


void cSummaryPeaksTableWidget::headerItemDoubleClicked(int index) {
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


void cSummaryPeaksTableWidget::filterRows() {
	Qt::CaseSensitivity casesensitive = rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;
	QString str = rowsfilterline->text();
	int rowcount = database->rowCount();
	bool match;
	int i, j;

	QProgressDialog progress("Updating...", "Cancel", 0, rowcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	for (i = 0; i < rowcount; i++) {

		match = false;
		for (j = 0; j < database->columnCount(); j++) {
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

			if (database->item(i, j)->text().contains(str, casesensitive)) {
				match = true;
				break;
			}
		}
		database->setRowHidden(i, !match);
		progress.setValue(i);

		if (progress.wasCanceled()) {
			resetFilter();
			break;
		}

	}

	progress.setValue(rowcount);
}


void cSummaryPeaksTableWidget::resetFilter() {
	rowsfilterline->setText("");
	int rowcount = database->rowCount();

	QProgressDialog progress("Updating...", 0, 0, rowcount, this);
	progress.setMinimumWidth(250);
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


void cSummaryPeaksTableWidget::exportToCsv() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), lastdirexporttocsv, tr("Files (*.csv)"));
	
	if (!filename.isEmpty()) {
		lastdirexporttocsv = filename;

		QProgressDialog progress("Exporting CSV file...", "Cancel", 0, database->rowCount(), this);
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

		for (int i = 0; i < database->columnCount(); i++) {
			out << "\"" << database->horizontalHeaderItem(i)->text() << "\"";
			if (i < database->columnCount() - 1) {
				out << ",";
			}
		}
		out << endl;

		for (int i = 0; i < database->rowCount(); i++) {

			if (database->isRowHidden(i)) {
				continue;
			}

			for (int j = 0; j < database->columnCount(); j++) {
				if (database->item(i, j)) {
					out << "\"" << database->item(i, j)->data(Qt::DisplayRole).toString() << "\"";
					if (j < database->columnCount() - 1) {
						out << ",";
					}
				}
				else if (database->cellWidget(i, j)) {
					out << "\"" << ((QLabel *)(database->cellWidget(i, j)))->text() << "\"";
					if (j < database->columnCount() - 1) {
						out << ",";
					}
				}
				else {
					if (j < database->columnCount() - 1) {
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

		progress.setValue(database->rowCount());
	}
}

