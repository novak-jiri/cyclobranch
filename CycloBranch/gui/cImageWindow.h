/**
	\file cImageWindow.h
	\brief Image window.
*/


#ifndef _CIMAGEWINDOW_H
#define _CIMAGEWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QToolBar>
#include <QAction>
#include <QGridLayout>
#include <QCheckBox>
#include <QSlider>
#include <QSplitter>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QProgressDialog>
#include "core/utilities.h"
#include "core/cGlobalPreferences.h"
#include "gui/cImageWindowWidget.h"
#include "gui/cImageWindowImportDialog.h"
#include "gui/cImageWindowTabOptical.h"


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QImage;
class QLabel;
class QAction;
class QToolBar;
class QMenuBar;
class QMenu;


/**
	\brief Paramaters of a microscopy layer.
*/
struct microscopyLayerInfo {
	/**
		\brief Type (0 = FEI SEM, 1 = LEICA LIF).
	*/
	int type;


	/**
		\brief Index of navigation layer.
	*/
	int navigationlayer;


	/**
		\brief Source file name.
	*/
	QString sourcename;


	/**
		\brief Image name.
	*/
	QString imagename;


	/**
		\brief Flip horizontally.
	*/
	bool flipx;


	/**
		\brief Flip vertically.
	*/
	bool flipy;


	/**
		\brief X position.
	*/
	double x;


	/**
		\brief Y position.
	*/
	double y;


	/**
		\brief Width.
	*/
	double width;


	/**
		\brief Height.
	*/
	double height;


	/**
		\brief Rotation angle [degrees].
	*/
	double angle;


	/**
		\brief Default width.
	*/
	double defaultwidth;


	/**
		\brief Default height.
	*/
	double defaultheight;


	/**
		\brief Stage X.
	*/
	double stagex;


	/**
		\brief Stage Y.
	*/
	double stagey;


	/**
		\brief QImage.
	*/
	QImage* image;


	/**
		\brief True if the image size has been reduced.
	*/
	bool reduced;


	/**
		\brief Noise threshold.
	*/
	int thresholdmin;


	/**
		\brief Signal threshold.
	*/
	int thresholdmax;


	/**
		\brief The constructor.
	*/
	microscopyLayerInfo() {
		image = new QImage();
		clear();
	}


	/**
		\brief The copy constructor.
	*/
	microscopyLayerInfo(const microscopyLayerInfo& layer) {
		type = layer.type;
		navigationlayer = layer.navigationlayer;
		sourcename = layer.sourcename;
		imagename = layer.imagename;
		flipx = layer.flipx;
		flipy = layer.flipy;
		x = layer.x;
		y = layer.y;
		width = layer.width;
		height = layer.height;
		angle = layer.angle;
		defaultwidth = layer.defaultwidth;
		defaultheight = layer.defaultheight;
		stagex = layer.stagex;
		stagey = layer.stagey;
		image = new QImage();
		*image = *layer.image;
		reduced = layer.reduced;
		thresholdmin = layer.thresholdmin;
		thresholdmax = layer.thresholdmax;
	}

	
	/**
		\brief The destructor.
	*/
	~microscopyLayerInfo() {
		delete image;
	}


	/**
		\brief Clear the structure.
	*/
	void clear() {
		type = -1;
		navigationlayer = 3;
		sourcename.clear();
		imagename.clear();
		flipx = false;
		flipy = false;
		x = 0;
		y = 0;
		width = 0;
		height = 0;
		angle = 0;
		defaultwidth = 0;
		defaultheight = 0;
		stagex = 0;
		stagey = 0;

		delete image;
		image = new QImage();

		reduced = false;
		thresholdmin = 0;
		thresholdmax = 255;
	}
};


