#include "gui/cImageWindow.h"
#include "gui/cEventFilter.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QImage>
#include <QSpinBox>
#include <QMenuBar>
#include <QMenu>


cImageWindow::cImageWindow(cGlobalPreferences* globalpreferences, QWidget* parent) {
	editorname = "Image Fusion";

	this->globalpreferences = globalpreferences;
	this->parent = parent;

	redrawenabled = false;

	setWindowTitle(editorname);
	setWindowIcon(QIcon(":/images/icons/image_edit.png"));

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

	pixelsizexlabel = new QLabel("Pixel Width: ");

	pixelsizexspinbox = new QSpinBox();
	pixelsizexspinbox->setRange(1, 100000);
	pixelsizexspinbox->setSingleStep(1);
	pixelsizexspinbox->setValue(100);
	pixelsizexspinbox->setSuffix(" um");
	pixelsizexspinbox->setToolTip("Pixel width [um].");

	pixelsizeylabel = new QLabel("Pixel Height: ");

	pixelsizeyspinbox = new QSpinBox();
	pixelsizeyspinbox->setRange(1, 100000);
	pixelsizeyspinbox->setSingleStep(1);
	pixelsizeyspinbox->setValue(100);
	pixelsizeyspinbox->setSuffix(" um");
	pixelsizeyspinbox->setToolTip("Pixel height [um].");

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
	maxcoordinateshbox->addWidget(pixelsizexlabel);
	maxcoordinateshbox->addWidget(pixelsizexspinbox);
	maxcoordinateshbox->addWidget(pixelsizeylabel);
	maxcoordinateshbox->addWidget(pixelsizeyspinbox);
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
	microscopyx->setDecimals(3);
	microscopyx->setSingleStep(1);
	microscopyx->setValue(0);
	microscopyx->setSuffix(" um");
	microscopyx->setToolTip("Horizontal position of microscopy image [micrometers].");

	microscopyylabel = new QLabel("Y: ");

	microscopyy = new QDoubleSpinBox();
	microscopyy->setRange(0, 1000000);
	microscopyy->setDecimals(3);
	microscopyy->setSingleStep(1);
	microscopyy->setValue(0);
	microscopyy->setSuffix(" um");
	microscopyy->setToolTip("Vertical position of microscopy image [micrometers].");

	microscopywidthlabel = new QLabel("Width: ");

	microscopywidth = new QDoubleSpinBox();
	microscopywidth->setRange(0, 1000000);
	microscopywidth->setDecimals(3);
	microscopywidth->setSingleStep(1);
	microscopywidth->setValue(0);
	microscopywidth->setSuffix(" um");
	microscopywidth->setToolTip("Width of microscopy image [micrometers].");

	microscopyheightlabel = new QLabel("Height: ");

	microscopyheight = new QDoubleSpinBox();
	microscopyheight->setRange(0, 1000000);
	microscopyheight->setDecimals(3);
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

	microscopynavigationlabel = new QLabel("Navigation: ");
	microscopynavigationcombobox = new QComboBox();
	microscopynavigationcombobox->setToolTip("The navigation layer used for currently selected image.");
	updateMicroscopyNavigationCombobox(numberoflayers);

	microscopydefaultbutton = new QPushButton(" Default ");
	microscopydefaultbutton->setToolTip("Set default position of microscopy image.");

	microscopygobutton = new QPushButton(" Go ");
	microscopygobutton->setToolTip("Go to position.");

	microscopyprevbutton = new QPushButton("Previous");
	microscopyprevbutton->setToolTip("Go to previous position.");

	microscopynextbutton = new QPushButton("Next");
	microscopynextbutton->setToolTip("Go to next position.");

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
	microscopyhbox->addWidget(microscopynavigationlabel);
	microscopyhbox->addWidget(microscopynavigationcombobox);
	microscopyhbox->addWidget(microscopydefaultbutton);
	microscopyhbox->addWidget(microscopygobutton);
	microscopyhbox->addWidget(microscopyprevbutton);
	microscopyhbox->addWidget(microscopynextbutton);

	microscopywidget = new QWidget();
	microscopywidget->setLayout(microscopyhbox);


	image = new QImage();
	histologyimage = new QImage();

	microscopylayers.resize(numberoflayers);

	imagewindowwidget = new cImageWindowWidget();

	layerslayout = new QGridLayout();
	layerslayout->setAlignment(Qt::AlignTop);

	connect(this, SIGNAL(layerChanged(int, bool, int, int, bool)), imagewindowwidget, SLOT(changeLayer(int, bool, int, int, bool)));
	connect(this, SIGNAL(activeLayerChanged(int)), imagewindowwidget, SLOT(changeActiveLayer(int)));
	connect(imagewindowwidget, SIGNAL(imageWidgetDoubleClicked(int)), this, SLOT(imageDoubleClickedSlot(int)));

	layerscount = 0;
	activelayer = 0;

	addLayer("Compounds");
	addLayer("Optical Image");
	addLayer("Histology Image");
	addLayer("Microscopy Image (nav.)");

	QString microscopylayername = "Microscopy Image ";
	QString tmpname;
	for (int i = 0; i < numberoflayers - 4; i++) {
		tmpname = microscopylayername + QVariant(i + 1).toString();
		addLayer(tmpname);
	}

	colorSpinBoxes(0);

	layerswidget = new QWidget();
	layerswidget->setLayout(layerslayout);

	layersscrollarea = new QScrollArea();
	layersscrollarea->setWidgetResizable(true);
	layersscrollarea->setWidget(layerswidget);

	mainwidget = new QSplitter();
	mainwidget->setOrientation(Qt::Horizontal);
	mainwidget->addWidget(imagewindowwidget);
	mainwidget->addWidget(layersscrollarea);
	mainwidget->setStretchFactor(0, 7);
	mainwidget->setStretchFactor(1, 3);

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
	connect(microscopynavigationcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(microscopyNavigationChanged(int)));
	connect(microscopydefaultbutton, SIGNAL(released()), this, SLOT(microscopyDefaultButtonReleased()));
	connect(microscopygobutton, SIGNAL(released()), this, SLOT(microscopyGoButtonReleased()));
	connect(microscopyprevbutton, SIGNAL(released()), this, SLOT(microscopyPrevButtonReleased()));
	connect(microscopynextbutton, SIGNAL(released()), this, SLOT(microscopyNextButtonReleased()));

	toolbarFile = addToolBar(tr("File"));
				
	actionLoadLayers = new QAction(QIcon(":/images/icons/52.png"), tr("&Load Layers"), this);
	actionLoadLayers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	actionLoadLayers->setToolTip("Load Layers (Ctrl + L)");
	toolbarFile->addAction(actionLoadLayers);
	connect(actionLoadLayers, SIGNAL(triggered()), this, SLOT(loadLayers()));

	actionSaveLayers = new QAction(QIcon(":/images/icons/22.png"), tr("&Save Layers"), this);
	actionSaveLayers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	actionSaveLayers->setToolTip("Save Layers (Ctrl + S)");
	toolbarFile->addAction(actionSaveLayers);
	connect(actionSaveLayers, SIGNAL(triggered()), this, SLOT(saveLayers()));

	actionSaveLayersAs = new QAction(QIcon(":/images/icons/86.png"), tr("Save La&yers As..."), this);
	actionSaveLayersAs->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	actionSaveLayersAs->setToolTip("Save Layers As... (Ctrl + D)");
	toolbarFile->addAction(actionSaveLayersAs);
	connect(actionSaveLayersAs, SIGNAL(triggered()), this, SLOT(saveLayersAs()));

	toolbarFile->addSeparator();

	actionOpenImage = new QAction(QIcon(":/images/icons/image_open.png"), tr("&Open Image"), this);
	actionOpenImage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	actionOpenImage->setToolTip("Open Image (Ctrl + O)");
	toolbarFile->addAction(actionOpenImage);
	connect(actionOpenImage, SIGNAL(triggered()), this, SLOT(selectImageType()));

	actionImportDialog = new QAction(QIcon(":/images/icons/import.png"), tr("I&mport Images..."), this);
	actionImportDialog->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
	actionImportDialog->setToolTip("Import Images... (Ctrl + M)");
	toolbarFile->addAction(actionImportDialog);
	connect(actionImportDialog, SIGNAL(triggered()), this, SLOT(openImportDialog()));

	actionExportImage = new QAction(QIcon(":/images/icons/66.png"), tr("Export Ima&ge"), this);
	actionExportImage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
	actionExportImage->setToolTip("Export Image (Ctrl + G)");
	toolbarFile->addAction(actionExportImage);
	connect(actionExportImage, SIGNAL(triggered()), this, SLOT(exportImage()));

	toolbarFile->addSeparator();

	actionClearAll = new QAction(QIcon(":/images/icons/clear.png"), tr("Clear &All Layers"), this);
	actionClearAll->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	actionClearAll->setToolTip("Clear All Layers (Ctrl + W)");
	toolbarFile->addAction(actionClearAll);
	connect(actionClearAll, SIGNAL(triggered()), this, SLOT(clearAllLayersSlot()));

	toolbarFile->addSeparator();

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
	rulerValue->setDecimals(3);
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

	addToolBarBreak();

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

	addToolBarBreak();

	toolbarMicroscopy = addToolBar(tr("Microscopy Image"));

	actionFlipMicroscopyHorizontal = new QAction(QIcon(":/images/icons/flip_horizontal.png"), tr("Flip &Microscopy Horizontally"), this);
	actionFlipMicroscopyHorizontal->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_H));
	actionFlipMicroscopyHorizontal->setToolTip("Flip Microscopy Horizontally (Ctrl + Shift + H)");
	actionFlipMicroscopyHorizontal->setCheckable(true);
	toolbarMicroscopy->addAction(actionFlipMicroscopyHorizontal);
	connect(actionFlipMicroscopyHorizontal, SIGNAL(toggled(bool)), this, SLOT(microscopyFlipStateChanged(bool)));

	actionFlipMicroscopyVertical = new QAction(QIcon(":/images/icons/flip_vertical.png"), tr("Flip Microscopy Ver&tically"), this);
	actionFlipMicroscopyVertical->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));
	actionFlipMicroscopyVertical->setToolTip("Flip Microscopy Vertically (Ctrl + Shift + V)");
	actionFlipMicroscopyVertical->setCheckable(true);
	toolbarMicroscopy->addAction(actionFlipMicroscopyVertical);
	connect(actionFlipMicroscopyVertical, SIGNAL(toggled(bool)), this, SLOT(microscopyFlipStateChanged(bool)));

	toolbarMicroscopy->addSeparator();
	toolbarMicroscopy->addWidget(microscopywidget);

	menuFile->addAction(actionLoadLayers);
	menuFile->addAction(actionSaveLayers);
	menuFile->addAction(actionSaveLayersAs);
	menuFile->addSeparator();
	menuFile->addAction(actionOpenImage);
	menuFile->addAction(actionImportDialog);
	menuFile->addAction(actionExportImage);
	menuFile->addSeparator();
	menuFile->addAction(actionClearAll);
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
	menuView->addSeparator();
	menuView->addAction(actionFlipMicroscopyHorizontal);
	menuView->addAction(actionFlipMicroscopyVertical);

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
	connect(imagewindowwidget, SIGNAL(updateMicroscopyPosition(bool, bool, double, double, double, double, double)), this, SLOT(updateMicroscopySlot(bool, bool, double, double, double, double, double)));

	resize(defaultwinsizex, defaultwinsizey);

	applyGlobalPreferences(globalpreferences);

	layersfileloaded = false;
	saveascalled = false;
	
	imzmlfilename.clear();
	defaultmaxx = 1;
	defaultmaxy = 1;
	defaultpixelsizex = 1;
	defaultpixelsizey = 1;
	vendor = unknownvendor;

	redrawenabled = true;
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
	delete pixelsizexlabel;
	delete pixelsizexspinbox;
	delete pixelsizeylabel;
	delete pixelsizeyspinbox;
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
	delete microscopynavigationlabel;
	delete microscopynavigationcombobox;
	delete microscopygobutton;
	delete microscopyprevbutton;
	delete microscopynextbutton;
	delete microscopyhbox;
	delete microscopywidget;

	delete image;
	delete histologyimage;

	delete imagewindowwidget;
	delete layerslayout;
	delete layerswidget;
	delete layersscrollarea;
	delete mainwidget;

	delete rulerLabel;
	delete rulerValue;

	delete actionLoadLayers;
	delete actionSaveLayers;
	delete actionSaveLayersAs;
	delete actionOpenImage;
	delete actionImportDialog;
	delete actionExportImage;
	delete actionClearAll;
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


