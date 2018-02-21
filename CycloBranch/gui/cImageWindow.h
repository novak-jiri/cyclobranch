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
#include "core/utilities.h"
#include "gui/cImageWindowWidget.h"


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QImage;
class QLabel;
class QSpinBox;
class QAction;
class QToolBar;
class QMenuBar;
class QMenu;


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
		\param maxcountx max count of pixel x - parsed from imzML file
		\param maxcounty max count of pixel y - parsed from imzML file
		\param vendor vendor type
	*/
	void setDefaultMaxXY(int maxcountx, int maxcounty, eVendorType vendor);


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
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	QAction* actionColorScale;
	QAction* actionAbsoluteIntensity;

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
	QAction* actionCorrelatePointsArea;
	QWidget* maxcoordinateswidget;
	QHBoxLayout* maxcoordinateshbox;
	QLabel* minxlabel;
	QSpinBox* minx;
	QLabel* minylabel;
	QSpinBox* miny;
	QLabel* maxxlabel;
	QSpinBox* maxx;
	QLabel* maxylabel;
	QSpinBox* maxy;
	QPushButton* setmaxbutton;
	QPushButton* setdefaultbutton;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QImage* image;
	cImageWindowWidget* imagewindowwidget;
	QVBoxLayout* mainlayout;
	QWidget* mainwidget;

	QString lastimagedir;

	int maxcountx;
	int maxcounty;
	eVendorType vendor;

protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:


	void closeWindow();


	void openImage();


	void saveImage();


	void setFilterOptions(vector<cCoordinates> coordinates, string filterstring, bool casesensitive);


	void setMaxButtonReleased();


	void setDefaultButtonReleased();


	void setRegionButtonReleased();


	void showHTMLDocumentation();


	void updateSelection(int xmin, int xmax, int ymin, int ymax);

	
	void clearSelection();


	void resetSelection();


	void updatePointsArea(int xmin, int xmax, int ymin, int ymax);


signals:


	/**
		\brief The signal is emitted when the region selection was changed.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/ 
	void updateSummaryPeaksTableFilter(int xmin, int xmax, int ymin, int ymax); 

};

#endif