/**
	\brief Image window.
*/
class cImageWindow : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/ 
	cImageWindow(cGlobalPreferences* globalpreferences, QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cImageWindow();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/
	void closeEvent(QCloseEvent *event);


	/**
		\brief Set the filename and some default values parsed from the imzML file.
		\param imzmlfilename name of imzML file
		\param defaultmaxx max count of pixel x - parsed from imzML file
		\param defaultmaxy max count of pixel y - parsed from imzML file
		\param defaultpixelsizex pixel size x - parsed from imzML file
		\param defaultpixelsizey pixel size y - parsed from imzML file
		\param vendor vendor type
	*/
	void setDefaultValues(string imzmlfilename, int defaultmaxx, int defaultmaxy, int defaultpixelsizex, int defaultpixelsizey, eVendorType vendor);


	/**
		\brief Add a layer.
		\param name layer name
	*/
	void addLayer(QString name);


	/**
		\brief Apply new global preferences.
		\param globalpreferences global preferences of the application
	*/
	void applyGlobalPreferences(cGlobalPreferences* globalpreferences);


private:

	QString editorname;
	
	cImageWindowImportDialog importdialog;

	cGlobalPreferences* globalpreferences;
	QWidget* parent;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuView;
	QMenu* menuHelp;
	
	QToolBar* toolbarFile;
	QAction* actionLoadLayers;
	QAction* actionSaveLayers;
	QAction* actionSaveLayersAs;
	QAction* actionOpenImage;
	QAction* actionImportDialog;
	QAction* actionExportImage;
	QAction* actionClearAll;
	QAction* actionCloseWindow;

	QToolBar* toolbarView;
	QAction* actionShowSelection;
	QAction* actionColorScale;
	QAction* actionAbsoluteIntensity;
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	QAction* actionFlipHistologyHorizontal;
	QAction* actionFlipHistologyVertical;
	QAction* actionFlipMicroscopyHorizontal;
	QAction* actionFlipMicroscopyVertical;

	QLabel* rulerLabel;
	QDoubleSpinBox* rulerValue;

	QToolBar* toolbarRegion;
	QWidget* regionwidget;
	QHBoxLayout* regionhbox;
	QLabel* xfromlabel;
	QSpinBox* xfrom;
	QLabel* xtolabel;
	QSpinBox* xto;
	QLabel* yfromlabel;
	QSpinBox* yfrom;
	QLabel* ytolabel;
	QSpinBox* yto;
	QPushButton* setregionbutton;
	QPushButton* resetregionbutton;

	QToolBar* toolbarMaxCoordinates;
	QWidget* maxcoordinateswidget;
	QHBoxLayout* maxcoordinateshbox;
	QLabel* leftshiftlabel;
	QSpinBox* leftshift;
	QLabel* topshiftlabel;
	QSpinBox* topshift;
	QLabel* maxxlabel;
	QSpinBox* maxx;
	QLabel* maxylabel;
	QSpinBox* maxy;
	QLabel* pixelsizexlabel;
	QSpinBox* pixelsizexspinbox;
	QLabel* pixelsizeylabel;
	QSpinBox* pixelsizeyspinbox;
	QPushButton* setmaxbutton;
	QPushButton* setdefaultbutton;

	QToolBar* toolbarHistology;
	QWidget* histologywidget;
	QHBoxLayout* histologyhbox;
	QLabel* histologyxlabel;
	QSpinBox* histologyx;
	QLabel* histologyylabel;
	QSpinBox* histologyy;
	QLabel* histologywidthlabel;
	QSpinBox* histologywidth;
	QLabel* histologyheightlabel;
	QSpinBox* histologyheight;
	QLabel* histologyanglelabel;
	QDoubleSpinBox* histologyangle;

	QToolBar* toolbarMicroscopy;
	QWidget* microscopywidget;
	QHBoxLayout* microscopyhbox;
	QLabel* microscopyxlabel;
	QDoubleSpinBox* microscopyx;
	QLabel* microscopyylabel;
	QDoubleSpinBox* microscopyy;
	QLabel* microscopywidthlabel;
	QDoubleSpinBox* microscopywidth;
	QLabel* microscopyheightlabel;
	QDoubleSpinBox* microscopyheight;
	QLabel* microscopyanglelabel;
	QDoubleSpinBox* microscopyangle;
	QLabel* microscopynavigationlabel;
	QComboBox* microscopynavigationcombobox;
	QPushButton* microscopydefaultbutton;
	QPushButton* microscopygobutton;
	QPushButton* microscopyprevbutton;
	QPushButton* microscopynextbutton;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QImage* image;
	QImage* histologyimage;

	// layer == 0 - compounds
	// layer == 1 - optical image
	// layer == 2 - histology image
	// layer == 3 - microscopy navigation image
	// layer >= 4 - microscopy images
	vector<microscopyLayerInfo> microscopylayers;

	cImageWindowWidget* imagewindowwidget;
	QGridLayout* layerslayout;
	QWidget* layerswidget;
	QScrollArea* layersscrollarea;
	QSplitter* mainwidget;

	int layerscount;
	int activelayer;

	QString lastdiropticalimage;
	QString lastdirhistologyimage;
	QString lastdirmicroscopyimage;
	QString lastdirexportimage;
	QString lastdirlayersfile;

	bool layersfileloaded;
	bool saveascalled;

	string imzmlfilename;

	int defaultmaxx;
	int defaultmaxy;

	int defaultpixelsizex;
	int defaultpixelsizey;

	eVendorType vendor;

	bool redrawenabled;

	void colorSpinBoxes(int layerid);

	void openOpticalImage();

	void reopenOpticalImage(QString filename);

	void openHistologyImage();

	void reopenHistologyImage(QString filename);

	bool parseMicroscopyImage(QString& filename, double& pixelwidth, double& pixelheight, int& resolutionx, int& resolutiony, double& realwidth, double& stagex, double& stagey);

	void openMicroscopyImage(int layer, const QString &layername);

	void reopenMicroscopyImage(int layer, QString filename, bool reopen);

	void clearLayers();

	void updateMicroscopyNavigationCombobox(int numberofitems);

	bool checkLoadedImage(QImage* image);

	void readLifImageOptical(string filename, cLifMetadata& metadata, QImage& image, bool enableprogress);

	void readLifImageFluorescence(string filename, cLifMetadata& metadata, QImage& image, int minthreshold, int maxthreshold, bool enableprogress);

	void openLifImage(string liffilename, int layer, cLifMetadata& metadata, bool enableprogress);

	void setNavigationLayer(int layer, int navigation);


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


	/**
		\brief Handle a key release event.
		\param event pointer to QKeyEvent
	*/
	void keyReleaseEvent(QKeyEvent *event);


private slots:


	void closeWindow();


	void selectImageType();


	void imageTypeSelected(const QString &s);


	void openImportDialog();


	void exportImage();


	void setFilterOptionsSlot(vector<cCoordinateInfo> coordinates, bool operatortype, string columnname1, string comparatorname1, string filterstring1, string columnname2, string comparatorname2, string filterstring2, bool casesensitive, bool wholeword);


	void setMaxButtonReleased();


	void setDefaultButtonReleased();


	void setRegionButtonReleased();


	void histologyChanged();


	void histologyPositionChanged(int value);


	void histologyAngleChanged(double value);


	void microscopyChanged();


	void microscopyDefaultButtonReleased();


	void microscopyGoButtonReleased();


	void microscopyPrevButtonReleased();


	void microscopyNextButtonReleased();


	void microscopyPositionChanged(double value);


	void microscopyAngleChanged(double value);


	void microscopyFlipStateChanged(bool state);


	void showHTMLDocumentation();


	void updateSelection(int xmin, int xmax, int ymin, int ymax);

	
	void clearSelection();


	void resetSelection();


	void updatePointsArea(int xmin, int xmax, int ymin, int ymax);


	void layerSpinBoxChangedSlot(int alpha);


	void layerZValueChangedSlot(int zvalue);


	void layerCheckBoxChangedSlot(int state);


	void activeLayerChangedSlot(bool checked);


	void updateHistologySlot(int x, int y, int width, int height, double angle);


	void updateMicroscopySlot(bool flipx, bool flipy, double x, double y, double width, double height, double angle);


	void updateRulerSlot(double value);


	void imageDoubleClickedSlot(int spectrumid);


	void loadLayers();


	void saveLayers();


	void saveLayersAs();


	void clearAllLayersSlot();


	void microscopyNavigationChanged(int index);


	void clearLayer();


signals:


	/**
		\brief The signal is emitted when the region selection was changed.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/
	void updateSummaryPeaksTableFilter(int xmin, int xmax, int ymin, int ymax);


	/**
		\brief The signal is emitted when a layer was changed.
		\param layerid id of the layer
		\param checked true if the layer was checked, false if the layer was unchecked
		\param alpha transparency of the layer
		\param zvalue z-value of the layer
		\param redraw redraw scene
	*/
	void layerChanged(int layerid, bool checked, int alpha, int zvalue, bool redraw);


	/**
		\brief The signal is emitted when a layer is selected.
		\param layerid id of the layer
	*/
	void activeLayerChanged(int layerid);


	/**
		\brief The signal is emitted the image is double clicked.
		\param spectrumid id of a spectrum
	*/
	void doubleClickedSpectrumIDSignal(int spectrumid);


};

#endif
