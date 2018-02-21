#include "gui/cSpectrumDetailWidget.h"

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
#include <QKeyEvent>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>


cSpectrumDetailWidget::cSpectrumDetailWidget() {
	parent = 0;
	parameters = 0;
	preparedToShow = false;
	theoreticalspectrum = new cTheoreticalSpectrum();
}


cSpectrumDetailWidget::cSpectrumDetailWidget(const cSpectrumDetailWidget& sd) {
	*this = sd;
}


cSpectrumDetailWidget& cSpectrumDetailWidget::operator=(const cSpectrumDetailWidget& sd) {
	parameters = sd.parameters;

	preparedToShow = false;
	theoreticalspectrum = new cTheoreticalSpectrum();

	if (parameters && sd.theoreticalspectrum) {
		initialize(parameters, *sd.theoreticalspectrum, sd.parent);
	}

	if (parameters && sd.preparedToShow) {
		prepareToShow(parameters->peptidetype);
	}

	setWindowTitle(sd.windowTitle());

	return *this;
}


void cSpectrumDetailWidget::initialize(cParameters* parameters, cTheoreticalSpectrum& theoreticalspectrum, QWidget* parent) {
	this->parent = parent;
	this->parameters = parameters;
	*this->theoreticalspectrum = theoreticalspectrum;
}


cTheoreticalSpectrum& cSpectrumDetailWidget::getTheoreticalSpectrum() {
	return *theoreticalspectrum;
}


string cSpectrumDetailWidget::getDetailsAsHTMLString() {
	string s = "";

	if (theoreticalspectrum) {

		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {

			s += "Acronym Peptide Name:<br/>";
			s += theoreticalspectrum->getCandidate().getAcronymPeptideNameWithHTMLReferences();
			s += "<br/><br/>";
			s += "Full Peptide Name:<br/>";
			s += theoreticalspectrum->getCandidate().getRealPeptideName();

			string lname = parameters->searchedmodifications[theoreticalspectrum->getCandidate().getStartModifID()].name;
			if (lname.empty()) {
				lname = "-";
			}
			else {
				lname += " (" + to_string(parameters->searchedmodifications[theoreticalspectrum->getCandidate().getStartModifID()].massdifference) + " Da)";
			}

			string bname = parameters->searchedmodifications[theoreticalspectrum->getCandidate().getMiddleModifID()].name;
			if (bname.empty()) {
				bname = "-";
			}
			else {
				bname += " (" + to_string(parameters->searchedmodifications[theoreticalspectrum->getCandidate().getMiddleModifID()].massdifference) + " Da)";
			}

			string rname = parameters->searchedmodifications[theoreticalspectrum->getCandidate().getEndModifID()].name;
			if (rname.empty()) {
				rname = "-";
			}
			else {
				rname += " (" + to_string(parameters->searchedmodifications[theoreticalspectrum->getCandidate().getEndModifID()].massdifference) + " Da)";
			}
			
			switch (parameters->peptidetype)
			{
			case linear:
				s += "<br/><br/>";
				s += "N-terminal Modification: " + lname + "<br/>";
				s += "C-terminal Modification: " + rname + "<br/>";
				break;
			case cyclic:
				s += "<br/>";
				break;
			case branched:
				s += "<br/><br/>";
				s += "N-terminal Modification: " + lname + "<br/>";
				s += "Branch Modification: " + bname + "<br/>";
				s += "C-terminal Modification: " + rname + "<br/>";
				break;
			case branchcyclic:
				s += "<br/><br/>";
				s += "Branch Modification: " + bname + "<br/>";
				break;
			case linearpolyketide:
				s += "<br/><br/>";
				s += "Left Modification: " + lname + "<br/>";
				s += "Right Modification: " + rname + "<br/>";
				break;
			case cyclicpolyketide:
				s += "<br/>";
				break;
			case other:
				break;
			default:
				break;
			}
			
			if ((int)theoreticalspectrum->getCandidate().getPathAsString().size() > 0) {
				s += "<br/>";
				s += "Path in the De Novo Graph:<br/>";
				s += theoreticalspectrum->getCandidate().getPathAsString();
			}
			
			s += "<br/>";

		}

	}

	return s;
}


