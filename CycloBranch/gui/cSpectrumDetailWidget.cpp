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
#include <QAction>
#include <QLabel>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QClipboard>
#include <QApplication>
#include <QProcess>


cSpectrumDetailWidget::cSpectrumDetailWidget() {
	parent = 0;
	parameters = 0;

	rawdata = 0;
	imzmlprofilemetadata = 0;
	profilemz64precision = false;
	profileintensity64precision = false;

	rowid = 0;
	preparedToShow = false;
	localneutralosses.clear();
	theoreticalspectrum = new cTheoreticalSpectrum();

	activefileid = 0;
}


cSpectrumDetailWidget::cSpectrumDetailWidget(const cSpectrumDetailWidget& sd) {
	*this = sd;
}


cSpectrumDetailWidget& cSpectrumDetailWidget::operator=(const cSpectrumDetailWidget& sd) {
	parameters = sd.parameters;

	rawdata = sd.rawdata;
	imzmlprofilemetadata = sd.imzmlprofilemetadata;
	profilemz64precision = sd.profilemz64precision;
	profileintensity64precision = sd.profileintensity64precision;

	preparedToShow = false;
	localneutralosses = sd.localneutralosses;
	theoreticalspectrum = new cTheoreticalSpectrum();

	if (parameters && sd.theoreticalspectrum) {
		initialize(sd.rowid, sd.activefileid, globalpreferences, parameters, *sd.theoreticalspectrum, sd.parent);
	}

	if (parameters && sd.preparedToShow) {
		prepareToShow(actionShowIsomers, rawdata,  imzmlprofilemetadata, profilemz64precision, profileintensity64precision, true);
	}

	setWindowTitle(sd.windowTitle());

	return *this;
}


void cSpectrumDetailWidget::initialize(int rowid, int activefileid, cGlobalPreferences* globalpreferences, cParameters* parameters, cTheoreticalSpectrum& theoreticalspectrum, QWidget* parent) {
	this->rowid = rowid;
	this->activefileid = activefileid;
	this->globalpreferences = globalpreferences;
	this->parameters = parameters;
	*this->theoreticalspectrum = theoreticalspectrum;
	this->parent = parent;
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
			
			switch (parameters->peptidetype) {
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


string cSpectrumDetailWidget::getPeaksTableAsHTMLString(bool unmatchedtheoreticalpeaks, bool unmatchedexperimentalpeaks, bool exportisomers) {
	string s = "";
	int rowcount, columncount, thpeakscount;
	int secondspace, langle, rangle, tmp1, tmp2;
	cPeaksList* thpeaks;
	cPeak* peak;
	bool isred;
	string tdwidth;
	string desc;
	QString celltext;
	size_t pos;

	if (theoreticalspectrum && parameters) {

		s += "<table class=\"tablesorter\" cellpadding=\"0\" cellspacing=\"0\" border=\"1\" width=\"100%\"><thead><tr>";

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
			if (parameters->generateisotopepattern) {
				if (parameters->calculatefdrs) {
					columncount = 12;
				}
				else {
					columncount = 10;
				}
			}
			else {
				columncount = 9;
			}
		}
		else {
			if (parameters->generateisotopepattern) {
				columncount = 9;
			}
			else {
				columncount = 8;
			}
		}

		tdwidth = to_string(100/columncount);

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
			if (parameters->generateisotopepattern) {
				s += "<th width=\"" + tdwidth + "%\"><b>Pattern Type</b></th>";
			}
			else {
				s += "<th width=\"" + tdwidth + "%\"><b>Ion Type</b></th>";
			}
		}
		else {
			s += "<th width=\"" + tdwidth + "%\"><b>Fragment Type</b></th>";
		}

		s += "<th width=\"" + tdwidth + "%\"><b>Theoretical m/z</b></th>";

		if (parameters->generateisotopepattern) {
			s += "<th width=\"" + tdwidth + "%\"><b>Theoretical Intensity [%]</b></th>";
		}

		s += "<th width=\"" + tdwidth + "%\"><b>Experimental m/z</b></th>";
		s += "<th width=\"" + tdwidth + "%\"><b>Relative Intensity [%]</b></th>";
		s += "<th width=\"" + tdwidth + "%\"><b>Absolute Intensity</b></th>";
		s += "<th width=\"" + tdwidth + "%\"><b>Error [ppm]</b></th>";

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
			if (parameters->generateisotopepattern && parameters->calculatefdrs) {
				s += "<th width=\"" + tdwidth + "%\"><b>Score</b></th>";
				s += "<th width=\"" + tdwidth + "%\"><b>FDR</b></th>";
			}
			s += "<th width=\"" + tdwidth + "%\"><b>Summary Formula</b></th>";
			s += "<th width=\"" + tdwidth + "%\"><b>Name</b></th>";
			s += "<th width=\"" + tdwidth + "%\"><b>Reference</b></th>";
		}
		else {
			s += "<th width=\"" + tdwidth + "%\"><b>Summary Formula</b></th>";
			s += "<th width=\"" + tdwidth + "%\"><b>Sequence</b></th>";
		}

		s += "</tr></thead><tbody>";

		thpeaks = theoreticalspectrum->getTheoreticalPeaks();
		thpeakscount = theoreticalspectrum->getTheoreticalPeaks()->size();

		rowcount = thpeakscount + theoreticalspectrum->getUnmatchedExperimentalPeaksCount();

		// theoretical peaks
		for (int i = 0; i < thpeakscount; i++) {
			peak = &((*thpeaks)[i]);
			
			if (peak->descriptionid != -1) {
				peak->description = parameters->peakidtodesc[peak->descriptionid];
			}

			if (peak->isotopeformulaid != -1) {
				peak->isotopeformula = parameters->isotopeformulaidtodesc[peak->isotopeformulaid];
			}

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
		
			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);
				s += printHTMLTableCell(peak->description.substr(0, secondspace), isred);
			}
			else {
				s += printHTMLTableCell(peak->description.substr(0, peak->description.find(':')), isred);
			}
		
			s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->mzratio)), isred);

			if (parameters->generateisotopepattern) {
				if (peak->relativeintensity > 0) {
					s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->relativeintensity)), isred);
				}
				else {
					s += "<td></td>";
				}
			}

			if (peak->matchedmz > 0) {
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedmz)), isred);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedrelativeintensity)), isred);
				s += printHTMLTableCell(QVariant(cropDecimalsByteArray(peak->matchedabsoluteintensity)).toString().toStdString(), isred);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedppm)), isred);
				if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
					if (parameters->generateisotopepattern && parameters->calculatefdrs) {
						s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(theoreticalspectrum->getTargetPatternScore(peak->groupid))), isred);
						s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(theoreticalspectrum->getTargetPatternFDR(peak->groupid))), isred);
					}
				}
			}
			else {
				s += "<td></td><td></td><td></td><td></td>";
				if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
					if (parameters->generateisotopepattern && parameters->calculatefdrs) {
						s += "<td></td><td></td>";
					}
				}
			}

			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				s += printHTMLTableCell(peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1), isred);

				langle = (int)peak->description.rfind("</a>");
				rangle = (int)peak->description.find('>');
				if ((langle != string::npos) && (rangle != string::npos)) {
					s += printHTMLTableCell(peak->description.substr(rangle + 1, langle - rangle - 1), isred);

					tmp1 = (int)peak->description.find('<');
					tmp2 = (int)peak->description.rfind("</a>");
					tmp2 += 3;

					celltext = peak->description.substr(tmp1, rangle - tmp1 + 1).c_str();
					if (celltext.contains("https://pubchem.ncbi.nlm.nih.gov/#query=")) {
						celltext += "search";
					}
					else {
						celltext += "view";
					}
					celltext += peak->description.substr(langle, tmp2 - langle + 1).c_str();
					s += printHTMLTableCell(celltext.toStdString(), isred);
				}
				else {
					s += printHTMLTableCell(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2), isred);
					s += "<td></td>";
				}
			}
			else {
				if (parameters->generateisotopepattern) {
					s += printHTMLTableCell(peak->isotopeformula, isred);
				}
				else {
					s += printHTMLTableCell(peak->formula.getFancySummary(peak->charge).c_str(), isred);
				}

				pos = peak->description.find(':');
				if ((pos != string::npos) && (pos + 2 < peak->description.size())) {
					desc = peak->description.substr(pos + 2);
					if (!exportisomers) {
						stripIsomers(desc);
					}
					s += printHTMLTableCell(desc, isred);
				}
				else {
					s += "<td></td>";
				}
			}

			s += "</tr>";
		}

		// unmatched experimental peaks
		if (unmatchedexperimentalpeaks) {
			int numberofexperimentalpeaks = theoreticalspectrum->getExperimentalSpectrum().size();
			for (int i = 0; i < numberofexperimentalpeaks; i++) {
				peak = &(theoreticalspectrum->getExperimentalSpectrum()[i]);
				if (peak->matched > 0) {
					continue;
				}
				s += "<tr>";
				s += "<td></td><td></td>";
				if (parameters->generateisotopepattern) {
					s += "<td></td>";
				}
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->mzratio)), false);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->relativeintensity)), false);
				s += printHTMLTableCell(QVariant(cropDecimalsByteArray(peak->absoluteintensity)).toString().toStdString(), false);
				s += "<td></td><td></td><td></td>";
				if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
					s += "<td></td>";
					if (parameters->generateisotopepattern && parameters->calculatefdrs) {
						s += "<td></td><td></td>";
					}
				}
				s += "</tr>";
			}
		}

		s += "</tbody></table>";

	}

	return s;
}


