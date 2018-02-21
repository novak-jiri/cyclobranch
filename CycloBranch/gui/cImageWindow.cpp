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

	setWindowTitle("CrossVis");
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
	xfrom->setToolTip("Horizontal range of selected compounds [number of points].");

	xtolabel = new QLabel("-");

	xto = new QSpinBox();
	xto->setRange(0, 100000);
	xto->setSingleStep(1);
	xto->setValue(0);
	xto->setToolTip("Horizontal range of selected compounds [number of points].");

	yfromlabel = new QLabel("Y: ");

	yfrom = new QSpinBox();
	yfrom->setRange(0, 100000);
	yfrom->setSingleStep(1);
	yfrom->setValue(0);
	yfrom->setToolTip("Vertical range of selected compounds [number of points].");

	ytolabel = new QLabel("-");

	yto = new QSpinBox();
	yto->setRange(0, 100000);
	yto->setSingleStep(1);
	yto->setValue(0);
	yto->setToolTip("Vertical range of selected compounds [number of points].");

	setregionbutton = new QPushButton(" Select Region ");
	setregionbutton->setToolTip("Select a region of points.");

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


	leftshiftlabel = new QLabel("Horizontal Shift: ");

	leftshift = new QSpinBox();
	leftshift->setRange(-100000, 100000);
	leftshift->setSingleStep(1);
	leftshift->setValue(0);
	leftshift->setToolTip("Horizontal shift of points between measured area and the optical image [number of points].");

	topshiftlabel = new QLabel("Vertical Shift: ");

	topshift = new QSpinBox();
	topshift->setRange(-100000, 100000);
	topshift->setSingleStep(1);
	topshift->setValue(0);
	topshift->setToolTip("Vertical shift of points between measured area and the optical image [number of points].");

	maxxlabel = new QLabel("Max X: ");

	maxx = new QSpinBox();
	maxx->setRange(1, 100000);
	maxx->setSingleStep(1);
	maxx->setValue(500);
	maxx->setToolTip("The X coordinate of the bottom-right corner of an optical image [number of points]; see manual for vendor specific details.");

	maxylabel = new QLabel("Max Y: ");

	maxy = new QSpinBox();
	maxy->setRange(1, 100000);
	maxy->setSingleStep(1);
	maxy->setValue(500);
	maxy->setToolTip("The Y coordinate of the bottom-right corner of an optical image [number of points]; see manual for vendor specific details.");


	setmaxbutton = new QPushButton(" Correlate ");
	setmaxbutton->setToolTip("Correlate the measured region of compounds with optical image.");

	setdefaultbutton = new QPushButton(" Default ");
	setdefaultbutton->setToolTip("Use the default values from input imzML file.");

	maxcoordinateshbox = new QHBoxLayout();
	maxcoordinateshbox->addWidget(leftshiftlabel);
	maxcoordinateshbox->addWidget(leftshift);
	maxcoordinateshbox->addWidget(topshiftlabel);
	maxcoordinateshbox->addWidget(topshift);
	maxcoordinateshbox->addWidget(maxxlabel);
	maxcoordinateshbox->addWidget(maxx);
	maxcoordinateshbox->addWidget(maxylabel);
	maxcoordinateshbox->addWidget(maxy);
	maxcoordinateshbox->addWidget(setmaxbutton);
	maxcoordinateshbox->addWidget(setdefaultbutton);

	maxcoordinateswidget = new QWidget();
	maxcoordinateswidget->setLayout(maxcoordinateshbox);


	histologyxlabel = new QLabel("X: ");

	histologyx = new QSpinBox();
	histologyx->setRange(0, 100000);
	histologyx->setSingleStep(1);
	histologyx->setValue(0);
	histologyx->setToolTip("Horizontal position of histology image [pixels].");

	histologyylabel = new QLabel("Y: ");

	histologyy = new QSpinBox();
	histologyy->setRange(0, 100000);
	histologyy->setSingleStep(1);
	histologyy->setValue(0);
	histologyy->setToolTip("Vertical position of histology image [pixels].");

	histologywidthlabel = new QLabel("Width: ");

	histologywidth = new QSpinBox();
	histologywidth->setRange(0, 100000);
	histologywidth->setSingleStep(1);
	histologywidth->setValue(0);
	histologywidth->setToolTip("Width of histology image [pixels].");

	histologyheightlabel = new QLabel("Height: ");

	histologyheight = new QSpinBox();
	histologyheight->setRange(0, 100000);
	histologyheight->setSingleStep(1);
	histologyheight->setValue(0);
	histologyheight->setToolTip("Height of histology image [pixels].");

	histologyanglelabel = new QLabel("Angle: ");

	histologyangle = new QDoubleSpinBox();
	histologyangle->setDecimals(2);
	histologyangle->setRange(-360, 360);
	histologyangle->setSingleStep(1);
	histologyangle->setValue(0);
	histologyangle->setToolTip("Rotation angle of histology image [degrees].");

	histologyhbox = new QHBoxLayout();
	histologyhbox->addWidget(histologyxlabel);
	histologyhbox->addWidget(histologyx);
	histologyhbox->addWidget(histologyylabel);
	histologyhbox->addWidget(histologyy);
	histologyhbox->addWidget(histologywidthlabel);
	histologyhbox->addWidget(histologywidth);
	histologyhbox->addWidget(histologyheightlabel);
	histologyhbox->addWidget(histologyheight);
	histologyhbox->addWidget(histologyanglelabel);
	histologyhbox->addWidget(histologyangle);

	histologywidget = new QWidget();
	histologywidget->setLayout(histologyhbox);


	microscopyxlabel = new QLabel("X: ");

	microscopyx = new QDoubleSpinBox();
	microscopyx->setRange(0, 1000000);
	microscopyx->setDecimals(6);
	microscopyx->setSingleStep(1);
	microscopyx->setValue(0);
	microscopyx->setSuffix(" um");
	microscopyx->setToolTip("Horizontal position of microscopy image [micrometers].");

	microscopyylabel = new QLabel("Y: ");

	microscopyy = new QDoubleSpinBox();
	microscopyy->setRange(0, 1000000);
	microscopyy->setDecimals(6);
	microscopyy->setSingleStep(1);
	microscopyy->setValue(0);
	microscopyy->setSuffix(" um");
	microscopyy->setToolTip("Vertical position of microscopy image [micrometers].");

	microscopywidthlabel = new QLabel("Width: ");

	microscopywidth = new QDoubleSpinBox();
	microscopywidth->setRange(0, 1000000);
	microscopywidth->setDecimals(6);
	microscopywidth->setSingleStep(1);
	microscopywidth->setValue(0);
	microscopywidth->setSuffix(" um");
	microscopywidth->setToolTip("Width of microscopy image [micrometers].");

	microscopyheightlabel = new QLabel("Height: ");

	microscopyheight = new QDoubleSpinBox();
	microscopyheight->setRange(0, 1000000);
	microscopyheight->setDecimals(6);
	microscopyheight->setSingleStep(1);
	microscopyheight->setValue(0);
	microscopyheight->setSuffix(" um");
	microscopyheight->setToolTip("Height of microscopy image [micrometers].");

	microscopyanglelabel = new QLabel("Angle: ");

	microscopyangle = new QDoubleSpinBox();
	microscopyangle->setDecimals(2);
	microscopyangle->setRange(-360, 360);
	microscopyangle->setSingleStep(1);
	microscopyangle->setValue(0);
	microscopyangle->setToolTip("Rotation angle of microscopy image [degrees].");

	microscopydefaultbutton = new QPushButton(" Default ");
	microscopydefaultbutton->setToolTip("Set default position of microscopy image.");

	microscopygobutton = new QPushButton(" Go ");
	microscopygobutton->setToolTip("Go to position.");

	microscopyhbox = new QHBoxLayout();
	microscopyhbox->addWidget(microscopyxlabel);
	microscopyhbox->addWidget(microscopyx);
	microscopyhbox->addWidget(microscopyylabel);
	microscopyhbox->addWidget(microscopyy);
	microscopyhbox->addWidget(microscopywidthlabel);
	microscopyhbox->addWidget(microscopywidth);
	microscopyhbox->addWidget(microscopyheightlabel);
	microscopyhbox->addWidget(microscopyheight);
	microscopyhbox->addWidget(microscopyanglelabel);
	microscopyhbox->addWidget(microscopyangle);
	microscopyhbox->addWidget(microscopydefaultbutton);
	microscopyhbox->addWidget(microscopygobutton);

	microscopywidget = new QWidget();
	microscopywidget->setLayout(microscopyhbox);


	image = new QImage();
	histologyimage = new QImage();

	microscopylayers.resize(layer_end);

	imagewindowwidget = new cImageWindowWidget();

	layerslayout = new QGridLayout();
	layerslayout->setAlignment(Qt::AlignTop);
	connect(this, SIGNAL(layerChanged(int, bool, int, int)), imagewindowwidget, SLOT(changeLayer(int, bool, int, int)));
	connect(this, SIGNAL(activeLayerChanged(int)), imagewindowwidget, SLOT(changeActiveLayer(int)));

	layerscount = 0;
	activelayer = 0;

	addLayer("Compounds");
	addLayer("Optical Image");
	addLayer("Histology Image");
