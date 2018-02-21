#include "gui/cImageWindow.h"
#include "gui/cEventFilter.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QSpinBox>
#include <QMenuBar>
#include <QMenu>


cImageWindow::cImageWindow(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Image Window");
	setWindowIcon(QIcon(":/images/icons/23.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	xfromlabel = new QLabel("X: ");

	xfrom = new QSpinBox();
	xfrom->setRange(0, 100000);
	xfrom->setSingleStep(1);
	xfrom->setValue(0);

	xtolabel = new QLabel("-");

	xto = new QSpinBox();
	xto->setRange(0, 100000);
	xto->setSingleStep(1);
	xto->setValue(0);

	yfromlabel = new QLabel("Y: ");

	yfrom = new QSpinBox();
	yfrom->setRange(0, 100000);
	yfrom->setSingleStep(1);
	yfrom->setValue(0);

	ytolabel = new QLabel("-");

	yto = new QSpinBox();
	yto->setRange(0, 100000);
	yto->setSingleStep(1);
	yto->setValue(0);

	setregionbutton = new QPushButton(" Select Region ");
	setregionbutton->setToolTip("Select a region.");

	resetregionbutton = new QPushButton(" Reset Region ");
	resetregionbutton->setToolTip("Clear the selection and view all points.");
	resetregionbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));

	regionhbox = new QHBoxLayout();
	regionhbox->addWidget(xfromlabel);
	regionhbox->addWidget(xfrom);
	regionhbox->addWidget(xtolabel);
	regionhbox->addWidget(xto);
	regionhbox->addWidget(yfromlabel);
	regionhbox->addWidget(yfrom);
	regionhbox->addWidget(ytolabel);
	regionhbox->addWidget(yto);
	regionhbox->addWidget(setregionbutton);
	regionhbox->addWidget(resetregionbutton);

	regionwidget = new QWidget();
	regionwidget->setLayout(regionhbox);
	regionwidget->setMaximumWidth(640);

	minxlabel = new QLabel("Left Shift: ");

	minx = new QSpinBox();
	minx->setRange(-100000, 100000);
	minx->setSingleStep(1);
	minx->setValue(0);

	minylabel = new QLabel("Top Shift: ");

	miny = new QSpinBox();
	miny->setRange(-100000, 100000);
	miny->setSingleStep(1);
	miny->setValue(0);

	maxxlabel = new QLabel("Max X: ");

	maxx = new QSpinBox();
	maxx->setRange(1, 100000);
	maxx->setSingleStep(1);
	maxx->setValue(500);

	maxylabel = new QLabel("Max Y: ");

	maxy = new QSpinBox();
	maxy->setRange(1, 100000);
	maxy->setSingleStep(1);
	maxy->setValue(500);

	setmaxbutton = new QPushButton("Correlate");
	setmaxbutton->setToolTip("Correlate the points area with optical image.");

	setdefaultbutton = new QPushButton("Default");
	setdefaultbutton->setToolTip("Set the default correlation.");

	maxcoordinateshbox = new QHBoxLayout();
	maxcoordinateshbox->addWidget(minxlabel);
	maxcoordinateshbox->addWidget(minx);
	maxcoordinateshbox->addWidget(minylabel);
	maxcoordinateshbox->addWidget(miny);
	maxcoordinateshbox->addWidget(maxxlabel);
	maxcoordinateshbox->addWidget(maxx);
	maxcoordinateshbox->addWidget(maxylabel);
	maxcoordinateshbox->addWidget(maxy);
	maxcoordinateshbox->addWidget(setmaxbutton);
	maxcoordinateshbox->addWidget(setdefaultbutton);

	maxcoordinateswidget = new QWidget();
	maxcoordinateswidget->setLayout(maxcoordinateshbox);
	maxcoordinateswidget->setMaximumWidth(640);

	image = new QImage();
	imagewindowwidget = new cImageWindowWidget();

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(imagewindowwidget);

	mainwidget = new QWidget();
	mainwidget->setLayout(mainlayout);

	connect(setregionbutton, SIGNAL(released()), this, SLOT(setRegionButtonReleased()));
	connect(resetregionbutton, SIGNAL(released()), this, SLOT(resetSelection()));
	connect(setmaxbutton, SIGNAL(released()), this, SLOT(setMaxButtonReleased()));
	connect(setdefaultbutton, SIGNAL(released()), this, SLOT(setDefaultButtonReleased()));

	toolbarFile = addToolBar(tr("File"));
				
	actionOpenImage = new QAction(QIcon(":/images/icons/52.png"), tr("&Open Image"), this);
	actionOpenImage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	actionOpenImage->setToolTip("Open Image (Ctrl + O)");
	toolbarFile->addAction(actionOpenImage);
	connect(actionOpenImage, SIGNAL(triggered()), this, SLOT(openImage()));

	actionSaveImage = new QAction(QIcon(":/images/icons/22.png"), tr("&Save Image"), this);
	actionSaveImage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	actionSaveImage->setToolTip("Save Image (Ctrl + S)");
	toolbarFile->addAction(actionSaveImage);
	connect(actionSaveImage, SIGNAL(triggered()), this, SLOT(saveImage()));

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
	connect(actionZoomIn, SIGNAL(triggered()), imagewindowwidget, SLOT(zoomIn()));

	actionZoomOut = new QAction(QIcon(":/images/icons/82.png"), tr("Zoom &Out"), this);
	actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
	actionZoomOut->setToolTip("Zoom Out (Ctrl -)");
	toolbarView->addAction(actionZoomOut);
	connect(actionZoomOut, SIGNAL(triggered()), imagewindowwidget, SLOT(zoomOut()));

	actionZoomReset = new QAction(QIcon(":/images/icons/84.png"), tr("&Reset Zoom"), this);
	actionZoomReset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	actionZoomReset->setToolTip("Zoom Reset (Ctrl + R)");
	toolbarView->addAction(actionZoomReset);
	connect(actionZoomReset, SIGNAL(triggered()), imagewindowwidget, SLOT(normalSize()));

	actionColorScale = new QAction(QIcon(":/images/icons/50.png"), tr("&Color Scale"), this);
	actionColorScale->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
	actionColorScale->setToolTip("Color Scale (Ctrl + C)");
	actionColorScale->setCheckable(true);
	actionColorScale->setChecked(true);
	toolbarView->addSeparator();
	toolbarView->addAction(actionColorScale);
	connect(actionColorScale, SIGNAL(toggled(bool)), imagewindowwidget, SLOT(colorScaleStateChanged(bool)));

	actionAbsoluteIntensity = new QAction(QIcon(":/images/icons/11.png"), tr("Absolute &Intensity"), this);
	actionAbsoluteIntensity->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionAbsoluteIntensity->setToolTip("Absolute Intensity (Ctrl + I)");
	actionAbsoluteIntensity->setCheckable(true);
	toolbarView->addAction(actionAbsoluteIntensity);
	connect(actionAbsoluteIntensity, SIGNAL(toggled(bool)), imagewindowwidget, SLOT(absoluteIntensityStateChanged(bool)));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	addToolBarBreak();

	toolbarRegion = addToolBar(tr("Select Region"));
	toolbarRegion->addWidget(regionwidget);

	actionCorrelatePointsArea = new QAction(QIcon(":/images/icons/64.png"), tr("Correlate the points area with optical image."), this);
	actionCorrelatePointsArea->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
	actionCorrelatePointsArea->setToolTip("Correlate the points area with optical image (Ctrl + T)");
	actionCorrelatePointsArea->setCheckable(true);
	actionCorrelatePointsArea->setChecked(false);
	actionCorrelatePointsArea->setEnabled(true);
	connect(actionCorrelatePointsArea, SIGNAL(toggled(bool)), imagewindowwidget, SLOT(enableCorrelateSelectionTool(bool)));

	toolbarMaxCoordinates = addToolBar(tr("Correlate Area"));
	toolbarMaxCoordinates->addAction(actionCorrelatePointsArea);
	toolbarMaxCoordinates->addWidget(maxcoordinateswidget);

	menuFile->addAction(actionOpenImage);
	menuFile->addAction(actionSaveImage);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuView->addAction(actionZoomIn);
	menuView->addAction(actionZoomOut);
	menuView->addAction(actionZoomReset);
	menuView->addSeparator();
	menuView->addAction(actionColorScale);
	menuView->addAction(actionAbsoluteIntensity);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	setCentralWidget(mainwidget);
	centralWidget()->setContentsMargins(0, 0, 0, 0);

	connect(imagewindowwidget, SIGNAL(updateFilter(int, int, int, int)), this, SLOT(updateSelection(int, int, int, int)));
	connect(imagewindowwidget, SIGNAL(updateFilter(int, int, int, int)), this->parent, SLOT(updateSummaryPeaksTableFilter(int, int, int, int)));
	connect(this, SIGNAL(updateSummaryPeaksTableFilter(int, int, int, int)), this->parent, SLOT(updateSummaryPeaksTableFilter(int, int, int, int)));
	connect(imagewindowwidget, SIGNAL(updateCorrelation(int, int, int, int)), this, SLOT(updatePointsArea(int, int, int, int)));

	resize(1280, 770);

	lastimagedir = "./";

	maxcountx = 1;
	maxcounty = 1;
	vendor = unknownvendor;
}