string cSpectrumDetailWidget::getPartialPeaksTableAsHTMLString(int id, bool exportisomers) {
	string s = "";
	int thpeakscount;
	int secondspace, langle, rangle, tmp1, tmp2;
	cPeaksList* thpeaks;
	cPeak* peak;
	bool isred;
	string desc;
	QString celltext;
	size_t pos;

	if (theoreticalspectrum && parameters) {

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
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

			if (peak->descriptionid != -1) {
				peak->description = parameters->peakidtodesc[peak->descriptionid];
			}

			if (peak->isotopeformulaid != -1) {
				peak->isotopeformula = parameters->isotopeformulaidtodesc[peak->isotopeformulaid];
			}

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
			s += printHTMLTableCell(to_string(id + 1), isred);
		
			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				if ((parameters->peaklistfileformats[activefileid] == mis) || (parameters->peaklistfileformats[activefileid] == imzML)) {
					s += printHTMLTableCell(to_string(theoreticalspectrum->getExperimentalSpectrum().getCoordinateX()), isred);
					s += printHTMLTableCell(to_string(theoreticalspectrum->getExperimentalSpectrum().getCoordinateY()), isred);
				}
				else {
					s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(theoreticalspectrum->getExperimentalSpectrum().getRetentionTime())), isred);
				}
				secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);
				s += printHTMLTableCell(peak->description.substr(0, secondspace), isred);
			}
			
			if (parameters->mode == denovoengine) {
				s += printHTMLTableCell(to_string(theoreticalspectrum->getPathId() + 1), isred);
			}

			if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {
				s += printHTMLTableCell(peak->description.substr(0, peak->description.find(':')), isred);
			}
		
			s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->mzratio)), isred);

			if (parameters->generateisotopepattern) {
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->relativeintensity)), isred);
			}

			if (peak->matchedmz > 0) {
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedmz)), isred);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedrelativeintensity)), isred);
				s += printHTMLTableCell(QVariant(cropDecimalsByteArray(peak->matchedabsoluteintensity)).toString().toStdString(), isred);
				s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(peak->matchedppm)), isred);
				if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
					if (parameters->generateisotopepattern && parameters->calculatefdrs) {
						s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(theoreticalspectrum->getTargetPatternScore(peak->groupid))), isred);
						s += printHTMLTableCell(to_string(cropPrecisionToSixDecimals(theoreticalspectrum->getTargetPatternFDR(peak->groupid))), isred);
					}
				}
			}
			else {
				s += "<td></td><td></td><td></td><td></td>";
				if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
					if (parameters->generateisotopepattern && parameters->calculatefdrs) {
						s += "<td></td><td></td>";
					}
				}
			}

			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				s += printHTMLTableCell(peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1), isred);

				langle = (int)peak->description.rfind("</a>");
				rangle = (int)peak->description.find('>');
				if ((langle != string::npos) && (rangle != string::npos)) {
					s += printHTMLTableCell(peak->description.substr(rangle + 1, langle - rangle - 1), isred);

					tmp1 = (int)peak->description.find('<');
					tmp2 = (int)peak->description.rfind("</a>");
					tmp2 += 3;

					celltext = peak->description.substr(tmp1, rangle - tmp1 + 1).c_str();
					if (celltext.contains("https://pubchem.ncbi.nlm.nih.gov/#query=")) {
						celltext += "search";
					}
					else {
						celltext += "view";
					}
					celltext += peak->description.substr(langle, tmp2 - langle + 1).c_str();
					s += printHTMLTableCell(celltext.toStdString(), isred);
				}
				else {
					s += printHTMLTableCell(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2), isred);
					s += "<td></td>";
				}
			}
			else {
				if (parameters->generateisotopepattern) {
					s += printHTMLTableCell(peak->isotopeformula, isred);
				}
				else {
					s += printHTMLTableCell(peak->formula.getFancySummary(peak->charge).c_str(), isred);
				}

				pos = peak->description.find(':');
				if ((pos != string::npos) && (pos + 2 < peak->description.size())) {
					desc = peak->description.substr(pos + 2);
					if (!exportisomers) {
						stripIsomers(desc);
					}
					s += printHTMLTableCell(desc, isred);
				}
				else {
					s += "<td></td>";
				}
			}

			s += "</tr>";
		}

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
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
		delete labelseparator;
		delete maxmz;
		delete setmzinterval;
		delete resetmzinterval;
		delete hboxmz;
		delete widgetmz;

		delete ionserieslabel;
		delete ionseriescombobox;
		delete ionserieshbox;
		delete ionserieswidget;

		delete neutrallosslabel;
		delete neutrallosscombobox;
		delete neutrallosshbox;
		delete neutrallosswidget;

		delete rotationlabel;
		delete rotationcombobox;
		delete rotationhbox;
		delete rotationwidget;

		delete trotationlabel;
		delete trotationcombobox;
		delete trotationhbox;
		delete trotationwidget;

		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {

			switch (parameters->peptidetype) {
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

		delete actionExportTable;
		delete actionExportSpectrum;
		delete actionCloseWindow;
		delete actionFind;
		delete actionPrevious;
		delete actionNext;
		delete actionZoomIn;
		delete actionZoomOut;
		delete actionZoomReset;
		delete actionAbsoluteIntensity;
		delete actionRawData;
		delete actionHideMatched;
		delete actionHideUnmatched;
		delete actionHideScrambled;
		delete actionHideLabels;
		delete actionMouseMzSelection;
		delete actionHTMLDocumentation;

		delete finddialog;
		delete exportdialog;

		delete menuFile;
		delete menuFind;
		delete menuView;
		delete menuHelp;

		delete menuBar;

	}
}


void cSpectrumDetailWidget::closeEvent(QCloseEvent *event) {
	hide();
	event->accept();
}