#ifdef MICROSCOPY_IMAGES
	addLayer("Microscopy Image (nav.)");
	addLayer("Microscopy Image 1");
	addLayer("Microscopy Image 2");
	addLayer("Microscopy Image 3");
	addLayer("Microscopy Image 4");
	addLayer("Microscopy Image 5");
	addLayer("Microscopy Image 6");
	addLayer("Microscopy Image 7");
	addLayer("Microscopy Image 8");
	addLayer("Microscopy Image 9");
	addLayer("Microscopy Image 10");
#endif
	colorSpinBoxes(0);

	layerswidget = new QWidget();
	layerswidget->setLayout(layerslayout);

	mainwidget = new QSplitter();
	mainwidget->setOrientation(Qt::Horizontal);
	mainwidget->addWidget(imagewindowwidget);
	mainwidget->addWidget(layerswidget);
	mainwidget->setStretchFactor(0, 9);
	mainwidget->setStretchFactor(1, 1);

	connect(setregionbutton, SIGNAL(released()), this, SLOT(setRegionButtonReleased()));
	connect(resetregionbutton, SIGNAL(released()), this, SLOT(resetSelection()));

	connect(setmaxbutton, SIGNAL(released()), this, SLOT(setMaxButtonReleased()));
	connect(setdefaultbutton, SIGNAL(released()), this, SLOT(setDefaultButtonReleased()));

	connect(histologyx, SIGNAL(valueChanged(int)), this, SLOT(histologyPositionChanged(int)));
	connect(histologyy, SIGNAL(valueChanged(int)), this, SLOT(histologyPositionChanged(int)));
	connect(histologywidth, SIGNAL(valueChanged(int)), this, SLOT(histologyPositionChanged(int)));
	connect(histologyheight, SIGNAL(valueChanged(int)), this, SLOT(histologyPositionChanged(int)));
	connect(histologyangle, SIGNAL(valueChanged(double)), this, SLOT(histologyAngleChanged(double)));

	connect(microscopyx, SIGNAL(valueChanged(double)), this, SLOT(microscopyPositionChanged(double)));
	connect(microscopyy, SIGNAL(valueChanged(double)), this, SLOT(microscopyPositionChanged(double)));
	connect(microscopywidth, SIGNAL(valueChanged(double)), this, SLOT(microscopyPositionChanged(double)));
	connect(microscopyheight, SIGNAL(valueChanged(double)), this, SLOT(microscopyPositionChanged(double)));
	connect(microscopyangle, SIGNAL(valueChanged(double)), this, SLOT(microscopyAngleChanged(double)));
	connect(microscopydefaultbutton, SIGNAL(released()), this, SLOT(microscopyDefaultButtonReleased()));
	connect(microscopygobutton, SIGNAL(released()), this, SLOT(microscopyGoButtonReleased()));

	toolbarFile = addToolBar(tr("File"));
				
	actionOpenImage = new QAction(QIcon(":/images/icons/52.png"), tr("&Open Image"), this);
	actionOpenImage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	actionOpenImage->setToolTip("Open Image (Ctrl + O)");
	toolbarFile->addAction(actionOpenImage);
	connect(actionOpenImage, SIGNAL(triggered()), this, SLOT(selectImageType()));

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

	actionShowSelection = new QAction(QIcon(":/images/icons/rectangle.png"), tr("Show S&election"), this);
	actionShowSelection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	actionShowSelection->setToolTip("Show Selection (Ctrl + E)");
	actionShowSelection->setCheckable(true);
	toolbarView->addAction(actionShowSelection);
	connect(actionShowSelection, SIGNAL(toggled(bool)), imagewindowwidget, SLOT(showSelectionStateChanged(bool)));
				
	actionColorScale = new QAction(QIcon(":/images/icons/50.png"), tr("&Color Scale and Ruler"), this);
	actionColorScale->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
	actionColorScale->setToolTip("Color Scale and Ruler (Ctrl + C)");
	actionColorScale->setCheckable(true);
	actionColorScale->setChecked(true);
	toolbarView->addAction(actionColorScale);
	connect(actionColorScale, SIGNAL(toggled(bool)), imagewindowwidget, SLOT(colorScaleStateChanged(bool)));

	actionAbsoluteIntensity = new QAction(QIcon(":/images/icons/11.png"), tr("Absolute &Intensity"), this);
	actionAbsoluteIntensity->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionAbsoluteIntensity->setToolTip("Absolute Intensity (Ctrl + I)");
	actionAbsoluteIntensity->setCheckable(true);
	toolbarView->addAction(actionAbsoluteIntensity);
	connect(actionAbsoluteIntensity, SIGNAL(toggled(bool)), imagewindowwidget, SLOT(absoluteIntensityStateChanged(bool)));

	actionZoomIn = new QAction(QIcon(":/images/icons/83.png"), tr("Zoom &In"), this);
	actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
	actionZoomIn->setToolTip("Zoom In (Ctrl +)");
	toolbarView->addSeparator();
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

	rulerLabel = new QLabel("  Ruler Size: ");
	toolbarView->addSeparator();
	toolbarView->addWidget(rulerLabel);

	rulerValue = new QDoubleSpinBox();
	rulerValue->setRange(0.000001, 1000000);
	rulerValue->setDecimals(6);
	rulerValue->setSingleStep(1);
	rulerValue->setValue(1000);
	rulerValue->setSuffix(" um");
	rulerValue->setToolTip("Ruler size [micrometers]");
	toolbarView->addWidget(rulerValue);
	connect(imagewindowwidget, SIGNAL(updateRuler(double)), this, SLOT(updateRulerSlot(double)));
	connect(rulerValue, SIGNAL(valueChanged(double)), imagewindowwidget, SLOT(setRulerValue(double)));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	addToolBarBreak();

	toolbarRegion = addToolBar(tr("Compounds"));
	toolbarRegion->addWidget(regionwidget);

	toolbarMaxCoordinates = addToolBar(tr("Optical Image"));
	toolbarMaxCoordinates->addWidget(maxcoordinateswidget);

	addToolBarBreak();

	toolbarHistology = addToolBar(tr("Histology Image"));

	actionFlipHistologyHorizontal = new QAction(QIcon(":/images/icons/flip_horizontal.png"), tr("Flip Histology &Horizontally"), this);
	actionFlipHistologyHorizontal->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
	actionFlipHistologyHorizontal->setToolTip("Flip Histology Horizontally (Ctrl + H)");
	actionFlipHistologyHorizontal->setCheckable(true);
	toolbarHistology->addAction(actionFlipHistologyHorizontal);
	connect(actionFlipHistologyHorizontal, SIGNAL(toggled(bool)), imagewindowwidget, SLOT(flipHistologyHorizontallyStateChanged(bool)));

	actionFlipHistologyVertical = new QAction(QIcon(":/images/icons/flip_vertical.png"), tr("Flip Histology &Vertically"), this);
	actionFlipHistologyVertical->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
	actionFlipHistologyVertical->setToolTip("Flip Histology Vertically (Ctrl + V)");
	actionFlipHistologyVertical->setCheckable(true);
	toolbarHistology->addAction(actionFlipHistologyVertical);
	connect(actionFlipHistologyVertical, SIGNAL(toggled(bool)), imagewindowwidget, SLOT(flipHistologyVerticallyStateChanged(bool)));

	toolbarHistology->addSeparator();
	toolbarHistology->addWidget(histologywidget);