string cSpectrumDetailWidget::getPeaksTableAsHTMLString(bool unmatchedtheoreticalpeaks, bool unmatchedexperimentalpeaks) {
	string s = "";
	int rowcount, columncount, thpeakscount;
	int secondspace, langle, rangle, tmp1, tmp2;
	cPeaksList* thpeaks;
	cPeak* peak;
	bool isred;
	string tdwidth;

	if (theoreticalspectrum && parameters) {

		s += "<table class=\"tablesorter\" cellpadding=\"0\" cellspacing=\"0\" border=\"1\" width=\"100%\"><thead><tr>";

		if (parameters->mode == dereplication) {
			columncount = 8;
		}
		else {
			columncount = 6;
		}

		tdwidth = to_string(100/columncount);

		if (parameters->mode == dereplication) {
			s += "<th width=\"" + tdwidth + "%\"><b>Ion Type</b></th>";
		}
		else {
			s += "<th width=\"" + tdwidth + "%\"><b>Fragment Type</b></th>";
		}

		s += "<th width=\"" + tdwidth + "%\"><b>Theoretical m/z</b></th>";
		s += "<th width=\"" + tdwidth + "%\"><b>Experimental m/z</b></th>";
		s += "<th width=\"" + tdwidth + "%\"><b>Intensity [%]</b></th>";
		s += "<th width=\"" + tdwidth + "%\"><b>Error [ppm]</b></th>";

		if (parameters->mode == dereplication) {
			s += "<th width=\"" + tdwidth + "%\"><b>Summary Formula</b></th>";
			s += "<th width=\"" + tdwidth + "%\"><b>Name</b></th>";
			s += "<th width=\"" + tdwidth + "%\"><b>Reference</b></th>";
		}
		else {
			s += "<th width=\"" + tdwidth + "%\"><b>Sequence</b></th>";
		}

		s += "</tr></thead><tbody>";

		if (parameters->mode == dereplication) {
			thpeaks = new cPeaksList();
			for (int i = 0; i < (int)theoreticalspectrum->getTheoreticalPeaks()->size(); i++) {
				peak = &((*(theoreticalspectrum->getTheoreticalPeaks()))[i]);
				if (peak->matchedmz > 0) {
					thpeaks->add(*peak);
				}
			}
			thpeakscount = thpeaks->size();
		}
		else {
			thpeaks = theoreticalspectrum->getTheoreticalPeaks();
			thpeakscount = theoreticalspectrum->getTheoreticalPeaks()->size();
		}

		rowcount = thpeakscount + theoreticalspectrum->getUnmatchedPeaks()->size();

		// theoretical peaks
		for (int i = 0; i < thpeakscount; i++) {
			peak = &((*thpeaks)[i]);

			if (peak->matchedmz > 0) {
				isred = true;
			}
			else {
				isred = false;
			}

			if (!isred && !unmatchedtheoreticalpeaks) {
				continue;
			}

			s += "<tr>";
		
			if (parameters->mode == dereplication) {
				secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);
				s += printHTMLTableCell(peak->description.substr(0, secondspace), isred);
			}
			else {
				s += printHTMLTableCell(peak->description.substr(0, peak->description.find(':')), isred);
			}
		
			s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->mzratio)), isred);

			if (peak->matchedmz > 0) {
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedmz)), isred);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedintensity)), isred);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedppm)), isred);
			}
			else {
				s += "<td></td><td></td><td></td>";
			}

			if (parameters->mode == dereplication) {
				s += printHTMLTableCell(peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1), isred);

				langle = (int)peak->description.rfind('<');
				rangle = (int)peak->description.find('>');
				if ((langle != string::npos) && (rangle != string::npos)) {
					s += printHTMLTableCell(peak->description.substr(rangle + 1, langle - rangle - 1), isred);

					tmp1 = (int)peak->description.find('<');
					tmp2 = (int)peak->description.rfind('>');
					s += printHTMLTableCell(peak->description.substr(tmp1, rangle - tmp1 + 1) + "view" + peak->description.substr(langle, tmp2 - langle + 1), isred);
				}
				else {
					s += printHTMLTableCell(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2), isred);
					s += "<td></td>";
				}
			}
			else {
				if (peak->description.find(':') + 2 < peak->description.size()) {
					s += printHTMLTableCell(peak->description.substr(peak->description.find(':') + 2), isred);
				}
				else {
					s += "<td></td>";
				}
			}

			s += "</tr>";
		}

		// unmatched experimental peaks
		if (unmatchedexperimentalpeaks) {
			for (int i = thpeakscount; i < thpeakscount + theoreticalspectrum->getUnmatchedPeaks()->size(); i++) {
				s += "<tr>";
				peak = &((*(theoreticalspectrum->getUnmatchedPeaks()))[i - thpeakscount]);
				s += "<td></td><td></td>";
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->mzratio)), false);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->intensity)), false);
				s += "<td></td><td></td>";
				if (parameters->mode == dereplication) {
					s += "<td></td><td></td>";
				}
				s += "</tr>";
			}
		}

		s += "</tbody></table>";

		if (parameters->mode == dereplication) {
			delete thpeaks;
		}

	}

	return s;
}


string cSpectrumDetailWidget::getPartialPeaksTableAsHTMLString(int id) {
	string s = "";
	int thpeakscount;
	int secondspace, langle, rangle, tmp1, tmp2;
	cPeaksList* thpeaks;
	cPeak* peak;
	bool isred;

	if (theoreticalspectrum && parameters) {

		if (parameters->mode == dereplication) {
			thpeaks = new cPeaksList();
			for (int i = 0; i < (int)theoreticalspectrum->getTheoreticalPeaks()->size(); i++) {
				peak = &((*(theoreticalspectrum->getTheoreticalPeaks()))[i]);
				if (peak->matchedmz > 0) {
					thpeaks->add(*peak);
				}
			}
			thpeakscount = thpeaks->size();
		}
		else {
			thpeaks = theoreticalspectrum->getTheoreticalPeaks();
			thpeakscount = theoreticalspectrum->getTheoreticalPeaks()->size();
		}

		// theoretical peaks
		for (int i = 0; i < thpeakscount; i++) {
			peak = &((*thpeaks)[i]);

			if (peak->matchedmz > 0) {
				isred = true;
			}
			else {
				isred = false;
			}

			if (!isred) {
				continue;
			}

			s += "<tr>";
			s += "<td>";
			s += to_string(id + 1);
			s += "</td>";
		
			if (parameters->mode == dereplication) {
				if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
					s += "<td>";
					s += to_string(theoreticalspectrum->getExperimentalSpectrum().getCoordinateX());
					s += "</td>";			
					s += "<td>";
					s += to_string(theoreticalspectrum->getExperimentalSpectrum().getCoordinateY());
					s += "</td>";			
				}
				secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);
				s += printHTMLTableCell(peak->description.substr(0, secondspace), isred);
			}
			
			if (parameters->mode == denovoengine) {
				s += printHTMLTableCell(to_string(theoreticalspectrum->getPathId() + 1), isred);
			}

			if (parameters->mode != dereplication) {
				s += printHTMLTableCell(peak->description.substr(0, peak->description.find(':')), isred);
			}
		
			s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->mzratio)), isred);

			if (peak->matchedmz > 0) {
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedmz)), isred);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedintensity)), isred);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedppm)), isred);
			}
			else {
				s += "<td></td><td></td><td></td>";
			}

			if (parameters->mode == dereplication) {
				s += printHTMLTableCell(peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1), isred);

				langle = (int)peak->description.rfind('<');
				rangle = (int)peak->description.find('>');
				if ((langle != string::npos) && (rangle != string::npos)) {
					s += printHTMLTableCell(peak->description.substr(rangle + 1, langle - rangle - 1), isred);

					tmp1 = (int)peak->description.find('<');
					tmp2 = (int)peak->description.rfind('>');
					s += printHTMLTableCell(peak->description.substr(tmp1, rangle - tmp1 + 1) + "view" + peak->description.substr(langle, tmp2 - langle + 1), isred);
				}
				else {
					s += printHTMLTableCell(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2), isred);
					s += "<td></td>";
				}
			}
			else {
				if (peak->description.find(':') + 2 < peak->description.size()) {
					s += printHTMLTableCell(peak->description.substr(peak->description.find(':') + 2), isred);
				}
				else {
					s += "<td></td>";
				}
			}

			s += "</tr>";
		}

		if (parameters->mode == dereplication) {
			delete thpeaks;
		}

	}

	return s;
}


