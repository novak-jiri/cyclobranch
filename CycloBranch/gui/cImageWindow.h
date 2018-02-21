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


private:

	QWidget* parent;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuZoom;
	QMenu* menuHelp;
	
	QToolBar* toolbarFile;
	QAction* actionOpenImage;
	QAction* actionSaveImage;
	QAction* actionCloseWindow;

	QToolBar* toolbarZoom;
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	//QAction* actionViewAllPoints;

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
	QLabel* maxxlabel;
	QSpinBox* maxx;
	QLabel* maxylabel;
	QSpinBox* maxy;
	QLabel* leftmarginlabel;
	QSpinBox* leftmargin;
	QLabel* topmarginlabel;
	QSpinBox* topmargin;
	QPushButton* setmaxbutton;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QImage* image;
	cImageWindowWidget* imagewindowwidget;
	QVBoxLayout* mainlayout;
	QWidget* mainwidget;

	QString lastimagedir;


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


	void setCoordinates(vector<cCoordinates> coordinates);


	void setMaxButtonReleased();


	void setRegionButtonReleased();


	void showHTMLDocumentation();


	void updateSelection(int xmin, int xmax, int ymin, int ymax);

	
	void resetSelection();


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