#ifdef MICROSCOPY_IMAGES
	addToolBarBreak();

	toolbarMicroscopy = addToolBar(tr("Microscopy Image"));
	toolbarMicroscopy->addWidget(microscopywidget);
#endif

	menuFile->addAction(actionOpenImage);
	menuFile->addAction(actionSaveImage);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuView->addAction(actionShowSelection);
	menuView->addAction(actionColorScale);
	menuView->addAction(actionAbsoluteIntensity);
	menuView->addSeparator();
	menuView->addAction(actionZoomIn);
	menuView->addAction(actionZoomOut);
	menuView->addAction(actionZoomReset);
	menuView->addSeparator();
	menuView->addAction(actionFlipHistologyHorizontal);
	menuView->addAction(actionFlipHistologyVertical);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	setCentralWidget(mainwidget);
	centralWidget()->setContentsMargins(0, 0, 0, 0);

	connect(imagewindowwidget, SIGNAL(updateFilter(int, int, int, int)), this, SLOT(updateSelection(int, int, int, int)));
	connect(imagewindowwidget, SIGNAL(updateFilter(int, int, int, int)), this->parent, SLOT(updateSummaryPeaksTableFilterSlot(int, int, int, int)));
	connect(this, SIGNAL(updateSummaryPeaksTableFilter(int, int, int, int)), this->parent, SLOT(updateSummaryPeaksTableFilterSlot(int, int, int, int)));
	connect(imagewindowwidget, SIGNAL(updateCorrelation(int, int, int, int)), this, SLOT(updatePointsArea(int, int, int, int)));
	connect(imagewindowwidget, SIGNAL(updateHistologyPosition(int, int, int, int, double)), this, SLOT(updateHistologySlot(int, int, int, int, double)));
	connect(imagewindowwidget, SIGNAL(updateMicroscopyPosition(double, double, double, double, double)), this, SLOT(updateMicroscopySlot(double, double, double, double, double)));

	resize(1280, 770);

	lastimagedir = "./";
	
	defaultmaxx = 1;
	defaultmaxy = 1;
	pixelsize = 1;
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

	delete leftshiftlabel;
	delete leftshift;
	delete topshiftlabel;
	delete topshift;
	delete maxxlabel;
	delete maxx;
	delete maxylabel;
	delete maxy;
	delete setmaxbutton;
	delete setdefaultbutton;
	delete maxcoordinateshbox;
	delete maxcoordinateswidget;

	delete histologyxlabel;
	delete histologyx;
	delete histologyylabel;
	delete histologyy;
	delete histologywidthlabel;
	delete histologywidth;
	delete histologyheightlabel;
	delete histologyheight;
	delete histologyanglelabel;
	delete histologyangle;
	delete histologyhbox;
	delete histologywidget;

	delete microscopyxlabel;
	delete microscopyx;
	delete microscopyylabel;
	delete microscopyy;
	delete microscopywidthlabel;
	delete microscopywidth;
	delete microscopyheightlabel;
	delete microscopyheight;
	delete microscopyanglelabel;
	delete microscopyangle;
	delete microscopydefaultbutton;
	delete microscopygobutton;
	delete microscopyhbox;
	delete microscopywidget;

	delete image;
	delete histologyimage;

	delete imagewindowwidget;
	delete layerslayout;

	delete layerswidget;

	delete mainwidget;

	delete rulerLabel;
	delete rulerValue;

	delete actionOpenImage;
	delete actionSaveImage;
	delete actionCloseWindow;
	delete actionShowSelection;
	delete actionColorScale;
	delete actionAbsoluteIntensity;
	delete actionZoomIn;
	delete actionZoomOut;
	delete actionZoomReset;
	delete actionFlipHistologyHorizontal;
	delete actionFlipHistologyVertical;
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