cSpectrumDetailWidget::~cSpectrumDetailWidget() {

	delete theoreticalspectrum;

	if (preparedToShow) {
	
		delete spectrumscene;

		delete textbrowser;

		for (int i = 0; i < peakstablemodel->columnCount(); i++) {
			if (peakstable->itemDelegateForColumn(i)) {
				delete peakstable->itemDelegateForColumn(i);
			}
		}
		peakstablemodel->clear();

		delete peakstablemodel;
		delete proxymodel;
		delete peakstable;

		delete labelmz;
		delete minmz;
		delete maxmz;
		delete setmzinterval;
		delete resetmzinterval;
		delete hboxmz;
		delete widgetmz;

		delete labelrotation;
		delete rotation;
		delete hboxrotation;
		delete widgetrotation;

		delete labeltrotation;
		delete trotation;
		delete hboxtrotation;
		delete widgettrotation;

		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {

			switch (parameters->peptidetype)
			{
			case linear:
			case linearpolyketide:
				delete linearwidget;
				break;
			case cyclic:
			case cyclicpolyketide:
				delete cyclicwidget;
				break;
			case branched:
				delete branchedwidget;
				break;
			case branchcyclic:
				delete branchcyclicwidget;
				break;
			case other:
				break;
			default:
				break;
			}

		}
		
		delete hsplitter1;
		delete hsplitter2;
		
		delete vsplitter;

		delete actionExportSpectrum;
		delete actionFind;
		delete actionPrevious;
		delete actionNext;
		delete actionZoomIn;
		delete actionZoomOut;
		delete actionZoomReset;
		delete actionHideMatched;
		delete actionHideUnmatched;
		delete actionHideScrambled;
		delete actionMouseMzSelection;

		delete finddialog;
		delete exportdialog;

	}
}


void cSpectrumDetailWidget::closeEvent(QCloseEvent *event) {
	hide();
	event->accept();
}


