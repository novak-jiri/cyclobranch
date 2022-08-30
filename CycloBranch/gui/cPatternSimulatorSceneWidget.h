/**
	\file cPatternSimulatorSceneWidget.h
	\brief The implementation of an isotopic pattern simulator (graphics).
*/


#ifndef _CPATTERNSIMULATORSCENEWIDGET_H
#define _CPATTERNSIMULATORSCENEWIDGET_H

#include <iostream>
#include <QWidget>
#include <QGraphicsView>

#include "core/cTheoreticalSpectrum.h"


using namespace std;


// forward declaration
class QPaintEvent;
class QGraphicsScene;
class QMatrix;
class QGraphicsItemGroup;
class QGraphicsRectItem;
class QGraphicsSimpleTextItem;


/**
	\brief Compute value of Gaussian function.
	\param x x
	\param a a
	\param ny ny
	\param sigma sigma
	\retval double f(x)
*/
double computeGaussFunction(double x, double a, double ny, double sigma);


/**
	\brief Compute height of Gaussian function (a).
	\param x x
	\param y y
	\param ny ny
	\param sigma sigma
	\retval double a
*/
double computeGaussHeight(double x, double y, double ny, double sigma);


/**
	\brief The implementation of an isotopic pattern simulator (graphics).
*/
class cPatternSimulatorSceneWidget : public QGraphicsView
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cPatternSimulatorSceneWidget(QWidget* parent = 0);


	/**
		\brief The destructor.
	*/ 
	~cPatternSimulatorSceneWidget();


	/**
		\brief Initialize the widget.
		\param isotopicpattern an isotopic pattern
		\param fwhm fwhm
	*/ 
	void initialize(cPeaksList isotopicpattern, double fwhm);


	/**
		\brief Export spectrum scene into a PDF or a PS file.
		\param filename filename
		\param postscript if true then PS file is generated instead of PDF
	*/ 
	void exportToPDF(QString filename, bool postscript);


	/**
		\brief Export spectrum scene into a SVG file.
		\param filename filename
	*/ 
	void exportToSVG(QString filename);


	/**
		\brief Export spectrum scene into a PNG file.
		\param filename filename
	*/ 
	void exportToPNG(QString filename);


	/**
		\brief Hide/Show labels of peaks.
		\param state the peak labels are hidden when \a state is true
	*/
	void hidePeakLabels(bool state);


protected:


	/**
		\brief Handle the mouse wheel event.
		\param event pointer to QWheelEvent
	*/ 
	void wheelEvent(QWheelEvent *event);


	/**
		\brief Handle the mouse move event.
		\param event pointer to QMouseEvent
	*/ 
	void mouseMoveEvent(QMouseEvent *event);


	/**
		\brief Handle the mouse release event.
		\param event pointer to QMouseEvent
	*/ 
	void mouseReleaseEvent(QMouseEvent *event);


	/**
		\brief Handle the mouse press event.
		\param event pointer to QMouseEvent
	*/ 
	void mousePressEvent(QMouseEvent *event);


	/**
		\brief Handle the widget show event.
		\param event pointer to QShowEvent
	*/
	void showEvent(QShowEvent *event);


	/**
		\brief Handle the widget resize event.
		\param event pointer to QResizeEvent
	*/
	void resizeEvent(QResizeEvent *event);


private:

	QWidget* parent;
	QGraphicsScene* scene;
	QMatrix originalmatrix;

	QGraphicsItemGroup* zoomgroup;
	QGraphicsRectItem* zoomrect;
	QGraphicsSimpleTextItem* zoomsimpletextitem;

	QGraphicsSimpleTextItem* cursorsimpletextitem;

	cPeaksList isotopicpattern;

	qreal currentscale;
	qreal factor;

	int origwidth;
	int origheight;

	bool calledbyresizeevent;
	deque<int> oldwidth;
	deque<int> oldheight;

	bool absoluteintensity;
	bool rawdatastate;
	bool hidelabels;

	int pressedx;
	int pressedy;

	int currentx;
	int currenty;

	double minmzratio;
	double maxmzratio;

	int topmargin;
	int bottommargin;
	int leftmargin;
	int rightmargin;

	bool firstshow;
	bool enablemousemzselection;

	double mzoverhead;
	double fwhm;


	double getMZRatioFromXPosition(int x, int w);


	int getXPositionFromMZRatio(double mzratio, int w);


	double getIntensityFromYPosition(int y, int h);


	int getYPositionFromIntensity(double intensity, int h);


	void redrawScene();


	void updateZoomGroup();


	void calculateMinMaxMZ();


	double getMaximumIntensity();


signals:


	/**
		\brief The signal is emitted when the range of m/z ratios has been changed.
		\param minmz a minimum threshold of m/z ratio
		\param maxmz a maximum threshold of m/z ratio
	*/ 
	void updateMZInterval(double minmz, double maxmz);


private slots:


	void zoomIn();


	void zoomOut();

	
	void normalSize();


	void absoluteIntensityStateChanged(bool state);


	void rawDataStateChanged(bool state);

	
	void setMZInterval(double minmz, double maxmz);


	void resetMZInterval();


	void enableMouseMzSelectionTool(bool enable);

};

#endif