void cImageWindow::setDefaultMaxXY(int defaultmaxx, int defaultmaxy, int pixelsize, eVendorType vendor) {
	this->defaultmaxx = defaultmaxx;
	this->defaultmaxy = defaultmaxy;
	this->pixelsize = pixelsize;
	this->vendor = vendor;
	imagewindowwidget->setDefaultMaxXY(defaultmaxx, defaultmaxy, pixelsize, vendor);
}


void cImageWindow::addLayer(QString name) {
	QRadioButton* layerradiobutton = new QRadioButton();
	layerradiobutton->setToolTip("Active Layer");

	QCheckBox* layercheckbox = new QCheckBox();
	layercheckbox->setToolTip("Show/Hide Layer");

	if (layerscount == 0) {
		layerradiobutton->setChecked(true);
		layercheckbox->setChecked(true);
	}

	QLabel* layerlabel = new QLabel();
	layerlabel->setText(name);

	QSlider* layerslider = new QSlider(Qt::Horizontal);
	layerslider->setTickPosition(QSlider::TicksBelow);
	layerslider->setRange(0, 100);
	layerslider->setSingleStep(1);
	layerslider->setValue(100);
	layerslider->setToolTip("Opacity of Layer");

	QSpinBox* layerspinbox = new QSpinBox();
	layerspinbox->setRange(0, 100);
	layerspinbox->setSingleStep(1);
	layerspinbox->setValue(100);
	layerspinbox->setToolTip("Opacity of Layer");

	QSpinBox* layerzvalue = new QSpinBox();
	layerzvalue->setRange(1, 999);
	layerzvalue->setSingleStep(1);

	if (layerscount == 0) {
		layerzvalue->setValue(2);
	}
	else if (layerscount == 1) {
		layerzvalue->setValue(1);
	}
	else {
		layerzvalue->setValue(layerscount + 1);
	}
	layerzvalue->setToolTip("Order of Layer");

	// the ownership is taken by QGridLayout
	layerslayout->addWidget(layerradiobutton, layerscount, 0);
	layerslayout->addWidget(layercheckbox, layerscount, 1);
	layerslayout->addWidget(layerlabel, layerscount, 2);
	layerslayout->addWidget(layerslider, layerscount, 3);
	layerslayout->addWidget(layerspinbox, layerscount, 4);
	layerslayout->addWidget(layerzvalue, layerscount, 5);

	connect(layerspinbox, SIGNAL(valueChanged(int)), layerslider, SLOT(setValue(int)));
	connect(layerslider, SIGNAL(valueChanged(int)), layerspinbox, SLOT(setValue(int)));

	connect(layerradiobutton, SIGNAL(toggled(bool)), this, SLOT(activeLayerChangedSlot(bool)));

	connect(layerspinbox, SIGNAL(valueChanged(int)), this, SLOT(layerSpinBoxChangedSlot(int)));
	connect(layercheckbox, SIGNAL(stateChanged(int)), this, SLOT(layerCheckBoxChangedSlot(int)));

	connect(layerzvalue, SIGNAL(valueChanged(int)), this, SLOT(layerZValueChangedSlot(int)));

	emit layerChanged(layerscount, layercheckbox->isChecked(), layerspinbox->value(), layerzvalue->value());
	
	layerscount++;
}


void cImageWindow::colorSpinBoxes(int layerid) {

	if ((eLayerType)layerid == layer_compounds) {
		xfrom->setStyleSheet("QSpinBox { background-color: lime; }");
		xto->setStyleSheet("QSpinBox { background-color: lime; }");
		yfrom->setStyleSheet("QSpinBox { background-color: lime; }");
		yto->setStyleSheet("QSpinBox { background-color: lime; }");
	}
	else {
		xfrom->setStyleSheet("QSpinBox { background-color: white; }");
		xto->setStyleSheet("QSpinBox { background-color: white; }");
		yfrom->setStyleSheet("QSpinBox { background-color: white; }");
		yto->setStyleSheet("QSpinBox { background-color: white; }");
	}

	if ((eLayerType)layerid == layer_optical_image) {
		leftshift->setStyleSheet("QSpinBox { background-color: lime; }");
		topshift->setStyleSheet("QSpinBox { background-color: lime; }");
		maxx->setStyleSheet("QSpinBox { background-color: lime; }");
		maxy->setStyleSheet("QSpinBox { background-color: lime; }");
	}
	else {
		leftshift->setStyleSheet("QSpinBox { background-color: white; }");
		topshift->setStyleSheet("QSpinBox { background-color: white; }");
		maxx->setStyleSheet("QSpinBox { background-color: white; }");
		maxy->setStyleSheet("QSpinBox { background-color: white; }");
	}

	if ((eLayerType)layerid == layer_histology_image) {
		histologyx->setStyleSheet("QSpinBox { background-color: lime; }");
		histologyy->setStyleSheet("QSpinBox { background-color: lime; }");
		histologywidth->setStyleSheet("QSpinBox { background-color: lime; }");
		histologyheight->setStyleSheet("QSpinBox { background-color: lime; }");
		histologyangle->setStyleSheet("QDoubleSpinBox { background-color: lime; }");
	}
	else {
		histologyx->setStyleSheet("QSpinBox { background-color: white; }");
		histologyy->setStyleSheet("QSpinBox { background-color: white; }");
		histologywidth->setStyleSheet("QSpinBox { background-color: white; }");
		histologyheight->setStyleSheet("QSpinBox { background-color: white; }");
		histologyangle->setStyleSheet("QDoubleSpinBox { background-color: white; }");
	}

	if (((eLayerType)layerid >= layer_microscopy_navigation_image) && (((eLayerType)layerid < layer_end))) {
		microscopyx->setStyleSheet("QDoubleSpinBox { background-color: lime; }");
		microscopyy->setStyleSheet("QDoubleSpinBox { background-color: lime; }");
		microscopywidth->setStyleSheet("QDoubleSpinBox { background-color: lime; }");
		microscopyheight->setStyleSheet("QDoubleSpinBox { background-color: lime; }");
		microscopyangle->setStyleSheet("QDoubleSpinBox { background-color: lime; }");
	}
	else {
		microscopyx->setStyleSheet("QDoubleSpinBox { background-color: white; }");
		microscopyy->setStyleSheet("QDoubleSpinBox { background-color: white; }");
		microscopywidth->setStyleSheet("QDoubleSpinBox { background-color: white; }");
		microscopyheight->setStyleSheet("QDoubleSpinBox { background-color: white; }");
		microscopyangle->setStyleSheet("QDoubleSpinBox { background-color: white; }");
	}

}


