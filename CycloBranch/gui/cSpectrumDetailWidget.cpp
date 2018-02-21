#include "gui/cSpectrumDetailWidget.h"

#include <QTextEdit>
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
		initialize(parameters, *sd.theoreticalspectrum);
	}

	if (parameters && sd.preparedToShow) {
		prepareToShow(parameters->peptidetype);
	}

	setWindowTitle(sd.windowTitle());

	return *this;
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

		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {

			s += "Acronym Peptide Name:<br/>";
			s += theoreticalspectrum->getCandidate().getAcronymPeptideNameWithHTMLReferences();
			s += "<br/><br/>";
			s += "Full Peptide Name:<br/>";
			s += theoreticalspectrum->getCandidate().getRealPeptideName() + "<br/>";

			if ((int)theoreticalspectrum->getCandidate().getPathAsString().size() > 0) {
				s += "<br/>";
				s += "Path in the De Novo Graph:<br/>";
				s += theoreticalspectrum->getCandidate().getPathAsString();
			}
			
			s += "<br/>";

		}

		s += "Unmatched Measured Peaks:<br/>" + theoreticalspectrum->getUnmatchedPeaks() + "<br/><br/>";
		s += "Theoretical Peaks:<br/>" + theoreticalspectrum->getTheoreticalPeaks()->print(true);

	}

	return s;
}


cSpectrumDetailWidget::~cSpectrumDetailWidget() {

	delete theoreticalspectrum;

	if (preparedToShow) {
	
		delete spectrumscene;

		delete textedit;
		delete textbrowser;

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
				delete linearwidget;
				break;
			case cyclic:
				delete cyclicwidget;
				break;
			case branched:
				delete branchedwidget;
				break;
			case lasso:
				delete lassowidget;
				break;
			case linearpolysaccharide:
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
}


void cSpectrumDetailWidget::prepareToShow(peptideType peptidetype) {

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
				linearwidget = new cLinearWidget();
				break;
			case cyclic:
				cyclicwidget = new cCyclicWidget();
				break;
			case branched:
				branchedwidget = new cBranchedWidget();
				break;
			case lasso:
				lassowidget = new cLassoWidget();
				break;
			case linearpolysaccharide:
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

		actionExportSpectrum = new QAction(QIcon(":/images/icons/66.png"), tr("Export Image"), this);
		actionExportSpectrum->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
		actionExportSpectrum->setToolTip("Export Image (Ctrl + E)");
		toolbarExport->addAction(actionExportSpectrum);
		connect(actionExportSpectrum, SIGNAL(triggered()), this, SLOT(openExportDialog()));


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
		connect(actionHideMatched, SIGNAL(toggled(bool)), spectrumscene, SLOT(hideMatchedPeaks(bool)));

		actionHideUnmatched = new QAction(QIcon(":/images/icons/81.png"), tr("Hide Unmatched Peaks"), this);
		actionHideUnmatched->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
		actionHideUnmatched->setToolTip("Hide Unmatched Peaks (Ctrl + U)");
		actionHideUnmatched->setCheckable(true);
		toolbarHide->addAction(actionHideUnmatched);
		connect(actionHideUnmatched, SIGNAL(toggled(bool)), spectrumscene, SLOT(hideUnmatchedPeaks(bool)));

		actionHideScrambled = new QAction(QIcon(":/images/icons/80.png"), tr("Hide Scrambled Peaks"), this);
		actionHideScrambled->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
		actionHideScrambled->setToolTip("Hide Scrambled Peaks (Ctrl + S)");
		actionHideScrambled->setCheckable(true);
		actionHideScrambled->setEnabled(false);
		toolbarHide->addAction(actionHideScrambled);


		if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {

			if ((parameters->peptidetype == cyclic) && parameters->enablescrambling) {
				actionHideScrambled->setEnabled(true);
				connect(actionHideScrambled, SIGNAL(toggled(bool)), spectrumscene, SLOT(hideScrambledPeaks(bool)));
			}

		}

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
			if (theoreticalspectrum && (parameters->peptidetype == cyclic)) {
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
			if (parameters && theoreticalspectrum && (parameters->peptidetype == lasso)) {
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
				connect(rotation, SIGNAL(currentIndexChanged(int)), lassowidget, SLOT(rotationChanged(int)));

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
				connect(trotation, SIGNAL(currentIndexChanged(int)), lassowidget, SLOT(trotationChanged(int)));
				
				toolbarTrotation = addToolBar(tr("Linearized sequence"));
				toolbarTrotation->addWidget(widgettrotation);
				toolbarTrotation->setMovable(false);
				toolbarTrotation->setFloatable(false);
			}

		}

		hsplitter1->addWidget(spectrumscene);
		hsplitter2->addWidget(textbrowser);

		QList<int> sizes;
		sizes.push_back(100);
		sizes.push_back(100);

		if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {

			switch (peptidetype)
			{
			case linear:
				hsplitter1->addWidget(linearwidget);
				break;
			case cyclic:
				hsplitter1->addWidget(cyclicwidget);
				break;
			case branched:
				hsplitter1->addWidget(branchedwidget);
				break;
			case lasso:
				hsplitter1->addWidget(lassowidget);
				break;
			case linearpolysaccharide:
				break;
			case other:
				break;
			default:
				break;
			}

			hsplitter2->addWidget(textedit);

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

		resize(1280, 700);

		if (parameters && theoreticalspectrum) {

			if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {

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
					lassowidget->initialize(parameters, theoreticalspectrum);
					break;
				case linearpolysaccharide:
					break;
				case other:
					break;
				default:
					break;
				}

				textedit->setHtml(theoreticalspectrum->getCoverageBySeries().c_str());
			
			}

			spectrumscene->initialize(parameters, theoreticalspectrum);
			textbrowser->setHtml(getDetailsAsHTMLString().c_str());

		}

		preparedToShow = true;
	}

}


