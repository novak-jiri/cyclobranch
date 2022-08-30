#include "gui/cChromatogramWindow.h"
#include "gui/cEventFilter.h"


cChromatogramWindow::cChromatogramWindow(cGlobalPreferences* globalpreferences, cTheoreticalSpectrumList& listoftheoreticalspectra, QWidget* parent) {
	this->globalpreferences = globalpreferences;
	this->listoftheoreticalspectra = &listoftheoreticalspectra;
	this->parent = parent;

	setWindowTitle("Chromatogram");
	setWindowIcon(QIcon(":/images/icons/chromatography.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	chromatogramwindowwidget = new cChromatogramWindowWidget(listoftheoreticalspectra, this);
	connect(chromatogramwindowwidget, SIGNAL(chromatogramWidgetDoubleClicked(int)), this, SLOT(chromatogramDoubleClickedSlot(int)));


	toolbarFile = addToolBar(tr("File"));
			
	actionExportImage = new QAction(QIcon(":/images/icons/66.png"), tr("&Export Image"), this);
	actionExportImage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	actionExportImage->setToolTip("Export Image (Ctrl + E)");
	toolbarFile->addAction(actionExportImage);
	connect(actionExportImage, SIGNAL(triggered()), this, SLOT(exportImage()));

	actionCloseWindow = new QAction(QIcon(":/images/icons/33.png"), tr("&Close"), this);
	actionCloseWindow->setShortcut(QKeySequence(Qt::Key_Escape));
	actionCloseWindow->setToolTip("Close (Esc)");
	toolbarFile->addAction(actionCloseWindow);
	connect(actionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));


	toolbarView = addToolBar(tr("View"));
		
	actionZoomIn = new QAction(QIcon(":/images/icons/83.png"), tr("Zoom &In"), this);
	actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
	actionZoomIn->setToolTip("Zoom In (Ctrl +)");
	toolbarView->addAction(actionZoomIn);
	connect(actionZoomIn, SIGNAL(triggered()), chromatogramwindowwidget, SLOT(zoomIn()));

	actionZoomOut = new QAction(QIcon(":/images/icons/82.png"), tr("Zoom &Out"), this);
	actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
	actionZoomOut->setToolTip("Zoom Out (Ctrl -)");
	toolbarView->addAction(actionZoomOut);
	connect(actionZoomOut, SIGNAL(triggered()), chromatogramwindowwidget, SLOT(zoomOut()));

	actionZoomReset = new QAction(QIcon(":/images/icons/84.png"), tr("&Reset Zoom"), this);
	actionZoomReset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	actionZoomReset->setToolTip("Zoom Reset (Ctrl + R)");
	toolbarView->addAction(actionZoomReset);
	connect(actionZoomReset, SIGNAL(triggered()), chromatogramwindowwidget, SLOT(normalSize()));

	toolbarView->addSeparator();

	actionMouseSelection = new QAction(QIcon(":/images/icons/64.png"), tr("Mouse Selection Tool"), this);
	actionMouseSelection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
	actionMouseSelection->setToolTip("Mouse Selection Tool (Ctrl + T)");
	actionMouseSelection->setCheckable(true);
	actionMouseSelection->setChecked(true);
	actionMouseSelection->setEnabled(true);
	toolbarView->addAction(actionMouseSelection);
	connect(actionMouseSelection, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(enableMouseSelectionTool(bool)));

	actionRetentionTime = new QAction(QIcon(":/images/icons/12.png"), tr("Retention Ti&me"), this);
	actionRetentionTime->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
	actionRetentionTime->setToolTip("Retention Time (Ctrl + M)");
	actionRetentionTime->setCheckable(true);
	actionRetentionTime->setChecked(true);
	toolbarView->addAction(actionRetentionTime);
	connect(actionRetentionTime, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(retentionTimeStateChanged(bool)));

	actionAbsoluteIntensity = new QAction(QIcon(":/images/icons/11.png"), tr("&Absolute Intensity"), this);
	actionAbsoluteIntensity->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionAbsoluteIntensity->setToolTip("Absolute Intensity (Ctrl + I)");
	actionAbsoluteIntensity->setCheckable(true);
	actionAbsoluteIntensity->setChecked(true);
	toolbarView->addAction(actionAbsoluteIntensity);
	connect(actionAbsoluteIntensity, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(absoluteIntensityStateChanged(bool)));

	actionRawData = new QAction(QIcon(":/images/icons/chromatography.png"), tr("&Profile Chromatogram"), this);
	actionRawData->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	actionRawData->setToolTip("Profile Chromatogram (Ctrl + P)");
	actionRawData->setCheckable(true);
	toolbarView->addAction(actionRawData);
	connect(actionRawData, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(rawDataStateChanged(bool)));

	peakshapelabel = new QLabel();
	peakshapelabel->setText("Peak Shape: ");

	comboboxpeakshape = new QComboBox();
	comboboxpeakshape->setToolTip("Select chromatographic peak shape to be used for area and concentration calculations:\nGaussian = basic Gaussian function,\nex-Gaussian (tailing) = Exponentially Modified Gaussian for chromatographic peaks with tailing,\nex-Gaussian (fronting) = Exponentially Modified Gaussian for chromatographic peaks with fronting.");
	comboboxpeakshape->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	comboboxpeakshape->addItem("Gaussian");
	comboboxpeakshape->addItem("ex-Gaussian (tailing)");
	comboboxpeakshape->addItem("ex-Gaussian (fronting)");
	connect(comboboxpeakshape, SIGNAL(currentIndexChanged(int)), chromatogramwindowwidget, SLOT(peakShapeChanged(int)));

	peakshapehboxlayout = new QHBoxLayout();
	peakshapehboxlayout->addWidget(peakshapelabel);
	peakshapehboxlayout->addWidget(comboboxpeakshape);

	peakshapewidget = new QWidget();
	peakshapewidget->setLayout(peakshapehboxlayout);
	toolbarView->addWidget(peakshapewidget);

	toolbarView->addSeparator();

	actionHideTIC = new QAction(QIcon(":/images/icons/81.png"), tr("Hide &TIC"), this);
	actionHideTIC->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
	actionHideTIC->setToolTip("Hide TIC (Ctrl + Shift + T)");
	actionHideTIC->setCheckable(true);
	actionHideTIC->setChecked(true);
	toolbarView->addAction(actionHideTIC);
	connect(actionHideTIC, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(hideTIC(bool)));

	actionHideEIC = new QAction(QIcon(":/images/icons/20.png"), tr("Hide &EIC"), this);
	actionHideEIC->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_E));
	actionHideEIC->setToolTip("Hide EIC (Ctrl + Shift + E)");
	actionHideEIC->setCheckable(true);
	toolbarView->addAction(actionHideEIC);
	connect(actionHideEIC, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(hideEIC(bool)));

	actionHideLabels = new QAction(QIcon(":/images/icons/79.png"), tr("Hide &Labels"), this);
	actionHideLabels->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	actionHideLabels->setToolTip("Hide Labels (Ctrl + L)");
	actionHideLabels->setCheckable(true);
	toolbarView->addAction(actionHideLabels);
	connect(actionHideLabels, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(hideLabels(bool)));


	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	addToolBarBreak();


	toolbarTime = addToolBar(tr("Time and Spectrum ID"));

	labelretentiontime = new QLabel(tr("Time: "));

	minretentiontime = new QDoubleSpinBox();
	minretentiontime->setToolTip("Minimum time");
	minretentiontime->setDecimals(6);
	minretentiontime->setRange(0, 10000000);
	minretentiontime->setSingleStep(1);

	labelseparatorretentiontime = new QLabel(tr("-"));

	maxretentiontime = new QDoubleSpinBox();
	maxretentiontime->setToolTip("Maximum time");
	maxretentiontime->setDecimals(6);
	maxretentiontime->setRange(0, 10000000);
	maxretentiontime->setSingleStep(1);

	setretentiontimeinterval = new QPushButton("Set Time");
	setretentiontimeinterval->setMinimumWidth(75);
	connect(setretentiontimeinterval, SIGNAL(released()), this, SLOT(setRetentionTimeInterval()));
	connect(this, SIGNAL(emitRetentionTimeInterval(double, double)), chromatogramwindowwidget, SLOT(setRetentionTimeInterval(double, double)));

	resetretentiontimeinterval = new QPushButton("Reset Time");
	resetretentiontimeinterval->setMinimumWidth(75);
	connect(resetretentiontimeinterval, SIGNAL(released()), chromatogramwindowwidget, SLOT(resetRetentionTimeInterval()));
	connect(chromatogramwindowwidget, SIGNAL(updateRetentionTimeInterval(double, double)), this, SLOT(updateRetentionTimeInterval(double, double)));

	hboxretentiontime = new QHBoxLayout();
	hboxretentiontime->addWidget(labelretentiontime);
	hboxretentiontime->addWidget(minretentiontime);
	hboxretentiontime->addWidget(labelseparatorretentiontime);
	hboxretentiontime->addWidget(maxretentiontime);
	hboxretentiontime->addSpacing(5);
	hboxretentiontime->addWidget(setretentiontimeinterval);
	hboxretentiontime->addSpacing(5);
	hboxretentiontime->addWidget(resetretentiontimeinterval);

	widgetretentiontime = new QWidget();
	widgetretentiontime->setLayout(hboxretentiontime);

	toolbarTime->addWidget(widgetretentiontime);
	toolbarTime->addSeparator();

	labelscanid = new QLabel(tr("Spectrum ID: "));

	minscanid = new QSpinBox();
	minscanid->setToolTip("Minimum spectrum ID");
	minscanid->setRange(1, 1000000);
	minscanid->setSingleStep(1);

	labelseparatorscanid = new QLabel(tr("-"));

	maxscanid = new QSpinBox();
	maxscanid->setToolTip("Maximum spectrum ID");
	maxscanid->setRange(1, 1000000);
	maxscanid->setSingleStep(1);

	setscanidinterval = new QPushButton("Set ID");
	setscanidinterval->setMinimumWidth(75);
	connect(setscanidinterval, SIGNAL(released()), this, SLOT(setScanIDInterval()));
	connect(this, SIGNAL(emitScanIDInterval(int, int)), chromatogramwindowwidget, SLOT(setScanIDInterval(int, int)));

	resetscanidinterval = new QPushButton("Reset ID");
	resetscanidinterval->setMinimumWidth(75);
	connect(resetscanidinterval, SIGNAL(released()), chromatogramwindowwidget, SLOT(resetScanIDInterval()));
	connect(chromatogramwindowwidget, SIGNAL(updateScanIDInterval(int, int)), this, SLOT(updateScanIDInterval(int, int)));

	hboxscanid = new QHBoxLayout();
	hboxscanid->addWidget(labelscanid);
	hboxscanid->addWidget(minscanid);
	hboxscanid->addWidget(labelseparatorscanid);
	hboxscanid->addWidget(maxscanid);
	hboxscanid->addSpacing(5);
	hboxscanid->addWidget(setscanidinterval);
	hboxscanid->addSpacing(5);
	hboxscanid->addWidget(resetscanidinterval);

	widgetscanid = new QWidget();
	widgetscanid->setLayout(hboxscanid);

	toolbarTime->addWidget(widgetscanid);


	menuFile->addAction(actionExportImage);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuView->addAction(actionZoomIn);
	menuView->addAction(actionZoomOut);
	menuView->addAction(actionZoomReset);
	menuView->addSeparator();
	menuView->addAction(actionMouseSelection);
	menuView->addAction(actionRetentionTime);
	menuView->addAction(actionAbsoluteIntensity);
	menuView->addAction(actionRawData);
	menuView->addSeparator();
	menuView->addAction(actionHideTIC);
	menuView->addAction(actionHideEIC);
	menuView->addAction(actionHideLabels);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	setCentralWidget(chromatogramwindowwidget);
	centralWidget()->setContentsMargins(0, 0, 0, 0);

	applyGlobalPreferences(globalpreferences);

	resize(defaultwinsizex, defaultwinsizey);
}