void cImageWindow::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {

		if (xfrom->hasFocus() || xto->hasFocus() || yfrom->hasFocus() || yto->hasFocus()) {
			setregionbutton->click();
		}

		if (leftshift->hasFocus() || topshift->hasFocus() || maxx->hasFocus() || maxy->hasFocus()) {
			setmaxbutton->click();
		}

	}
	event->accept();
}


void cImageWindow::closeWindow() {
	hide();
}


void cImageWindow::selectImageType() {
	QStringList imagetypes;
	imagetypes << "Optical Image";
	imagetypes << "Histology Image";
#ifdef MICROSCOPY_IMAGES
	imagetypes << "Microscopy Image (navigation)";
	imagetypes << "Microscopy Image 1";
	imagetypes << "Microscopy Image 2";
	imagetypes << "Microscopy Image 3";
	imagetypes << "Microscopy Image 4";
	imagetypes << "Microscopy Image 5";
	imagetypes << "Microscopy Image 6";
	imagetypes << "Microscopy Image 7";
	imagetypes << "Microscopy Image 8";
	imagetypes << "Microscopy Image 9";
	imagetypes << "Microscopy Image 10";
#endif

	QInputDialog input;
	input.setInputMode(QInputDialog::TextInput);
	input.setOptions(QInputDialog::UseListViewForComboBoxItems);
	input.setComboBoxItems(imagetypes);
	input.setWindowTitle("Open Image...");
	input.setLabelText("Select Image Type:");

	connect(&input, SIGNAL(textValueSelected(const QString &)), this, SLOT(imageTypeSelected(const QString &)));

	input.exec();
}


void cImageWindow::imageTypeSelected(const QString &s) {
	if (s.compare("Optical Image") == 0) {
		openOpticalImage();
	}
	if (s.compare("Histology Image") == 0) {
		openHistologyImage();
	}
	if (s.compare("Microscopy Image (navigation)") == 0) {
		openMicroscopyImage(layer_microscopy_navigation_image, s);
	}
	if (s.compare("Microscopy Image 1") == 0) {
		openMicroscopyImage(layer_microscopy_image_1, s);
	}
	if (s.compare("Microscopy Image 2") == 0) {
		openMicroscopyImage(layer_microscopy_image_2, s);
	}
	if (s.compare("Microscopy Image 3") == 0) {
		openMicroscopyImage(layer_microscopy_image_3, s);
	}
	if (s.compare("Microscopy Image 4") == 0) {
		openMicroscopyImage(layer_microscopy_image_4, s);
	}
	if (s.compare("Microscopy Image 5") == 0) {
		openMicroscopyImage(layer_microscopy_image_5, s);
	}
	if (s.compare("Microscopy Image 6") == 0) {
		openMicroscopyImage(layer_microscopy_image_6, s);
	}
	if (s.compare("Microscopy Image 7") == 0) {
		openMicroscopyImage(layer_microscopy_image_7, s);
	}
	if (s.compare("Microscopy Image 8") == 0) {
		openMicroscopyImage(layer_microscopy_image_8, s);
	}
	if (s.compare("Microscopy Image 9") == 0) {
		openMicroscopyImage(layer_microscopy_image_9, s);
	}
	if (s.compare("Microscopy Image 10") == 0) {
		openMicroscopyImage(layer_microscopy_image_10, s);
	}
}


void cImageWindow::openOpticalImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Optical Image ..."), lastimagedir, tr("Image Files (*.jpg *.jpeg *.png *.tif *.tiff *.bmp *.gif)"));

	if (!filename.isEmpty()) {
		lastimagedir = filename;

		image->load(filename);

		leftshift->setValue(0);
		topshift->setValue(0);
		maxx->setValue(defaultmaxx);
		maxy->setValue(defaultmaxy);
		
		setMaxButtonReleased();

		imagewindowwidget->setOpticalImage(image);

		((QCheckBox *)layerslayout->itemAtPosition((int)layer_optical_image, 1)->widget())->setChecked(true);
	}
}


void cImageWindow::openHistologyImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Histology Image ..."), lastimagedir, tr("Image Files (*.jpg *.jpeg *.png *.tif *.tiff *.bmp *.gif)"));

	if (!filename.isEmpty()) {
		lastimagedir = filename;

		histologyimage->load(filename);
		imagewindowwidget->setHistologyImage(histologyimage);

		((QRadioButton *)layerslayout->itemAtPosition((int)layer_histology_image, 0)->widget())->setChecked(true);
		((QCheckBox *)layerslayout->itemAtPosition((int)layer_histology_image, 1)->widget())->setChecked(true);

		histologyx->blockSignals(true);
		histologyy->blockSignals(true);
		histologywidth->blockSignals(true);
		histologyheight->blockSignals(true);
		histologyangle->blockSignals(true);

		histologyx->setValue(0);
		histologyy->setValue(0);
		histologywidth->setValue(400);
		histologyheight->setValue((int)((double)histologyimage->height() / (double)histologyimage->width() * (double)histologywidth->value()));
		histologyangle->setValue(0);

		histologyx->blockSignals(false);
		histologyy->blockSignals(false);
		histologywidth->blockSignals(false);
		histologyheight->blockSignals(false);
		histologyangle->blockSignals(false);

		histologyChanged();
	}
}