void cImageWindow::setDefaultValues(string imzmlfilename, int defaultmaxx, int defaultmaxy, int defaultpixelsizex, int defaultpixelsizey, eVendorType vendor) {
	this->imzmlfilename = imzmlfilename;
	this->defaultmaxx = defaultmaxx;
	this->defaultmaxy = defaultmaxy;
	this->defaultpixelsizex = defaultpixelsizex;
	this->defaultpixelsizey = defaultpixelsizey;
	this->vendor = vendor;
	imagewindowwidget->setDefaultMaxXY(defaultmaxx, defaultmaxy, defaultpixelsizex, defaultpixelsizey, vendor);

	string settingsfile = imzmlfilename;
	if (settingsfile.rfind('.') == string::npos) {
		return;
	}
	settingsfile = settingsfile.substr(0, settingsfile.rfind('.'));
	settingsfile += ".layers";

	lastdirlayersfile = settingsfile.c_str();
	layersfileloaded = false;

	QString shortname = getShortFileName(imzmlfilename).c_str();
	((QLabel *)layerslayout->itemAtPosition(0, 7)->widget())->setText(shortname);

	setWindowTitle(editorname);
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
	layerslider->setMinimumWidth(100);
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
		layerzvalue->setValue(500);
	}
	else {
		layerzvalue->setValue(layerscount);
	}
	layerzvalue->setToolTip("Order of Layer");

	QLabel* layerdescription = new QLabel();

	QPushButton* layerclearbutton = new QPushButton("x");
	layerclearbutton->setToolTip("Clear Layer");
	layerclearbutton->setFixedWidth(25);

	if (layerscount == 0) {
		layerclearbutton->setDisabled(true);
	}

	// the ownership is taken by QGridLayout
	layerslayout->addWidget(layerradiobutton, layerscount, 0);
	layerslayout->addWidget(layercheckbox, layerscount, 1);
	layerslayout->addWidget(layerlabel, layerscount, 2);
	layerslayout->addWidget(layerslider, layerscount, 3);
	layerslayout->addWidget(layerspinbox, layerscount, 4);
	layerslayout->addWidget(layerzvalue, layerscount, 5);
	layerslayout->addWidget(layerclearbutton, layerscount, 6);
	layerslayout->addWidget(layerdescription, layerscount, 7);

	connect(layerspinbox, SIGNAL(valueChanged(int)), layerslider, SLOT(setValue(int)));
	connect(layerslider, SIGNAL(valueChanged(int)), layerspinbox, SLOT(setValue(int)));

	connect(layerradiobutton, SIGNAL(toggled(bool)), this, SLOT(activeLayerChangedSlot(bool)));

	connect(layerspinbox, SIGNAL(valueChanged(int)), this, SLOT(layerSpinBoxChangedSlot(int)));
	connect(layercheckbox, SIGNAL(stateChanged(int)), this, SLOT(layerCheckBoxChangedSlot(int)));

	connect(layerzvalue, SIGNAL(valueChanged(int)), this, SLOT(layerZValueChangedSlot(int)));

	connect(layerclearbutton, SIGNAL(released()), this, SLOT(clearLayer()));

	emit layerChanged(layerscount, layercheckbox->isChecked(), layerspinbox->value(), layerzvalue->value(), redrawenabled);
	
	layerscount++;
}


void cImageWindow::applyGlobalPreferences(cGlobalPreferences* globalpreferences) {
	if (globalpreferences) {
		if ((lastdiropticalimage.right(4).compare(".jpg", Qt::CaseInsensitive) != 0) &&
			(lastdiropticalimage.right(5).compare(".jpeg", Qt::CaseInsensitive) != 0) &&
			(lastdiropticalimage.right(4).compare(".png", Qt::CaseInsensitive) != 0) &&
			(lastdiropticalimage.right(4).compare(".tif", Qt::CaseInsensitive) != 0) &&
			(lastdiropticalimage.right(5).compare(".tiff", Qt::CaseInsensitive) != 0) &&
			(lastdiropticalimage.right(4).compare(".bmp", Qt::CaseInsensitive) != 0) &&
			(lastdiropticalimage.right(4).compare(".gif", Qt::CaseInsensitive) != 0)) {
			lastdiropticalimage = globalpreferences->openopticalimagedir;
		}
		if ((lastdirhistologyimage.right(4).compare(".jpg", Qt::CaseInsensitive) != 0) &&
			(lastdirhistologyimage.right(5).compare(".jpeg", Qt::CaseInsensitive) != 0) &&
			(lastdirhistologyimage.right(4).compare(".png", Qt::CaseInsensitive) != 0) &&
			(lastdirhistologyimage.right(4).compare(".tif", Qt::CaseInsensitive) != 0) &&
			(lastdirhistologyimage.right(5).compare(".tiff", Qt::CaseInsensitive) != 0) &&
			(lastdirhistologyimage.right(4).compare(".bmp", Qt::CaseInsensitive) != 0) &&
			(lastdirhistologyimage.right(4).compare(".gif", Qt::CaseInsensitive) != 0)) {
			lastdirhistologyimage = globalpreferences->openhistologyimagedir;
		}
		if ((lastdirmicroscopyimage.right(4).compare(".jpg", Qt::CaseInsensitive) != 0) &&
			(lastdirmicroscopyimage.right(5).compare(".jpeg", Qt::CaseInsensitive) != 0) &&
			(lastdirmicroscopyimage.right(4).compare(".png", Qt::CaseInsensitive) != 0) &&
			(lastdirmicroscopyimage.right(4).compare(".tif", Qt::CaseInsensitive) != 0) &&
			(lastdirmicroscopyimage.right(5).compare(".tiff", Qt::CaseInsensitive) != 0) &&
			(lastdirmicroscopyimage.right(4).compare(".bmp", Qt::CaseInsensitive) != 0) &&
			(lastdirmicroscopyimage.right(4).compare(".gif", Qt::CaseInsensitive) != 0)) {
			lastdirmicroscopyimage = globalpreferences->openmicroscopyimagedir;
		}
		if (lastdirexportimage.right(4).compare(".png", Qt::CaseInsensitive) != 0) {
			lastdirexportimage = globalpreferences->exportimagedefaultdir;
		}
	}
}


void cImageWindow::colorSpinBoxes(int layerid) {

	if (layerid == 0) {
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

	if (layerid == 1) {
		leftshift->setStyleSheet("QSpinBox { background-color: lime; }");
		topshift->setStyleSheet("QSpinBox { background-color: lime; }");
		maxx->setStyleSheet("QSpinBox { background-color: lime; }");
		maxy->setStyleSheet("QSpinBox { background-color: lime; }");
		pixelsizexspinbox->setStyleSheet("QSpinBox { background-color: lime; }");
		pixelsizeyspinbox->setStyleSheet("QSpinBox { background-color: lime; }");
	}
	else {
		leftshift->setStyleSheet("QSpinBox { background-color: white; }");
		topshift->setStyleSheet("QSpinBox { background-color: white; }");
		maxx->setStyleSheet("QSpinBox { background-color: white; }");
		maxy->setStyleSheet("QSpinBox { background-color: white; }");
		pixelsizexspinbox->setStyleSheet("QSpinBox { background-color: white; }");
		pixelsizeyspinbox->setStyleSheet("QSpinBox { background-color: white; }");
	}

	if (layerid == 2) {
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

	if (layerid >= 3) {
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


void cImageWindow::setNavigationLayer(int layer, int navigation) {
	microscopylayers[layer].navigationlayer = navigation;
	imagewindowwidget->setNavigationLayer(layer, navigation);
}


void cImageWindow::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Control) {
		imagewindowwidget->setKeepAspectRatio(true);
	}

	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {

		if (xfrom->hasFocus() || xto->hasFocus() || yfrom->hasFocus() || yto->hasFocus()) {
			setregionbutton->click();
		}

		if (leftshift->hasFocus() || topshift->hasFocus() || maxx->hasFocus() || maxy->hasFocus() || pixelsizexspinbox->hasFocus() || pixelsizeyspinbox->hasFocus()) {
			setmaxbutton->click();
		}

	}

	event->accept();
}


void cImageWindow::keyReleaseEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Control) {
		imagewindowwidget->setKeepAspectRatio(false);
	}

	event->accept();
}


void cImageWindow::closeWindow() {
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, editorname, "Save changes ?", QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		saveascalled = true;
		saveLayers();
	}

	hide();
}


void cImageWindow::selectImageType() {
	QStringList imagetypes;
	imagetypes << "Optical Image";
	imagetypes << "Histology Image";
	imagetypes << "Microscopy Image (navigation)";

	QString microscopylayername = "Microscopy Image ";
	QString tmpname;
	int size = (int)microscopylayers.size();
	for (int i = 0; i < size - 4; i++) {
		tmpname = microscopylayername + QVariant(i + 1).toString();
		imagetypes << tmpname;
	}

	QInputDialog input;
	input.setInputMode(QInputDialog::TextInput);
	input.setOptions(QInputDialog::UseListViewForComboBoxItems);
	input.setComboBoxItems(imagetypes);
	input.setWindowTitle("Open Image...");
	input.setLabelText("Select Image Type:");
	input.setWindowIcon(QIcon(":/images/icons/image_open.png"));
	input.resize(300, 400);

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
		openMicroscopyImage(3, s);
	}

	QString microscopylayername = "Microscopy Image ";
	QString tmpname;
	int size = (int)microscopylayers.size();
	for (int i = 0; i < size - 4; i++) {
		tmpname = microscopylayername + QVariant(i + 1).toString();
		if (s.compare(tmpname) == 0) {
			openMicroscopyImage(i + 4, s);
		}
	}
}


void cImageWindow::openOpticalImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Optical Image ..."), lastdiropticalimage, tr("Image Files (*.tif *.tiff *.jpg *.jpeg *.bmp *.png *.gif)"));
	reopenOpticalImage(filename);
}


void cImageWindow::reopenOpticalImage(QString filename) {
	if (!filename.isEmpty()) {
		lastdiropticalimage = filename;

		if (!image->load(filename)) {
			QMessageBox msgBox;
			QString errstr = "The image cannot be opened.";
			msgBox.setText(errstr);
			msgBox.exec();
			return;
		}

		leftshift->setValue(0);
		topshift->setValue(0);
		maxx->setValue(defaultmaxx);
		maxy->setValue(defaultmaxy);
		pixelsizexspinbox->setValue(defaultpixelsizex);
		pixelsizeyspinbox->setValue(defaultpixelsizey);

		setMaxButtonReleased();

		imagewindowwidget->setOpticalImage(image);

		((QCheckBox *)layerslayout->itemAtPosition(1, 1)->widget())->setChecked(true);

		QString shortname = getShortFileName(filename.toStdString()).c_str();
		((QLabel *)layerslayout->itemAtPosition(1, 7)->widget())->setText(shortname);
	}
}


void cImageWindow::openHistologyImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Histology Image ..."), lastdirhistologyimage, tr("Image Files (*.tif *.tiff *.jpg *.jpeg *.bmp *.png *.gif)"));
	reopenHistologyImage(filename);
}