void cSpectrumDetailWidget::findAll(const QString& str, QTextDocument::FindFlags opt) {

	currentfinditem = 0;

	// textbrowser
	QList<QTextEdit::ExtraSelection> extraSelections;
	textbrowser->moveCursor(QTextCursor::Start);
	QColor color = QColor(Qt::yellow);
 
	while (textbrowser->find(str, opt)) {
		QTextEdit::ExtraSelection extra;
		extra.format.setBackground(color);        
 
		extra.cursor = textbrowser->textCursor();
		extraSelections.append(extra);
	}
 
	textbrowser->setExtraSelections(extraSelections);

	
	// textedit
	extraSelections.clear();
	textedit->moveCursor(QTextCursor::Start);

	while (textedit->find(str, opt)) {
		QTextEdit::ExtraSelection extra;
		extra.format.setBackground(color);        
 
		extra.cursor = textedit->textCursor();
		extraSelections.append(extra);
	}
 
	textedit->setExtraSelections(extraSelections);


	if (textbrowser->extraSelections().size() + textedit->extraSelections().size() == 0) {
		actionPrevious->setDisabled(true);
		actionNext->setDisabled(true);

		QMessageBox msgBox;
		QString errstr = "No results were found.";
		msgBox.setWindowTitle("Find Text");
		msgBox.setText(errstr);
		msgBox.exec();
	}
	else {
		actionPrevious->setDisabled(false);
		actionNext->setDisabled(false);
	}


	// order changed because of setFocus()
	if (textedit->extraSelections().size() > 0) {
		textedit->setTextCursor(textedit->extraSelections().at(0).cursor);
		textedit->setFocus();
	}


	if (textbrowser->extraSelections().size() > 0) {
		textbrowser->setTextCursor(textbrowser->extraSelections().at(0).cursor);
		textbrowser->setFocus();
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

		switch ((peptideType)parameters->peptidetype) {
			case linear:
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
			case lasso:
				rx = ".+\\.pdf$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					lassowidget->exportToPDF(filename, false);
					selected = true;
				}

				#if OS_TYPE == WIN
					rx = ".+\\.ps$";
					if (!selected && (regex_search(filename.toStdString(), rx))) {
						lassowidget->exportToPDF(filename, true);
						selected = true;
					}
				#endif
				
				rx = ".+\\.png$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					lassowidget->exportToPNG(filename);
					selected = true;
				}

				rx = ".+\\.svg$";
				if (!selected && (regex_search(filename.toStdString(), rx))) {
					lassowidget->exportToSVG(filename);
					selected = true;
				}
				break;
			case linearpolysaccharide:
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


void cSpectrumDetailWidget::openExportDialog() {
	if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {
		exportdialog->exec();
	}
	else {
		exportSpectrum();
	}
}


void cSpectrumDetailWidget::movePrevious() {
	int count = textbrowser->extraSelections().size() + textedit->extraSelections().size();
	currentfinditem = (currentfinditem + count - 1)%count;

	if (currentfinditem < textbrowser->extraSelections().size()) {
		textbrowser->setFocus();
		textbrowser->setTextCursor(textbrowser->extraSelections().at(currentfinditem).cursor);
	}
	else {
		textedit->setFocus();
		textedit->setTextCursor(textedit->extraSelections().at(currentfinditem - textbrowser->extraSelections().size()).cursor);
	}
}


void cSpectrumDetailWidget::moveNext() {
	int count = textbrowser->extraSelections().size() + textedit->extraSelections().size();
	currentfinditem = (currentfinditem + 1)%count;

	if (currentfinditem < textbrowser->extraSelections().size()) {
		textbrowser->setFocus();
		textbrowser->setTextCursor(textbrowser->extraSelections().at(currentfinditem).cursor);
	}
	else {
		textedit->setFocus();
		textedit->setTextCursor(textedit->extraSelections().at(currentfinditem - textbrowser->extraSelections().size()).cursor);
	}
}