void cSpectrumDetailWidget::prepareToShow(QAction* actionShowIsomers, cPeakListSeries* rawdata, vector<cImzMLItem>* imzmlprofilemetadata, bool profilemz64precision, bool profileintensity64precision, bool usedefaultsize) {

	if (!preparedToShow) {

		if (parameters) {
			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				string title = "Experimental Spectrum No. " + to_string(rowid);
				if (theoreticalspectrum) {
					if ((parameters->peaklistfileformats[activefileid] == mis) || (parameters->peaklistfileformats[activefileid] == imzML)) {
						title += " - X: " + to_string(theoreticalspectrum->getExperimentalSpectrum().getCoordinateX());
						title += " Y: " + to_string(theoreticalspectrum->getExperimentalSpectrum().getCoordinateY());
					}
					else {
						if (theoreticalspectrum->getExperimentalSpectrum().getRetentionTime() > 0) {
							title += " - Time: " + to_string(theoreticalspectrum->getExperimentalSpectrum().getRetentionTime());
						}
					}
				}
				setWindowTitle(title.c_str());
			}
			else if (parameters->mode == singlecomparison) {
				setWindowTitle(("Experimental Spectrum No. " + to_string(rowid)).c_str());
			}
			else {
				setWindowTitle(("Theoretical Spectrum No. " + to_string(rowid)).c_str());
			}
		}

		this->rawdata = rawdata;
		this->imzmlprofilemetadata = imzmlprofilemetadata;
		this->profilemz64precision = profilemz64precision;
		this->profileintensity64precision = profileintensity64precision;

		connect(actionShowIsomers, SIGNAL(triggered()), this, SLOT(showIsomersStateChanged()));
		this->actionShowIsomers = actionShowIsomers;
		reportisomers = actionShowIsomers->isChecked();

		vsplitter = new QSplitter();
		vsplitter->setOrientation(Qt::Vertical);

		hsplitter1 = new QSplitter();
		hsplitter1->setOrientation(Qt::Horizontal);

		hsplitter2 = new QSplitter();
		hsplitter2->setOrientation(Qt::Horizontal);


		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {
			switch (parameters->peptidetype) {
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


		menuBar = new QMenuBar(this);
		menuBar->setNativeMenuBar(false);
		menuFile = new QMenu(tr("&File"), this);
		menuFind = new QMenu(tr("Fin&d"), this);
		menuView = new QMenu(tr("&View"), this);
		menuHelp = new QMenu(tr("&Help"), this);


		toolbarFile = addToolBar(tr("File"));

		actionExportTable = new QAction(QIcon(":/images/icons/csv.png"), tr("&Export Table to CSV"), this);
		actionExportTable->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
		actionExportTable->setToolTip("Export Table to CSV (Ctrl + E)");
		toolbarFile->addAction(actionExportTable);
		connect(actionExportTable, SIGNAL(triggered()), this, SLOT(exportTableToCSV()));

		actionExportSpectrum = new QAction(QIcon(":/images/icons/66.png"), tr("Export Ima&ge"), this);
		actionExportSpectrum->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
		actionExportSpectrum->setToolTip("Export Image (Ctrl + G)");
		toolbarFile->addAction(actionExportSpectrum);
		connect(actionExportSpectrum, SIGNAL(triggered()), this, SLOT(openExportImageDialog()));

		actionCloseWindow = new QAction(QIcon(":/images/icons/33.png"), tr("&Close"), this);
		actionCloseWindow->setShortcut(QKeySequence(Qt::Key_Escape));
		actionCloseWindow->setToolTip("Close (Esc)");
		toolbarFile->addAction(actionCloseWindow);
		connect(actionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));


		toolbarFind = addToolBar(tr("Find"));
				
		actionFind = new QAction(QIcon(":/images/icons/65.png"), tr("&Find Text"), this);
		actionFind->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
		actionFind->setToolTip("Find Text (Ctrl + F)");
		toolbarFind->addAction(actionFind);
		connect(actionFind, SIGNAL(triggered()), this, SLOT(openFindDialog()));

		actionPrevious = new QAction(QIcon(":/images/icons/56.png"), tr("Find &Previous"), this);
		actionPrevious->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
		actionPrevious->setToolTip("Find Previous (Ctrl + 1)");
		toolbarFind->addAction(actionPrevious);
		connect(actionPrevious, SIGNAL(triggered()), this, SLOT(movePrevious()));
		actionPrevious->setDisabled(true);

		actionNext = new QAction(QIcon(":/images/icons/57.png"), tr("Find &Next"), this);
		actionNext->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
		actionNext->setToolTip("Find Next (Ctrl + 2)");
		toolbarFind->addAction(actionNext);
		connect(actionNext, SIGNAL(triggered()), this, SLOT(moveNext()));
		actionNext->setDisabled(true);


		toolbarView = addToolBar(tr("View"));
				
		actionZoomIn = new QAction(QIcon(":/images/icons/83.png"), tr("Zoom &In"), this);
		actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
		actionZoomIn->setToolTip("Zoom In (Ctrl +)");
		toolbarView->addAction(actionZoomIn);
		connect(actionZoomIn, SIGNAL(triggered()), spectrumscene, SLOT(zoomIn()));

		actionZoomOut = new QAction(QIcon(":/images/icons/82.png"), tr("Zoom &Out"), this);
		actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
		actionZoomOut->setToolTip("Zoom Out (Ctrl -)");
		toolbarView->addAction(actionZoomOut);
		connect(actionZoomOut, SIGNAL(triggered()), spectrumscene, SLOT(zoomOut()));

		actionZoomReset = new QAction(QIcon(":/images/icons/84.png"), tr("&Reset Zoom"), this);
		actionZoomReset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
		actionZoomReset->setToolTip("Zoom Reset (Ctrl + R)");
		toolbarView->addAction(actionZoomReset);
		connect(actionZoomReset, SIGNAL(triggered()), spectrumscene, SLOT(normalSize()));

		toolbarView->addSeparator();

		actionAbsoluteIntensity = new QAction(QIcon(":/images/icons/11.png"), tr("&Absolute Intensity"), this);
		actionAbsoluteIntensity->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
		actionAbsoluteIntensity->setToolTip("Absolute Intensity (Ctrl + I)");
		actionAbsoluteIntensity->setCheckable(true);
		toolbarView->addAction(actionAbsoluteIntensity);
		connect(actionAbsoluteIntensity, SIGNAL(toggled(bool)), spectrumscene, SLOT(absoluteIntensityStateChanged(bool)));

		actionRawData = new QAction(QIcon(":/images/icons/chromatography.png"), tr("&Profile Spectrum"), this);
		actionRawData->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
		actionRawData->setToolTip("Profile Spectrum (Ctrl + P)");
		actionRawData->setCheckable(true);
		toolbarView->addAction(actionRawData);
		connect(actionRawData, SIGNAL(toggled(bool)), this, SLOT(rawDataStateChanged(bool)));
		connect(this, SIGNAL(rawDataStateChangedSignal(bool)), spectrumscene, SLOT(rawDataStateChanged(bool)));

		toolbarView->addSeparator();

		actionHideMatched = new QAction(QIcon(":/images/icons/20.png"), tr("Hide &Matched Peaks"), this);
		actionHideMatched->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
		actionHideMatched->setToolTip("Hide Matched Peaks (Ctrl + M)");
		actionHideMatched->setCheckable(true);
		toolbarView->addAction(actionHideMatched);
		connect(actionHideMatched, SIGNAL(toggled(bool)), this, SLOT(hideMatchedPeaks(bool)));

		actionHideUnmatched = new QAction(QIcon(":/images/icons/81.png"), tr("Hide &Unmatched Peaks"), this);
		actionHideUnmatched->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
		actionHideUnmatched->setToolTip("Hide Unmatched Peaks (Ctrl + U)");
		actionHideUnmatched->setCheckable(true);
		toolbarView->addAction(actionHideUnmatched);
		connect(actionHideUnmatched, SIGNAL(toggled(bool)), this, SLOT(hideUnmatchedPeaks(bool)));

		actionHideScrambled = new QAction(QIcon(":/images/icons/80.png"), tr("Hide &Scrambled Peaks"), this);
		actionHideScrambled->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
		actionHideScrambled->setToolTip("Hide Scrambled Peaks (Ctrl + S)");
		actionHideScrambled->setCheckable(true);
		actionHideScrambled->setEnabled(false);
		toolbarView->addAction(actionHideScrambled);

		actionHideLabels = new QAction(QIcon(":/images/icons/79.png"), tr("Hide Peak &Labels"), this);
		actionHideLabels->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
		actionHideLabels->setToolTip("Hide Peak Labels (Ctrl + L)");
		actionHideLabels->setCheckable(true);
		toolbarView->addAction(actionHideLabels);
		connect(actionHideLabels, SIGNAL(toggled(bool)), this, SLOT(hidePeakLabels(bool)));


		toolbarHelp = addToolBar(tr("Help"));

		actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
		actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
		actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
		toolbarHelp->addAction(actionHTMLDocumentation);
		connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));


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

		connect(peakstable, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(tableDoubleClicked(const QModelIndex&)));

		labelmz = new QLabel(tr("m/z: "));

		minmz = new QDoubleSpinBox();
		minmz->setDecimals(6);
		minmz->setRange(0, 100000);
		minmz->setSingleStep(1);

		labelseparator = new QLabel(tr("-"));

		maxmz = new QDoubleSpinBox();
		maxmz->setDecimals(6);
		maxmz->setRange(0, 100000);
		maxmz->setSingleStep(1);

		setmzinterval = new QPushButton("Set");
		setmzinterval->setMinimumWidth(75);
		connect(setmzinterval, SIGNAL(released()), this, SLOT(setMZInterval()));
		connect(this, SIGNAL(emitMZInterval(double, double)), spectrumscene, SLOT(setMZInterval(double, double)));

		resetmzinterval = new QPushButton("Reset");
		resetmzinterval->setMinimumWidth(75);
		connect(resetmzinterval, SIGNAL(released()), spectrumscene, SLOT(resetMZInterval()));
		connect(spectrumscene, SIGNAL(updateMZInterval(double, double)), this, SLOT(updateMZInterval(double, double)));

		hboxmz = new QHBoxLayout();
		hboxmz->addWidget(labelmz);
		hboxmz->addWidget(minmz);
		hboxmz->addWidget(labelseparator);
		hboxmz->addWidget(maxmz);
		hboxmz->addSpacing(5);
		hboxmz->addWidget(setmzinterval);
		hboxmz->addSpacing(5);
		hboxmz->addWidget(resetmzinterval);

		widgetmz = new QWidget();
		widgetmz->setLayout(hboxmz);

		toolbarMz = addToolBar(tr("m/z"));

		actionMouseMzSelection = new QAction(QIcon(":/images/icons/64.png"), tr("Mouse m/z Selection Tool"), this);
		actionMouseMzSelection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
		actionMouseMzSelection->setToolTip("Mouse m/z Selection Tool (Ctrl + T)");
		actionMouseMzSelection->setCheckable(true);
		actionMouseMzSelection->setChecked(true);
		actionMouseMzSelection->setEnabled(true);
		toolbarMz->addAction(actionMouseMzSelection);
		connect(actionMouseMzSelection, SIGNAL(toggled(bool)), spectrumscene, SLOT(enableMouseMzSelectionTool(bool)));

		toolbarMz->addWidget(widgetmz);

		addToolBarBreak();


		ionserieslabel = new QLabel(tr("Fragment ion type: "));
		ionseriescombobox = new QComboBox();
		ionseriescombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

		ionserieshbox = new QHBoxLayout();
		ionserieshbox->addWidget(ionserieslabel);
		ionserieshbox->addWidget(ionseriescombobox);

		ionserieswidget = new QWidget();
		ionserieswidget->setLayout(ionserieshbox);


		neutrallosslabel = new QLabel(tr("Neutral loss type: "));
		neutrallosscombobox = new QComboBox();
		neutrallosscombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

		neutrallosshbox = new QHBoxLayout();
		neutrallosshbox->addWidget(neutrallosslabel);
		neutrallosshbox->addWidget(neutrallosscombobox);

		neutrallosswidget = new QWidget();
		neutrallosswidget->setLayout(neutrallosshbox);


		rotationlabel = new QLabel(tr("Ring break up point: "));
		rotationcombobox = new QComboBox();
		rotationcombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

		rotationhbox = new QHBoxLayout();
		rotationhbox->addWidget(rotationlabel);
		rotationhbox->addWidget(rotationcombobox);

		rotationwidget = new QWidget();
		rotationwidget->setLayout(rotationhbox);


		trotationlabel = new QLabel(tr("Linearized sequence: "));
		trotationcombobox = new QComboBox();
		trotationcombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

		trotationhbox = new QHBoxLayout();
		trotationhbox->addWidget(trotationlabel);
		trotationhbox->addWidget(trotationcombobox);

		trotationwidget = new QWidget();
		trotationwidget->setLayout(trotationhbox);


		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {

			toolbarIonSeries = addToolBar(tr("Fragment ion type"));
			toolbarIonSeries->addWidget(ionserieswidget);

			ionseriescombobox->addItem(tr("all"));
			for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {
				ionseriescombobox->addItem(tr(parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].name.c_str()));
			}

			connect(ionseriescombobox, SIGNAL(currentIndexChanged(QString)), spectrumscene, SLOT(ionSeriesChanged(QString)));
			connect(ionseriescombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterTableAfterIonSeriesChanged(int)));


			toolbarNeutralLoss = addToolBar(tr("Neutral loss type"));
			toolbarNeutralLoss->addWidget(neutrallosswidget);

			neutrallosscombobox->addItem(tr("all"));
			
			connect(neutrallosscombobox, SIGNAL(currentIndexChanged(QString)), spectrumscene, SLOT(neutralLossChanged(QString)));
			connect(neutrallosscombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterTableAfterNeutralLossChanged(int)));


			switch (parameters->peptidetype) {
				case linear:
				case linearpolyketide:
					connect(ionseriescombobox, SIGNAL(currentIndexChanged(QString)), linearwidget, SLOT(ionSeriesChanged(QString)));
					connect(neutrallosscombobox, SIGNAL(currentIndexChanged(QString)), linearwidget, SLOT(neutralLossChanged(QString)));
					break;
				case cyclic:
				case cyclicpolyketide:
					connect(ionseriescombobox, SIGNAL(currentIndexChanged(QString)), cyclicwidget, SLOT(ionSeriesChanged(QString)));
					connect(neutrallosscombobox, SIGNAL(currentIndexChanged(QString)), cyclicwidget, SLOT(neutralLossChanged(QString)));
					break;
				case branched:
					connect(ionseriescombobox, SIGNAL(currentIndexChanged(QString)), branchedwidget, SLOT(ionSeriesChanged(QString)));
					connect(neutrallosscombobox, SIGNAL(currentIndexChanged(QString)), branchedwidget, SLOT(neutralLossChanged(QString)));
					break;
				case branchcyclic:
					connect(ionseriescombobox, SIGNAL(currentIndexChanged(QString)), branchcyclicwidget, SLOT(ionSeriesChanged(QString)));
					connect(neutrallosscombobox, SIGNAL(currentIndexChanged(QString)), branchcyclicwidget, SLOT(neutralLossChanged(QString)));
					break;
				case other:
					break;
				default:
					break;
			}


			// cyclic
			if (theoreticalspectrum && ((parameters->peptidetype == cyclic) || (parameters->peptidetype == cyclicpolyketide))) {			
				vector<string> rotationslabels = theoreticalspectrum->getLabelsOfRotations();
				rotationcombobox->addItem(tr("all"));
				for (auto& it : rotationslabels) {
					rotationcombobox->addItem(tr(it.c_str()));
				}

				connect(rotationcombobox, SIGNAL(currentIndexChanged(int)), spectrumscene, SLOT(rotationChanged(int)));
				connect(rotationcombobox, SIGNAL(currentIndexChanged(QString)), spectrumscene, SLOT(rotationChanged(QString)));
				connect(rotationcombobox, SIGNAL(currentIndexChanged(int)), cyclicwidget, SLOT(rotationChanged(int)));
				connect(rotationcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterTableAfterRotationChanged(int)));

				toolbarRotation = addToolBar(tr("Ring break up point"));
				toolbarRotation->addWidget(rotationwidget);
			}


			// branched
			if (parameters->peptidetype == branched) {
				trotationcombobox->addItem(tr("all"));
				trotationcombobox->addItem(tr("1 (N-term to C-term)"));
				trotationcombobox->addItem(tr("2 (Branch to C-term)"));
				trotationcombobox->addItem(tr("3 (C-term to N-term)"));
				trotationcombobox->addItem(tr("4 (N-term to Branch)"));
				trotationcombobox->addItem(tr("5 (Branch to N-term)"));
				trotationcombobox->addItem(tr("6 (C-term to Branch)"));

				connect(trotationcombobox, SIGNAL(currentIndexChanged(int)), spectrumscene, SLOT(trotationChanged(int)));
				connect(trotationcombobox, SIGNAL(currentIndexChanged(int)), branchedwidget, SLOT(trotationChanged(int)));
				connect(trotationcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterTableAfterTRotationChanged(int)));

				toolbarTrotation = addToolBar(tr("Linearized sequence"));
				toolbarTrotation->addWidget(trotationwidget);
			}


			// branch-cyclic
			if (parameters && theoreticalspectrum && (parameters->peptidetype == branchcyclic)) {
				vector<string> rotationslabels = theoreticalspectrum->getLabelsOfRotations();
				rotationcombobox->addItem(tr("all"));
				for (auto& it : rotationslabels) {
					rotationcombobox->addItem(tr(it.c_str()));
				}

				connect(rotationcombobox, SIGNAL(currentIndexChanged(int)), spectrumscene, SLOT(rotationChanged(int)));
				connect(rotationcombobox, SIGNAL(currentIndexChanged(QString)), spectrumscene, SLOT(rotationChanged(QString)));
				connect(rotationcombobox, SIGNAL(currentIndexChanged(int)), branchcyclicwidget, SLOT(rotationChanged(int)));
				connect(rotationcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterTableAfterRotationChanged(int)));

				toolbarRotation = addToolBar(tr("Ring break up point"));
				toolbarRotation->addWidget(rotationwidget);

				trotationcombobox->addItem(tr("all"));
				trotationcombobox->addItem(tr("1 (N-term to C-term)"));
				trotationcombobox->addItem(tr("2 (Branch to C-term)"));
				trotationcombobox->addItem(tr("3 (C-term to N-term)"));
				trotationcombobox->addItem(tr("4 (N-term to Branch)"));
				trotationcombobox->addItem(tr("5 (Branch to N-term)"));
				trotationcombobox->addItem(tr("6 (C-term to Branch)"));

				connect(trotationcombobox, SIGNAL(currentIndexChanged(int)), spectrumscene, SLOT(trotationChanged(int)));
				connect(trotationcombobox, SIGNAL(currentIndexChanged(int)), branchcyclicwidget, SLOT(trotationChanged(int)));
				connect(trotationcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterTableAfterTRotationChanged(int)));
								
				toolbarTrotation = addToolBar(tr("Linearized sequence"));
				toolbarTrotation->addWidget(trotationwidget);
			}


		}

		hsplitter1->addWidget(spectrumscene);
		hsplitter2->addWidget(peakstable);

		QList<int> sizes;
		sizes.push_back(100);
		sizes.push_back(100);

		if ((parameters->mode == denovoengine) || ((parameters->mode == singlecomparison) && (parameters->peptidetype != other)) || ((parameters->mode == databasesearch) && (parameters->peptidetype != other))) {

			switch (parameters->peptidetype) {
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
		centralWidget()->setContentsMargins(5, 5, 5, 5);

		finddialog = new cFindDialog(this);
		exportdialog = new cExportDialog(this);

		menuFile->addAction(actionExportTable);
		menuFile->addAction(actionExportSpectrum);
		menuFile->addSeparator();
		menuFile->addAction(actionCloseWindow);

		menuFind->addAction(actionFind);
		menuFind->addAction(actionPrevious);
		menuFind->addAction(actionNext);

		menuView->addAction(actionZoomIn);
		menuView->addAction(actionZoomOut);
		menuView->addAction(actionZoomReset);
		menuView->addSeparator();
		menuView->addAction(actionAbsoluteIntensity);
		menuView->addAction(actionRawData);
		menuView->addSeparator();
		menuView->addAction(actionHideMatched);
		menuView->addAction(actionHideUnmatched);
		menuView->addAction(actionHideScrambled);
		menuView->addAction(actionHideLabels);

		menuHelp->addAction(actionHTMLDocumentation);

		menuBar->addMenu(menuFile);
		menuBar->addMenu(menuFind);
		menuBar->addMenu(menuView);
		menuBar->addMenu(menuHelp);

		setMenuBar(menuBar);

		if (usedefaultsize) {
			resize(defaultwinsizex, defaultwinsizey);
		}

		QRect geometry = normalGeometry();

		if (parameters && theoreticalspectrum) {

			if ((parameters->mode == denovoengine) || ((parameters->mode == singlecomparison) && (parameters->peptidetype != other)) || ((parameters->mode == databasesearch) && (parameters->peptidetype != other))) {

				switch (parameters->peptidetype) {
					case linear:
					case linearpolyketide:
						linearwidget->initialize(parameters, theoreticalspectrum);
						linearwidget->setReportIsomers(reportisomers);
						break;
					case cyclic:
					case cyclicpolyketide:
						cyclicwidget->initialize(parameters, theoreticalspectrum);
						cyclicwidget->setReportIsomers(reportisomers);
						break;
					case branched:
						branchedwidget->initialize(parameters, theoreticalspectrum);
						branchedwidget->setReportIsomers(reportisomers);
						break;
					case branchcyclic:
						branchcyclicwidget->initialize(parameters, theoreticalspectrum);
						branchcyclicwidget->setReportIsomers(reportisomers);
						break;
					case other:
						break;
					default:
						break;
				}

				string textinfo = getDetailsAsHTMLString() + theoreticalspectrum->getCoverageBySeries();
				if (!reportisomers) {
					stripIsomers(textinfo);
				}
				textbrowser->setHtml(textinfo.c_str());
			
			}

			spectrumscene->initialize(parameters, theoreticalspectrum, rawdata, rowid, activefileid);

			preparePeaksTable(geometry);

			if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {
				if (localneutralosses.size() > 0) {
					neutrallosscombobox->addItem(tr("none"));
					if (localneutralosses.size() <= 10000) {
						vector<string> tmpstrvector;
						tmpstrvector.reserve(localneutralosses.size());
						for (auto& it : localneutralosses) {
							tmpstrvector.push_back(it);
						}

						compareStringBySize cmp;
						sort(tmpstrvector.begin(), tmpstrvector.end(), cmp);

						for (auto& it : tmpstrvector) {
							neutrallosscombobox->addItem(tr(it.c_str()));
						}
					}
				}
			}

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

		for (int j = 0; j < (((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) ? peakstablemodel->columnCount() - 1 : peakstablemodel->columnCount()); j++) {
			item = peakstablemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, j)));

			if (!item) {
				continue;
			}

			item->setBackground(QBrush(Qt::white));

			if (str.isEmpty()) {
				continue;
			}

			string itemstr = item->text().toStdString();
			QString qstr = stripHTML(itemstr).c_str();

			if (opt & QTextDocument::FindWholeWords) {
				if (qstr.compare(str, (opt & QTextDocument::FindCaseSensitively)?Qt::CaseSensitive:Qt::CaseInsensitive) == 0) {
					tpos.row = i;
					tpos.column = j;
					tablematches.push_back(tpos);
					item->setBackground(QBrush(color));
				}
			}
			else {
				if (qstr.contains(str, (opt & QTextDocument::FindCaseSensitively)?Qt::CaseSensitive:Qt::CaseInsensitive)) {
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


void cSpectrumDetailWidget::zoomToPeak(double value) {
	if (value > 0) {
		emit emitMZInterval(value - 5.0, value + 5.0);
	}
}


set<string>& cSpectrumDetailWidget::getLocalNeutralLosses() {
	return localneutralosses;
}


void cSpectrumDetailWidget::setProfileSpectrumEnabled(bool enable) {
	actionRawData->setChecked(enable);
}


bool cSpectrumDetailWidget::hasProfileSpectrumEnabled() {
	return actionRawData->isChecked();
}


void cSpectrumDetailWidget::setAbsoluteIntensityEnabled(bool enable) {
	actionAbsoluteIntensity->setChecked(enable);
}


bool cSpectrumDetailWidget::hasAbsoluteIntensityEnabled() {
	return actionAbsoluteIntensity->isChecked();
}


void cSpectrumDetailWidget::setHideMatchedPeaksEnabled(bool enable) {
	actionHideMatched->setChecked(enable);
}


bool cSpectrumDetailWidget::hasHideMatchedPeaksEnabled() {
	return actionHideMatched->isChecked();
}


void cSpectrumDetailWidget::setHideUnmatchedPeaksEnabled(bool enable) {
	actionHideUnmatched->setChecked(enable);
}


bool cSpectrumDetailWidget::hasHideUnmatchedPeaksEnabled() {
	return actionHideUnmatched->isChecked();
}


void cSpectrumDetailWidget::setHideScrambledPeaksEnabled(bool enable) {
	actionHideScrambled->setChecked(enable);
}


bool cSpectrumDetailWidget::hasHideScrambledPeaksEnabled() {
	return actionHideScrambled->isChecked();
}


void cSpectrumDetailWidget::setHidePeakLabelsEnabled(bool enable) {
	actionHideLabels->setChecked(enable);
}


bool cSpectrumDetailWidget::hasHidePeakLabelsEnabled() {
	return actionHideLabels->isChecked();
}


void cSpectrumDetailWidget::setMouseMzSelectionEnabled(bool enable) {
	actionMouseMzSelection->setChecked(enable);
}


bool cSpectrumDetailWidget::hasMouseMzSelectionEnabled() {
	return actionMouseMzSelection->isChecked();
}


void cSpectrumDetailWidget::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		setMZInterval();
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


void cSpectrumDetailWidget::moveEvent(QMoveEvent *event) {
	emit lastActiveDetail(rowid);
}


void cSpectrumDetailWidget::preparePeaksTable(QRect geometry) {
	peakstable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	peakstable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	peakstable->horizontalHeader()->setSectionsMovable(true);
	//peakstable->verticalHeader()->setDefaultSectionSize(30);

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if (parameters->generateisotopepattern) {
			if (parameters->calculatefdrs) {
				peakstablemodel->setColumnCount(12);
			}
			else {
				peakstablemodel->setColumnCount(10);
			}
		}
		else {
			peakstablemodel->setColumnCount(9);
		}
	}
	else {
		if (parameters->generateisotopepattern) {
			peakstablemodel->setColumnCount(9);
		}
		else {
			peakstablemodel->setColumnCount(8);
		}
	}

	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		peakstablemodel->setHorizontalHeaderItem(i, new QStandardItem());
	}

	int currentcolumn = 0;
	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if (parameters->generateisotopepattern) {
			peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Pattern Type");
		}
		else {
			peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Ion Type");
		}
	}
	else {
		peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Fragment Type");
	}
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical m/z");
	currentcolumn++;

	if (parameters->generateisotopepattern) {
		peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical Intensity [%]");
		currentcolumn++;
	}

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Experimental m/z");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Relative Intensity [%]");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Absolute Intensity");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Error [ppm]");
	currentcolumn++;

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {

		if (parameters->generateisotopepattern && parameters->calculatefdrs) {
			peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Score");
			currentcolumn++;

			peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("FDR");
			currentcolumn++;
		}

		peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Summary Formula");
		if (parameters->generateisotopepattern) {
			peakstable->setItemDelegateForColumn(currentcolumn, new cHTMLDelegate());
		}
		currentcolumn++;

		peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Name");
		currentcolumn++;

		peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Reference");
		peakstable->setItemDelegateForColumn(currentcolumn, new cViewButtonDelegate());
		currentcolumn++;
	}
	else {
		peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Summary Formula");
		if (parameters->generateisotopepattern) {
			peakstable->setItemDelegateForColumn(currentcolumn, new cHTMLDelegate());
		}
		currentcolumn++;

		peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Sequence");
		currentcolumn++;
	}

	connect(peakstable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerItemClicked(int)));

	cPeaksList* thpeaks;
	int thpeakscount;
	cPeak* peak;
	int secondspace, langle, rangle, tmp;
	string stmp, desc, fragname;
	size_t pos;
	QBrush brush;

	thpeaks = theoreticalspectrum->getTheoreticalPeaks();
	thpeakscount = theoreticalspectrum->getTheoreticalPeaks()->size();
	
	peakstablemodel->setRowCount(thpeakscount + theoreticalspectrum->getUnmatchedExperimentalPeaksCount());

	QProgressDialog progress("Preparing the peaklist...", /*"Cancel"*/0, 0, thpeakscount + theoreticalspectrum->getUnmatchedExperimentalPeaksCount(), this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);

	progress.move(geometry.x(), geometry.y());

	QRect progressgeometry;
	progressgeometry.setRect(geometry.x() + geometry.width() / 2 - progress.width() / 2, geometry.y() + geometry.height() / 2 - progress.height() / 2, progress.width(), progress.height());

	progress.setGeometry(progressgeometry);

	progress.setValue(0);

	peakstable->setModel(0);
	proxymodel->setSourceModel(0);
	peakstable->setSortingEnabled(false);

	// theoretical peaks
	for (int i = 0; i < thpeakscount; i++) {
		peak = &((*thpeaks)[i]);

		if (peak->descriptionid != -1) {
			peak->description = parameters->peakidtodesc[peak->descriptionid];
		}

		if (peak->isotopeformulaid != -1) {
			peak->isotopeformula = parameters->isotopeformulaidtodesc[peak->isotopeformulaid];
		}

		if (peak->matchedmz > 0) {
			brush.setColor(QColor(255, 0, 0));
		}
		else {
			brush.setColor(QColor(0, 0, 0));
		}
		
		currentcolumn = 0;
		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
			secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);

			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			peakstablemodel->item(i, currentcolumn)->setText(peak->description.substr(0, secondspace).c_str());
		}
		else {
			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			fragname = peak->description.substr(0, peak->description.find(':'));
			peakstablemodel->item(i, currentcolumn)->setText(fragname.c_str());
			if (fragname.rfind(" -") != string::npos) {
				fragname = fragname.substr(fragname.rfind(" -") + 2);
				if (fragname.find('_') != string::npos) {
					fragname = fragname.substr(0, fragname.find('_'));
				}
				if (fragname.find(' ') != string::npos) {
					fragname = fragname.substr(0, fragname.find(' '));
				}
				localneutralosses.insert(fragname);
			}
		}
		currentcolumn++;
		
		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setForeground(brush);
		peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->mzratio)), Qt::DisplayRole);
		currentcolumn++;

		if (parameters->generateisotopepattern) {
			if (peak->relativeintensity > 0) {
				peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
				peakstablemodel->item(i, currentcolumn)->setForeground(brush);
				peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->relativeintensity)), Qt::DisplayRole);
			}
			currentcolumn++;
		}

		if (peak->matchedmz > 0) {
			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedmz)), Qt::DisplayRole);
			currentcolumn++;

			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedrelativeintensity)), Qt::DisplayRole);
			currentcolumn++;

			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropDecimalsByteArray(peak->matchedabsoluteintensity)), Qt::DisplayRole);
			currentcolumn++;

			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedppm)), Qt::DisplayRole);
			currentcolumn++;

			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				if (parameters->generateisotopepattern && parameters->calculatefdrs) {
					peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
					peakstablemodel->item(i, currentcolumn)->setForeground(brush);
					peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum->getTargetPatternScore(peak->groupid))), Qt::DisplayRole);
					currentcolumn++;

					peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
					peakstablemodel->item(i, currentcolumn)->setForeground(brush);
					peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum->getTargetPatternFDR(peak->groupid))), Qt::DisplayRole);
					currentcolumn++;
				}
			}
		}
		else {
			currentcolumn += 4;
			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				if (parameters->generateisotopepattern && parameters->calculatefdrs) {
					currentcolumn += 2;
				}
			}
		}

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			QString summary;
			if (parameters->generateisotopepattern && (peak->matched > 0)) {
				summary += "<font color=\"red\">";
			}
			summary += peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1).c_str();
			if (parameters->generateisotopepattern && (peak->matched > 0)) {
				summary += "</font>";
			}
			peakstablemodel->item(i, currentcolumn)->setText(summary);
			currentcolumn++;

			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			langle = (int)peak->description.rfind("</a>");
			rangle = (int)peak->description.find('>');
			if ((langle != string::npos) && (rangle != string::npos)) {
				peakstablemodel->item(i, currentcolumn)->setText(peak->description.substr(rangle + 1, langle - rangle - 1).c_str());
				currentcolumn++;

				tmp = (int)peak->description.find('<');
				stmp = peak->description.substr(tmp, rangle - tmp + 1);
				stmp = stmp.substr(stmp.find('\"') + 1);
				stmp = stmp.substr(0, stmp.rfind('\"'));

				peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
				peakstablemodel->item(i, currentcolumn)->setForeground(brush);
				peakstablemodel->item(i, currentcolumn)->setText(stmp.c_str());
				currentcolumn++;
			}
			else {
				peakstablemodel->item(i, currentcolumn)->setText(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2).c_str());
				currentcolumn++;
			}
		}
		else {
			peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
			peakstablemodel->item(i, currentcolumn)->setForeground(brush);
			if (parameters->generateisotopepattern) {
				QString summary;
				if (peak->matched > 0) {
					summary += "<font color=\"red\">";
				}
				summary += peak->isotopeformula.c_str();
				if (peak->matched > 0) {
					summary += "</font>";
				}
				peakstablemodel->item(i, currentcolumn)->setText(summary);
			}
			else {
				peakstablemodel->item(i, currentcolumn)->setText(peak->formula.getFancySummary(peak->charge).c_str());
			}
			currentcolumn++;

			pos = peak->description.find(':');
			if ((pos != string::npos) && (pos + 2 < peak->description.size())) {
				peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
				peakstablemodel->item(i, currentcolumn)->setForeground(brush);
				desc = peak->description.substr(pos + 2);
				if (!reportisomers) {
					stripIsomers(desc);
				}
				peakstablemodel->item(i, currentcolumn)->setText(desc.c_str());
				currentcolumn++;
			}
		}

		progress.setValue(i);
		//if (progress.wasCanceled()) {
		//	break;
		//}
	}

	// unmatched experimental peaks
	int numberofexperimentalpeaks = theoreticalspectrum->getExperimentalSpectrum().size();
	int currentrow = thpeakscount;
	for (int i = 0; i < numberofexperimentalpeaks; i++) {
		peak = &(theoreticalspectrum->getExperimentalSpectrum()[i]);

		if (peak->matched > 0) {
			continue;
		}

		if (parameters->generateisotopepattern) {
			currentcolumn = 3;
		} 
		else {
			currentcolumn = 2;
		}

		peakstablemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		peakstablemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->mzratio)), Qt::DisplayRole);
		currentcolumn++;

		peakstablemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		peakstablemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->relativeintensity)), Qt::DisplayRole);
		currentcolumn++;

		peakstablemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		peakstablemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropDecimalsByteArray(peak->absoluteintensity)), Qt::DisplayRole);
		currentcolumn++;

		progress.setValue(currentrow);
		//if (progress.wasCanceled()) {
		//	break;
		//}

		currentrow++;
	}

	proxymodel->setSourceModel(peakstablemodel);
	peakstable->setModel(proxymodel);
	peakstable->setSortingEnabled(true);

	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		peakstable->resizeColumnToContents(i);
	}

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if (parameters->generateisotopepattern) {
			if (parameters->calculatefdrs) {
				peakstable->setColumnWidth(9, min(400, peakstable->columnWidth(9)));
			}
			else {
				peakstable->setColumnWidth(7, min(400, peakstable->columnWidth(7)));
			}
		}
		else {
			peakstable->setColumnWidth(6, min(400, peakstable->columnWidth(6)));
		}
	}
	else {
		if (parameters->generateisotopepattern) {
			peakstable->setColumnWidth(7, min(400, peakstable->columnWidth(7)));
		}
	}

	progress.setValue(thpeakscount + theoreticalspectrum->getUnmatchedExperimentalPeaksCount());

	tablematches.clear();

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
	QString currentdir = "./";
	if (globalpreferences) {
		currentdir = globalpreferences->exportimagedefaultdir;
	}

	#if OS_TYPE == WIN
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Spectrum..."), currentdir, "PDF Files (*.pdf);; PS Files (*.ps);; PNG Files (*.png);; SVG Files (*.svg)");
	#else
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Spectrum..."), currentdir, "PDF Files (*.pdf);; PNG Files (*.png);; SVG Files (*.svg)");
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
	QString currentdir = "./";
	if (globalpreferences) {
		currentdir = globalpreferences->exportimagedefaultdir;
	}

	#if OS_TYPE == WIN
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Peptide..."), currentdir, "PDF Files (*.pdf);; PS Files (*.ps);; PNG Files (*.png);; SVG Files (*.svg)");
	#else
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Peptide..."), currentdir, "PDF Files (*.pdf);; PNG Files (*.png);; SVG Files (*.svg)");
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
	if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) && (parameters->peptidetype != other)) {
		exportdialog->exec();
	}
	else {
		exportSpectrum();
	}
}