void cImageWindow::reopenHistologyImage(QString filename) {
	if (!filename.isEmpty()) {
		lastdirhistologyimage = filename;

		if (!histologyimage->load(filename)) {
			QMessageBox msgBox;
			QString errstr = "The image cannot be opened.";
			msgBox.setText(errstr);
			msgBox.exec();
			return;
		}
		
		checkLoadedImage(histologyimage);

		imagewindowwidget->setHistologyImage(histologyimage);

		((QRadioButton *)layerslayout->itemAtPosition(2, 0)->widget())->setChecked(true);
		((QCheckBox *)layerslayout->itemAtPosition(2, 1)->widget())->setChecked(true);

		QString shortname = getShortFileName(filename.toStdString()).c_str();
		((QLabel *)layerslayout->itemAtPosition(2, 7)->widget())->setText(shortname);

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


bool cImageWindow::parseMicroscopyImage(QString& filename, double& pixelwidth, double& pixelheight, int& resolutionx, int& resolutiony, double& realwidth, double& stagex, double& stagey) {
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

	pos = desc.find("[Image]");
	if (pos != string::npos) {
		pos = desc.find("MagCanvasRealWidth=", pos);
		if (pos != string::npos) {
			sscanf_s(desc.substr(pos + 19).c_str(), "%lf", &realwidth);
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


void cImageWindow::openMicroscopyImage(int layer, const QString &layername) {
	QString title = "Open " + layername + " ...";

	string imagetypes = "Image Files (*.tif *.tiff *.jpg *.jpeg *.bmp *.png *.gif *.lif)";

	QString filename = QFileDialog::getOpenFileName(this, tr(title.toStdString().c_str()), lastdirmicroscopyimage, tr(imagetypes.c_str()));

	if ((filename.toStdString().size() > 4) && (filename.toStdString().substr(filename.toStdString().size() - 4).compare(".lif") == 0)) {
		lastdirmicroscopyimage = filename;

		vector<cLifMetadata> parsedmetadata;
		readLifMetadata(filename.toStdString(), parsedmetadata);

		if (parsedmetadata.size() == 0) {
			QMessageBox msgBox;
			QString errstr = "The selected LIF file does not contain any image !";
			msgBox.setText(errstr);
			msgBox.exec();
			return;
		}

		int imagepos = -1;

		if (parsedmetadata.size() == 1) {
			imagepos = 0;
		}

		if (parsedmetadata.size() > 1) {
			QStringList items;
			QString tmpname;

			for (int i = 0; i < (int)parsedmetadata.size(); i++) {
				tmpname = QVariant(parsedmetadata[i].magnification).toString();
				tmpname += "x, ";
				tmpname += parsedmetadata[i].imagename.c_str();
				//if (parsedmetadata[i].microscopytype == 1) {
				//	if (parsedmetadata[i].luts.size() == 1) {
				//		tmpname += ", ";
				//		tmpname += parsedmetadata[i].luts[0].c_str();
				//	}
				//	if (parsedmetadata[i].luts.size() > 1) {
				//		tmpname += ", number of luts: ";
				//		tmpname += QVariant((int)parsedmetadata[i].luts.size()).toString();
				//	}
				//}
				items << tmpname;
			}

			QInputDialog input;
			input.setInputMode(QInputDialog::TextInput);
			input.setOptions(QInputDialog::UseListViewForComboBoxItems);
			input.setComboBoxItems(items);
			input.setWindowTitle("Open Image...");
			input.setLabelText("Select Image:");
			input.setWindowIcon(QIcon(":/images/icons/image_open.png"));
			input.resize(300, 400);

			if (input.exec()) {
				for (int i = 0; i < items.size(); i++) {
					if (input.textValue().compare(items[i]) == 0) {
						imagepos = i;
						break;
					}
				}
			}
		}

		if (imagepos == -1) {
			return;
		}

		QInputDialog minthreshold;
		minthreshold.setInputMode(QInputDialog::IntInput);
		minthreshold.setIntMinimum(0);
		minthreshold.setIntMaximum(255);
		minthreshold.setIntValue(0);
		minthreshold.setWindowTitle("Open Image...");
		minthreshold.setLabelText("Noise Threshold:");
		minthreshold.setWindowIcon(QIcon(":/images/icons/image_open.png"));

		if (!minthreshold.exec()) {
			return;
		}

		QInputDialog maxthreshold;
		maxthreshold.setInputMode(QInputDialog::IntInput);
		maxthreshold.setIntMinimum(0);
		maxthreshold.setIntMaximum(255);
		maxthreshold.setIntValue(255);
		maxthreshold.setWindowTitle("Open Image...");
		maxthreshold.setLabelText("Signal Threshold:");
		maxthreshold.setWindowIcon(QIcon(":/images/icons/image_open.png"));

		if (!maxthreshold.exec()) {
			return;
		}

		microscopylayers[layer].thresholdmin = minthreshold.intValue();
		microscopylayers[layer].thresholdmax = maxthreshold.intValue();

		openLifImage(filename.toStdString(), layer, parsedmetadata[imagepos], true);

		//parsedmetadata[imagepos].print();

		setNavigationLayer(layer, layer/*3*/);

		microscopyDefaultButtonReleased();
	}
	else {
		reopenMicroscopyImage(layer, filename, false);
	}
}


void cImageWindow::reopenMicroscopyImage(int layer, QString filename, bool reopen) {
	if (!filename.isEmpty()) {
		if (!reopen) {
			lastdirmicroscopyimage = filename;
		}

		int microscopycountx, microscopycounty;
		double microscopypixelwidth, microscopypixelheight, realwidth, stagex, stagey;

		microscopylayers[layer].sourcename = filename;
		microscopylayers[layer].imagename = filename;

		QString description = getShortFileName(filename.toStdString()).c_str();

		if (parseMicroscopyImage(filename, microscopypixelwidth, microscopypixelheight, microscopycountx, microscopycounty, realwidth, stagex, stagey)) {
			QImage image;

			if (!image.load(filename)) {
				QMessageBox msgBox;
				QString errstr = "The image cannot be opened.";
				msgBox.setText(errstr);
				msgBox.exec();
				return;
			}

			*microscopylayers[layer].image = image.copy(0, 0, microscopycountx, microscopycounty);

			checkLoadedImage(microscopylayers[layer].image);

			imagewindowwidget->setMicroscopyImage(layer, microscopylayers[layer].image);

			((QRadioButton *)layerslayout->itemAtPosition(layer, 0)->widget())->setChecked(true);
			((QCheckBox *)layerslayout->itemAtPosition(layer, 1)->widget())->setChecked(true);

			int magnification = 0;
			if ((double)microscopycountx * microscopypixelwidth > 0) {
				magnification = round(realwidth / ((double)microscopycountx * microscopypixelwidth));
			}
			if (magnification > 0) {
				description = QVariant(magnification).toString() + "x, " + description;
			}

			((QLabel *)layerslayout->itemAtPosition(layer, 7)->widget())->setText(description);

			if (!reopen) {
				microscopylayers[layer].defaultwidth = (double)microscopycountx * microscopypixelwidth * 1000000.0;
				microscopylayers[layer].defaultheight = (double)microscopycounty * microscopypixelheight * 1000000.0;

				microscopylayers[layer].stagex = stagex;
				microscopylayers[layer].stagey = stagey;

				microscopylayers[layer].type = 0;

				microscopyDefaultButtonReleased();
			}
		}
		else {
			
			if (!microscopylayers[layer].image->load(filename)) {
				QMessageBox msgBox;
				QString errstr = "The image cannot be opened.";
				msgBox.setText(errstr);
				msgBox.exec();
				return;
			}

			microscopylayers[layer].reduced = checkLoadedImage(microscopylayers[layer].image);

			imagewindowwidget->setMicroscopyImage(layer, microscopylayers[layer].image);

			((QRadioButton *)layerslayout->itemAtPosition(layer, 0)->widget())->setChecked(true);
			((QCheckBox *)layerslayout->itemAtPosition(layer, 1)->widget())->setChecked(true);

			if (!reopen) {

				QMessageBox::StandardButton reply;
				QString s = "The microscopy image does not include any metadata.\nDo you want to load the metadata from a LIF file ?\n\nYes = Select the LIF file.\nNo = Set up the image width and height manually.";

				reply = QMessageBox::question(this, "Load metadata ?", s, QMessageBox::Yes | QMessageBox::No);

				QString liffile;
				vector<cLifMetadata> parsedmetadata;

				microscopylayers[layer].defaultwidth = microscopylayers[layer].image->width();
				microscopylayers[layer].defaultheight = microscopylayers[layer].image->height();

				microscopylayers[layer].stagex = 0;
				microscopylayers[layer].stagey = 0;

				microscopylayers[layer].type = 1;

				if (reply == QMessageBox::Yes) {

					liffile = QFileDialog::getOpenFileName(this, tr("Select LIF file ..."), lastdirmicroscopyimage, tr("LIF Files (*.lif)"));
					if (!liffile.isEmpty()) {

						readLifMetadata(liffile.toStdString(), parsedmetadata);

						string tmpimagename;
						for (auto& it : parsedmetadata) {
							tmpimagename = it.imagename;

							if ((it.microscopytype == 1) && (it.luts.size() == 1) && (it.luts[0].size() > 0)) {
								size_t lutcomma = it.imagename.rfind(',');
								if (lutcomma != string::npos) {
									tmpimagename = tmpimagename.substr(0, lutcomma);
								}
							}

							if (microscopylayers[layer].sourcename.toStdString().find(tmpimagename) != string::npos) {
								if (microscopylayers[layer].reduced || (!microscopylayers[layer].reduced && (microscopylayers[layer].image->width() == it.dimx) && (microscopylayers[layer].image->height() == it.dimy))) {
									if (it.dim10 == 0) {
										microscopylayers[layer].defaultwidth = it.width * 1000000.0;
										microscopylayers[layer].defaultheight = it.height * 1000000.0;

										// move stagex/stagey from image center to top-left corner
										if (it.tiles.size() == 1) {
											microscopylayers[layer].stagex = max(0.0, it.tiles[0].posx - it.width / 2.0);
											microscopylayers[layer].stagey = max(0.0, it.tiles[0].posy - it.height / 2.0);
										}
										else {
											microscopylayers[layer].stagex = max(0.0, it.stageposx - it.width / 2.0);
											microscopylayers[layer].stagey = max(0.0, it.stageposy - it.height / 2.0);
										}

										if (it.magnification > 0) {
											description = QVariant(it.magnification).toString() + "x, " + description;
										}

										//if ((it.microscopytype == 1) && (it.luts.size() == 1) && (it.luts[0].size() > 0)) {
										//	description += ", ";
										//	description += it.luts[0].c_str();
										//}

										break;
									}
								}
							}
						}

					}

				}

				setNavigationLayer(layer, 3);

				microscopyDefaultButtonReleased();

				if ((reply == QMessageBox::Yes) && (liffile.isEmpty() || parsedmetadata.empty())) {
					QMessageBox msgBox;
					QString errstr = "The file does not include any metadata.\nThe image width and height must be set up manually !";
					msgBox.setText(errstr);
					msgBox.exec();
				}

			}

			((QLabel *)layerslayout->itemAtPosition(layer, 7)->widget())->setText(description);

		}

	}
}


void cImageWindow::clearLayers() {
	redrawenabled = false;

	activelayer = 0;
	imagewindowwidget->clearLayers();

	int size = (int)microscopylayers.size();
	for (int i = 0; i < size; i++) {
		((QRadioButton *)layerslayout->itemAtPosition(i, 0)->widget())->setChecked(false);
		((QCheckBox *)layerslayout->itemAtPosition(i, 1)->widget())->setChecked(false);
		((QSlider *)layerslayout->itemAtPosition(i, 3)->widget())->setValue(100);
		((QSpinBox *)layerslayout->itemAtPosition(i, 4)->widget())->setValue(100);

		if (i == 0) {
			((QSpinBox *)layerslayout->itemAtPosition(i, 5)->widget())->setValue(500);
		}
		else {
			((QSpinBox *)layerslayout->itemAtPosition(i, 5)->widget())->setValue(i);
		}

		((QLabel *)layerslayout->itemAtPosition(i, 7)->widget())->setText("");

		microscopylayers[i].clear();
	}

	leftshift->setValue(0);
	topshift->setValue(0);
	maxx->setValue(500);
	maxy->setValue(500);
	pixelsizexspinbox->setValue(100);
	pixelsizeyspinbox->setValue(100);

	setMaxButtonReleased();

	//actionFlipHistologyHorizontal->blockSignals(true);
	//actionFlipHistologyVertical->blockSignals(true);
	histologyx->blockSignals(true);
	histologyy->blockSignals(true);
	histologywidth->blockSignals(true);
	histologyheight->blockSignals(true);
	histologyangle->blockSignals(true);

	actionFlipHistologyHorizontal->setChecked(false);
	actionFlipHistologyVertical->setChecked(false);

	histologyx->setValue(0);
	histologyy->setValue(0);
	histologywidth->setValue(0);
	histologyheight->setValue(0);
	histologyangle->setValue(0);

	//actionFlipHistologyHorizontal->blockSignals(false);
	//actionFlipHistologyVertical->blockSignals(false);
	histologyx->blockSignals(false);
	histologyy->blockSignals(false);
	histologywidth->blockSignals(false);
	histologyheight->blockSignals(false);
	histologyangle->blockSignals(false);

	histologyChanged();

	actionFlipMicroscopyHorizontal->blockSignals(true);
	actionFlipMicroscopyVertical->blockSignals(true);
	microscopyx->blockSignals(true);
	microscopyy->blockSignals(true);
	microscopywidth->blockSignals(true);
	microscopyheight->blockSignals(true);
	microscopyangle->blockSignals(true);
	microscopynavigationcombobox->blockSignals(true);

	actionFlipMicroscopyHorizontal->setChecked(false);
	actionFlipMicroscopyVertical->setChecked(false);

	microscopyx->setValue(0);
	microscopyy->setValue(0);
	microscopywidth->setValue(0);
	microscopyheight->setValue(0);
	microscopyangle->setValue(0);
	microscopynavigationcombobox->setCurrentIndex(0);

	actionFlipMicroscopyHorizontal->blockSignals(false);
	actionFlipMicroscopyVertical->blockSignals(false);
	microscopyx->blockSignals(false);
	microscopyy->blockSignals(false);
	microscopywidth->blockSignals(false);
	microscopyheight->blockSignals(false);
	microscopyangle->blockSignals(false);
	microscopynavigationcombobox->blockSignals(false);

	redrawenabled = true;
}


void cImageWindow::updateMicroscopyNavigationCombobox(int numberofitems) {
	QString layername = "Microscopy Image ";
	QString tmpname = layername + "(nav.)";

	microscopynavigationcombobox->blockSignals(true);
	microscopynavigationcombobox->clear();
	microscopynavigationcombobox->insertItem(0, tmpname);
	for (int i = 0; i < numberofitems - 4; i++) {
		tmpname = layername + QVariant(i + 1).toString();
		microscopynavigationcombobox->insertItem(i + 1, tmpname);
	}
	microscopynavigationcombobox->blockSignals(false);
}


bool cImageWindow::checkLoadedImage(QImage* image) {
	double checkw = image->width();
	double checkh = image->height();
	double maxrotationsize = sqrt(checkw * checkw + checkh * checkh);

	if (maxrotationsize == 0) {
		return false;
	}

	// can be increased up to max uint
	double maximumsize = 18000.0;
	double ratio = maximumsize / maxrotationsize;
	
	if (maxrotationsize > maximumsize) {
		checkw = checkw * ratio;
		checkh = checkh * ratio;
		*image = image->scaled((int)checkw, (int)checkh, Qt::KeepAspectRatio);
		return true;
	}

	return false;
}


void cImageWindow::readLifImageOptical(string filename, cLifMetadata& metadata, QImage& image, bool enableprogress) {
	if ((metadata.memoryblockoffsets.size() == 0) || (metadata.memoryblockoffsets[0] == 0) || (metadata.dimx == 0) || (metadata.dimy == 0) || (metadata.dimx > 32767) || (metadata.dimy > 32767)) {
		return;
	}

	int flipx = (metadata.tilescaninfoflipx == -1) ? metadata.flipx : metadata.tilescaninfoflipx;
	int flipy = (metadata.tilescaninfoflipy == -1) ? metadata.flipy : metadata.tilescaninfoflipy;

	if (flipx != flipy) {
		return;
	}

	int x, y;
	int r, g, b, value;
	QRgb pixelcolor;

	ifstream is;
	is.open(filename, ifstream::binary);

	if (!is.good()) {
		return;
	}

	is.seekg(metadata.memoryblockoffsets[0]);

	image = QImage(metadata.dimx, metadata.dimy, QImage::Format_RGB32);

	if (metadata.channels.size() != 3) {
		// empty image
		return;
	}

	int lastdimy = 0;

	QProgressDialog progress("Loading image...", /*"Cancel"*/0, 0, metadata.dimy, this);
	if (enableprogress) {
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);
		progress.setValue(0);
	}

	for (unsigned i = 0; i < (unsigned)metadata.memoryblocksize; i++) {
		if (!is.good()) {
			break;
		}

		value = is.get();

		if (metadata.channels[i % 3] == red) {
			r = value;
		}
		if (metadata.channels[i % 3] == green) {
			g = value;
		}
		if (metadata.channels[i % 3] == blue) {
			b = value;
		}

		if (i % 3 == 2) {
			x = (i / 3) % metadata.dimx;
			y = (i / 3) / metadata.dimx;

			pixelcolor = qRgb(r, g, b);

			if (!flipx && !flipy) {
				image.setPixel(x, y, pixelcolor);
			}

			if (flipx && flipy) {
				image.setPixel(metadata.dimx - 1 - x, metadata.dimy - 1 - y, pixelcolor);
			}

			if (enableprogress) {
				if (y != lastdimy) {
					progress.setValue(y);
					lastdimy = y;
				}
			}
		}
	}

	if (enableprogress) {
		progress.setValue(metadata.dimy);
	}
}


void cImageWindow::readLifImageFluorescence(string filename, cLifMetadata& metadata, QImage& image, int minthreshold, int maxthreshold, bool enableprogress) {
	if ((metadata.memoryblockoffsets.size() == 0) || (metadata.memoryblockoffsets[0] == 0) || (metadata.dimx == 0) || (metadata.dimy == 0) || (metadata.dimx > 32767) || (metadata.dimy > 32767)) {
		return;
	}

	int flipx = (metadata.tilescaninfoflipx == -1) ? metadata.flipx : metadata.tilescaninfoflipx;
	int flipy = (metadata.tilescaninfoflipy == -1) ? metadata.flipy : metadata.tilescaninfoflipy;

	if (flipx != flipy) {
		return;
	}

	int x, y;
	int value;
	QRgb pixelrgb;
	QColor pixelclr;

	ifstream is;
	is.open(filename, ifstream::binary);

	if (!is.good()) {
		return;
	}

	image = QImage(metadata.dimx, metadata.dimy, QImage::Format_RGB32);

	if ((metadata.channels.size() != metadata.luts.size()) || (metadata.channels.size() != 1)) {
		// empty image
		return;
	}

	int offsetssize = (int)metadata.memoryblockoffsets.size();

	QProgressDialog progress("Loading image...", /*"Cancel"*/0, 0, metadata.dimy * offsetssize, this);
	if (enableprogress) {
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);
		progress.setValue(0);
	}

	eChannelType currentchannel = gray;
	if (metadata.luts[0].compare("Red") == 0) {
		currentchannel = red;
	}
	if (metadata.luts[0].compare("Green") == 0) {
		currentchannel = green;
	}
	if (metadata.luts[0].compare("Blue") == 0) {
		currentchannel = blue;
	}

	if (currentchannel == gray) {
		// empty image
		return;
	}

	int lastdimy = 0;

	for (int i = 0; i < offsetssize; i++) {

		is.seekg(metadata.memoryblockoffsets[i]);

		for (unsigned j = 0; j < metadata.memoryblocksize; j++) {
			if (!is.good()) {
				break;
			}

			value = is.get();
			
			if (value < minthreshold) {
				value = 0;
			}

			if (value >= maxthreshold) {
				value = 255;
			}

			x = j % metadata.dimx;
			y = j / metadata.dimx;

			if (i > 0) {
				if (!flipx && !flipy) {
					pixelclr = image.pixel(x, y);
				}

				if (flipx && flipy) {
					pixelclr = image.pixel(metadata.dimx - 1 - x, metadata.dimy - 1 - y);
				}
			}

			switch (currentchannel) {
				case red:
					if (i > 0) {
						pixelrgb = qRgb(max(value, pixelclr.red()), 0, 0);
					}
					else {
						pixelrgb = qRgb(value, 0, 0);
					}
					break;
				case green:
					if (i > 0) {
						pixelrgb = qRgb(0, max(value, pixelclr.green()), 0);
					}
					else {
						pixelrgb = qRgb(0, value, 0);
					}
					break;
				case blue:
					if (i > 0) {
						pixelrgb = qRgb(0, 0, max(value, pixelclr.blue()));
					}
					else {
						pixelrgb = qRgb(0, 0, value);
					}
					break;
				default:
					pixelrgb = qRgb(0, 0, 0);
					break;
			}

			if (!flipx && !flipy) {
				image.setPixel(x, y, pixelrgb);
			}

			if (flipx && flipy) {
				image.setPixel(metadata.dimx - 1 - x, metadata.dimy - 1 - y, pixelrgb);
			}

			if (enableprogress) {
				if (y != lastdimy) {
					progress.setValue(y + i * metadata.dimy);
					lastdimy = y;
				}
			}
		}

	}

	if (enableprogress) {
		progress.setValue(metadata.dimy * offsetssize);
	}
}


void cImageWindow::openLifImage(string liffilename, int layer, cLifMetadata& metadata, bool enableprogress) {
	microscopylayers[layer].sourcename = liffilename.c_str();
	microscopylayers[layer].imagename = metadata.imagename.c_str();

	QString description = metadata.imagename.c_str();

	if (metadata.microscopytype == 1) {
		readLifImageFluorescence(liffilename, metadata, *microscopylayers[layer].image, microscopylayers[layer].thresholdmin, microscopylayers[layer].thresholdmax, enableprogress);
	}
	else {
		readLifImageOptical(liffilename, metadata, *microscopylayers[layer].image, enableprogress);
	}

	microscopylayers[layer].reduced = checkLoadedImage(microscopylayers[layer].image);

	imagewindowwidget->setMicroscopyImage(layer, microscopylayers[layer].image);

	((QRadioButton *)layerslayout->itemAtPosition(layer, 0)->widget())->setChecked(true);
	((QCheckBox *)layerslayout->itemAtPosition(layer, 1)->widget())->setChecked(true);

	microscopylayers[layer].defaultwidth = microscopylayers[layer].image->width();
	microscopylayers[layer].defaultheight = microscopylayers[layer].image->height();

	microscopylayers[layer].stagex = 0;
	microscopylayers[layer].stagey = 0;

	microscopylayers[layer].type = 1;

	if (metadata.dim10 == 0) {
		microscopylayers[layer].defaultwidth = metadata.width * 1000000.0;
		microscopylayers[layer].defaultheight = metadata.height * 1000000.0;

		// move stagex/stagey from image center to top-left corner
		if (metadata.tiles.size() == 1) {
			microscopylayers[layer].stagex = max(0.0, metadata.tiles[0].posx - metadata.width / 2.0);
			microscopylayers[layer].stagey = max(0.0, metadata.tiles[0].posy - metadata.height / 2.0);
		}
		else {
			microscopylayers[layer].stagex = max(0.0, metadata.stageposx - metadata.width / 2.0);
			microscopylayers[layer].stagey = max(0.0, metadata.stageposy - metadata.height / 2.0);
		}

		if (metadata.magnification > 0) {
			description = QVariant(metadata.magnification).toString() + "x, " + description;
		}

		//if ((metadata.microscopytype == 1) && (metadata.luts.size() == 1) && (metadata.luts[0].size() > 0)) {
		//	description += ", ";
		//	description += metadata.luts[0].c_str();
		//}
	}

	((QLabel *)layerslayout->itemAtPosition(layer, 7)->widget())->setText(description);
}


void cImageWindow::openImportDialog() {
	importdialog.setFormValues(lastdirmicroscopyimage, (int)microscopylayers.size());

	if (importdialog.exec() != QDialog::Accepted) {
		importdialog.setFormValues(lastdirmicroscopyimage, (int)microscopylayers.size());
		return;
	}

	int startfromindex;
	int navigationindex;
	QStringList tiffilenames;
	QString liffilename;

	int layer;

	vector<cLifMetadata> parsedmetadata;

	int tabindex = importdialog.getFormValues(startfromindex, tiffilenames, navigationindex, liffilename);
	
	int numberofimages = tiffilenames.size();
	bool lifopen = false;

	switch (tabindex) {
		case 0:
			if (tiffilenames.size() == 0) {
				return;
			}
			break;
		case 1:
			readLifMetadata(liffilename.toStdString(), parsedmetadata);

			if (tiffilenames.size() == 0) {
				if (parsedmetadata.size() == 0) {
					return;
				}
				numberofimages = (int)parsedmetadata.size();
				lifopen = true;
			}
			break;
		default:
			break;
	}


	QProgressDialog progress("Importing images...", /*"Cancel"*/0, 0, numberofimages, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);
	progress.setValue(0);

	switch (tabindex) {
		case 0:
			for (int i = 0; i < tiffilenames.size(); i++) {
				reopenMicroscopyImage(startfromindex + 4 + i, tiffilenames[i], false);
				progress.setValue(i + 1);
			}
			break;
		case 1:
			if (parsedmetadata.size() > 0) {
				startfromindex += 4;
				navigationindex += 3;

				for (int i = 0; i < numberofimages; i++) {
					layer = startfromindex + i;

					if (layer >= microscopylayers.size()) {
						QMessageBox msgBox;
						QString errstr = "Not enough free layers ! Number of imported images: " + QVariant(i).toString() + ".";
						msgBox.setText(errstr);
						msgBox.exec();
						break;
					}

					if (lifopen) {
						microscopylayers[layer].thresholdmin = 0;
						microscopylayers[layer].thresholdmax = 255;

						openLifImage(liffilename.toStdString(), layer, parsedmetadata[i], false);
					}
					else {
						reopenMicroscopyImage(layer, tiffilenames[i], true);

						microscopylayers[layer].defaultwidth = microscopylayers[layer].image->width();
						microscopylayers[layer].defaultheight = microscopylayers[layer].image->height();

						microscopylayers[layer].stagex = 0;
						microscopylayers[layer].stagey = 0;

						microscopylayers[layer].type = 1;

						string tmpimagename;
						for (auto& it : parsedmetadata) {
							tmpimagename = it.imagename;

							if ((it.microscopytype == 1) && (it.luts.size() == 1) && (it.luts[0].size() > 0)) {
								size_t lutcomma = it.imagename.rfind(',');
								if (lutcomma != string::npos) {
									tmpimagename = tmpimagename.substr(0, lutcomma);
								}
							}

							if (tiffilenames[i].toStdString().find(tmpimagename) != string::npos) {
								if (microscopylayers[layer].reduced || (!microscopylayers[layer].reduced && (microscopylayers[layer].image->width() == it.dimx) && (microscopylayers[layer].image->height() == it.dimy))) {
									if (it.dim10 == 0) {
										microscopylayers[layer].defaultwidth = it.width * 1000000.0;
										microscopylayers[layer].defaultheight = it.height * 1000000.0;

										// move stagex/stagey from image center to top-left corner
										if (it.tiles.size() == 1) {
											microscopylayers[layer].stagex = max(0.0, it.tiles[0].posx - it.width / 2.0);
											microscopylayers[layer].stagey = max(0.0, it.tiles[0].posy - it.height / 2.0);
										}
										else {
											microscopylayers[layer].stagex = max(0.0, it.stageposx - it.width / 2.0);
											microscopylayers[layer].stagey = max(0.0, it.stageposy - it.height / 2.0);
										}

										if (it.magnification > 0) {
											QString description = ((QLabel *)layerslayout->itemAtPosition(layer, 7)->widget())->text();
											description = QVariant(it.magnification).toString() + "x, " + description;
											((QLabel *)layerslayout->itemAtPosition(layer, 7)->widget())->setText(description);
										}

										break;
									}
								}
							}
						}
					}

					setNavigationLayer(layer, navigationindex);

					microscopyDefaultButtonReleased();

					progress.setValue(i + 1);
				}
			}
			break;
		default:
			break;
	}

	progress.setValue(numberofimages);
}


void cImageWindow::exportImage() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export Image ..."), lastdirexportimage, "PNG Files (*.png)");

	if (!filename.isEmpty()) {
		lastdirexportimage = filename;
		imagewindowwidget->getImage().save(filename, "PNG");
	}
}


void cImageWindow::setFilterOptionsSlot(vector<cCoordinateInfo> coordinateinfo, bool operatortype, string columnname1, string comparatorname1, string filterstring1, string columnname2, string comparatorname2, string filterstring2, bool casesensitive, bool wholeword) {
	imagewindowwidget->setFilterOptions(coordinateinfo, operatortype, columnname1, comparatorname1, filterstring1, columnname2, comparatorname2, filterstring2, casesensitive, wholeword);
}


void cImageWindow::setMaxButtonReleased() {
	imagewindowwidget->setCorrelationValues(leftshift->value(), topshift->value(), maxx->value(), maxy->value(), pixelsizexspinbox->value(), pixelsizeyspinbox->value());
	resetregionbutton->click();
}


void cImageWindow::setDefaultButtonReleased() {
	leftshift->setValue(0);
	topshift->setValue(0);
	maxx->setValue(defaultmaxx);
	maxy->setValue(defaultmaxy);
	pixelsizexspinbox->setValue(defaultpixelsizex);
	pixelsizeyspinbox->setValue(defaultpixelsizey);
	imagewindowwidget->setCorrelationValues(leftshift->value(), topshift->value(), maxx->value(), maxy->value(), pixelsizexspinbox->value(), pixelsizeyspinbox->value());
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
	double lastnavigationangle = 0;
	double widthratio, heightratio;
	int size = (int)microscopylayers.size();

	if (activelayer >= 3) {
		if (activelayer == 3) {
			lastnavigationangle = microscopylayers[activelayer].angle;
		}

		microscopylayers[activelayer].flipx = actionFlipMicroscopyHorizontal->isChecked();
		microscopylayers[activelayer].flipy = actionFlipMicroscopyVertical->isChecked();
		microscopylayers[activelayer].x = microscopyx->value();
		microscopylayers[activelayer].y = microscopyy->value();
		microscopylayers[activelayer].width = microscopywidth->value();
		microscopylayers[activelayer].height = microscopyheight->value();
		microscopylayers[activelayer].angle = microscopyangle->value();

		//if (microscopylayers[activelayer].flipx) {
		//	microscopylayers[activelayer].y = microscopylayers[microscopylayers[activelayer].navigationlayer].y + microscopylayers[microscopylayers[activelayer].navigationlayer].height - (microscopylayers[activelayer].y + microscopylayers[activelayer].height - microscopylayers[microscopylayers[activelayer].navigationlayer].y);
		//}

		//if (microscopylayers[activelayer].flipy) {
		//	 microscopylayers[activelayer].x = microscopylayers[microscopylayers[activelayer].navigationlayer].x + microscopylayers[microscopylayers[activelayer].navigationlayer].width - (microscopylayers[activelayer].x + microscopylayers[activelayer].width - microscopylayers[microscopylayers[activelayer].navigationlayer].x);
		//}

		imagewindowwidget->setMicroscopyPosition(activelayer, microscopylayers[activelayer].flipx, microscopylayers[activelayer].flipy, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);

		if (activelayer == 3) {
			if (microscopylayers[activelayer].type == 0) {
				for (int i = 4; i < size; i++) {
					if ((microscopylayers[i].navigationlayer == 3) && (!microscopylayers[i].image->size().isEmpty())) {
						widthratio = microscopylayers[3].width / microscopylayers[3].defaultwidth;
						heightratio = microscopylayers[3].height / microscopylayers[3].defaultheight;

						microscopylayers[i].flipx = microscopylayers[3].flipx;
						microscopylayers[i].flipy = microscopylayers[3].flipy;
						microscopylayers[i].x = microscopylayers[3].x + (microscopylayers[3].width - microscopylayers[i].defaultwidth * widthratio) / 2.0 + (microscopylayers[i].stagex - microscopylayers[3].stagex) * widthratio * 1000000.0;
						microscopylayers[i].y = microscopylayers[3].y + (microscopylayers[3].height - microscopylayers[i].defaultheight * heightratio) / 2.0 + (microscopylayers[i].stagey - microscopylayers[3].stagey) * heightratio * 1000000.0;
						microscopylayers[i].width = microscopylayers[i].defaultwidth * widthratio;
						microscopylayers[i].height = microscopylayers[i].defaultheight * heightratio;
						microscopylayers[i].angle = microscopylayers[3].angle - lastnavigationangle + microscopylayers[i].angle;

						//if (microscopylayers[i].flipx) {
						//	microscopylayers[i].y = microscopylayers[3].y + microscopylayers[3].height - (microscopylayers[activelayer].y + microscopylayers[activelayer].height - microscopylayers[3].y);
						//}

						//if (microscopylayers[i].flipy) {
						//	microscopylayers[i].x = microscopylayers[3].x + microscopylayers[3].width - (microscopylayers[activelayer].x + microscopylayers[activelayer].width - microscopylayers[3].x);
						//}

						imagewindowwidget->setMicroscopyPosition(i, microscopylayers[i].flipx, microscopylayers[i].flipy, microscopylayers[i].x, microscopylayers[i].y, microscopylayers[i].width, microscopylayers[i].height, microscopylayers[i].angle);
					}
				}
			}

			if (microscopylayers[activelayer].type == 1) {
				for (int i = 4; i < size; i++) {
					if ((microscopylayers[i].navigationlayer == 3) && (!microscopylayers[i].image->size().isEmpty())) {
						widthratio = microscopylayers[3].width / microscopylayers[3].defaultwidth;
						heightratio = microscopylayers[3].height / microscopylayers[3].defaultheight;

						microscopylayers[i].flipx = microscopylayers[3].flipx;
						microscopylayers[i].flipy = microscopylayers[3].flipy;
						microscopylayers[i].x = microscopylayers[3].x + (microscopylayers[i].stagex - microscopylayers[3].stagex) * widthratio * 1000000.0;
						microscopylayers[i].y = microscopylayers[3].y + (microscopylayers[i].stagey - microscopylayers[3].stagey) * heightratio * 1000000.0;
						microscopylayers[i].width = microscopylayers[i].defaultwidth * widthratio;
						microscopylayers[i].height = microscopylayers[i].defaultheight * heightratio;
						microscopylayers[i].angle = microscopylayers[3].angle - lastnavigationangle + microscopylayers[i].angle;

						//if (microscopylayers[i].flipx) {
						//	microscopylayers[i].y = microscopylayers[3].y + microscopylayers[3].height - (microscopylayers[activelayer].y + microscopylayers[activelayer].height - microscopylayers[3].y);
						//}

						//if (microscopylayers[i].flipy) {
						//	microscopylayers[i].x = microscopylayers[3].x + microscopylayers[3].width - (microscopylayers[activelayer].x + microscopylayers[activelayer].width - microscopylayers[3].x);
						//}

						imagewindowwidget->setMicroscopyPosition(i, microscopylayers[i].flipx, microscopylayers[i].flipy, microscopylayers[i].x, microscopylayers[i].y, microscopylayers[i].width, microscopylayers[i].height, microscopylayers[i].angle);
					}
				}
			}
		}

		imagewindowwidget->redraw();
	}
}


void cImageWindow::microscopyDefaultButtonReleased() {
	if (activelayer >= 3) {
		if (activelayer == 3) {
			int size = (int)microscopylayers.size();

			if (microscopylayers[activelayer].type == 0) {
				microscopylayers[activelayer].flipx = false;
				microscopylayers[activelayer].flipy = false;
				microscopylayers[activelayer].x = 0;
				microscopylayers[activelayer].y = 0;
				microscopylayers[activelayer].width = microscopylayers[activelayer].defaultwidth;
				microscopylayers[activelayer].height = microscopylayers[activelayer].defaultheight;
				microscopylayers[activelayer].angle = 0;
				imagewindowwidget->setMicroscopyPosition(activelayer, microscopylayers[activelayer].flipx, microscopylayers[activelayer].flipy, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);

				for (int i = 4; i < size; i++) {
					if ((microscopylayers[i].navigationlayer == 3) && (!microscopylayers[i].image->size().isEmpty())) {
						microscopylayers[i].flipx = false;
						microscopylayers[i].flipy = false;
						microscopylayers[i].x = microscopylayers[3].x + (microscopylayers[3].width - microscopylayers[i].defaultwidth) / 2.0 + (microscopylayers[i].stagex - microscopylayers[3].stagex) * 1000000.0;
						microscopylayers[i].y = microscopylayers[3].y + (microscopylayers[3].height - microscopylayers[i].defaultheight) / 2.0 + (microscopylayers[i].stagey - microscopylayers[3].stagey) * 1000000.0;
						microscopylayers[i].width = microscopylayers[i].defaultwidth;
						microscopylayers[i].height = microscopylayers[i].defaultheight;
						microscopylayers[i].angle = 0;
						imagewindowwidget->setMicroscopyPosition(i, microscopylayers[i].flipx, microscopylayers[i].flipy, microscopylayers[i].x, microscopylayers[i].y, microscopylayers[i].width, microscopylayers[i].height, microscopylayers[i].angle);
					}
				}
			}

			if (microscopylayers[activelayer].type == 1) {
				microscopylayers[activelayer].flipx = false;
				microscopylayers[activelayer].flipy = false;
				microscopylayers[activelayer].x = microscopylayers[activelayer].stagex * 1000000.0;
				microscopylayers[activelayer].y = microscopylayers[activelayer].stagey * 1000000.0;
				microscopylayers[activelayer].width = microscopylayers[activelayer].defaultwidth;
				microscopylayers[activelayer].height = microscopylayers[activelayer].defaultheight;
				microscopylayers[activelayer].angle = 0;

				// fix the coordinates if they are outside of range
				if (vendor == bruker) {
					if ((microscopylayers[activelayer].x + microscopylayers[activelayer].width > (maxx->value() + 1) * pixelsizexspinbox->value()) || (microscopylayers[activelayer].y + microscopylayers[activelayer].height > (maxy->value() + 1) * pixelsizeyspinbox->value())) {
						microscopylayers[activelayer].x = 0;
						microscopylayers[activelayer].y = 0;
					}
				}
				else {
					if ((microscopylayers[activelayer].x + microscopylayers[activelayer].width > maxx->value() * pixelsizexspinbox->value()) || (microscopylayers[activelayer].y + microscopylayers[activelayer].height > maxy->value() * pixelsizeyspinbox->value())) {
						microscopylayers[activelayer].x = 0;
						microscopylayers[activelayer].y = 0;
					}
				}

				imagewindowwidget->setMicroscopyPosition(activelayer, microscopylayers[activelayer].flipx, microscopylayers[activelayer].flipy, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);

				for (int i = 4; i < size; i++) {
					if ((microscopylayers[i].navigationlayer == 3) && (!microscopylayers[i].image->size().isEmpty())) {
						microscopylayers[i].flipx = false;
						microscopylayers[i].flipy = false;
						microscopylayers[i].x = microscopylayers[3].x + (microscopylayers[i].stagex - microscopylayers[3].stagex) * 1000000.0;
						microscopylayers[i].y = microscopylayers[3].y + (microscopylayers[i].stagey - microscopylayers[3].stagey) * 1000000.0;
						microscopylayers[i].width = microscopylayers[i].defaultwidth;
						microscopylayers[i].height = microscopylayers[i].defaultheight;
						microscopylayers[i].angle = 0;

						if ((microscopylayers[i].x < 0) || (microscopylayers[i].y < 0)) {
							microscopylayers[i].x = 0;
							microscopylayers[i].y = 0;
						}

						imagewindowwidget->setMicroscopyPosition(i, microscopylayers[i].flipx, microscopylayers[i].flipy, microscopylayers[i].x, microscopylayers[i].y, microscopylayers[i].width, microscopylayers[i].height, microscopylayers[i].angle);
					}
				}
			}
		}

		if (activelayer > 3) {
			if ((microscopylayers[activelayer].type == 0) && (microscopylayers[activelayer].navigationlayer >= 3)) {
				if (activelayer != microscopylayers[activelayer].navigationlayer) {
					microscopylayers[activelayer].flipx = microscopylayers[microscopylayers[activelayer].navigationlayer].flipx;
					microscopylayers[activelayer].flipy = microscopylayers[microscopylayers[activelayer].navigationlayer].flipy;
					microscopylayers[activelayer].x = microscopylayers[microscopylayers[activelayer].navigationlayer].x + (microscopylayers[microscopylayers[activelayer].navigationlayer].width - microscopylayers[activelayer].defaultwidth) / 2.0 + (microscopylayers[activelayer].stagex - microscopylayers[microscopylayers[activelayer].navigationlayer].stagex) * 1000000.0;
					microscopylayers[activelayer].y = microscopylayers[microscopylayers[activelayer].navigationlayer].y + (microscopylayers[microscopylayers[activelayer].navigationlayer].height - microscopylayers[activelayer].defaultheight) / 2.0 + (microscopylayers[activelayer].stagey - microscopylayers[microscopylayers[activelayer].navigationlayer].stagey) * 1000000.0;
					microscopylayers[activelayer].width = microscopylayers[activelayer].defaultwidth;
					microscopylayers[activelayer].height = microscopylayers[activelayer].defaultheight;
					microscopylayers[activelayer].angle = microscopylayers[microscopylayers[activelayer].navigationlayer].angle;
					imagewindowwidget->setMicroscopyPosition(activelayer, microscopylayers[activelayer].flipx, microscopylayers[activelayer].flipy, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);
				}
				else {
					microscopylayers[activelayer].flipx = false;
					microscopylayers[activelayer].flipy = false;
					microscopylayers[activelayer].x = 0;
					microscopylayers[activelayer].y = 0;
					microscopylayers[activelayer].width = microscopylayers[activelayer].defaultwidth;
					microscopylayers[activelayer].height = microscopylayers[activelayer].defaultheight;
					microscopylayers[activelayer].angle = 0;
					imagewindowwidget->setMicroscopyPosition(activelayer, microscopylayers[activelayer].flipx, microscopylayers[activelayer].flipy, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);
				}
			}

			if ((microscopylayers[activelayer].type == 1) && (microscopylayers[activelayer].navigationlayer >= 3)) {
				if (activelayer != microscopylayers[activelayer].navigationlayer) {
					microscopylayers[activelayer].flipx = microscopylayers[microscopylayers[activelayer].navigationlayer].flipx;
					microscopylayers[activelayer].flipy = microscopylayers[microscopylayers[activelayer].navigationlayer].flipy;
					microscopylayers[activelayer].x = microscopylayers[microscopylayers[activelayer].navigationlayer].x + (microscopylayers[activelayer].stagex - microscopylayers[microscopylayers[activelayer].navigationlayer].stagex) * 1000000.0;
					microscopylayers[activelayer].y = microscopylayers[microscopylayers[activelayer].navigationlayer].y + (microscopylayers[activelayer].stagey - microscopylayers[microscopylayers[activelayer].navigationlayer].stagey) * 1000000.0;
					microscopylayers[activelayer].width = microscopylayers[activelayer].defaultwidth;
					microscopylayers[activelayer].height = microscopylayers[activelayer].defaultheight;
					microscopylayers[activelayer].angle = microscopylayers[microscopylayers[activelayer].navigationlayer].angle;

					if ((microscopylayers[activelayer].x < 0) || (microscopylayers[activelayer].y < 0)) {
						microscopylayers[activelayer].x = 0;
						microscopylayers[activelayer].y = 0;
					}

					imagewindowwidget->setMicroscopyPosition(activelayer, microscopylayers[activelayer].flipx, microscopylayers[activelayer].flipy, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);
				}
				else {
					microscopylayers[activelayer].flipx = false;
					microscopylayers[activelayer].flipy = false;
					microscopylayers[activelayer].x = 0;
					microscopylayers[activelayer].y = 0;
					microscopylayers[activelayer].width = microscopylayers[activelayer].defaultwidth;
					microscopylayers[activelayer].height = microscopylayers[activelayer].defaultheight;
					microscopylayers[activelayer].angle = 0;
					imagewindowwidget->setMicroscopyPosition(activelayer, microscopylayers[activelayer].flipx, microscopylayers[activelayer].flipy, microscopylayers[activelayer].x, microscopylayers[activelayer].y, microscopylayers[activelayer].width, microscopylayers[activelayer].height, microscopylayers[activelayer].angle);
				}
			}
		}

		actionFlipMicroscopyHorizontal->blockSignals(true);
		actionFlipMicroscopyVertical->blockSignals(true);
		microscopyx->blockSignals(true);
		microscopyy->blockSignals(true);
		microscopywidth->blockSignals(true);
		microscopyheight->blockSignals(true);
		microscopyangle->blockSignals(true);
		microscopynavigationcombobox->blockSignals(true);

		actionFlipMicroscopyHorizontal->setChecked(microscopylayers[activelayer].flipx);
		actionFlipMicroscopyVertical->setChecked(microscopylayers[activelayer].flipy);
		microscopyx->setValue(microscopylayers[activelayer].x);
		microscopyy->setValue(microscopylayers[activelayer].y);
		microscopywidth->setValue(microscopylayers[activelayer].width);
		microscopyheight->setValue(microscopylayers[activelayer].height);
		microscopyangle->setValue(microscopylayers[activelayer].angle);
		microscopynavigationcombobox->setCurrentIndex(max(0, microscopylayers[activelayer].navigationlayer - 3));

		actionFlipMicroscopyHorizontal->blockSignals(false);
		actionFlipMicroscopyVertical->blockSignals(false);
		microscopyx->blockSignals(false);
		microscopyy->blockSignals(false);
		microscopywidth->blockSignals(false);
		microscopyheight->blockSignals(false);
		microscopyangle->blockSignals(false);
		microscopynavigationcombobox->blockSignals(false);

		imagewindowwidget->redraw();
	}
}


void cImageWindow::microscopyGoButtonReleased() {
	if (activelayer >= 3) {
		imagewindowwidget->goToMicroscopyPosition(activelayer);
	}
}


void cImageWindow::microscopyPrevButtonReleased() {
	if ((activelayer > 3) && (activelayer < microscopylayers.size())) {
		((QRadioButton *)layerslayout->itemAtPosition(activelayer - 1, 0)->widget())->setChecked(true);
		imagewindowwidget->goToMicroscopyPosition(activelayer);
	}
}


void cImageWindow::microscopyNextButtonReleased() {
	if ((activelayer >= 3) && (activelayer < microscopylayers.size() - 1)) {
		((QRadioButton *)layerslayout->itemAtPosition(activelayer + 1, 0)->widget())->setChecked(true);
		imagewindowwidget->goToMicroscopyPosition(activelayer);
	}
}


void cImageWindow::microscopyPositionChanged(double value) {
	microscopyChanged();
}


void cImageWindow::microscopyAngleChanged(double value) {
	microscopyChanged();
}


void cImageWindow::microscopyFlipStateChanged(bool state) {
	microscopyChanged();
}


void cImageWindow::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/imagefusion.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/imagefusion.html").absoluteFilePath()));
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

	emit layerChanged(row, ((QCheckBox *)layerslayout->itemAtPosition(row, 1)->widget())->isChecked(), alpha, ((QSpinBox *)layerslayout->itemAtPosition(row, 5)->widget())->value(), redrawenabled);
}


void cImageWindow::layerZValueChangedSlot(int zvalue) {
	int row, column, rowSpan, columnSpan;
	QSpinBox* spinbox = qobject_cast<QSpinBox*>(sender());

	layerslayout->getItemPosition(layerslayout->indexOf(spinbox), &row, &column, &rowSpan, &columnSpan);

	emit layerChanged(row, ((QCheckBox *)layerslayout->itemAtPosition(row, 1)->widget())->isChecked(), ((QSpinBox *)layerslayout->itemAtPosition(row, 4)->widget())->value(), zvalue, redrawenabled);
}


void cImageWindow::layerCheckBoxChangedSlot(int state) {
	int row, column, rowSpan, columnSpan;
	QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());

	layerslayout->getItemPosition(layerslayout->indexOf(checkbox), &row, &column, &rowSpan, &columnSpan);

	emit layerChanged(row, ((QCheckBox *)layerslayout->itemAtPosition(row, 1)->widget())->isChecked(), ((QSpinBox *)layerslayout->itemAtPosition(row, 4)->widget())->value(), ((QSpinBox *)layerslayout->itemAtPosition(row, 5)->widget())->value(), redrawenabled);
}