void cSpectrumDetailWidget::prepareToShow(ePeptideType peptidetype) {

	if (!preparedToShow) {

		vsplitter = new QSplitter();
		vsplitter->setOrientation(Qt::Vertical);

		hsplitter1 = new QSplitter();
		hsplitter1->setOrientation(Qt::Horizontal);

		hsplitter2 = new QSplitter();
		hsplitter2->setOrientation(Qt::Horizontal);


		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {
			switch (peptidetype)
			{
			case linear:
			case linearpolyketide:
				linearwidget = new cLinearWidget();
				break;
			case cyclic:
			case cyclicpolyketide:
				cyclicwidget = new cCyclicWidget();
				break;
			case branched:
				branchedwidget = new cBranchedWidget();
				break;
			case branchcyclic:
				branchcyclicwidget = new cBranchCyclicWidget();
				break;
			case other:
				break;
			default:
				break;
			}
		}

		spectrumscene = new cSpectrumSceneWidget(this);

		toolbarExport = addToolBar(tr("Export"));
		toolbarExport->setMovable(false);
		toolbarExport->setFloatable(false);

		actionExportTable = new QAction(QIcon(":/images/icons/62.png"), tr("Export Table to CSV"), this);
		actionExportTable->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
		actionExportTable->setToolTip("Export Table to CSV (Ctrl + E)");
		toolbarExport->addAction(actionExportTable);
		connect(actionExportTable, SIGNAL(triggered()), this, SLOT(exportTableToCSV()));

		actionExportSpectrum = new QAction(QIcon(":/images/icons/66.png"), tr("Export Image"), this);
		actionExportSpectrum->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
		actionExportSpectrum->setToolTip("Export Image (Ctrl + I)");
		toolbarExport->addAction(actionExportSpectrum);
		connect(actionExportSpectrum, SIGNAL(triggered()), this, SLOT(openExportImageDialog()));


		toolbarFind = addToolBar(tr("Find"));
		toolbarFind->setMovable(false);
		toolbarFind->setFloatable(false);
				
		actionFind = new QAction(QIcon(":/images/icons/65.png"), tr("Find Text"), this);
		actionFind->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
		actionFind->setToolTip("Find Text (Ctrl + F)");
		toolbarFind->addAction(actionFind);
		connect(actionFind, SIGNAL(triggered()), this, SLOT(openFindDialog()));

		actionPrevious = new QAction(QIcon(":/images/icons/56.png"), tr("Find Previous"), this);
		actionPrevious->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
		actionPrevious->setToolTip("Find Previous (Ctrl + 1)");
		toolbarFind->addAction(actionPrevious);
		connect(actionPrevious, SIGNAL(triggered()), this, SLOT(movePrevious()));
		actionPrevious->setDisabled(true);

		actionNext = new QAction(QIcon(":/images/icons/57.png"), tr("Find Next"), this);
		actionNext->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
		actionNext->setToolTip("Find Next (Ctrl + 2)");
		toolbarFind->addAction(actionNext);
		connect(actionNext, SIGNAL(triggered()), this, SLOT(moveNext()));
		actionNext->setDisabled(true);


		toolbarZoom = addToolBar(tr("Zoom"));
		toolbarZoom->setMovable(false);
		toolbarZoom->setFloatable(false);
				
		actionZoomIn = new QAction(QIcon(":/images/icons/83.png"), tr("Zoom In"), this);
		actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
		actionZoomIn->setToolTip("Zoom In (Ctrl +)");
		toolbarZoom->addAction(actionZoomIn);
		connect(actionZoomIn, SIGNAL(triggered()), spectrumscene, SLOT(zoomIn()));

		actionZoomOut = new QAction(QIcon(":/images/icons/82.png"), tr("Zoom Out"), this);
		actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
		actionZoomOut->setToolTip("Zoom Out (Ctrl -)");
		toolbarZoom->addAction(actionZoomOut);
		connect(actionZoomOut, SIGNAL(triggered()), spectrumscene, SLOT(zoomOut()));

		actionZoomReset = new QAction(QIcon(":/images/icons/84.png"), tr("Reset Zoom"), this);
		actionZoomReset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
		actionZoomReset->setToolTip("Zoom Reset (Ctrl + R)");
		toolbarZoom->addAction(actionZoomReset);
		connect(actionZoomReset, SIGNAL(triggered()), spectrumscene, SLOT(normalSize()));


		toolbarHide = addToolBar(tr("Show/Hide Peaks"));
		toolbarHide->setMovable(false);
		toolbarHide->setFloatable(false);

		actionHideMatched = new QAction(QIcon(":/images/icons/20.png"), tr("Hide Matched Peaks"), this);
		actionHideMatched->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
		actionHideMatched->setToolTip("Hide Matched Peaks (Ctrl + M)");
		actionHideMatched->setCheckable(true);
		toolbarHide->addAction(actionHideMatched);
		connect(actionHideMatched, SIGNAL(toggled(bool)), this, SLOT(hideMatchedPeaks(bool)));

		actionHideUnmatched = new QAction(QIcon(":/images/icons/81.png"), tr("Hide Unmatched Peaks"), this);
		actionHideUnmatched->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
		actionHideUnmatched->setToolTip("Hide Unmatched Peaks (Ctrl + U)");
		actionHideUnmatched->setCheckable(true);
		toolbarHide->addAction(actionHideUnmatched);
		connect(actionHideUnmatched, SIGNAL(toggled(bool)), this, SLOT(hideUnmatchedPeaks(bool)));

		actionHideScrambled = new QAction(QIcon(":/images/icons/80.png"), tr("Hide Scrambled Peaks"), this);
		actionHideScrambled->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
		actionHideScrambled->setToolTip("Hide Scrambled Peaks (Ctrl + S)");
		actionHideScrambled->setCheckable(true);
		actionHideScrambled->setEnabled(false);
		toolbarHide->addAction(actionHideScrambled);


		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {

			if ((parameters->peptidetype == cyclic) && parameters->enablescrambling) {
				actionHideScrambled->setEnabled(true);
				connect(actionHideScrambled, SIGNAL(toggled(bool)), this, SLOT(hideScrambledPeaks(bool)));
			}

		}

		textbrowser = new QTextBrowser();
		textbrowser->setReadOnly(true);
		textbrowser->setFont(QFont("Courier", 9));
		textbrowser->setLineWrapMode(QTextEdit::NoWrap);
		textbrowser->setAcceptRichText(true);
		textbrowser->setOpenExternalLinks(true);
		
		peakstable = new QTableView(this);
		peakstablemodel = new QStandardItemModel(0, 0, this);
		proxymodel = new cSpectrumDetailProxyModel(this);
		proxymodel->setSourceModel(peakstablemodel);
		proxymodel->setDynamicSortFilter(false);
		peakstable->setModel(proxymodel);
		peakstable->setSortingEnabled(true);

		labelmz = new QLabel(tr("View m/z (from - to): "));

		minmz = new QDoubleSpinBox();
		minmz->setDecimals(6);
		minmz->setRange(0, 100000);
		minmz->setSingleStep(1);

		maxmz = new QDoubleSpinBox();
		maxmz->setDecimals(6);
		maxmz->setRange(0, 100000);
		maxmz->setSingleStep(1);

		setmzinterval = new QPushButton("Set");
		setmzinterval->setMaximumWidth(50);
		connect(setmzinterval, SIGNAL(released()), this, SLOT(setMZInterval()));
		connect(this, SIGNAL(emitMZInterval(double, double)), spectrumscene, SLOT(setMZInterval(double, double)));

		resetmzinterval = new QPushButton("Reset");
		resetmzinterval->setMaximumWidth(50);
		connect(resetmzinterval, SIGNAL(released()), spectrumscene, SLOT(resetMZInterval()));
		connect(spectrumscene, SIGNAL(updateMZInterval(double, double)), this, SLOT(updateMZInterval(double, double)));

		hboxmz = new QHBoxLayout();
		hboxmz->addSpacing(1);
		hboxmz->addWidget(labelmz);
		hboxmz->addSpacing(1);
		hboxmz->addWidget(minmz);
		hboxmz->addSpacing(1);
		hboxmz->addWidget(maxmz);
		hboxmz->addSpacing(1);
		hboxmz->addWidget(setmzinterval);
		hboxmz->addSpacing(1);
		hboxmz->addWidget(resetmzinterval);
		hboxmz->addSpacing(1);

		widgetmz = new QWidget();
		widgetmz->setLayout(hboxmz);

		toolbarMz = addToolBar(tr("Range of m/z"));
		toolbarMz->setMovable(false);
		toolbarMz->setFloatable(false);

		actionMouseMzSelection = new QAction(QIcon(":/images/icons/64.png"), tr("Mouse m/z Selection Tool"), this);
		actionMouseMzSelection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
		actionMouseMzSelection->setToolTip("Mouse m/z Selection Tool (Ctrl + T)");
		actionMouseMzSelection->setCheckable(true);
		actionMouseMzSelection->setChecked(true);
		actionMouseMzSelection->setEnabled(true);
		toolbarMz->addAction(actionMouseMzSelection);
		connect(actionMouseMzSelection, SIGNAL(toggled(bool)), spectrumscene, SLOT(enableMouseMzSelectionTool(bool)));

		toolbarMz->addWidget(widgetmz);


		labelrotation = new QLabel(tr("Ring break up point: "));
		rotation = new QComboBox();

		hboxrotation = new QHBoxLayout();
		hboxrotation->addSpacing(1);
		hboxrotation->addWidget(labelrotation);
		hboxrotation->addSpacing(1);
		hboxrotation->addWidget(rotation);
		hboxrotation->addSpacing(1);

		widgetrotation = new QWidget();
		widgetrotation->setLayout(hboxrotation);


		labeltrotation = new QLabel(tr("Linearized sequence: "));
		trotation = new QComboBox();

		hboxtrotation = new QHBoxLayout();
		hboxtrotation->addSpacing(1);
		hboxtrotation->addWidget(labeltrotation);
		hboxtrotation->addSpacing(1);
		hboxtrotation->addWidget(trotation);
		hboxtrotation->addSpacing(1);

		widgettrotation = new QWidget();
		widgettrotation->setLayout(hboxtrotation);


		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {

			// cyclic
			if (theoreticalspectrum && ((parameters->peptidetype == cyclic) || (parameters->peptidetype == cyclicpolyketide))) {
				int r = (int)theoreticalspectrum->getCandidate().getAcronyms().size();
				int hint = (int)theoreticalspectrum->getVisualCoverage().size()/(2*r);
				
				rotation->addItem(tr("all"));

				string s;
				if (theoreticalspectrum->getVisualCoverage().size() > 0) {
					for (int i = 0; i < 2*r; i++) {
						s = theoreticalspectrum->getVisualCoverage()[i*hint].name.substr(0, theoreticalspectrum->getVisualCoverage()[i*hint].name.rfind('_'));
						rotation->addItem(tr(s.c_str()));
					}
				}

				connect(rotation, SIGNAL(currentIndexChanged(int)), spectrumscene, SLOT(rotationChanged(int)));
				connect(rotation, SIGNAL(currentIndexChanged(QString)), spectrumscene, SLOT(rotationChanged(QString)));
				connect(rotation, SIGNAL(currentIndexChanged(int)), cyclicwidget, SLOT(rotationChanged(int)));

				toolbarRotation = addToolBar(tr("Ring break up point"));
				toolbarRotation->addWidget(widgetrotation);
				toolbarRotation->setMovable(false);
				toolbarRotation->setFloatable(false);
			}

			// branched
			if (parameters->peptidetype == branched) {
				trotation->addItem(tr("all"));
				trotation->addItem(tr("1 (left-to-right)"));
				trotation->addItem(tr("2 (top-to-right)"));
				trotation->addItem(tr("3 (right-to-left)"));
				trotation->addItem(tr("4 (left-to-top)"));
				trotation->addItem(tr("5 (top-to-left)"));
				trotation->addItem(tr("6 (right-to-top)"));

				connect(trotation, SIGNAL(currentIndexChanged(int)), spectrumscene, SLOT(trotationChanged(int)));
				connect(trotation, SIGNAL(currentIndexChanged(int)), branchedwidget, SLOT(trotationChanged(int)));

				toolbarTrotation = addToolBar(tr("Linearized sequence"));
				toolbarTrotation->addWidget(widgettrotation);
				toolbarTrotation->setMovable(false);
				toolbarTrotation->setFloatable(false);
			}

			// branch-cyclic
			if (parameters && theoreticalspectrum && (parameters->peptidetype == branchcyclic)) {
				int r = (int)theoreticalspectrum->getCandidate().getAcronyms().size() - (int)theoreticalspectrum->getCandidate().getBranchSize();
				int hint = (int)theoreticalspectrum->getVisualCoverage().size()/(2*r);

				rotation->addItem(tr("all"));

				string s;
				if (theoreticalspectrum->getVisualCoverage().size() > 0) {
					for (int i = 0; i < 2*r; i++) {
						s = theoreticalspectrum->getVisualCoverage()[i*hint].name.substr(0, theoreticalspectrum->getVisualCoverage()[i*hint].name.find('_'));
						rotation->addItem(tr(s.c_str()));
					}
				}

				connect(rotation, SIGNAL(currentIndexChanged(int)), spectrumscene, SLOT(rotationChanged(int)));
				connect(rotation, SIGNAL(currentIndexChanged(QString)), spectrumscene, SLOT(rotationChanged(QString)));
				connect(rotation, SIGNAL(currentIndexChanged(int)), branchcyclicwidget, SLOT(rotationChanged(int)));

				toolbarRotation = addToolBar(tr("Ring break up point"));
				toolbarRotation->addWidget(widgetrotation);
				toolbarRotation->setMovable(false);
				toolbarRotation->setFloatable(false);

				trotation->addItem(tr("all"));
				trotation->addItem(tr("1 (left-to-right)"));
				trotation->addItem(tr("2 (top-to-right)"));
				trotation->addItem(tr("3 (right-to-left)"));
				trotation->addItem(tr("4 (left-to-top)"));
				trotation->addItem(tr("5 (top-to-left)"));
				trotation->addItem(tr("6 (right-to-top)"));

				connect(trotation, SIGNAL(currentIndexChanged(int)), spectrumscene, SLOT(trotationChanged(int)));
				connect(trotation, SIGNAL(currentIndexChanged(int)), branchcyclicwidget, SLOT(trotationChanged(int)));
				
				toolbarTrotation = addToolBar(tr("Linearized sequence"));
				toolbarTrotation->addWidget(widgettrotation);
				toolbarTrotation->setMovable(false);
				toolbarTrotation->setFloatable(false);
			}

		}

		hsplitter1->addWidget(spectrumscene);
		hsplitter2->addWidget(peakstable);

		QList<int> sizes;
		sizes.push_back(100);
		sizes.push_back(100);

		if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {

			switch (peptidetype)
			{
			case linear:
			case linearpolyketide:
				hsplitter1->addWidget(linearwidget);
				break;
			case cyclic:
			case cyclicpolyketide:
				hsplitter1->addWidget(cyclicwidget);
				break;
			case branched:
				hsplitter1->addWidget(branchedwidget);
				break;
			case branchcyclic:
				hsplitter1->addWidget(branchcyclicwidget);
				break;
			case other:
				break;
			default:
				break;
			}

			hsplitter2->addWidget(textbrowser);

			hsplitter1->setSizes(sizes);
			hsplitter2->setSizes(sizes);

		}

		vsplitter->addWidget(hsplitter1);
		vsplitter->addWidget(hsplitter2);

		vsplitter->setSizes(sizes);

		setCentralWidget(vsplitter);
		centralWidget()->setContentsMargins(10, 10, 10, 10);

		finddialog = new cFindDialog(this);
		exportdialog = new cExportDialog(this);

		resize(1280, 750);

		if (parameters && theoreticalspectrum) {

			if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {

				switch (peptidetype)
				{
				case linear:
				case linearpolyketide:
					linearwidget->initialize(parameters, theoreticalspectrum);
					break;
				case cyclic:
				case cyclicpolyketide:
					cyclicwidget->initialize(parameters, theoreticalspectrum);
					break;
				case branched:
					branchedwidget->initialize(parameters, theoreticalspectrum);
					break;
				case branchcyclic:
					branchcyclicwidget->initialize(parameters, theoreticalspectrum);
					break;
				case other:
					break;
				default:
					break;
				}

				textbrowser->setHtml((getDetailsAsHTMLString() + theoreticalspectrum->getCoverageBySeries()).c_str());
			
			}

			spectrumscene->initialize(parameters, theoreticalspectrum);

			preparePeaksTable();

		}

		preparedToShow = true;

	}

}