cChromatogramWindow::~cChromatogramWindow() {
	delete chromatogramwindowwidget;

	delete peakshapelabel;
	delete comboboxpeakshape;
	delete peakshapehboxlayout;
	delete peakshapewidget;

	delete labelretentiontime;
	delete minretentiontime;
	delete labelseparatorretentiontime;
	delete maxretentiontime;
	delete setretentiontimeinterval;
	delete resetretentiontimeinterval;
	delete hboxretentiontime;
	delete widgetretentiontime;

	delete labelscanid;
	delete minscanid;
	delete labelseparatorscanid;
	delete maxscanid;
	delete setscanidinterval;
	delete resetscanidinterval;
	delete hboxscanid;
	delete widgetscanid;

	delete actionExportImage;
	delete actionCloseWindow;
	delete actionZoomIn;
	delete actionZoomOut;
	delete actionZoomReset;
	delete actionMouseSelection;
	delete actionRetentionTime;
	delete actionAbsoluteIntensity;
	delete actionRawData;
	delete actionHideTIC;
	delete actionHideEIC;
	delete actionHideLabels;
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuView;
	delete menuHelp;

	delete menuBar;
}


void cChromatogramWindow::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


void cChromatogramWindow::recalculateTICChromatogram(int activefileid) {
	chromatogramwindowwidget->recalculateTICChromatogram(activefileid);
}