bool cImageWindow::parseMicroscopyImage(QString& filename, double& pixelwidth, double& pixelheight, int& resolutionx, int& resolutiony, double& stagex, double& stagey) {
	ifstream is;
	char buffer;
	bool tags = false;
	string desc = "";
	size_t pos;

	is.open(filename.toStdString(), ios::binary);
	while (is.good()) {
		is.read(&buffer, 1);
		if (!tags && (buffer == '[')) {
			is.read(&buffer, 1);
			if (buffer == 'U') {
				is.read(&buffer, 1);
				if (buffer == 's') {
					is.read(&buffer, 1);
					if (buffer == 'e') {
						is.read(&buffer, 1);
						if (buffer == 'r') {
							is.read(&buffer, 1);
							if (buffer == ']') {
								tags = true;
								continue;
							}
						}
					}
				}
			}
		}
		if (tags) {
			desc += buffer;
		}
	}
	is.close();

	if (!tags) {
		return false;
	}

	pos = desc.find("[Scan]");
	if (pos != string::npos) {
		pos = desc.find("PixelWidth=", pos);
		if (pos != string::npos) {
			sscanf_s(desc.substr(pos + 11).c_str(), "%lf", &pixelwidth);
		}
	}

	pos = desc.find("[Scan]");
	if (pos != string::npos) {
		pos = desc.find("PixelHeight=", pos);
		if (pos != string::npos) {
			sscanf_s(desc.substr(pos + 12).c_str(), "%lf", &pixelheight);
		}
	}

	pos = desc.find("[Image]");
	if (pos != string::npos) {
		pos = desc.find("ResolutionX=", pos);
		if (pos != string::npos) {
			sscanf_s(desc.substr(pos + 12).c_str(), "%i", &resolutionx);
		}
	}

	pos = desc.find("[Image]");
	if (pos != string::npos) {
		pos = desc.find("ResolutionY=", pos);
		if (pos != string::npos) {
			sscanf_s(desc.substr(pos + 12).c_str(), "%i", &resolutiony);
		}
	}

	pos = desc.find("[Stage]");
	if (pos != string::npos) {
		pos = desc.find("StageX=", pos);
		if (pos != string::npos) {
			sscanf_s(desc.substr(pos + 7).c_str(), "%lf", &stagex);
		}
	}

	pos = desc.find("[Stage]");
	if (pos != string::npos) {
		pos = desc.find("StageY=", pos);
		if (pos != string::npos) {
			sscanf_s(desc.substr(pos + 7).c_str(), "%lf", &stagey);
		}
	}

	//cout << desc << endl;

	return true;
}


void cImageWindow::openMicroscopyImage(eLayerType layer, const QString &layername) {
	QString title = "Open " + layername + " ...";
	QString filename;

	filename = QFileDialog::getOpenFileName(this, tr(title.toStdString().c_str()), lastimagedir, tr("Image Files (*.jpg *.jpeg *.png *.tif *.tiff *.bmp *.gif)"));

	if (!filename.isEmpty()) {
		lastimagedir = filename;

		int microscopycountx, microscopycounty;
		double microscopypixelwidth, microscopypixelheight, stagex, stagey;

		if (parseMicroscopyImage(filename, microscopypixelwidth, microscopypixelheight, microscopycountx, microscopycounty, stagex, stagey)) {
			QImage image;
			image.load(filename);

			*microscopylayers[layer].image = image.copy(0, 0, microscopycountx, microscopycounty);
			imagewindowwidget->setMicroscopyImage(layer, microscopylayers[layer].image);

			((QRadioButton *)layerslayout->itemAtPosition((int)layer, 0)->widget())->setChecked(true);
			((QCheckBox *)layerslayout->itemAtPosition((int)layer, 1)->widget())->setChecked(true);

			microscopylayers[layer].defaultwidth = (double)microscopycountx * microscopypixelwidth * 1000000.0;
			microscopylayers[layer].defaultheight = (double)microscopycounty * microscopypixelheight * 1000000.0;

			microscopylayers[layer].stagex = stagex;
			microscopylayers[layer].stagey = stagey;

			microscopyDefaultButtonReleased();
		}
		else {
			microscopylayers[layer].image->load(filename);
			imagewindowwidget->setMicroscopyImage(layer, microscopylayers[layer].image);

			((QRadioButton *)layerslayout->itemAtPosition((int)layer, 0)->widget())->setChecked(true);
			((QCheckBox *)layerslayout->itemAtPosition((int)layer, 1)->widget())->setChecked(true);

			microscopylayers[layer].defaultwidth = microscopylayers[layer].image->width();
			microscopylayers[layer].defaultheight = microscopylayers[layer].image->height();

			microscopylayers[layer].stagex = 0;
			microscopylayers[layer].stagex = 0;

			microscopyDefaultButtonReleased();
				
			QMessageBox msgBox;
			QString errstr = "The microscopy image does not include any metadata.\nThe width and height of the image must be set up manually !";
			msgBox.setText(errstr);
			msgBox.exec();
		}
	
	}
}


void cImageWindow::saveImage() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image ..."), "./", "PNG Files (*.png)");

	if (!filename.isEmpty()) {
		lastimagedir = filename;
		imagewindowwidget->getImage().save(filename, "PNG");
	}
}


void cImageWindow::setFilterOptionsSlot(vector<cCoordinates> coordinates, string columnname, string comparatorname, string filterstring, bool casesensitive, bool wholeword) {
	imagewindowwidget->setFilterOptions(coordinates, columnname, comparatorname, filterstring, casesensitive, wholeword);
}


void cImageWindow::setMaxButtonReleased() {
	imagewindowwidget->setCorrelationValues(leftshift->value(), topshift->value(), maxx->value(), maxy->value());
	resetregionbutton->click();
}


