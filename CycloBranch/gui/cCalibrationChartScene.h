/**
	\file cCalibrationChartScene.h
	\brief Calibration chart scene.
*/


#ifndef _CCALIBRATIONCHARTSCENE_H
#define _CCALIBRATIONCHARTSCENE_H

#include <iostream>
#include <cmath>
#include <deque>
#include <QWidget>
#include <QGraphicsView>
#include <QRectF>


using namespace std;


// forward declaration
class QPaintEvent;
class QGraphicsScene;
class QMatrix;
class QGraphicsItemGroup;
class QGraphicsRectItem;
class QGraphicsSimpleTextItem;


/**
	\brief Calculate the coefficient of determination R^2.
	\param a a
	\param b b
	\param xvalues x coordinates of points
	\param yvalues y coordinates of points
	\retval double R^2
*/
double calculateR2Value(double a, double b, vector<double> xvalues, vector<double> yvalues);


/**
	\brief Calculate limit of detection.
	\param a a
	\param xvalues x coordinates of points
	\param yvalues y coordinates of points
	\retval double limit of detection
*/
double calculateLOD(double a, vector<double> xvalues, vector<double> yvalues);


/**
	\brief Calculate limit of quantitation.
	\param a a
	\param xvalues x coordinates of points
	\param yvalues y coordinates of points
	\retval double limit of quantitation
*/
double calculateLOQ(double a, vector<double> xvalues, vector<double> yvalues);


/**
	\brief The implementation of an isotopic pattern simulator (graphics).
*/
class cCalibrationChartScene : public QGraphicsView
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cCalibrationChartScene(QWidget* parent = 0);


	/**
		\brief The destructor.
	*/ 
	~cCalibrationChartScene();


	/**
		\brief Initialize the widget.
	*/ 
	void initialize();


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
		\brief Hide labels of calibration points.
		\param state state of labels
	*/
	void hideLabels(bool state);


	/**
		\brief Set parameters of calibration curve.
		\param equationtype 0 (y = a*x), 1 (y = a*x + b), 2 (manual mode)
		\param a a
		\param b b 
		\param xvalues x coordinates of points
		\param yvalues y coordinates of points
		\param sd standard deviation of y-values
	*/
	void setLineParameters(int equationtype, double a, double b, vector<double> xvalues, vector<double> yvalues, vector<double> sd);


	/**
		\brief Set parameters of calibration curve.
		\param datax x coordinates of data points
		\param datay y coordinates of data points
		\param datasd standard deviation of y-values
		\param datagroups vector of group ids
	*/
	void setData(vector<double> datax, vector<double> datay, vector<double> datasd, vector<int> datagroups);


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

	//cPeaksList isotopicpattern;

	qreal currentscale;
	qreal factor;

	int origwidth;
	int origheight;

	bool calledbyresizeevent;
	deque<int> oldwidth;
	deque<int> oldheight;

	int pressedx;
	int pressedy;

	int currentx;
	int currenty;

	double minconcentration;
	double maxconcentration;

	int topmargin;
	int bottommargin;
	int leftmargin;
	int rightmargin;

	bool firstshow;
	bool enablemouseconcentrationselection;

	int equationtype;

	double a;
	double b;

	vector<double> xvalues;
	vector<double> yvalues;

	vector<double> sd;

	vector<double> datax;
	vector<double> datay;

	vector<double> datasd;
	vector<int> datagroups;

	bool hidelabels;


	double getConcentrationFromXPosition(int x, int w);


	int getXPositionFromConcentration(double concentration, int w);


	double getIntensityFromYPosition(int y, int h);


	int getYPositionFromIntensity(double intensity, int h);


	void redrawScene();


	void updateZoomGroup();


	void calculateMinMaxConcentration();


	double getMaximumIntensity();


signals:


	/**
		\brief The signal is emitted when the range of concentration has been changed.
		\param minconcentration a minimum threshold of concentration
		\param maxconcentration a maximum threshold of concentration
	*/ 
	void updateConcentrationInterval(double minconcentration, double maxconcentration);


private slots:


	void zoomIn();


	void zoomOut();


	void normalSize();


	void setConcentrationInterval(double minconcentration, double maxconcentration);


	void resetConcentrationInterval();


	//void enableMouseConcentrationSelectionTool(bool enable);

};

#endif