void cChromatogramWindow::setAbsoluteIntensityEnabled(bool enabled) {
	actionAbsoluteIntensity->setChecked(enabled);
}


void cChromatogramWindow::setPeakShape(int peakshape) {
	comboboxpeakshape->setCurrentIndex(peakshape);
}


void cChromatogramWindow::applyGlobalPreferences(cGlobalPreferences* globalpreferences) {
	if (globalpreferences) {
		#if OS_TYPE == WIN
			if ((lastdirexportimage.right(4).compare(".pdf", Qt::CaseInsensitive) != 0) && (lastdirexportimage.right(3).compare(".ps", Qt::CaseInsensitive) != 0) && (lastdirexportimage.right(4).compare(".png", Qt::CaseInsensitive) != 0) && (lastdirexportimage.right(4).compare(".svg", Qt::CaseInsensitive) != 0)) {
				lastdirexportimage = globalpreferences->exportimagedefaultdir;
			}
		#else
			if ((lastdirexportimage.right(4).compare(".pdf", Qt::CaseInsensitive) != 0) && (lastdirexportimage.right(4).compare(".png", Qt::CaseInsensitive) != 0) && (lastdirexportimage.right(4).compare(".svg", Qt::CaseInsensitive) != 0)) {
				lastdirexportimage = globalpreferences->exportimagedefaultdir;
			}
		#endif
	}
}


