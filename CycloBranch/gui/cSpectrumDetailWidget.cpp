#include "gui/cSpectrumDetailWidget.h"

#include <QTextEdit>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QCheckBox>
#include <QPushButton>
#include <QSplitter>
#include <QComboBox>
#include <QDoubleSpinBox>


cSpectrumDetailWidget::cSpectrumDetailWidget() {
	rotation = 0;
	parameters = 0;

	preparedToShow = false;
	theoreticalspectrum = new cTheoreticalSpectrum();
}


cSpectrumDetailWidget::cSpectrumDetailWidget(const cSpectrumDetailWidget& sd) {
	rotation = 0;
	parameters = sd.parameters;

	preparedToShow = false;
	theoreticalspectrum = new cTheoreticalSpectrum();

	if (parameters && sd.theoreticalspectrum) {
		initialize(parameters, *sd.theoreticalspectrum);
	}

	if (parameters && sd.preparedToShow) {
		prepareToShow(parameters->peptidetype);
	}

	setWindowTitle(sd.windowTitle());
}


void cSpectrumDetailWidget::initialize(cParameters* parameters, cTheoreticalSpectrum& theoreticalspectrum) {
	this->parameters = parameters;
	*this->theoreticalspectrum = theoreticalspectrum;
}


cTheoreticalSpectrum& cSpectrumDetailWidget::getTheoreticalSpectrum() {
	return *theoreticalspectrum;
}


string cSpectrumDetailWidget::getDetailsAsHTMLString() {
	string s = "";

	if (theoreticalspectrum) {
		s += "Acronym Peptide Name:<br/>";
		s += theoreticalspectrum->getAcronymPeptideNameWithHTMLReferences();
		s += "<br/><br/>";
		s += "Full Peptide Name:<br/>";
		s += theoreticalspectrum->getRealPeptideName() + "<br/>";

		if ((int)theoreticalspectrum->getPath().size() > 0) {
			s += "<br/>";
			s += "Path in the De Novo Graph:<br/>";
			s += theoreticalspectrum->getPath();
		}
			
		s += "<br/>";

		s += "Unmatched Measured Peaks:<br/>" + theoreticalspectrum->getUnmatchedPeaks() + "<br/><br/>";
		s += "Theoretical Peaks:<br/>" + theoreticalspectrum->getTheoreticalPeaks()->print(true);
	}

	return s;
}


cSpectrumDetailWidget::~cSpectrumDetailWidget() {

	delete theoreticalspectrum;

	if (preparedToShow) {
	
		delete graphicalspectrum;
		delete graphicalspectrumscroll;

		delete textedit;
		delete textbrowser;

		delete zoomin;
		delete zoomout;
		delete normalsize;
		delete zoom;

		delete minmz;
		delete maxmz;
		delete setmzinterval;
		delete resetmzinterval;
		delete mzinterval;

		delete hideunmatched;
		delete hidematched;

		if (rotation) {
			delete rotation;
		}

		delete formlayout;
		delete formwidget;

		if (parameters) {
			switch (parameters->peptidetype)
			{
			case linear:
				delete linearwidget;
				break;
			case cyclic:
				delete cyclicwidget;
				break;
			case branched:
				delete branchedwidget;
				break;
			case lasso:
				break;
			case linearpolysaccharide:
				break;
			default:
				break;
			}
		}
		
		delete vsplitter1;
		delete vsplitter2;

		delete hsplitter;

		delete mainbox;
	}
}


void cSpectrumDetailWidget::closeEvent(QCloseEvent *event) {
	hide();
}


