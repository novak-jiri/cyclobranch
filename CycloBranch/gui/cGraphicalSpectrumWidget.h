/**
	\file cGraphicalSpectrumWidget.h
	\brief The implementation of a widget painting a 2D mass spectrum.
*/


#ifndef _CGRAPHICALSPECTRUMWIDGET_H
#define _CGRAPHICALSPECTRUMWIDGET_H

#include <iostream>
#include <QWidget>

#include "core/cTheoreticalSpectrum.h"


using namespace std;


// forward declaration
class QPaintEvent;
class QScrollBar;


/**
	\brief The widget representing a 2D mass spectrum.
*/
class cGraphicalSpectrumWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cGraphicalSpectrumWidget(QWidget* parent = 0);


	/**
		\brief The destructor.
	*/ 
	~cGraphicalSpectrumWidget();


	/**
		\brief Initialize the widget.
		\param parameters a pointer to parameters
		\param theoreticalspectrum a pointer to a theoretical spectrum
	*/ 
	void initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum);


protected:


	/**
		\brief Handle the paint event.
		\param event pointer to QPaintEvent
	*/ 
	void paintEvent(QPaintEvent *event);


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


private:

	void setDefaultSize();

	double getMZRatioFromXPosition(int x, int w);

	int getXPositionFromMZRatio(double mzratio, int w);

	void drawVerticalText(QPainter* painter, QString text, int x, int y);

	QWidget* parent;
	cParameters* parameters;
	cTheoreticalSpectrum* theoreticalspectrum;

	qreal scale;
	qreal factor;

	int origwidth, origheight;
	int coloredrotationid;
	string coloredrotationstring;
	int coloredtrotationid;

	bool hideunmatched;
	bool hidematched;

	int pressedx;
	int pressedy;

	int currentx;
	int currenty;

	double minmzratio;
	double maxmzratio;

	static const int topmargin = 120;
	static const int bottommargin = 30;
	static const int leftmargin = 30;
	static const int rightmargin = 60;

signals:

	/**
		\brief The signal is emitted when the range of m/z ratios has been changed.
		\param minmz a minimum threshold of m/z ratio
		\param maxmz a maximum threshold of m/z ratio
	*/ 
	void updateMZInterval(double minmz, double maxmz);

private slots:

	void hideUnmatchedPeaks(int state);

	void hideMatchedPeaks(int state);

	void zoomIn();

	void zoomOut();

	void normalSize();

	void setMZInterval(double minmz, double maxmz);

	void resetMZInterval();

	void rotationChanged(int index);

	void trotationChanged(int index);

	void rotationChanged(QString text);

};

#endif