void cChromatogramWindow::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (minscanid->hasFocus() || maxscanid->hasFocus()) {
			setScanIDInterval();
		}
		if (minretentiontime->hasFocus() || maxretentiontime->hasFocus()) {
			setRetentionTimeInterval();
		}
	}

	event->accept();
}


void cChromatogramWindow::exportImage() {
	#if OS_TYPE == WIN
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Image..."), lastdirexportimage, "PDF Files (*.pdf);; PS Files (*.ps);; PNG Files (*.png);; SVG Files (*.svg)");
	#else
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Image..."), lastdirexportimage, "PDF Files (*.pdf);; PNG Files (*.png);; SVG Files (*.svg)");
	#endif
	if (!filename.isEmpty()) {
		lastdirexportimage = filename;

		regex rx;
		bool selected = false;

		rx = ".+\\.pdf$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			chromatogramwindowwidget->exportToPDF(filename, false);
			selected = true;
		}

	#if OS_TYPE == WIN
		rx = ".+\\.ps$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			chromatogramwindowwidget->exportToPDF(filename, true);
			selected = true;
		}
	#endif

		rx = ".+\\.png$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			chromatogramwindowwidget->exportToPNG(filename);
			selected = true;
		}

		rx = ".+\\.svg$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			chromatogramwindowwidget->exportToSVG(filename);
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


void cChromatogramWindow::closeWindow() {
	hide();
}


void cChromatogramWindow::updateRetentionTimeInterval(double mintime, double maxtime) {
	this->minretentiontime->setValue(mintime);
	this->maxretentiontime->setValue(maxtime);
}


void cChromatogramWindow::setRetentionTimeInterval() {
	emit emitRetentionTimeInterval(minretentiontime->value(), maxretentiontime->value());
}


void cChromatogramWindow::updateScanIDInterval(int minid, int maxid) {
	this->minscanid->setValue(minid);
	this->maxscanid->setValue(maxid);
}


void cChromatogramWindow::setScanIDInterval() {
	emit emitScanIDInterval(minscanid->value(), maxscanid->value());
}


void cChromatogramWindow::setFilterOptionsSlot(cPeaksList eic) {
	chromatogramwindowwidget->setEICChromatogram(eic);
}


void cChromatogramWindow::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/chromatogram.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/chromatogram.html").absoluteFilePath()));
	#endif
}


void cChromatogramWindow::chromatogramDoubleClickedSlot(int scanid) {
	emit doubleClickedScanIDSignal(scanid);
}