void cImageWindow::activeLayerChangedSlot(bool checked) {
	if (checked) {
		int row, column, rowSpan, columnSpan;
		QRadioButton* radiobutton = qobject_cast<QRadioButton*>(sender());

		layerslayout->getItemPosition(layerslayout->indexOf(radiobutton), &row, &column, &rowSpan, &columnSpan);

		colorSpinBoxes(row);

		activelayer = row;
		emit activeLayerChanged(row);

		actionFlipMicroscopyHorizontal->blockSignals(true);
		actionFlipMicroscopyVertical->blockSignals(true);
		microscopyx->blockSignals(true);
		microscopyy->blockSignals(true);
		microscopywidth->blockSignals(true);
		microscopyheight->blockSignals(true);
		microscopyangle->blockSignals(true);
		microscopynavigationcombobox->blockSignals(true);

		if ((activelayer >= 3) && (!microscopylayers[activelayer].image->size().isEmpty())) {
			actionFlipMicroscopyHorizontal->setChecked(microscopylayers[activelayer].flipx);
			actionFlipMicroscopyVertical->setChecked(microscopylayers[activelayer].flipy);
			microscopyx->setValue(microscopylayers[activelayer].x);
			microscopyy->setValue(microscopylayers[activelayer].y);
			microscopywidth->setValue(microscopylayers[activelayer].width);
			microscopyheight->setValue(microscopylayers[activelayer].height);
			microscopyangle->setValue(microscopylayers[activelayer].angle);
			microscopynavigationcombobox->setCurrentIndex(max(0, microscopylayers[activelayer].navigationlayer - 3));
		}
		else {
			actionFlipMicroscopyHorizontal->setChecked(false);
			actionFlipMicroscopyVertical->setChecked(false);
			microscopyx->setValue(0);
			microscopyy->setValue(0);
			microscopywidth->setValue(0);
			microscopyheight->setValue(0);
			microscopyangle->setValue(0);
			microscopynavigationcombobox->setCurrentIndex(0);
		}

		actionFlipMicroscopyHorizontal->blockSignals(false);
		actionFlipMicroscopyVertical->blockSignals(false);
		microscopyx->blockSignals(false);
		microscopyy->blockSignals(false);
		microscopywidth->blockSignals(false);
		microscopyheight->blockSignals(false);
		microscopyangle->blockSignals(false);
		microscopynavigationcombobox->blockSignals(false);

		if ((activelayer == 0) || (activelayer == 1)) {
			if (actionShowSelection->isChecked()) {
				actionShowSelection->setChecked(false);
			}
		}

		if (activelayer >= 2) {
			if (!actionShowSelection->isChecked()) {
				actionShowSelection->setChecked(true);
			}
		}
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


void cImageWindow::updateMicroscopySlot(bool flipx, bool flipy, double x, double y, double width, double height, double angle) {
	actionFlipMicroscopyHorizontal->blockSignals(true);
	actionFlipMicroscopyVertical->blockSignals(true);
	microscopyx->blockSignals(true);
	microscopyy->blockSignals(true);
	microscopywidth->blockSignals(true);
	microscopyheight->blockSignals(true);
	microscopyangle->blockSignals(true);

	actionFlipMicroscopyHorizontal->setChecked(flipx);
	actionFlipMicroscopyVertical->setChecked(flipy);
	microscopyx->setValue(x);
	microscopyy->setValue(y);
	microscopywidth->setValue(width);
	microscopyheight->setValue(height);
	microscopyangle->setValue(angle);

	actionFlipMicroscopyHorizontal->blockSignals(false);
	actionFlipMicroscopyVertical->blockSignals(false);
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


void cImageWindow::imageDoubleClickedSlot(int spectrumid) {
	emit doubleClickedSpectrumIDSignal(spectrumid);
}


void cImageWindow::loadLayers() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Layers..."), lastdirlayersfile, tr("Layers File (*.layers)"));

	if (filename.isEmpty()) {
		return;
	}

	lastdirlayersfile = filename;
	layersfileloaded = true;

	QString shortname;
	if (lastdirlayersfile.toStdString().rfind('/') != string::npos) {
		shortname = lastdirlayersfile.toStdString().substr(lastdirlayersfile.toStdString().rfind('/') + 1).c_str();
	}
	if (!shortname.isEmpty()) {
		setWindowTitle(editorname + QString(" - ") + shortname);
	}
	else {
		setWindowTitle(editorname);
	}

	clearLayers();

	QString qloadstring, qlayername;
	QSettings settings(lastdirlayersfile, QSettings::IniFormat);

	QString opticalfilename = settings.value("opticalimage_file", "").toString();
	QFileInfo opticalfilecheck(opticalfilename);
	if (!(opticalfilecheck.exists() && opticalfilecheck.isFile())) {
		QMessageBox msgBox;
		QString errstr = "The optical image does not exist. Cannot open the file '" + opticalfilename + "'.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	QProgressDialog progress("Loading layers...", /*"Cancel"*/0, 0, max(5, (int)microscopylayers.size()), this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);
	progress.setValue(0);

	reopenOpticalImage(opticalfilename);

	leftshift->setValue(settings.value("opticalimage_horizontal_shift", 0).toInt());
	topshift->setValue(settings.value("opticalimage_vertical_shift", 0).toInt());
	maxx->setValue(settings.value("opticalimage_max_x", defaultmaxx).toInt());
	maxy->setValue(settings.value("opticalimage_max_y", defaultmaxy).toInt());
	pixelsizexspinbox->setValue(settings.value("opticalimage_pixel_width", defaultpixelsizex).toInt());
	pixelsizeyspinbox->setValue(settings.value("opticalimage_pixel_height", defaultpixelsizey).toInt());

	setMaxButtonReleased();

	progress.setValue(1);

	QString histologyfilename = settings.value("histologyimage_file", "").toString();
	if (!histologyfilename.isEmpty()) {
		QFileInfo histologyfilecheck(histologyfilename);
		if (histologyfilecheck.exists() && histologyfilecheck.isFile()) {
			reopenHistologyImage(histologyfilename);

			histologyx->blockSignals(true);
			histologyy->blockSignals(true);
			histologywidth->blockSignals(true);
			histologyheight->blockSignals(true);
			histologyangle->blockSignals(true);

			qloadstring = "histologyimage_flip_horizontal";
			settings.value(qloadstring, 0).toInt() == 0 ? actionFlipHistologyHorizontal->setChecked(false) : actionFlipHistologyHorizontal->setChecked(true);

			qloadstring = "histologyimage_flip_vertical";
			settings.value(qloadstring, 0).toInt() == 0 ? actionFlipHistologyVertical->setChecked(false) : actionFlipHistologyVertical->setChecked(true);

			histologyx->setValue(settings.value("histologyimage_x", 0).toInt());
			histologyy->setValue(settings.value("histologyimage_y", 0).toInt());
			histologywidth->setValue(settings.value("histologyimage_width", 400).toInt());
			histologyheight->setValue(settings.value("histologyimage_height", (int)((double)histologyimage->height() / (double)histologyimage->width() * (double)histologywidth->value())).toInt());
			histologyangle->setValue(settings.value("histologyimage_angle", 0).toDouble());

			histologyx->blockSignals(false);
			histologyy->blockSignals(false);
			histologywidth->blockSignals(false);
			histologyheight->blockSignals(false);
			histologyangle->blockSignals(false);

			histologyChanged();
		}
		else {
			QMessageBox msgBox;
			QString errstr = "The histology image does not exist. Cannot open the file '" + histologyfilename + "'.";
			msgBox.setText(errstr);
			msgBox.exec();
			progress.setValue(max(5, (int)microscopylayers.size()));
			return;
		}
	}

	progress.setValue(2);

	int size = settings.value("microscopy_layers_count", 0).toInt();
	microscopylayers.resize(max(3, size));

	for (int i = 3; i < size; i++) {
		qlayername = "microscopy_layer_" + QVariant(i).toString() + "_";

		qloadstring = qlayername + "sourcename";
		microscopylayers[i].sourcename = settings.value(qloadstring, "").toString();

		qloadstring = qlayername + "imagename";
		microscopylayers[i].imagename = settings.value(qloadstring, "").toString();

		qloadstring = qlayername + "thresholdmin";
		microscopylayers[i].thresholdmin = settings.value(qloadstring, 0).toInt();

		qloadstring = qlayername + "thresholdmax";
		microscopylayers[i].thresholdmax = settings.value(qloadstring, 255).toInt();

		if (!microscopylayers[i].sourcename.isEmpty()) {
			QFileInfo microscopyfilecheck(microscopylayers[i].sourcename);
			if (microscopyfilecheck.exists() && microscopyfilecheck.isFile()) {

				if ((microscopylayers[i].sourcename.toStdString().size() > 4) && (microscopylayers[i].sourcename.toStdString().substr(microscopylayers[i].sourcename.toStdString().size() - 4).compare(".lif") == 0)) {
					vector<cLifMetadata> parsedmetadata;
					readLifMetadata(microscopylayers[i].sourcename.toStdString(), parsedmetadata);

					for (auto& it : parsedmetadata) {
						if (it.imagename.compare(microscopylayers[i].imagename.toStdString()) == 0) {
							if (i == 3) {
								openLifImage(microscopylayers[i].sourcename.toStdString(), i, it, true);
							}
							else {
								openLifImage(microscopylayers[i].sourcename.toStdString(), i, it, false);
							}
							break;
						}
					}
				}
				else {
					reopenMicroscopyImage(i, microscopylayers[i].sourcename, true);
				}

				qloadstring = qlayername + "type";
				microscopylayers[i].type = settings.value(qloadstring, -1).toInt();

				qloadstring = qlayername + "navigationlayer";
				setNavigationLayer(i, settings.value(qloadstring, 3).toInt());

				qloadstring = qlayername + "flipx";
				microscopylayers[i].flipx = settings.value(qloadstring, 0).toBool();

				qloadstring = qlayername + "flipy";
				microscopylayers[i].flipy = settings.value(qloadstring, 0).toBool();

				qloadstring = qlayername + "x";
				microscopylayers[i].x = settings.value(qloadstring, 0).toDouble();

				qloadstring = qlayername + "y";
				microscopylayers[i].y = settings.value(qloadstring, 0).toDouble();

				qloadstring = qlayername + "width";
				microscopylayers[i].width = settings.value(qloadstring, 0).toDouble();

				qloadstring = qlayername + "height";
				microscopylayers[i].height = settings.value(qloadstring, 0).toDouble();

				qloadstring = qlayername + "angle";
				microscopylayers[i].angle = settings.value(qloadstring, 0).toDouble();

				qloadstring = qlayername + "defaultwidth";
				microscopylayers[i].defaultwidth = settings.value(qloadstring, 0).toDouble();

				qloadstring = qlayername + "defaultheight";
				microscopylayers[i].defaultheight = settings.value(qloadstring, 0).toDouble();

				qloadstring = qlayername + "stagex";
				microscopylayers[i].stagex = settings.value(qloadstring, 0).toDouble();

				qloadstring = qlayername + "stagey";
				microscopylayers[i].stagey = settings.value(qloadstring, 0).toDouble();

				imagewindowwidget->setMicroscopyPosition(i, microscopylayers[i].flipx, microscopylayers[i].flipy, microscopylayers[i].x, microscopylayers[i].y, microscopylayers[i].width, microscopylayers[i].height, microscopylayers[i].angle);
			}
			else {
				QMessageBox msgBox;
				QString errstr = "The microscopy image does not exist. Cannot open the file '" + microscopylayers[i].sourcename + "'.";
				msgBox.setText(errstr);
				msgBox.exec();
				progress.setValue(max(5, (int)microscopylayers.size()));
				return;
			}
		}

		progress.setValue(i);
	}

	redrawenabled = false;

	for (int i = 0; i < size; i++) {
		qlayername = "layer_info_" + QVariant(i).toString() + "_";

		qloadstring = qlayername + "selected";
		settings.value(qloadstring, 0).toInt() == 0 ? ((QRadioButton *)layerslayout->itemAtPosition(i, 0)->widget())->setChecked(false) : ((QRadioButton *)layerslayout->itemAtPosition(i, 0)->widget())->setChecked(true);

		if (i >= 3) {
			if (settings.value(qloadstring, 0).toInt() == 1) {
				actionFlipMicroscopyHorizontal->blockSignals(true);
				actionFlipMicroscopyVertical->blockSignals(true);
				microscopyx->blockSignals(true);
				microscopyy->blockSignals(true);
				microscopywidth->blockSignals(true);
				microscopyheight->blockSignals(true);
				microscopyangle->blockSignals(true);
				microscopynavigationcombobox->blockSignals(true);

				actionFlipMicroscopyHorizontal->setChecked(microscopylayers[i].flipx);
				actionFlipMicroscopyVertical->setChecked(microscopylayers[i].flipy);
				microscopyx->setValue(microscopylayers[i].x);
				microscopyy->setValue(microscopylayers[i].y);
				microscopywidth->setValue(microscopylayers[i].width);
				microscopyheight->setValue(microscopylayers[i].height);
				microscopyangle->setValue(microscopylayers[i].angle);
				microscopynavigationcombobox->setCurrentIndex(max(0, microscopylayers[i].navigationlayer - 3));

				actionFlipMicroscopyHorizontal->blockSignals(false);
				actionFlipMicroscopyVertical->blockSignals(false);
				microscopyx->blockSignals(false);
				microscopyy->blockSignals(false);
				microscopywidth->blockSignals(false);
				microscopyheight->blockSignals(false);
				microscopyangle->blockSignals(false);
				microscopynavigationcombobox->blockSignals(false);
			}
		}

		qloadstring = qlayername + "checked";
		settings.value(qloadstring, 0).toInt() == 0 ? ((QCheckBox *)layerslayout->itemAtPosition(i, 1)->widget())->setChecked(false) : ((QCheckBox *)layerslayout->itemAtPosition(i, 1)->widget())->setChecked(true);

		qloadstring = qlayername + "label";
		((QLabel *)layerslayout->itemAtPosition(i, 2)->widget())->setText(settings.value(qloadstring, "Unknown Layer").toString());

		qloadstring = qlayername + "slider_alpha";
		((QSlider *)layerslayout->itemAtPosition(i, 3)->widget())->setValue(settings.value(qloadstring, 100).toInt());

		qloadstring = qlayername + "spinbox_alpha";
		((QSpinBox *)layerslayout->itemAtPosition(i, 4)->widget())->setValue(settings.value(qloadstring, 100).toInt());

		qloadstring = qlayername + "spinbox_zvalue";
		((QSpinBox *)layerslayout->itemAtPosition(i, 5)->widget())->setValue(settings.value(qloadstring, 1).toInt());

		qloadstring = qlayername + "description";
		((QLabel *)layerslayout->itemAtPosition(i, 7)->widget())->setText(settings.value(qloadstring, "").toString());
	}

	redrawenabled = true;

	imagewindowwidget->redraw();

	progress.setValue(max(5, (int)microscopylayers.size()));
}


void cImageWindow::saveLayers() {
	if (!layersfileloaded || saveascalled) {
		QString filename = QFileDialog::getSaveFileName(this, tr("Save Layers As..."), lastdirlayersfile, tr("Layers File (*.layers)"));
		if (filename.isEmpty()) {
			saveascalled = false;
			return;
		}
		lastdirlayersfile = filename;
	}

	if (layersfileloaded && !saveascalled) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, editorname, "Save changes ?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No) {
			return;
		}
	}

	layersfileloaded = true;
	saveascalled = false;

	QString shortname;
	if (lastdirlayersfile.toStdString().rfind('/') != string::npos) {
		shortname = lastdirlayersfile.toStdString().substr(lastdirlayersfile.toStdString().rfind('/') + 1).c_str();
	}
	if (!shortname.isEmpty()) {
		setWindowTitle(editorname + QString(" - ") + shortname);
	}
	else {
		setWindowTitle(editorname);
	}

	QString qsavestring, qlayername;
	QSettings settings(lastdirlayersfile, QSettings::IniFormat);
	settings.clear();

	//QProgressDialog progress("Saving layers...", /*"Cancel"*/0, 0, max(5, (int)microscopylayers.size()), this);
	//progress.setMinimumWidth(250);
	//cEventFilter filter;
	//progress.installEventFilter(&filter);
	//progress.setMinimumDuration(0);
	//progress.setWindowModality(Qt::ApplicationModal);
	//progress.setValue(0);

	settings.setValue("opticalimage_file", lastdiropticalimage);
	settings.setValue("opticalimage_horizontal_shift", leftshift->value());
	settings.setValue("opticalimage_vertical_shift", topshift->value());
	settings.setValue("opticalimage_max_x", maxx->value());
	settings.setValue("opticalimage_max_y", maxy->value());
	settings.setValue("opticalimage_pixel_width", pixelsizexspinbox->value());
	settings.setValue("opticalimage_pixel_height", pixelsizeyspinbox->value());

	//progress.setValue(1);

	qsavestring = "histologyimage_flip_horizontal";
	actionFlipHistologyHorizontal->isChecked() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);

	qsavestring = "histologyimage_flip_vertical";
	actionFlipHistologyVertical->isChecked() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);

	QFileInfo histologyfilecheck(lastdirhistologyimage);
	if (histologyfilecheck.exists() && histologyfilecheck.isFile()) {
		settings.setValue("histologyimage_file", lastdirhistologyimage);
	}
	else {
		settings.setValue("histologyimage_file", "");
	}

	settings.setValue("histologyimage_x", histologyx->value());
	settings.setValue("histologyimage_y", histologyy->value());
	settings.setValue("histologyimage_width", histologywidth->value());
	settings.setValue("histologyimage_height", histologyheight->value());
	settings.setValue("histologyimage_angle", histologyangle->value());

	//progress.setValue(2);

	int size = (int)microscopylayers.size();
	settings.setValue("microscopy_layers_count", size);

	for (int i = 3; i < size; i++) {
		qlayername = "microscopy_layer_" + QVariant(i).toString() + "_";

		qsavestring = qlayername + "type";
		settings.setValue(qsavestring, microscopylayers[i].type);

		qsavestring = qlayername + "navigationlayer";
		settings.setValue(qsavestring, microscopylayers[i].navigationlayer);

		qsavestring = qlayername + "sourcename";
		settings.setValue(qsavestring, microscopylayers[i].sourcename);

		qsavestring = qlayername + "imagename";
		settings.setValue(qsavestring, microscopylayers[i].imagename);

		qsavestring = qlayername + "flipx";
		settings.setValue(qsavestring, microscopylayers[i].flipx);

		qsavestring = qlayername + "flipy";
		settings.setValue(qsavestring, microscopylayers[i].flipy);

		qsavestring = qlayername + "x";
		settings.setValue(qsavestring, microscopylayers[i].x);

		qsavestring = qlayername + "y";
		settings.setValue(qsavestring, microscopylayers[i].y);

		qsavestring = qlayername + "width";
		settings.setValue(qsavestring, microscopylayers[i].width);

		qsavestring = qlayername + "height";
		settings.setValue(qsavestring, microscopylayers[i].height);

		qsavestring = qlayername + "angle";
		settings.setValue(qsavestring, microscopylayers[i].angle);

		qsavestring = qlayername + "defaultwidth";
		settings.setValue(qsavestring, microscopylayers[i].defaultwidth);

		qsavestring = qlayername + "defaultheight";
		settings.setValue(qsavestring, microscopylayers[i].defaultheight);

		qsavestring = qlayername + "stagex";
		settings.setValue(qsavestring, microscopylayers[i].stagex);

		qsavestring = qlayername + "stagey";
		settings.setValue(qsavestring, microscopylayers[i].stagey);

		qsavestring = qlayername + "thresholdmin";
		settings.setValue(qsavestring, microscopylayers[i].thresholdmin);

		qsavestring = qlayername + "thresholdmax";
		settings.setValue(qsavestring, microscopylayers[i].thresholdmax);

		//progress.setValue(i);
	}

	for (int i = 0; i < size; i++) {
		qlayername = "layer_info_" + QVariant(i).toString() + "_";

		qsavestring = qlayername + "selected";
		((QRadioButton *)layerslayout->itemAtPosition(i, 0)->widget())->isChecked() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);

		qsavestring = qlayername + "checked";
		((QCheckBox *)layerslayout->itemAtPosition(i, 1)->widget())->isChecked() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);
		
		qsavestring = qlayername + "label";
		settings.setValue(qsavestring, ((QLabel *)layerslayout->itemAtPosition(i, 2)->widget())->text());

		qsavestring = qlayername + "slider_alpha";
		settings.setValue(qsavestring, ((QSlider *)layerslayout->itemAtPosition(i, 3)->widget())->value());

		qsavestring = qlayername + "spinbox_alpha";
		settings.setValue(qsavestring, ((QSpinBox *)layerslayout->itemAtPosition(i, 4)->widget())->value());

		qsavestring = qlayername + "spinbox_zvalue";
		settings.setValue(qsavestring, ((QSpinBox *)layerslayout->itemAtPosition(i, 5)->widget())->value());

		qsavestring = qlayername + "description";
		settings.setValue(qsavestring, ((QLabel *)layerslayout->itemAtPosition(i, 7)->widget())->text());
	}

	//progress.setValue(max(5, (int)microscopylayers.size()));
}