void cSpectrumDetailWidget::exportTableToCSV() {
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
	bool hi = false;
	bool hn = false;
	bool hr = false;
	bool ht = false;

	QString pattern;

	if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {
		
		if ((parameters->peptidetype == cyclic) || (parameters->peptidetype == branchcyclic) || (parameters->peptidetype == cyclicpolyketide)) {
			if (rotationcombobox->currentIndex() > 0) {
				pattern += rotationcombobox->currentText();
				hr = true;
			}
		}
		
		if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
			if (trotationcombobox->currentIndex() > 0) {
				if (parameters->peptidetype == branchcyclic) {
					pattern += "_";
				}
				pattern += QString::number(trotationcombobox->currentIndex());
				ht = true;
			}
			else {
				if (!pattern.isEmpty() && ((ionseriescombobox->currentIndex() > 0) || (neutrallosscombobox->currentIndex() > 0))) {
					if (parameters->peptidetype == branchcyclic) {
						pattern += "_";
					}
					pattern += "[0-9]";
					ht = true;
				}
			}
		}

		if (!pattern.isEmpty() && (parameters->peptidetype != linear) && (parameters->peptidetype != linearpolyketide)) {
			pattern += "_";
		}

		if (ionseriescombobox->currentIndex() > 0) {
			if ((parameters->peptidetype == linear) || (parameters->peptidetype == linearpolyketide)) {
				pattern += " ";
			}
			if (ionseriescombobox->currentText().size() > 2) {
				pattern += ionseriescombobox->currentText().toStdString().substr(0, 2).c_str();
				pattern += "[0-9]+\\";
				pattern += ionseriescombobox->currentText().toStdString().substr(2).c_str();
			}
			else {
				pattern += ionseriescombobox->currentText() + "[0-9]+(?!\\+2H|-2H)";
			}
			hi = true;
		}
		else {
			if (!pattern.isEmpty() && (neutrallosscombobox->currentIndex() > 0)) {
				pattern += "(?:A[0-9]+|B[0-9]+|C[0-9]+|X[0-9]+|Y[0-9]+|Z[0-9]+|LB[0-9]+|LB[0-9]+\\+2H|RB[0-9]+|RB[0-9]+\\+2H|LY[0-9]+-2H|LY[0-9]+|RY-2H[0-9]+|RY[0-9]+)";
				hi = true;
			}
		}

		if (neutrallosscombobox->currentIndex() > 0) {
			if (neutrallosscombobox->currentIndex() == 1) {
				if (pattern.isEmpty()) {
					pattern = "^";
				}
				pattern += "((?!";
				for (auto& it : localneutralosses) {
					pattern += " -";
					pattern += it.c_str();
					pattern += "(?:$|_| )";
					pattern += "|";
				}
				if (localneutralosses.size() > 0) {
					pattern = pattern.left(pattern.size() - 1);
				}
				pattern += ").)*$";
			}
			else {
				pattern += " -";
				pattern += neutrallosscombobox->currentText();
				pattern += "(?:$|_| )";
			}
			hn = true;
		}

	}

	proxymodel->setFlags(hm, hu, hs, hi, hn, hr, ht);
	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterRegExp(pattern);
}


