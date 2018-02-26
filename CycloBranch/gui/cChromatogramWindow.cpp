#include "gui/cChromatogramWindow.h"
#include "gui/cEventFilter.h"


cChromatogramWindow::cChromatogramWindow(cTheoreticalSpectrumList& theoreticalspectrumlist, QWidget* parent) {
	this->theoreticalspectrumlist = &theoreticalspectrumlist;
	this->parent = parent;

	setWindowTitle("Chromatogram");
	setWindowIcon(QIcon(":/images/icons/chromatography.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	chromatogramwindowwidget = new cChromatogramWindowWidget(theoreticalspectrumlist, this);


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

	actionAbsoluteIntensity = new QAction(QIcon(":/images/icons/11.png"), tr("&Absolute Intensity"), this);
	actionAbsoluteIntensity->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionAbsoluteIntensity->setToolTip("Absolute Intensity (Ctrl + I)");
	actionAbsoluteIntensity->setCheckable(true);
	actionAbsoluteIntensity->setChecked(true);
	toolbarView->addAction(actionAbsoluteIntensity);
	connect(actionAbsoluteIntensity, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(absoluteIntensityStateChanged(bool)));

	toolbarView->addSeparator();

	actionHideTIC = new QAction(QIcon(":/images/icons/81.png"), tr("Hide &TIC"), this);
	actionHideTIC->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
	actionHideTIC->setToolTip("Hide TIC (Ctrl + Shift + T)");
	actionHideTIC->setCheckable(true);
	toolbarView->addAction(actionHideTIC);
	connect(actionHideTIC, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(hideTIC(bool)));

	actionHideEIC = new QAction(QIcon(":/images/icons/20.png"), tr("Hide &EIC"), this);
	actionHideEIC->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_E));
	actionHideEIC->setToolTip("Hide EIC (Ctrl + Shift + E)");
	actionHideEIC->setCheckable(true);
	toolbarView->addAction(actionHideEIC);
	connect(actionHideEIC, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(hideEIC(bool)));


	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));


	labelscanid = new QLabel(tr("Spectrum ID: "));

	minscanid = new QSpinBox();
	minscanid->setRange(1, 1000000);
	minscanid->setSingleStep(1);

	labelseparator = new QLabel(tr("-"));

	maxscanid = new QSpinBox();
	maxscanid->setRange(1, 1000000);
	maxscanid->setSingleStep(1);

	setscanidinterval = new QPushButton("Set");
	setscanidinterval->setMinimumWidth(75);
	connect(setscanidinterval, SIGNAL(released()), this, SLOT(setScanIDInterval()));
	connect(this, SIGNAL(emitScanIDInterval(int, int)), chromatogramwindowwidget, SLOT(setScanIDInterval(int, int)));

	resetscanidinterval = new QPushButton("Reset");
	resetscanidinterval->setMinimumWidth(75);
	connect(resetscanidinterval, SIGNAL(released()), chromatogramwindowwidget, SLOT(resetScanIDInterval()));
	connect(chromatogramwindowwidget, SIGNAL(updateScanIDInterval(int, int)), this, SLOT(updateScanIDInterval(int, int)));

	hboxscanid = new QHBoxLayout();
	hboxscanid->addWidget(labelscanid);
	hboxscanid->addWidget(minscanid);
	hboxscanid->addWidget(labelseparator);
	hboxscanid->addWidget(maxscanid);
	hboxscanid->addSpacing(5);
	hboxscanid->addWidget(setscanidinterval);
	hboxscanid->addSpacing(5);
	hboxscanid->addWidget(resetscanidinterval);

	widgetscanid = new QWidget();
	widgetscanid->setLayout(hboxscanid);

	toolbarScanID = addToolBar(tr("Spectrum ID"));

	actionMouseSelection = new QAction(QIcon(":/images/icons/64.png"), tr("Mouse Spectrum ID Selection Tool"), this);
	actionMouseSelection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
	actionMouseSelection->setToolTip("Mouse Spectrum ID Selection Tool (Ctrl + T)");
	actionMouseSelection->setCheckable(true);
	actionMouseSelection->setChecked(true);
	actionMouseSelection->setEnabled(true);
	toolbarScanID->addAction(actionMouseSelection);
	connect(actionMouseSelection, SIGNAL(toggled(bool)), chromatogramwindowwidget, SLOT(enableMouseSelectionTool(bool)));

	toolbarScanID->addWidget(widgetscanid);


	menuFile->addAction(actionExportImage);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuView->addAction(actionZoomIn);
	menuView->addAction(actionZoomOut);
	menuView->addAction(actionZoomReset);
	menuView->addSeparator();
	menuView->addAction(actionAbsoluteIntensity);
	menuView->addSeparator();
	menuView->addAction(actionHideTIC);
	menuView->addAction(actionHideEIC);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	setCentralWidget(chromatogramwindowwidget);
	centralWidget()->setContentsMargins(0, 0, 0, 0);

	resize(1280, 780);

}


cChromatogramWindow::~cChromatogramWindow() {
	delete chromatogramwindowwidget;

	delete labelscanid;
	delete minscanid;
	delete labelseparator;
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
	delete actionAbsoluteIntensity;
	delete actionHideTIC;
	delete actionHideEIC;
	delete actionHTMLDocumentation;
	delete actionMouseSelection;

	delete menuFile;
	delete menuView;
	delete menuHelp;

	delete menuBar;
}


void cChromatogramWindow::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


void cChromatogramWindow::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		setScanIDInterval();
	}

	event->accept();
}


void cChromatogramWindow::exportImage() {
#if OS_TYPE == WIN
	QString filename = QFileDialog::getSaveFileName(this, tr("Export Image..."), "./", "PDF Files (*.pdf);; PS Files (*.ps);; PNG Files (*.png);; SVG Files (*.svg)");
#else
	QString filename = QFileDialog::getSaveFileName(this, tr("Export Image..."), "./", "PDF Files (*.pdf);; PNG Files (*.png);; SVG Files (*.svg)");
#endif
	if (!filename.isEmpty()) {
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