void cImageWindow::saveLayersAs() {
	saveascalled = true;
	saveLayers();
}


void cImageWindow::clearAllLayersSlot() {
	QMessageBox::StandardButton reply;
	QString s = "Clear all layers ?";

	reply = QMessageBox::question(this, "Are you sure ?", s, QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::No) {
		return;
	}

	clearLayers();

	((QCheckBox *)layerslayout->itemAtPosition(0, 1)->widget())->setChecked(true);

	QString shortname = getShortFileName(imzmlfilename).c_str();
	((QLabel *)layerslayout->itemAtPosition(0, 7)->widget())->setText(shortname);
}


void cImageWindow::microscopyNavigationChanged(int index) {
	if (activelayer < 3) {
		return;
	}

	setNavigationLayer(activelayer, index + 3);
	microscopyDefaultButtonReleased();
}


void cImageWindow::clearLayer() {
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	int index = layerslayout->indexOf(button);

	int row, column, rowspan, colspan;
	layerslayout->getItemPosition(index, &row, &column, &rowspan, &colspan);

	if (row == 0) {
		return;
	}

	((QLabel *)layerslayout->itemAtPosition(row, 7)->widget())->setText("");
	microscopylayers[row].clear();

	imagewindowwidget->clearLayer(row);
	
	// optical image
	if (row == 1) {
		redrawenabled = false;

		leftshift->setValue(0);
		topshift->setValue(0);
		maxx->setValue(500);
		maxy->setValue(500);
		pixelsizexspinbox->setValue(100);
		pixelsizeyspinbox->setValue(100);

		setMaxButtonReleased();

		redrawenabled = true;
	}

	// histology image
	if (row == 2) {
		redrawenabled = false;

		//actionFlipHistologyHorizontal->blockSignals(true);
		//actionFlipHistologyVertical->blockSignals(true);
		histologyx->blockSignals(true);
		histologyy->blockSignals(true);
		histologywidth->blockSignals(true);
		histologyheight->blockSignals(true);
		histologyangle->blockSignals(true);

		actionFlipHistologyHorizontal->setChecked(false);
		actionFlipHistologyVertical->setChecked(false);

		histologyx->setValue(0);
		histologyy->setValue(0);
		histologywidth->setValue(0);
		histologyheight->setValue(0);
		histologyangle->setValue(0);

		//actionFlipHistologyHorizontal->blockSignals(false);
		//actionFlipHistologyVertical->blockSignals(false);
		histologyx->blockSignals(false);
		histologyy->blockSignals(false);
		histologywidth->blockSignals(false);
		histologyheight->blockSignals(false);
		histologyangle->blockSignals(false);

		histologyChanged();

		redrawenabled = true;
	}

	// microscopy images
	if (row >= 3) {
		if (row == activelayer) {
			redrawenabled = false;

			actionFlipMicroscopyHorizontal->blockSignals(true);
			actionFlipMicroscopyVertical->blockSignals(true);
			microscopyx->blockSignals(true);
			microscopyy->blockSignals(true);
			microscopywidth->blockSignals(true);
			microscopyheight->blockSignals(true);
			microscopyangle->blockSignals(true);
			microscopynavigationcombobox->blockSignals(true);

			actionFlipMicroscopyHorizontal->setChecked(false);
			actionFlipMicroscopyVertical->setChecked(false);

			microscopyx->setValue(0);
			microscopyy->setValue(0);
			microscopywidth->setValue(0);
			microscopyheight->setValue(0);
			microscopyangle->setValue(0);
			microscopynavigationcombobox->setCurrentIndex(0);

			actionFlipMicroscopyHorizontal->blockSignals(false);
			actionFlipMicroscopyVertical->blockSignals(false);
			microscopyx->blockSignals(false);
			microscopyy->blockSignals(false);
			microscopywidth->blockSignals(false);
			microscopyheight->blockSignals(false);
			microscopyangle->blockSignals(false);
			microscopynavigationcombobox->blockSignals(false);

			redrawenabled = true;
		}
	}

	imagewindowwidget->redraw();
}