void cSpectrumDetailWidget::rawDataStateChanged(bool state) {
	ifstream peakliststream;
	int fileid;
	int targetid;
	string s;

	double maxmzoverhead = 5.0;
	int maximum = 100;

	QProgressDialog progress("Preparing profile data...", /*"Cancel"*/0, 0, maximum, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);
	progress.setValue(0);

	if (parameters && theoreticalspectrum && parameters->useprofiledata) {

		progress.setValue(10);

		if (parameters->peaklistfileformats[activefileid] == baf) {

			if ((parameters->mode == denovoengine) || (parameters->mode == databasesearch)) {
				fileid = parameters->scannumber;
			}
			else {
				fileid = rowid;
			}
			targetid = rowid - 1;

			if ((targetid >= 0) && (targetid < rawdata->size())) {
				if (state) {

					progress.setValue(33);

					if (globalpreferences) {

						string datafile = parameters->peaklistfilenames[activefileid] + ".profile." + to_string(fileid) + ".csv";

						#if OS_TYPE == WIN
							if (globalpreferences->profilebafprocessingmethod == 1) {
								if (!QFile(datafile.c_str()).exists()) {
									//string s = "External\\windows\\profilespectrumreader.exe \"" + parameters->peaklistfilenames[activefileid] + "\" " + to_string(fileid);
									//system(s.c_str());

									QString command = "External\\windows\\profilespectrumreader.exe";
									QString param1 = parameters->peaklistfilenames[activefileid].c_str();
									QString param2 = to_string(fileid).c_str();

									QStringList params;
									params << param1;
									params << param2;

									QProcess convert;
									convert.start(command, params);
									convert.waitForFinished(-1);
								}
							}
						#endif

						if (QFile(datafile.c_str()).exists()) {
							peakliststream.open(datafile);

							while (peakliststream.good() && !(strstr(s.c_str(), "M/Z"))) {
								getline(peakliststream, s);
							}

							(*rawdata)[targetid].clear();
							(*rawdata)[targetid].loadFromBAFStream(peakliststream);

							peakliststream.close();
						}

					}

					progress.setValue(66);

					(*rawdata)[targetid].sortbyMass();
					(*rawdata)[targetid].normalizeIntenzityByValue(theoreticalspectrum->getExperimentalSpectrum().getMaximumAbsoluteIntensity() * 100.0 / theoreticalspectrum->getExperimentalSpectrum().getMaximumRelativeIntensity());
					(*rawdata)[targetid].cropMinimumMZRatio(parameters->minimummz, parameters->fragmentmasserrortolerance);

					if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {
						(*rawdata)[targetid].cropMaximumMZRatio(charge(uncharge(parameters->precursormass, parameters->precursorcharge) + maxmzoverhead, (parameters->precursorcharge > 0) ? 1 : -1), parameters->precursormasserrortolerance);
					}
					
					progress.setValue(99);
				}
				else {
					(*rawdata)[targetid].clear();
				}
			}

			emit rawDataStateChangedSignal(state);

		}

		if (parameters->peaklistfileformats[activefileid] == dat) {

			targetid = rowid - 1;

			if ((targetid >= 0) && (targetid < rawdata->size())) {
				if (state) {
					string mgfname = parameters->peaklistfilenames[activefileid].substr(0, parameters->peaklistfilenames[activefileid].rfind('/'));
					mgfname = mgfname.substr(0, mgfname.size() - 4);
					mgfname += ".mgf";

					peakliststream.open(mgfname);

					(*rawdata)[targetid].clear();
					(*rawdata)[targetid].loadFromMGFStream(peakliststream, 1);
					while (peakliststream.good() && (theoreticalspectrum->getExperimentalSpectrum().getTitle().compare((*rawdata)[targetid].getTitle()) != 0)) {
						(*rawdata)[targetid].clear();
						(*rawdata)[targetid].loadFromMGFStream(peakliststream, 1);
					}

					if ((theoreticalspectrum->getExperimentalSpectrum().getTitle().compare((*rawdata)[targetid].getTitle()) != 0)) {
						(*rawdata)[targetid].clear();
					}

					peakliststream.close();

					progress.setValue(33);

					(*rawdata)[targetid].sortbyMass();
					(*rawdata)[targetid].normalizeIntenzityByValue(theoreticalspectrum->getExperimentalSpectrum().getMaximumAbsoluteIntensity() * 100.0 / theoreticalspectrum->getExperimentalSpectrum().getMaximumRelativeIntensity());
					(*rawdata)[targetid].cropMinimumMZRatio(parameters->minimummz, parameters->fragmentmasserrortolerance);

					if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {
						(*rawdata)[targetid].cropMaximumMZRatio(charge(uncharge(parameters->precursormass, parameters->precursorcharge) + maxmzoverhead, (parameters->precursorcharge > 0) ? 1 : -1), parameters->precursormasserrortolerance);
					}

					progress.setValue(66);
				}
				else {
					(*rawdata)[targetid].clear();
				}
			}

			emit rawDataStateChangedSignal(state);

		}

		if (parameters->peaklistfileformats[activefileid] == raw) {

			if ((parameters->mode == denovoengine) || (parameters->mode == databasesearch)) {
				fileid = parameters->scannumber - 1;
			}
			else {
				fileid = rowid - 1;
			}
			targetid = rowid - 1;

			if ((targetid >= 0) && (targetid < rawdata->size())) {
				if (state) {
					cMzML mzml;
					vector<cPeaksList> peaklists;

					bool terminatecomputation = false;

					string basename = parameters->peaklistfilenames[activefileid].substr(0, (int)parameters->peaklistfilenames[activefileid].size() - 3);
					string mzmlname = basename + "mzML";
					
					int resultcode = mzml.parse(mzmlname, peaklists, fileid, parameters->mode, 0 /* ok */, terminatecomputation /* ok */);
					if ((resultcode == 0) && (peaklists.size() == 1)) {

						progress.setValue(33);

						string mgfname = basename + "profile." + to_string(fileid) + ".mgf";

						ifstream mgfifstream;
						mgfifstream.open(mgfname);
						(*rawdata)[targetid].clear();
						(*rawdata)[targetid].loadFromMGFStream(mgfifstream, 1);
						mgfifstream.close();

						QFile::remove(mgfname.c_str());

						(*rawdata)[targetid].sortbyMass();
						(*rawdata)[targetid].normalizeIntenzityByValue(theoreticalspectrum->getExperimentalSpectrum().getMaximumAbsoluteIntensity() * 100.0 / theoreticalspectrum->getExperimentalSpectrum().getMaximumRelativeIntensity());
						(*rawdata)[targetid].cropMinimumMZRatio(parameters->minimummz, parameters->fragmentmasserrortolerance);

						progress.setValue(66);

					}
				}
				else {
					(*rawdata)[targetid].clear();
				}
			}

			emit rawDataStateChangedSignal(state);

		}

		if (parameters->peaklistfileformats[activefileid] == mzML) {

			if ((parameters->mode == denovoengine) || (parameters->mode == databasesearch)) {
				fileid = parameters->scannumber - 1;
			}
			else {
				fileid = rowid - 1;
			}
			targetid = rowid - 1;

			if ((targetid >= 0) && (targetid < rawdata->size())) {
				if (state) {
					cMzML mzml;
					vector<cPeaksList> peaklists;

					bool terminatecomputation = false;

					string basename = parameters->peaklistfilenames[activefileid].substr(0, (int)parameters->peaklistfilenames[activefileid].size() - 4);
					string mzmlname = basename + "mzML";

					int resultcode = mzml.parse(mzmlname, peaklists, fileid, parameters->mode, 0 /* ok */, terminatecomputation /* ok */);
					if ((resultcode == 0) && (peaklists.size() == 1)) {

						progress.setValue(33);

						string fname = basename + "profile." + to_string(fileid);

						string s;
						#if OS_TYPE == UNX
							s = installdir.toStdString() + "External/linux/correctprofile.sh " + fname + "," + to_string(parameters->fwhm);
						#else
						#if OS_TYPE == OSX
							s = installdir.toStdString() + "External/macosx/correctprofile.sh " + fname + "," + to_string(parameters->fwhm);
						#else		
							s = "External\\windows\\correctprofile.bat \"" + fname + "\" " + to_string(parameters->fwhm);
						#endif
						#endif

						progress.setValue(66);

						if (system(s.c_str()) == 0) {
							mzmlname = fname + ".mzML";
							string mgfname = fname + ".0000000000.mgf";

							cMzML correctedmzml;
							peaklists.clear();

							resultcode = correctedmzml.parse(mzmlname, peaklists, -1, singlecomparison /* ok */, 0 /* ok */, terminatecomputation /* ok */);
							if ((resultcode == 0) && (peaklists.size() == 1)) {
								QFile::remove(mzmlname.c_str());

								ifstream mgfifstream;
								mgfifstream.open(mgfname);
								(*rawdata)[targetid].clear();
								(*rawdata)[targetid].loadFromMGFStream(mgfifstream, 1);
								mgfifstream.close();

								QFile::remove(mgfname.c_str());

								(*rawdata)[targetid].sortbyMass();
								(*rawdata)[targetid].normalizeIntenzityByValue(theoreticalspectrum->getExperimentalSpectrum().getMaximumAbsoluteIntensity() * 100.0 / theoreticalspectrum->getExperimentalSpectrum().getMaximumRelativeIntensity());
								(*rawdata)[targetid].cropMinimumMZRatio(parameters->minimummz, parameters->fragmentmasserrortolerance);
							}
						}

					}
				}
				else {
					(*rawdata)[targetid].clear();
				}
			}

			emit rawDataStateChangedSignal(state);

		}

		if ((parameters->peaklistfileformats[activefileid] == imzML) && ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && (imzmlprofilemetadata->size() > 0)) {
			targetid = rowid - 1;

			if ((targetid >= 0) && (targetid < rawdata->size())) {
				if (state) {
					string ibdfilename = parameters->profiledatafilename.substr(0, (int)parameters->profiledatafilename.size() - 5);
					ibdfilename += "ibd";
					peakliststream.open(ibdfilename, std::ifstream::binary);

					(*rawdata)[targetid].clear();
					if ((targetid >= 0) && (targetid < imzmlprofilemetadata->size())) {
						(*rawdata)[targetid].loadFromIBDStream((*imzmlprofilemetadata)[targetid], peakliststream, profilemz64precision, profileintensity64precision);
					}

					peakliststream.close();

					progress.setValue(25);

					ofstream mgfofstream;
					ifstream mgfifstream;
					string fname;

					char tempstring[1024];
					string peaksstring;
					peaksstring.reserve(25000000);

					fname = parameters->profiledatafilename.substr(0, (int)parameters->profiledatafilename.size() - 5);
					fname += "scan." + to_string(rowid);
					mgfofstream.open(fname + ".mgf");

					mgfofstream << "BEGIN IONS" << endl;
					mgfofstream << "TITLE=" << endl;
					mgfofstream << "SCANS=" << to_string(rowid) << endl;
					mgfofstream << "PEPMASS=1" << endl;
					mgfofstream << "RTINSECONDS=1" << endl;
					mgfofstream << "CHARGE=1+" << endl << endl;

					for (int i = 0; i < (*rawdata)[targetid].size(); i++) {
						sprintf_s(tempstring, "%f %f\n\0", (*rawdata)[targetid][i].mzratio, (*rawdata)[targetid][i].absoluteintensity);
						peaksstring.append(tempstring);
					}
					mgfofstream << peaksstring;
					mgfofstream << "END IONS" << endl << endl;
					mgfofstream.close();

					progress.setValue(50);

					string s;
					#if OS_TYPE == UNX
						s = installdir.toStdString() + "External/linux/correctprofile.sh " + fname + "," + to_string(parameters->fwhm);
					#else
						#if OS_TYPE == OSX
							s = installdir.toStdString() + "External/macosx/correctprofile.sh " + fname + "," + to_string(parameters->fwhm);
						#else		
							s = "External\\windows\\correctprofile.bat \"" + fname + "\" " + to_string(parameters->fwhm);
						#endif
					#endif

					(*rawdata)[targetid].clear();

					progress.setValue(75);

					if (system(s.c_str()) == 0) {
						cMzML mzml;
						vector<cPeaksList> peaklists;
						bool terminatecomputation = false;

						string mzmlname = fname + ".mzML";
						string mgfname = fname + ".0000000000.mgf";

						int resultcode = mzml.parse(mzmlname, peaklists, -1, singlecomparison /* ok */, 0 /* ok */, terminatecomputation /* ok */);
						if ((resultcode == 0) && (peaklists.size() == 1)) {
							QFile::remove(mzmlname.c_str());

							mgfifstream.open(mgfname);
							(*rawdata)[targetid].loadFromMGFStream(mgfifstream, 1);
							mgfifstream.close();

							QFile::remove(mgfname.c_str());

							(*rawdata)[targetid].sortbyMass();
							(*rawdata)[targetid].normalizeIntenzityByValue(theoreticalspectrum->getExperimentalSpectrum().getMaximumAbsoluteIntensity() * 100.0 / theoreticalspectrum->getExperimentalSpectrum().getMaximumRelativeIntensity());
							(*rawdata)[targetid].cropMinimumMZRatio(parameters->minimummz, parameters->fragmentmasserrortolerance);
						}
					}
				}
				else {
					(*rawdata)[targetid].clear();
				}
			}

			emit rawDataStateChangedSignal(state);

		}
			
	}

	progress.setValue(maximum);
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


void cSpectrumDetailWidget::hidePeakLabels(bool hide) {
	if (!preparedToShow) {
		return;
	}

	spectrumscene->hidePeakLabels(hide);

	if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {
		switch (parameters->peptidetype) {
			case linear:
				linearwidget->hidePeakLabels(hide);
				break;
			case cyclic:
				cyclicwidget->hidePeakLabels(hide);
				break;
			case branched:
				branchedwidget->hidePeakLabels(hide);
				break;
			case branchcyclic:
				branchcyclicwidget->hidePeakLabels(hide);
				break;
			case linearpolyketide:
				linearwidget->hidePeakLabels(hide);
				break;
			case cyclicpolyketide:
				cyclicwidget->hidePeakLabels(hide);
				break;
			case other:
				break;
			default:
				break;
		}
	}
}


void cSpectrumDetailWidget::filterTableAfterIonSeriesChanged(int index) {
	filterPeaksTable();
}


void cSpectrumDetailWidget::filterTableAfterNeutralLossChanged(int index) {
	filterPeaksTable();
}


void cSpectrumDetailWidget::filterTableAfterRotationChanged(int index) {
	filterPeaksTable();
}


void cSpectrumDetailWidget::filterTableAfterTRotationChanged(int index) {
	filterPeaksTable();
}


void cSpectrumDetailWidget::showIsomersStateChanged() {

	if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {

		reportisomers = actionShowIsomers->isChecked();

		// update graphical widget
		switch (parameters->peptidetype) {
			case linear:
				linearwidget->setReportIsomers(reportisomers);
				break;
			case cyclic:
				cyclicwidget->setReportIsomers(reportisomers);
				break;
			case branched:
				branchedwidget->setReportIsomers(reportisomers);
				break;
			case branchcyclic:
				branchcyclicwidget->setReportIsomers(reportisomers);
				break;
			case linearpolyketide:
				linearwidget->setReportIsomers(reportisomers);
				break;
			case cyclicpolyketide:
				cyclicwidget->setReportIsomers(reportisomers);
				break;
			case other:
				break;
			default:
				break;
		}


		// update table of peaks
		cPeak* peak;
		string desc;
		size_t pos;

		cPeaksList*  thpeaks = theoreticalspectrum->getTheoreticalPeaks();
		int thpeakscount = theoreticalspectrum->getTheoreticalPeaks()->size();
		int currentcolumn = peakstablemodel->columnCount() - 1;

		for (int i = 0; i < thpeakscount; i++) {
			peak = &((*thpeaks)[i]);

			if (peak->descriptionid != -1) {
				peak->description = parameters->peakidtodesc[peak->descriptionid];
			}

			if (peak->isotopeformulaid != -1) {
				peak->isotopeformula = parameters->isotopeformulaidtodesc[peak->isotopeformulaid];
			}

			pos = peak->description.find(':');
			if ((pos != string::npos) && (pos + 2 < peak->description.size()) && peakstablemodel->item(i, currentcolumn)) {
				desc = peak->description.substr(pos + 2);
				if (!reportisomers) {
					stripIsomers(desc);
				}
				peakstablemodel->item(i, currentcolumn)->setText(desc.c_str());
			}
		}


		// update text info
		string textinfo = getDetailsAsHTMLString() + theoreticalspectrum->getCoverageBySeries();
		if (!reportisomers) {
			stripIsomers(textinfo);
		}
		textbrowser->setHtml(textinfo.c_str());


		// reset search tool
		findAll("", 0, false);

	}

}


void cSpectrumDetailWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		if (parameters->mode == dereplication) {
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/dereplication.html").absoluteFilePath()));
		}
		else if (parameters->mode == compoundsearch) {
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/compoundsearch.html").absoluteFilePath()));
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
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/metabolitedetail.html").absoluteFilePath()));
					break;
				default:
					break;
			}
		}
	#else
		if (parameters->mode == dereplication) {
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/dereplication.html").absoluteFilePath()));
		}
		else if (parameters->mode == compoundsearch) {
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/compoundsearch.html").absoluteFilePath()));
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
					QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/metabolitedetail.html").absoluteFilePath()));
					break;
				default:
					break;
			}
		}
	#endif
}


void cSpectrumDetailWidget::closeWindow() {
	hide();
}


void cSpectrumDetailWidget::tableDoubleClicked(const QModelIndex& index) {
	QStandardItem* item;
	double value;

	int theoreticalmzcolumn = 1;

	int experimentalmzcolumn = 2;
	if (parameters->generateisotopepattern) {
		experimentalmzcolumn = 3;
	}
	
	item = peakstablemodel->item(proxymodel->mapToSource(index).row(), experimentalmzcolumn);
	if (item) {
		value = item->data(Qt::DisplayRole).toDouble();
		if (value > 0) {
			emit emitMZInterval(max(0.0, value - 5.0), value + 5.0);
			return;
		}
	}

	item = peakstablemodel->item(proxymodel->mapToSource(index).row(), theoreticalmzcolumn);
	if (item) {
		value = item->data(Qt::DisplayRole).toDouble();
		if (value > 0) {
			emit emitMZInterval(max(0.0, value - 5.0), value + 5.0);
		}
	}
}

