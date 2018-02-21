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
	menuZoom = new QMenu(tr("&Zoom"), this);
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

	leftmarginlabel = new QLabel("Left Crop: ");

	leftmargin = new QSpinBox();
	leftmargin->setRange(0, 100000);
	leftmargin->setSingleStep(1);
	leftmargin->setValue(0);

	topmarginlabel = new QLabel("Top Crop: ");

	topmargin = new QSpinBox();
	topmargin->setRange(0, 100000);
	topmargin->setSingleStep(1);
	topmargin->setValue(0);

	setmaxbutton = new QPushButton("Set");
	setmaxbutton->setToolTip("Set up the minimum and maximum coordinates corresponding to the bottom-right corner of the image, set up the left margin and the top margin.");

	maxcoordinateshbox = new QHBoxLayout();
	maxcoordinateshbox->addWidget(maxxlabel);
	maxcoordinateshbox->addWidget(maxx);
	maxcoordinateshbox->addWidget(maxylabel);
	maxcoordinateshbox->addWidget(maxy);
	//maxcoordinateshbox->addWidget(leftmarginlabel);
	//maxcoordinateshbox->addWidget(leftmargin);
	//maxcoordinateshbox->addWidget(topmarginlabel);
	//maxcoordinateshbox->addWidget(topmargin);
	maxcoordinateshbox->addWidget(setmaxbutton);

	maxcoordinateswidget = new QWidget();
	maxcoordinateswidget->setLayout(maxcoordinateshbox);
	maxcoordinateswidget->setMaximumWidth(640);

	image = new QImage();
	imagewindowwidget = new cImageWindowWidget();

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(imagewindowwidget);

	mainwidget = new QWidget();
	mainwidget->setLayout(mainlayout);

	connect(setmaxbutton, SIGNAL(released()), this, SLOT(setMaxButtonReleased()));
	connect(setregionbutton, SIGNAL(released()), this, SLOT(setRegionButtonReleased()));
	connect(resetregionbutton, SIGNAL(released()), this, SLOT(resetSelection()));

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

	toolbarZoom = addToolBar(tr("Zoom"));
				
	actionZoomIn = new QAction(QIcon(":/images/icons/83.png"), tr("Zoom &In"), this);
	actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
	actionZoomIn->setToolTip("Zoom In (Ctrl +)");
	toolbarZoom->addAction(actionZoomIn);
	connect(actionZoomIn, SIGNAL(triggered()), imagewindowwidget, SLOT(zoomIn()));

	actionZoomOut = new QAction(QIcon(":/images/icons/82.png"), tr("Zoom &Out"), this);
	actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
	actionZoomOut->setToolTip("Zoom Out (Ctrl -)");
	toolbarZoom->addAction(actionZoomOut);
	connect(actionZoomOut, SIGNAL(triggered()), imagewindowwidget, SLOT(zoomOut()));

	actionZoomReset = new QAction(QIcon(":/images/icons/84.png"), tr("&Reset Zoom"), this);
	actionZoomReset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	actionZoomReset->setToolTip("Zoom Reset (Ctrl + R)");
	toolbarZoom->addAction(actionZoomReset);
	connect(actionZoomReset, SIGNAL(triggered()), imagewindowwidget, SLOT(normalSize()));

	toolbarRegion = addToolBar(tr("Select Region"));
	toolbarRegion->addWidget(regionwidget);

	toolbarMaxCoordinates = addToolBar(tr("Coordinates"));
	toolbarMaxCoordinates->addWidget(maxcoordinateswidget);

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	menuFile->addAction(actionOpenImage);
	menuFile->addAction(actionSaveImage);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuZoom->addAction(actionZoomIn);
	menuZoom->addAction(actionZoomOut);
	menuZoom->addAction(actionZoomReset);
	//menuZoom->addSeparator();
	//menuZoom->addAction(actionViewAllPoints);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuZoom);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	setCentralWidget(mainwidget);
	centralWidget()->setContentsMargins(0, 0, 0, 0);

	connect(imagewindowwidget, SIGNAL(updateFilter(int, int, int, int)), this, SLOT(updateSelection(int, int, int, int)));
	connect(imagewindowwidget, SIGNAL(updateFilter(int, int, int, int)), this->parent, SLOT(updateSummaryPeaksTableFilter(int, int, int, int)));
	connect(this, SIGNAL(updateSummaryPeaksTableFilter(int, int, int, int)), this->parent, SLOT(updateSummaryPeaksTableFilter(int, int, int, int)));

	resize(1280, 750);

	lastimagedir = "./";
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

	delete maxxlabel;
	delete maxx;
	delete maxylabel;
	delete maxy;
	delete leftmarginlabel;
	delete leftmargin;
	delete topmarginlabel;
	delete topmargin;
	delete setmaxbutton;
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
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuZoom;
	delete menuHelp;

	delete menuBar;
}


void cImageWindow::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


void cImageWindow::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (xfrom->hasFocus() || xto->hasFocus() || yfrom->hasFocus() || yto->hasFocus()) {
			setregionbutton->click();
		}
		if (maxx->hasFocus() || maxy->hasFocus()) {
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
	}
}


void cImageWindow::saveImage() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image ..."), "./", "PNG Files (*.png)");

	if (!filename.isEmpty()) {
		lastimagedir = filename;
		imagewindowwidget->getImage().save(filename, "PNG");
	}
}


void cImageWindow::setCoordinates(vector<cCoordinates> coordinates) {
	imagewindowwidget->setCoordinates(coordinates);
}


void cImageWindow::setMaxButtonReleased() {
	imagewindowwidget->setMaxXY(maxx->value(), maxy->value(), leftmargin->value(), topmargin->value());
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


void cImageWindow::resetSelection() {
	xfrom->setValue(0);
	yfrom->setValue(0);
	xto->setValue(maxx->value());
	yto->setValue(maxy->value());

	emit updateSummaryPeaksTableFilter(xfrom->value(), xto->value(), yfrom->value(), yto->value());
}