void cImageWindow::setDefaultButtonReleased() {
	leftshift->setValue(0);
	topshift->setValue(0);
	maxx->setValue(defaultmaxx);
	maxy->setValue(defaultmaxy);
	imagewindowwidget->setCorrelationValues(leftshift->value(), topshift->value(), maxx->value(), maxy->value());
	resetregionbutton->click();
}


void cImageWindow::setRegionButtonReleased() {
	imagewindowwidget->setSelectedRegion(xfrom->value(), xto->value(), yfrom->value(), yto->value());
	emit updateSummaryPeaksTableFilter(xfrom->value(), xto->value(), yfrom->value(), yto->value());
}


void cImageWindow::histologyChanged() {
	imagewindowwidget->setHistologyPosition(histologyx->value(), histologyy->value(), histologywidth->value(), histologyheight->value(), histologyangle->value());
	imagewindowwidget->redraw();
}


void cImageWindow::histologyPositionChanged(int value) {
	histologyChanged();
}


void cImageWindow::histologyAngleChanged(double value) {
	histologyChanged();
}


void cImageWindow::microscopyChanged() {
	if ((activelayer >= layer_microscopy_navigation_image) && (activelayer < layer_end)) {
		microscopylayers[activelayer].x = microscopyx->value();
		microscopylayers[activelayer].y = microscopyy->value();
		microscopylayers[activelayer].width = microscopywidth->value();
		microscopylayers[activelayer].height = microscopyheight->value();
		microscopylayers[activelayer].angle = microscopyangle->value();
		imagewindowwidget->setMicroscopyPosition((eLayerType)activelayer, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);

		if (activelayer == layer_microscopy_navigation_image) {
			double widthratio, heightratio;
			for (int i = layer_microscopy_navigation_image + 1; i < layer_end; i++) {
				if (!microscopylayers[i].image->size().isEmpty()) {
					widthratio = microscopylayers[layer_microscopy_navigation_image].width / microscopylayers[layer_microscopy_navigation_image].defaultwidth;
					heightratio = microscopylayers[layer_microscopy_navigation_image].height / microscopylayers[layer_microscopy_navigation_image].defaultheight;

					microscopylayers[i].x = microscopylayers[layer_microscopy_navigation_image].x + (microscopylayers[layer_microscopy_navigation_image].width - microscopylayers[i].defaultwidth * widthratio) / 2.0 + (microscopylayers[i].stagex - microscopylayers[layer_microscopy_navigation_image].stagex) * widthratio * 1000000.0;
					microscopylayers[i].y = microscopylayers[layer_microscopy_navigation_image].y + (microscopylayers[layer_microscopy_navigation_image].height - microscopylayers[i].defaultheight * heightratio) / 2.0 + (microscopylayers[i].stagey - microscopylayers[layer_microscopy_navigation_image].stagey) * heightratio * 1000000.0;
					microscopylayers[i].width = microscopylayers[i].defaultwidth * widthratio;
					microscopylayers[i].height = microscopylayers[i].defaultheight * heightratio;
					imagewindowwidget->setMicroscopyPosition((eLayerType)i, microscopylayers[i].x, microscopylayers[i].y, microscopylayers[i].width, microscopylayers[i].height, microscopylayers[i].angle);
				}
			}
		}

		imagewindowwidget->redraw();
	}
}


void cImageWindow::microscopyDefaultButtonReleased() {
	if ((activelayer >= layer_microscopy_navigation_image) && (activelayer < layer_end)) {
		if (activelayer == layer_microscopy_navigation_image) {
			microscopylayers[activelayer].x = 0;
			microscopylayers[activelayer].y = 0;
			microscopylayers[activelayer].width = microscopylayers[activelayer].defaultwidth;
			microscopylayers[activelayer].height = microscopylayers[activelayer].defaultheight;
			microscopylayers[activelayer].angle = 0;
			imagewindowwidget->setMicroscopyPosition((eLayerType)activelayer, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);

			for (int i = layer_microscopy_navigation_image + 1; i < layer_end; i++) {
				if (!microscopylayers[i].image->size().isEmpty()) {
					microscopylayers[i].x = microscopylayers[layer_microscopy_navigation_image].x + (microscopylayers[layer_microscopy_navigation_image].width - microscopylayers[i].defaultwidth) / 2.0 + (microscopylayers[i].stagex - microscopylayers[layer_microscopy_navigation_image].stagex) * 1000000.0;
					microscopylayers[i].y = microscopylayers[layer_microscopy_navigation_image].y + (microscopylayers[layer_microscopy_navigation_image].height - microscopylayers[i].defaultheight) / 2.0 + (microscopylayers[i].stagey - microscopylayers[layer_microscopy_navigation_image].stagey) * 1000000.0;
					microscopylayers[i].width = microscopylayers[i].defaultwidth;
					microscopylayers[i].height = microscopylayers[i].defaultheight;
					microscopylayers[i].angle = 0;
					imagewindowwidget->setMicroscopyPosition((eLayerType)i, microscopylayers[i].x, microscopylayers[i].y, microscopylayers[i].width, microscopylayers[i].height, microscopylayers[i].angle);
				}
			}
		}

		if ((activelayer > layer_microscopy_navigation_image) && (activelayer < layer_end)) {
			microscopylayers[activelayer].x = microscopylayers[layer_microscopy_navigation_image].x + (microscopylayers[layer_microscopy_navigation_image].width - microscopylayers[activelayer].defaultwidth) / 2.0 + (microscopylayers[activelayer].stagex - microscopylayers[layer_microscopy_navigation_image].stagex) * 1000000.0;
			microscopylayers[activelayer].y = microscopylayers[layer_microscopy_navigation_image].y + (microscopylayers[layer_microscopy_navigation_image].height - microscopylayers[activelayer].defaultheight) / 2.0 + (microscopylayers[activelayer].stagey - microscopylayers[layer_microscopy_navigation_image].stagey) * 1000000.0;
			microscopylayers[activelayer].width = microscopylayers[activelayer].defaultwidth;
			microscopylayers[activelayer].height = microscopylayers[activelayer].defaultheight;
			microscopylayers[activelayer].angle = 0;
			imagewindowwidget->setMicroscopyPosition((eLayerType)activelayer, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);
		}

		microscopyx->blockSignals(true);
		microscopyy->blockSignals(true);
		microscopywidth->blockSignals(true);
		microscopyheight->blockSignals(true);
		microscopyangle->blockSignals(true);

		microscopyx->setValue(microscopylayers[activelayer].x);
		microscopyy->setValue(microscopylayers[activelayer].y);
		microscopywidth->setValue(microscopylayers[activelayer].width);
		microscopyheight->setValue(microscopylayers[activelayer].height);
		microscopyangle->setValue(microscopylayers[activelayer].angle);

		microscopyx->blockSignals(false);
		microscopyy->blockSignals(false);
		microscopywidth->blockSignals(false);
		microscopyheight->blockSignals(false);
		microscopyangle->blockSignals(false);

		imagewindowwidget->redraw();
	}
}