cImageWindow::~cImageWindow() {
	delete xfromlabel;
	delete xfrom;
	delete xtolabel;
	delete xto;
	delete yfromlabel;
	delete yfrom;
	delete ytolabel;
	delete yto;
	delete setregionbutton;
	delete resetregionbutton;
	delete regionhbox;
	delete regionwidget;

	delete minxlabel;
	delete minx;
	delete minylabel;
	delete miny;
	delete maxxlabel;
	delete maxx;
	delete maxylabel;
	delete maxy;
	delete setmaxbutton;
	delete setdefaultbutton;
	delete maxcoordinateshbox;
	delete maxcoordinateswidget;

	delete image;
	delete imagewindowwidget;

	delete mainlayout;
	delete mainwidget;

	delete actionOpenImage;
	delete actionSaveImage;
	delete actionCloseWindow;
	delete actionZoomIn;
	delete actionZoomOut;
	delete actionZoomReset;
	delete actionColorScale;
	delete actionAbsoluteIntensity;
	delete actionCorrelatePointsArea;
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuView;
	delete menuHelp;

	delete menuBar;
}


void cImageWindow::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


void cImageWindow::setDefaultMaxXY(int maxcountx, int maxcounty, eVendorType vendor) {
	this->maxcountx = maxcountx;
	this->maxcounty = maxcounty;
	this->vendor = vendor;
	imagewindowwidget->setDefaultMaxXY(maxcountx, maxcounty, vendor);
}