void cSpectrumDetailWidget::findAll(const QString& str, QTextDocument::FindFlags opt, bool errormessage) {

	currentfinditem = 0;

	peakstable->clearSelection();
	tablematches.clear();

	// textbrowser
	QList<QTextEdit::ExtraSelection> extraSelections;
	QColor color = QColor(Qt::yellow);
 
	textbrowser->moveCursor(QTextCursor::Start);

	while (textbrowser->find(str, opt)) {
		QTextEdit::ExtraSelection extra;
		extra.format.setBackground(color);        
 
		extra.cursor = textbrowser->textCursor();
		extraSelections.append(extra);
	}
 
	textbrowser->setExtraSelections(extraSelections);


	cTablePosition tpos;
	QStandardItem* item;
	for (int i = 0; i < proxymodel->rowCount(); i++) {

		for (int j = 0; j < ((parameters->mode == dereplication)?peakstablemodel->columnCount() - 1:peakstablemodel->columnCount()); j++) {
			item = peakstablemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, j)));

			if (!item) {
				continue;
			}

			item->setBackground(QBrush(Qt::white));

			if (str.isEmpty()) {
				continue;
			}

			if (opt & QTextDocument::FindWholeWords) {
				if (item->text().compare(str, (opt & QTextDocument::FindCaseSensitively)?Qt::CaseSensitive:Qt::CaseInsensitive) == 0) {
					tpos.row = i;
					tpos.column = j;
					tablematches.push_back(tpos);
					item->setBackground(QBrush(color));
				}
			}
			else {
				if (item->text().contains(str, (opt & QTextDocument::FindCaseSensitively)?Qt::CaseSensitive:Qt::CaseInsensitive)) {
					tpos.row = i;
					tpos.column = j;
					tablematches.push_back(tpos);		
					item->setBackground(QBrush(color));
				}
			}	
		}
	}


	if (textbrowser->extraSelections().size() + (int)tablematches.size() == 0) {
		actionPrevious->setDisabled(true);
		actionNext->setDisabled(true);

		if (errormessage) {
			QMessageBox msgBox;
			QString errstr = "No results were found.";
			msgBox.setWindowTitle("Find Text");
			msgBox.setText(errstr);
			msgBox.exec();
		}
	}
	else {
		actionPrevious->setDisabled(false);
		actionNext->setDisabled(false);
	}


	// order changed because of setFocus()
	if (textbrowser->extraSelections().size() > 0) {
		textbrowser->setFocus();
		textbrowser->setTextCursor(textbrowser->extraSelections().at(0).cursor);
	}


	if (tablematches.size() > 0) {
		peakstable->setFocus();
		peakstable->scrollTo(proxymodel->index(tablematches[0].row, tablematches[0].column));
		peakstable->selectionModel()->select(proxymodel->index(tablematches[0].row, tablematches[0].column), QItemSelectionModel::Select);
	}

}