void cImageWindow::microscopyGoButtonReleased() {
	if ((activelayer >= layer_microscopy_navigation_image) && (activelayer < layer_end)) {
		imagewindowwidget->goToMicroscopyPosition((eLayerType)activelayer);
	}
}


void cImageWindow::microscopyPositionChanged(double value) {
	microscopyChanged();
}


void cImageWindow::microscopyAngleChanged(double value) {
	microscopyChanged();
}


void cImageWindow::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/crossvis.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/crossvis.html").absoluteFilePath()));
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
	imagewindowwidget->setSelectedRegion(xfrom->value(), xto->value(), yfrom->value(), yto->value());
	emit updateSummaryPeaksTableFilter(xfrom->value(), xto->value(), yfrom->value(), yto->value());
}


void cImageWindow::updatePointsArea(int xmin, int xmax, int ymin, int ymax) {
	leftshift->setValue(xmin);
	maxx->setValue(xmax);
	topshift->setValue(ymin);
	maxy->setValue(ymax);
	setMaxButtonReleased();
}


void cImageWindow::layerSpinBoxChangedSlot(int alpha) {
	int row, column, rowSpan, columnSpan;
	QSpinBox* spinbox = qobject_cast<QSpinBox*>(sender());

	layerslayout->getItemPosition(layerslayout->indexOf(spinbox), &row, &column, &rowSpan, &columnSpan);

	emit layerChanged(row, ((QCheckBox *)layerslayout->itemAtPosition(row, 1)->widget())->isChecked(), alpha, ((QSpinBox *)layerslayout->itemAtPosition(row, 5)->widget())->value());
}


void cImageWindow::layerZValueChangedSlot(int zvalue) {
	int row, column, rowSpan, columnSpan;
	QSpinBox* spinbox = qobject_cast<QSpinBox*>(sender());

	layerslayout->getItemPosition(layerslayout->indexOf(spinbox), &row, &column, &rowSpan, &columnSpan);

	emit layerChanged(row, ((QCheckBox *)layerslayout->itemAtPosition(row, 1)->widget())->isChecked(), ((QSpinBox *)layerslayout->itemAtPosition(row, 4)->widget())->value(), zvalue);
}


void cImageWindow::layerCheckBoxChangedSlot(int state) {
	int row, column, rowSpan, columnSpan;
	QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());

	layerslayout->getItemPosition(layerslayout->indexOf(checkbox), &row, &column, &rowSpan, &columnSpan);

	emit layerChanged(row, ((QCheckBox *)layerslayout->itemAtPosition(row, 1)->widget())->isChecked(), ((QSpinBox *)layerslayout->itemAtPosition(row, 4)->widget())->value(), ((QSpinBox *)layerslayout->itemAtPosition(row, 5)->widget())->value());
}


void cImageWindow::activeLayerChangedSlot(bool checked) {
	if (checked) {
		int row, column, rowSpan, columnSpan;
		QRadioButton* radiobutton = qobject_cast<QRadioButton*>(sender());

		layerslayout->getItemPosition(layerslayout->indexOf(radiobutton), &row, &column, &rowSpan, &columnSpan);

		colorSpinBoxes(row);

		activelayer = row;
		emit activeLayerChanged(row);

		microscopyx->blockSignals(true);
		microscopyy->blockSignals(true);
		microscopywidth->blockSignals(true);
		microscopyheight->blockSignals(true);
		microscopyangle->blockSignals(true);

		if ((activelayer >= layer_microscopy_navigation_image) && (activelayer < layer_end) && (!microscopylayers[activelayer].image->size().isEmpty())) {
			microscopyx->setValue(microscopylayers[activelayer].x);
			microscopyy->setValue(microscopylayers[activelayer].y);
			microscopywidth->setValue(microscopylayers[activelayer].width);
			microscopyheight->setValue(microscopylayers[activelayer].height);
			microscopyangle->setValue(microscopylayers[activelayer].angle);
		}
		else {
			microscopyx->setValue(0);
			microscopyy->setValue(0);
			microscopywidth->setValue(0);
			microscopyheight->setValue(0);
			microscopyangle->setValue(0);
		}

		microscopyx->blockSignals(false);
		microscopyy->blockSignals(false);
		microscopywidth->blockSignals(false);
		microscopyheight->blockSignals(false);
		microscopyangle->blockSignals(false);
	}
}


void cImageWindow::updateHistologySlot(int x, int y, int width, int height, double angle) {
	histologyx->blockSignals(true);
	histologyy->blockSignals(true);
	histologywidth->blockSignals(true);
	histologyheight->blockSignals(true);
	histologyangle->blockSignals(true);

	histologyx->setValue(x);
	histologyy->setValue(y);
	histologywidth->setValue(width);
	histologyheight->setValue(height);
	histologyangle->setValue(angle);

	histologyx->blockSignals(false);
	histologyy->blockSignals(false);
	histologywidth->blockSignals(false);
	histologyheight->blockSignals(false);
	histologyangle->blockSignals(false);

	histologyChanged();
}


void cImageWindow::updateMicroscopySlot(double x, double y, double width, double height, double angle) {
	microscopyx->blockSignals(true);
	microscopyy->blockSignals(true);
	microscopywidth->blockSignals(true);
	microscopyheight->blockSignals(true);
	microscopyangle->blockSignals(true);

	microscopyx->setValue(x);
	microscopyy->setValue(y);
	microscopywidth->setValue(width);
	microscopyheight->setValue(height);
	microscopyangle->setValue(angle);

	microscopyx->blockSignals(false);
	microscopyy->blockSignals(false);
	microscopywidth->blockSignals(false);
	microscopyheight->blockSignals(false);
	microscopyangle->blockSignals(false);

	microscopyChanged();
}


void cImageWindow::updateRulerSlot(double value) {
	rulerValue->setValue(value);
}