void cImageWindow::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (xfrom->hasFocus() || xto->hasFocus() || yfrom->hasFocus() || yto->hasFocus()) {
			setregionbutton->click();
		}
		if (minx->hasFocus() || miny->hasFocus() || maxx->hasFocus() || maxy->hasFocus()) {
			setmaxbutton->click();
		}
    }
	event->accept();
}


void cImageWindow::closeWindow() {
	hide();
}


void cImageWindow::openImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Image ..."), lastimagedir, tr("Image Files (*.jpg *.jpeg *.png *.tif *.tiff *.bmp *.gif)"));

	if (!filename.isEmpty()) {
		lastimagedir = filename;

		image->load(filename);
		imagewindowwidget->setPixmap(image);

		minx->setValue(0);
		miny->setValue(0);
		maxx->setValue(maxcountx);
		maxy->setValue(maxcounty);
		
		setMaxButtonReleased();
	}
}


void cImageWindow::saveImage() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image ..."), "./", "PNG Files (*.png)");

	if (!filename.isEmpty()) {
		lastimagedir = filename;
		imagewindowwidget->getImage().save(filename, "PNG");
	}
}


void cImageWindow::setFilterOptions(vector<cCoordinates> coordinates, string filterstring, bool casesensitive) {
	imagewindowwidget->setFilterOptions(coordinates, filterstring, casesensitive);
}


void cImageWindow::setMaxButtonReleased() {
	imagewindowwidget->setMaxXY(minx->value(), miny->value(), maxx->value(), maxy->value());
	resetregionbutton->click();
}


void cImageWindow::setDefaultButtonReleased() {
	minx->setValue(0);
	miny->setValue(0);
	maxx->setValue(maxcountx);
	maxy->setValue(maxcounty);
	imagewindowwidget->setMaxXY(minx->value(), miny->value(), maxx->value(), maxy->value());
	resetregionbutton->click();
}


void cImageWindow::setRegionButtonReleased() {
	emit updateSummaryPeaksTableFilter(xfrom->value(), xto->value(), yfrom->value(), yto->value());
}


void cImageWindow::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/imagewindow.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/imagewindow.html").absoluteFilePath()));
	#endif
}


void cImageWindow::updateSelection(int xmin, int xmax, int ymin, int ymax) {
	xfrom->setValue(xmin);
	xto->setValue(xmax);
	yfrom->setValue(ymin);
	yto->setValue(ymax);
}


void cImageWindow::clearSelection() {
	xfrom->setValue(0);
	yfrom->setValue(0);
	xto->setValue(maxx->value());
	yto->setValue(maxy->value());
}


void cImageWindow::resetSelection() {
	clearSelection();
	emit updateSummaryPeaksTableFilter(xfrom->value(), xto->value(), yfrom->value(), yto->value());
}


void cImageWindow::updatePointsArea(int xmin, int xmax, int ymin, int ymax) {
	minx->setValue(xmin);
	maxx->setValue(xmax);
	miny->setValue(ymin);
	maxy->setValue(ymax);
	setMaxButtonReleased();
}