void cSpectrumDetailWidget::exportImage(bool exportspectrum) {
	if (exportspectrum) {
		exportSpectrum();
	}
	else {
		exportPeptide();
	}
}


void cSpectrumDetailWidget::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Escape) {
		hide();
    }
	
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		setMZInterval();
    }

	if ((event->key() == Qt::Key_F1) && parameters) {
		#if OS_TYPE == WIN
			if (parameters->mode == dereplication) {
				QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/dereplication.html").absoluteFilePath()));
			}
			else {
				switch (parameters->peptidetype) {
				case linear:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/lineardetail.html").absoluteFilePath()));
					break;
				case cyclic:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/cyclicdetail.html").absoluteFilePath()));
					break;
				case branched:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/brancheddetail.html").absoluteFilePath()));
					break;
				case branchcyclic:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/branchcyclicdetail.html").absoluteFilePath()));
					break;
				case linearpolyketide:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/linearketidedetail.html").absoluteFilePath()));
					break;
				case cyclicpolyketide:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/cyclicketidedetail.html").absoluteFilePath()));
					break;
				case other:
					break;
				default:
					break;
				}
			}
		#else
			if (parameters->mode == dereplication) {
				QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/dereplication.html").absoluteFilePath()));
			}
			else {
				switch (parameters->peptidetype) {
				case linear:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/lineardetail.html").absoluteFilePath()));
					break;
				case cyclic:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/cyclicdetail.html").absoluteFilePath()));
					break;
				case branched:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/brancheddetail.html").absoluteFilePath()));
					break;
				case branchcyclic:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/branchcyclicdetail.html").absoluteFilePath()));
					break;
				case linearpolyketide:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/linearketidedetail.html").absoluteFilePath()));
					break;
				case cyclicpolyketide:
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/cyclicketidedetail.html").absoluteFilePath()));
					break;
				case other:
					break;
				default:
					break;
				}
			}
		#endif
	}

	event->accept();
}


