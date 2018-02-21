/**
	\file cSpectrumSceneWidget.h
	\brief The implementation of a widget painting a 2D mass spectrum.
*/


#ifndef _CSPECTRUMSCENEWIDGET_H
#define _CSPECTRUMSCENEWIDGET_H

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
	\brief The widget representing a 2D mass spectrum.
*/
class cSpectrumSceneWidget : public QGraphicsView
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cSpectrumSceneWidget(QWidget* parent = 0);


	/**
		\brief The destructor.
	*/ 
	~cSpectrumSceneWidget();


	/**
		\brief Initialize the widget.
		\param parameters a pointer to parameters
		\param theoreticalspectrum a pointer to a theoretical spectrum
	*/ 
	void initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum);


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
		\brief Hide/Show unmatched peaks.
		\param state the peaks are hidden when \a state is true
	*/ 
	void hideUnmatchedPeaks(bool state);


	/**
		\brief Hide/Show matched peaks.
		\param state the peaks are hidden when \a state is true
	*/ 
	void hideMatchedPeaks(bool state);


	/**
		\brief Hide/Show scrambled peaks.
		\param state the peaks are hidden when \a state is true
	*/ 
	void hideScrambledPeaks(bool state);


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


private:

	QWidget* parent;
	QGraphicsScene* scene;
	QMatrix originalmatrix;

	QGraphicsItemGroup* zoomgroup;
	QGraphicsRectItem* zoomrect;
	QGraphicsSimpleTextItem* zoomsimpletextitem;

	cParameters* parameters;
	cTheoreticalSpectrum* theoreticalspectrum;

	qreal currentscale;
	qreal factor;

	int origwidth;
	int origheight;

	int coloredrotationid;
	string coloredrotationstring;
	int coloredtrotationid;

	bool absoluteintensity;
	bool hideunmatched;
	bool hidematched;
	bool hidescrambled;

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


	double getMZRatioFromXPosition(int x, int w);


	int getXPositionFromMZRatio(double mzratio, int w);


	void redrawScene();


	void updateZoomGroup();


	void calculateMinMaxMZ();


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

	
	void setMZInterval(double minmz, double maxmz);


	void resetMZInterval();


	void rotationChanged(int index);


	void trotationChanged(int index);


	void rotationChanged(QString text);


	void enableMouseMzSelectionTool(bool enable);


};

#endif