void cSpectrumDetailWidget::prepareToShow(peptideType peptidetype) {

	if (!preparedToShow) {

		hsplitter = new QSplitter();
		hsplitter->setOrientation(Qt::Horizontal);

		vsplitter1 = new QSplitter();
		vsplitter1->setOrientation(Qt::Vertical);

		vsplitter2 = new QSplitter();
		vsplitter2->setOrientation(Qt::Vertical);

		switch (peptidetype)
		{
		case linear:
			linearwidget = new cLinearWidget();
			break;
		case cyclic:
			cyclicwidget = new cCyclicWidget();
			break;
		case branched:
			branchedwidget = new cBranchedWidget();
			break;
		case lasso:
			break;
		case linearpolysaccharide:
			break;
		default:
			break;
		}

		graphicalspectrumscroll = new QScrollArea();
		graphicalspectrum = new cGraphicalSpectrumWidget(graphicalspectrumscroll);
		graphicalspectrumscroll->setWidget(graphicalspectrum);
		//graphicalspectrumscroll->setWidgetResizable(true);

		textedit = new QTextEdit();
		textedit->setReadOnly(true);
		textedit->setFont(QFont("Courier", 9));
		textedit->setLineWrapMode(QTextEdit::NoWrap);

		textbrowser = new QTextBrowser();
		textbrowser->setReadOnly(true);
		textbrowser->setFont(QFont("Courier", 9));
		textbrowser->setLineWrapMode(QTextEdit::NoWrap);
		textbrowser->setAcceptRichText(true);
		textbrowser->setOpenExternalLinks(true);

		zoomin = new QPushButton("In");
		zoomin->setMaximumWidth(50);
		connect(zoomin, SIGNAL(released()), graphicalspectrum, SLOT(zoomIn()));

		zoomout = new QPushButton("Out");
		zoomout->setMaximumWidth(50);
		connect(zoomout, SIGNAL(released()), graphicalspectrum, SLOT(zoomOut()));

		normalsize = new QPushButton("Reset");
		normalsize->setMaximumWidth(50);
		connect(normalsize, SIGNAL(released()), graphicalspectrum, SLOT(normalSize()));

		zoom = new QHBoxLayout();
		zoom->addWidget(zoomin);
		zoom->addWidget(zoomout);
		zoom->addWidget(normalsize);

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
		connect(this, SIGNAL(emitMZInterval(double, double)), graphicalspectrum, SLOT(setMZInterval(double, double)));

		resetmzinterval = new QPushButton("Reset");
		resetmzinterval->setMaximumWidth(50);
		connect(resetmzinterval, SIGNAL(released()), graphicalspectrum, SLOT(resetMZInterval()));

		mzinterval = new QHBoxLayout();
		mzinterval->addWidget(minmz);
		mzinterval->addWidget(maxmz);
		mzinterval->addWidget(setmzinterval);
		mzinterval->addWidget(resetmzinterval);
		connect(graphicalspectrum, SIGNAL(updateMZInterval(double, double)), this, SLOT(updateMZInterval(double, double)));

		hideunmatched = new QCheckBox();
		connect(hideunmatched, SIGNAL(stateChanged(int)), graphicalspectrum, SLOT(hideUnmatchedPeaks(int)));

		hidematched = new QCheckBox();
		connect(hidematched, SIGNAL(stateChanged(int)), graphicalspectrum, SLOT(hideMatchedPeaks(int)));

		formlayout = new QFormLayout();
		formlayout->addRow(tr("Zoom: "), zoom);
		formlayout->addRow(tr("View m/z (from - to): "), mzinterval);
		formlayout->addRow(tr("Hide matched peaks: "), hidematched);
		formlayout->addRow(tr("Hide unmatched peaks: "), hideunmatched);

		// cyclic
		if (parameters && theoreticalspectrum && (parameters->peptidetype == cyclic)) {
			int r = (int)theoreticalspectrum->getAcronyms().size();
			int hint = (int)theoreticalspectrum->getVisualCoverage().size()/(2*r);

			rotation = new QComboBox();
			rotation->addItem(tr("all"));

			string s;
			for (int i = 0; i < 2*r; i++) {
				s = theoreticalspectrum->getVisualCoverage()[i*hint].name.substr(0, theoreticalspectrum->getVisualCoverage()[i*hint].name.rfind('_'));
				/*
				if (i < r) {
					s += " >>>";
				}
				else {
					s += " <<<";
				}
				*/
				rotation->addItem(tr(s.c_str()));
			}

			connect(rotation, SIGNAL(currentIndexChanged(int)), graphicalspectrum, SLOT(rotationChanged(int)));
			connect(rotation, SIGNAL(currentIndexChanged(int)), cyclicwidget, SLOT(rotationChanged(int)));
			formlayout->addRow(tr("Show matched series: "), rotation);
		}

		formwidget = new QWidget();
		formwidget->setLayout(formlayout);

		vsplitter1->addWidget(graphicalspectrumscroll);
		vsplitter1->addWidget(textbrowser);
		vsplitter1->setStretchFactor(0, 7);
		vsplitter1->setStretchFactor(1, 3);

		switch (peptidetype)
		{
		case linear:
			vsplitter2->addWidget(formwidget);
			vsplitter2->addWidget(linearwidget);
			vsplitter2->addWidget(textedit);
			vsplitter2->setStretchFactor(0, 2);
			vsplitter2->setStretchFactor(1, 3);
			vsplitter2->setStretchFactor(2, 5);
			break;
		case cyclic:
			vsplitter2->addWidget(formwidget);
			vsplitter2->addWidget(cyclicwidget);
			vsplitter2->addWidget(textedit);
			vsplitter2->setStretchFactor(0, 2);
			vsplitter2->setStretchFactor(1, 3);
			vsplitter2->setStretchFactor(2, 5);
			break;
		case branched:
			vsplitter2->addWidget(formwidget);
			vsplitter2->addWidget(branchedwidget);
			vsplitter2->addWidget(textedit);
			vsplitter2->setStretchFactor(0, 2);
			vsplitter2->setStretchFactor(1, 3);
			vsplitter2->setStretchFactor(2, 5);
			break;
		case lasso:
		case linearpolysaccharide:
			vsplitter2->addWidget(formwidget);
			vsplitter2->addWidget(textedit);
			vsplitter2->setStretchFactor(0, 2);
			vsplitter2->setStretchFactor(1, 8);
			break;
		default:
			break;
		}

		hsplitter->addWidget(vsplitter1);
		hsplitter->addWidget(vsplitter2);
		hsplitter->setStretchFactor(0, 6);
		hsplitter->setStretchFactor(1, 4);
		
		mainbox = new QHBoxLayout();
		mainbox->addWidget(hsplitter);
		setLayout(mainbox);

		resize(1280, 700);

		if (theoreticalspectrum) {

			switch (peptidetype)
			{
			case linear:
				linearwidget->initialize(parameters, theoreticalspectrum);
				break;
			case cyclic:
				cyclicwidget->initialize(parameters, theoreticalspectrum);
				break;
			case branched:
				branchedwidget->initialize(parameters, theoreticalspectrum);
				break;
			case lasso:
				break;
			case linearpolysaccharide:
				break;
			default:
				break;
			}
			
			graphicalspectrum->setTheoreticalSpectrum(theoreticalspectrum);

			textedit->setHtml(theoreticalspectrum->getCoverageBySeries().c_str());
			textbrowser->setHtml(getDetailsAsHTMLString().c_str());
		}

		preparedToShow = true;
	}

}


void cSpectrumDetailWidget::updateMZInterval(double minmz, double maxmz) {
	this->minmz->setValue(minmz);
	this->maxmz->setValue(maxmz);
}


void cSpectrumDetailWidget::setMZInterval() {
	emit emitMZInterval(minmz->value(), maxmz->value());
}