void cSpectrumDetailWidget::preparePeaksTable() {

	peakstable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	peakstable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	peakstable->horizontalHeader()->setSectionsMovable(true);
	peakstable->setSelectionMode(QAbstractItemView::SingleSelection);
	peakstable->verticalHeader()->setDefaultSectionSize(25);

	if (parameters->mode == dereplication) {
		peakstablemodel->setColumnCount(8);
	}
	else {
		peakstablemodel->setColumnCount(6);
	}

	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		peakstablemodel->setHorizontalHeaderItem(i, new QStandardItem());
	}

	peakstablemodel->horizontalHeaderItem(1)->setText("Theoretical m/z");
	peakstablemodel->horizontalHeaderItem(2)->setText("Experimental m/z");
	peakstablemodel->horizontalHeaderItem(3)->setText("Intensity [%]");
	peakstablemodel->horizontalHeaderItem(4)->setText("Error [ppm]");

	if (parameters->mode == dereplication) {
		peakstablemodel->horizontalHeaderItem(0)->setText("Ion Type");
		peakstablemodel->horizontalHeaderItem(5)->setText("Summary Formula");
		peakstablemodel->horizontalHeaderItem(6)->setText("Name");
		peakstablemodel->horizontalHeaderItem(7)->setText("Reference");
		peakstable->setItemDelegateForColumn(7, new cViewButtonDelegate());
	}
	else {
		peakstablemodel->horizontalHeaderItem(0)->setText("Fragment Type");
		peakstablemodel->horizontalHeaderItem(5)->setText("Sequence");
	}

	connect(peakstable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerItemClicked(int)));

	cPeaksList* thpeaks;
	int thpeakscount;
	cPeak* peak;
	int secondspace, langle, rangle, tmp;
	string stmp;
	QBrush brush;

	if (parameters->mode == dereplication) {
		thpeaks = new cPeaksList();
		for (int i = 0; i < (int)theoreticalspectrum->getTheoreticalPeaks()->size(); i++) {
			peak = &((*(theoreticalspectrum->getTheoreticalPeaks()))[i]);
			if (peak->matchedmz > 0) {
				thpeaks->add(*peak);
			}
		}
		thpeakscount = thpeaks->size();
	}
	else {
		thpeaks = theoreticalspectrum->getTheoreticalPeaks();
		thpeakscount = theoreticalspectrum->getTheoreticalPeaks()->size();
	}
	
	peakstablemodel->setRowCount(thpeakscount + theoreticalspectrum->getUnmatchedPeaks()->size());

	QProgressDialog progress("Preparing the peaklist...", /*"Cancel"*/0, 0, thpeakscount + theoreticalspectrum->getUnmatchedPeaks()->size(), parent);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	peakstable->setModel(0);
	proxymodel->setSourceModel(0);
	peakstable->setSortingEnabled(false);

	// theoretical peaks
	for (int i = 0; i < thpeakscount; i++) {
		peak = &((*thpeaks)[i]);

		if (peak->matchedmz > 0) {
			brush.setColor(QColor(255, 0, 0));
		}
		else {
			brush.setColor(QColor(0, 0, 0));
		}
		
		if (parameters->mode == dereplication) {
			secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);

			peakstablemodel->setItem(i, 0, new QStandardItem());
			peakstablemodel->item(i, 0)->setForeground(brush);
			peakstablemodel->item(i, 0)->setText(peak->description.substr(0, secondspace).c_str());
		}
		else {
			peakstablemodel->setItem(i, 0, new QStandardItem());
			peakstablemodel->item(i, 0)->setForeground(brush);
			peakstablemodel->item(i, 0)->setText(peak->description.substr(0, peak->description.find(':')).c_str());
		}
		
		peakstablemodel->setItem(i, 1, new QStandardItem());
		peakstablemodel->item(i, 1)->setForeground(brush);
		peakstablemodel->item(i, 1)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->mzratio)), Qt::DisplayRole);

		if (peak->matchedmz > 0) {
			peakstablemodel->setItem(i, 2, new QStandardItem());
			peakstablemodel->item(i, 2)->setForeground(brush);
			peakstablemodel->item(i, 2)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedmz)), Qt::DisplayRole);

			peakstablemodel->setItem(i, 3, new QStandardItem());
			peakstablemodel->item(i, 3)->setForeground(brush);
			peakstablemodel->item(i, 3)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedintensity)), Qt::DisplayRole);

			peakstablemodel->setItem(i, 4, new QStandardItem());
			peakstablemodel->item(i, 4)->setForeground(brush);
			peakstablemodel->item(i, 4)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedppm)), Qt::DisplayRole);
		}

		if (parameters->mode == dereplication) {
			peakstablemodel->setItem(i, 5, new QStandardItem());
			peakstablemodel->item(i, 5)->setForeground(brush);
			peakstablemodel->item(i, 5)->setText(peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1).c_str());

			peakstablemodel->setItem(i, 6, new QStandardItem());
			peakstablemodel->item(i, 6)->setForeground(brush);
			langle = (int)peak->description.rfind('<');
			rangle = (int)peak->description.find('>');
			if ((langle != string::npos) && (rangle != string::npos)) {
				peakstablemodel->item(i, 6)->setText(peak->description.substr(rangle + 1, langle - rangle - 1).c_str());

				tmp = (int)peak->description.find('<');
				stmp = peak->description.substr(tmp, rangle - tmp + 1);
				stmp = stmp.substr(stmp.find('\"') + 1);
				stmp = stmp.substr(0, stmp.rfind('\"'));

				peakstablemodel->setItem(i, 7, new QStandardItem());
				peakstablemodel->item(i, 7)->setForeground(brush);
				peakstablemodel->item(i, 7)->setText(stmp.c_str());
			}
			else {
				peakstablemodel->item(i, 6)->setText(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2).c_str());		
			}
		}
		else {
			if (peak->description.find(':') + 2 < peak->description.size()) {
				peakstablemodel->setItem(i, 5, new QStandardItem());
				peakstablemodel->item(i, 5)->setForeground(brush);
				peakstablemodel->item(i, 5)->setText(peak->description.substr(peak->description.find(':') + 2).c_str());
			}
		}

		progress.setValue(i);
		//if (progress.wasCanceled()) {
		//	break;
		//}
	}

	// unmatched experimental peaks
	for (int i = thpeakscount; i < thpeakscount + theoreticalspectrum->getUnmatchedPeaks()->size(); i++) {
		peak = &((*(theoreticalspectrum->getUnmatchedPeaks()))[i - thpeakscount]);

		peakstablemodel->setItem(i, 2, new QStandardItem());
		peakstablemodel->item(i, 2)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->mzratio)), Qt::DisplayRole);

		peakstablemodel->setItem(i, 3, new QStandardItem());
		peakstablemodel->item(i, 3)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->intensity)), Qt::DisplayRole);

		progress.setValue(i);
		//if (progress.wasCanceled()) {
		//	break;
		//}
	}

	proxymodel->setSourceModel(peakstablemodel);
	peakstable->setModel(proxymodel);
	peakstable->setSortingEnabled(true);

	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		peakstable->resizeColumnToContents(i);
	}

	progress.setValue(thpeakscount + theoreticalspectrum->getUnmatchedPeaks()->size());

	tablematches.clear();

	if (parameters->mode == dereplication) {
		delete thpeaks;
	}

}


string cSpectrumDetailWidget::printHTMLTableCell(string text, bool red) {
	string s;

	s += "<td>";

	if (red) {
		s += "<font color=\"red\">";
	}

	s += text;

	if (red) {
		s += "</font>";
	}

	s += "</td>";

	return s;
}


void cSpectrumDetailWidget::updateMZInterval(double minmz, double maxmz) {
	this->minmz->setValue(minmz);
	this->maxmz->setValue(maxmz);
}


void cSpectrumDetailWidget::setMZInterval() {
	emit emitMZInterval(minmz->value(), maxmz->value());
}


void cSpectrumDetailWidget::exportSpectrum() {
	#if OS_TYPE == WIN
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Spectrum..."), "./", "PDF Files (*.pdf);; PS Files (*.ps);; PNG Files (*.png);; SVG Files (*.svg)");
	#else
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Spectrum..."), "./", "PDF Files (*.pdf);; PNG Files (*.png);; SVG Files (*.svg)");
	#endif
	if (!filename.isEmpty()) {
		regex rx;
		bool selected = false;

		rx = ".+\\.pdf$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			spectrumscene->exportToPDF(filename, false);
			selected = true;
		}

		#if OS_TYPE == WIN
			rx = ".+\\.ps$";
			if (!selected && (regex_search(filename.toStdString(), rx))) {
				spectrumscene->exportToPDF(filename, true);
				selected = true;
			}
		#endif

		rx = ".+\\.png$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			spectrumscene->exportToPNG(filename);
			selected = true;
		}

		rx = ".+\\.svg$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			spectrumscene->exportToSVG(filename);
			selected = true;
		}

		if (!selected) {
			QMessageBox msgBox;
			QString errstr = "Unsupported file format.";
			msgBox.setText(errstr);
			msgBox.exec();
		}
	}
}


