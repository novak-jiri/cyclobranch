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
#include "core/utilities.h"
#include "gui/cImageWindowWidget.h"


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
		\brief The constructor.
	*/
	microscopyLayerInfo() {
		x = 0;
		y = 0;
		width = 0;
		height = 0;
		angle = 0;
		defaultwidth = 0;
		defaultheight = 0;
		stagex = 0;
		stagey = 0;
		image = new QImage();
	}


	/**
		\brief The copy constructor.
	*/
	microscopyLayerInfo(const microscopyLayerInfo& layer) {
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
	}

	
	/**
		\brief The destructor.
	*/
	~microscopyLayerInfo() {
		delete image;
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
		\param parent pointer to a parent widget
	*/ 
	cImageWindow(QWidget* parent = (QWidget *)0);


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
		\brief Set the default max count of pixel x/y values parsed from an imzML file.
		\param defaultmaxx max count of pixel x - parsed from imzML file
		\param defaultmaxy max count of pixel y - parsed from imzML file
		\param pixelsize pixel size
		\param vendor vendor type
	*/
	void setDefaultMaxXY(int defaultmaxx, int defaultmaxy, int pixelsize, eVendorType vendor);


	/**
		\brief Add a layer.
		\param name layer name
	*/
	void addLayer(QString name);


private:

	QWidget* parent;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuView;
	QMenu* menuHelp;
	
	QToolBar* toolbarFile;
	QAction* actionOpenImage;
	QAction* actionSaveImage;
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
	QPushButton* microscopydefaultbutton;
	QPushButton* microscopygobutton;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QImage* image;
	QImage* histologyimage;

	vector<microscopyLayerInfo> microscopylayers;

	cImageWindowWidget* imagewindowwidget;
	QGridLayout* layerslayout;
	QWidget* layerswidget;
	QSplitter* mainwidget;

	int layerscount;
	int activelayer;

	QString lastimagedir;

	int defaultmaxx;
	int defaultmaxy;
	int pixelsize;
	eVendorType vendor;

	void colorSpinBoxes(int layerid);

	void openOpticalImage();

	void openHistologyImage();

	bool parseMicroscopyImage(QString& filename, double& pixelwidth, double& pixelheight, int& resolutionx, int& resolutiony, double& stagex, double& stagey);

	void openMicroscopyImage(eLayerType layer, const QString &layername);

protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:


	void closeWindow();


	void selectImageType();


	void imageTypeSelected(const QString &s);


	void saveImage();


	void setFilterOptionsSlot(vector<cCoordinates> coordinates, string columnname, string comparatorname, string filterstring, bool casesensitive, bool wholeword);


	void setMaxButtonReleased();


	void setDefaultButtonReleased();


	void setRegionButtonReleased();


	void histologyChanged();


	void histologyPositionChanged(int value);


	void histologyAngleChanged(double value);


	void microscopyChanged();


	void microscopyDefaultButtonReleased();


	void microscopyGoButtonReleased();


	void microscopyPositionChanged(double value);


	void microscopyAngleChanged(double value);


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


	void updateMicroscopySlot(double x, double y, double width, double height, double angle);


	void updateRulerSlot(double value);


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
	*/
	void layerChanged(int layerid, bool checked, int alpha, int zvalue);


	/**
		\brief The signal is emitted when a layer is selected.
		\param layerid id of the layer
	*/
	void activeLayerChanged(int layerid);


};

#endif