void cSpectrumDetailWidget::exportPeptide() {
	#if OS_TYPE == WIN
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Peptide..."), "./", "PDF Files (*.pdf);; PS Files (*.ps);; PNG Files (*.png);; SVG Files (*.svg)");
	#else
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Peptide..."), "./", "PDF Files (*.pdf);; PNG Files (*.png);; SVG Files (*.svg)");
	#endif
	if (!filename.isEmpty() && parameters) {
		regex rx;
		bool selected = false;

		switch ((ePeptideType)parameters->peptidetype) {
			case linear:
			case linearpolyketide:
				rx = ".+\\.pdf$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					linearwidget->exportToPDF(filename, false);
					selected = true;
				}

				#if OS_TYPE == WIN
					rx = ".+\\.ps$";
					if (!selected && (regex_search(filename.toStdString(), rx))) {
						linearwidget->exportToPDF(filename, true);
						selected = true;
					}
				#endif

				rx = ".+\\.png$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					linearwidget->exportToPNG(filename);
					selected = true;
				}

				rx = ".+\\.svg$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					linearwidget->exportToSVG(filename);
					selected = true;
				}
				break;
			case cyclic:
			case cyclicpolyketide:
				rx = ".+\\.pdf$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					cyclicwidget->exportToPDF(filename, false);
					selected = true;
				}

				#if OS_TYPE == WIN
					rx = ".+\\.ps$";
					if (!selected && (regex_search(filename.toStdString(), rx))) {
						cyclicwidget->exportToPDF(filename, true);
						selected = true;
					}
				#endif
				
				rx = ".+\\.png$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					cyclicwidget->exportToPNG(filename);
					selected = true;
				}

				rx = ".+\\.svg$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					cyclicwidget->exportToSVG(filename);
					selected = true;
				}
				break;
			case branched:
				rx = ".+\\.pdf$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					branchedwidget->exportToPDF(filename, false);
					selected = true;
				}

				#if OS_TYPE == WIN
					rx = ".+\\.ps$";
					if (!selected && (regex_search(filename.toStdString(), rx))) {
						branchedwidget->exportToPDF(filename, true);
						selected = true;
					}
				#endif

				rx = ".+\\.png$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					branchedwidget->exportToPNG(filename);
					selected = true;
				}

				rx = ".+\\.svg$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					branchedwidget->exportToSVG(filename);
					selected = true;
				}
				break;
			case branchcyclic:
				rx = ".+\\.pdf$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					branchcyclicwidget->exportToPDF(filename, false);
					selected = true;
				}

				#if OS_TYPE == WIN
					rx = ".+\\.ps$";
					if (!selected && (regex_search(filename.toStdString(), rx))) {
						branchcyclicwidget->exportToPDF(filename, true);
						selected = true;
					}
				#endif
				
				rx = ".+\\.png$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					branchcyclicwidget->exportToPNG(filename);
					selected = true;
				}

				rx = ".+\\.svg$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					branchcyclicwidget->exportToSVG(filename);
					selected = true;
				}
				break;
			case other:
				break;
			default:
				break;
		}

		if (!selected) {
			QMessageBox msgBox;
			QString errstr = "Unsupported file format.";
			msgBox.setText(errstr);
			msgBox.exec();
		}
	}
}


void cSpectrumDetailWidget::openFindDialog() {
	finddialog->exec();
}


void cSpectrumDetailWidget::openExportImageDialog() {
	if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {
		exportdialog->exec();
	}
	else {
		exportSpectrum();
	}
}


void cSpectrumDetailWidget::exportTableToCSV() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export Table to CSV..."), "./", "Files (*.csv)");
	
	if (!filename.isEmpty()) {

		QProgressDialog progress("Exporting the CSV file...", "Cancel", 0, proxymodel->rowCount(), this);
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

		for (int i = 0; i < peakstablemodel->columnCount(); i++) {
			out << "\"" << peakstablemodel->horizontalHeaderItem(i)->text() << "\"";
			if (i < peakstablemodel->columnCount() - 1) {
				out << ",";
			}
		}
		out << endl;

		QStandardItem* item;
		for (int i = 0; i < proxymodel->rowCount(); i++) {

			for (int j = 0; j < peakstablemodel->columnCount(); j++) {
				item = peakstablemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, j)));
				if (item) {
					out << "\"" << item->data(Qt::DisplayRole).toString() << "\"";
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


void cSpectrumDetailWidget::movePrevious() {
	if (currentfinditem < (int)tablematches.size()) {
		peakstable->clearSelection();
	}

	int count = (int)tablematches.size() + textbrowser->extraSelections().size();
	currentfinditem = (currentfinditem + count - 1)%count;

	if (currentfinditem < (int)tablematches.size()) {
		peakstable->setFocus();
		peakstable->scrollTo(proxymodel->index(tablematches[currentfinditem].row, tablematches[currentfinditem].column));
		peakstable->selectionModel()->select(proxymodel->index(tablematches[currentfinditem].row, tablematches[currentfinditem].column), QItemSelectionModel::Select);
	}
	else {
		textbrowser->setFocus();
		textbrowser->setTextCursor(textbrowser->extraSelections().at(currentfinditem - (int)tablematches.size()).cursor);
	}
}


void cSpectrumDetailWidget::moveNext() {
	if (currentfinditem < (int)tablematches.size()) {
		peakstable->clearSelection();
	}

	int count = (int)tablematches.size() + textbrowser->extraSelections().size();
	currentfinditem = (currentfinditem + 1)%count;

	if (currentfinditem < (int)tablematches.size()) {
		peakstable->setFocus();
		peakstable->scrollTo(proxymodel->index(tablematches[currentfinditem].row, tablematches[currentfinditem].column));
		peakstable->selectionModel()->select(proxymodel->index(tablematches[currentfinditem].row, tablematches[currentfinditem].column), QItemSelectionModel::Select);
	}
	else {
		textbrowser->setFocus();
		textbrowser->setTextCursor(textbrowser->extraSelections().at(currentfinditem - (int)tablematches.size()).cursor);
	}
}


void cSpectrumDetailWidget::headerItemClicked(int index) {
	findAll("", 0, false);
}


void cSpectrumDetailWidget::filterPeaksTable() {
	findAll("", 0, false);

	bool hm = actionHideMatched->isChecked();
	bool hu = actionHideUnmatched->isChecked();
	bool hs = actionHideScrambled->isChecked();

	proxymodel->setFlags(hm, hu, hs);
	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterFixedString("scrambled");
}


void cSpectrumDetailWidget::hideMatchedPeaks(bool hide) {
	spectrumscene->hideMatchedPeaks(hide);
	filterPeaksTable();
}


void cSpectrumDetailWidget::hideUnmatchedPeaks(bool hide) {
	spectrumscene->hideUnmatchedPeaks(hide);
	filterPeaksTable();
}


void cSpectrumDetailWidget::hideScrambledPeaks(bool hide) {
	spectrumscene->hideScrambledPeaks(hide);
	filterPeaksTable();
}

