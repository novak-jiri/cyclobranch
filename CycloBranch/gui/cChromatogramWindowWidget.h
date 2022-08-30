/**
	\file cChromatogramWindowWidget.h
	\brief Chromatogram window widget.
*/


#ifndef _CCHROMATOGRAMWINDOWWIDGET_H
#define _CCHROMATOGRAMWINDOWWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QScrollBar>
#include <QMessageBox>
#include <QPrinter>
#include <QSvgGenerator>
#include <QImage>
#include <QMouseEvent>
#include <QProgressDialog>
#include "core/utilities.h"
#include "core/cTheoreticalSpectrumList.h"
#include "gui/cEventFilter.h"
#include "gui/cPatternSimulatorSceneWidget.h"


using namespace std;


/**
	\brief Get area under Gaussian function.
	\param sigma sigma
	\param height height
	\retval double area
*/
double getGaussianArea(double sigma, double height);


/**
	\brief Get area of EIC peaks (Gaussian function).
	\param sigmas input vector of sigmas
	\param heights input vector of heights
	\retval double area
*/
double getGaussianAreas(vector<double>& sigmas, vector<double>& heights);


/**
	\brief Get area under Exponentially Modified Gaussian function.
	\param x some value of x
	\param h height of Gaussian
	\param ny ny of Gaussian
	\param sigma sigma of Gaussian
	\param tau tau of Exponential
	\param fronting true = fronting enabled, false = tailing enabled
	\retval double area
*/
double getExponentiallyModifiedGaussianArea(double x, double h, double ny, double sigma, double tau, bool fronting);


/**
	\brief Get area of EIC peaks (Exponentially Modified Gaussian function).
	\param xvalues vector of x values
	\param heights vector of heights of Gaussians
	\param nys vector of nys of Gaussians
	\param sigmas vector of sigmas of Gaussians
	\param taus vector of taus of Exponentials
	\param fronting true = fronting enabled, false = tailing enabled
	\retval double area
*/
double getExponentiallyModifiedGaussianAreas(vector<double>& xvalues, vector<double>& heights, vector<double>& nys, vector<double>& sigmas, vector<double>& taus, bool fronting);


/**
	\brief Calculate erfcx(z).
	\param z input value
	\retval erfcx(z)
*/
double erfcx(double z);


/**
	\brief Calculate erfcxinv(z).
	\param z input value
	\retval erfcxinv(z)
*/
double erfcxinv(double z);


/**
	\brief Compute value of exponentially modified Gaussian function.
	\param x x
	\param h h 
	\param ny ny
	\param sigma sigma
	\param tau tau
	\param fronting true = fronting enabled, false = tailing enabled
	\retval double f(x)
*/
double computeExponentiallyModifiedGaussFunction(double x, double h, double ny, double sigma, double tau, bool fronting);


/**
	\brief Compute value of exponential function.
	\param x x
	\param base base
	\param tau tau
	\param fronting true = fronting enabled, false = tailing enabled
	\retval double f(x)
*/
double computeExponentialFunction(double x, double base, double tau, bool fronting);


/**
	\brief Calculate ny, sigma, and height using Caruana's algorithm.
	\param x input vector of x-coordinates
	\param y input vector of y-coordinates
	\param ny output value of ny
	\param sigma output value of sigma
	\param height output value of height
*/
void calculateCaruana(vector<double>& x, vector<double>& y, double& ny, double& sigma, double& height);


/**
	\brief Calculate ny, sigma, and height using Guo's algorithm.
	\param x input vector of x-coordinates
	\param y input vector of y-coordinates
	\param iterations number of iterations
	\param ny output value of ny
	\param sigma output value of sigma
	\param height output value of height
*/
void calculateGuo(vector<double>& x, vector<double>& y, int iterations, double& ny, double& sigma, double& height);


/**
	\brief Calculate base and tau for exponential distribution.
	\param x input vector of x-coordinates
	\param y input vector of y-coordinates
	\param iterations number of iterations
	\param fronting true = fronting enabled, false = tailing enabled
	\param ynorm y axis normalization value
	\param base output value of base
	\param tau output value of tau
*/
void calculateBaseTau(vector<double>& x, vector<double>& y, int iterations, bool fronting, double ynorm, double& base, double& tau);


/**
	\brief Calculate tau for exponential distribution using weighted average.
	\param x input vector of x-coordinates
	\param y input vector of y-coordinates
	\param fronting true = fronting enabled, false = tailing enabled
	\param tau output value of tau
*/
void calculateTauUsingWeightedAverage(vector<double>& x, vector<double>& y, bool fronting, double& tau);


/**
	\brief Get parameters of Gaussians functions (nys, sigmas, and heights).
	\param eicchromatogram input EIC chromatogram
	\param rtimes input retention time vector
	\param timeunit RT time unit
	\param absoluteintensity use absolute intensities
	\param usert use RT instead of scan ids
	\param nys output vector of nys
	\param sigmas output vector of sigmas
	\param as output vector of heights
*/
void calculateGaussianParameters(cPeaksList& eicchromatogram, vector<double>& rtimes, int timeunit, bool absoluteintensity, bool usert, vector<double>& nys, vector<double>& sigmas, vector<double>& as);


/**
	\brief Get parameters of Exponential functions (bases and taus).
	\param eicchromatogram input EIC chromatogram
	\param rtimes input retention time vector
	\param timeunit RT time unit
	\param absoluteintensity use absolute intensities
	\param usert use RT instead of scan ids
	\param fronting true = fronting enabled, false = tailing enabled
	\param ynorm y axis normalization value
	\param bases output vector of bases
	\param taus output vector of taus
*/
void calculateExponentialParameters(cPeaksList& eicchromatogram, vector<double>& rtimes, int timeunit, bool absoluteintensity, bool usert, bool fronting, double ynorm, vector<double>& bases, vector<double>& taus);


/**
	\brief Chromatogram window widget.
*/
class cChromatogramWindowWidget : public QGraphicsView
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param listoftheoreticalspectra lists of theoretical spectra
		\param parent pointer to a parent widget
	*/ 
	cChromatogramWindowWidget(cTheoreticalSpectrumList& listoftheoreticalspectra, QWidget* parent = 0);


	/**
		\brief The destructor.
	*/ 
	~cChromatogramWindowWidget();


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
		\brief Recalculate TIC chromatogram.
		\param activefileid id of an active file
	*/
	void recalculateTICChromatogram(int activefileid);


	/**
		\brief Set EIC chromatogram.
	*/
	void setEICChromatogram(cPeaksList eic);


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
		\brief Handle the mouse double click event.
		\param event pointer to QMouseEvent
	*/
	void mouseDoubleClickEvent(QMouseEvent *event);


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

	cTheoreticalSpectrumList* listoftheoreticalspectra;
	cPeaksList ticchromatogram;
	cPeaksList eicchromatogram;
	vector<double> rtimes;

	QWidget* parent;
	QGraphicsScene* scene;
	QMatrix originalmatrix;

	QGraphicsItemGroup* zoomgroup;
	QGraphicsRectItem* zoomrect;
	QGraphicsSimpleTextItem* zoomsimpletextitem;

	int origwidth;
	int origheight;

	qreal currentscale;
	qreal factor;

	int pressedx;
	int pressedy;

	int currentx;
	int currenty;

	double mintime;
	double maxtime;

	int minscan;
	int maxscan;

	int topmargin;
	int bottommargin;
	int leftmargin;
	int rightmargin;

	bool firstshow;
	bool enablemouseselection;

	bool retentiontime;
	bool absoluteintensity;
	bool rawdatastate;
	int peakshape;
	bool hidetic;
	bool hideeic;
	bool hidelabels;


	int getScanIDFromXPosition(int x, int w);


	int getXPositionFromScanID(int scanid, int w);


	void redrawScene();


	void updateZoomGroup();


	void calculateMinMaxScan();


signals:

	/**
		\brief The signal is emitted when the range of retention time has been changed.
		\param mintime minimum retention time
		\param maxtime maximum retention time
	*/
	void updateRetentionTimeInterval(double mintime, double maxtime);


	/**
		\brief The signal is emitted when the range of scan numbers has been changed.
		\param minid minimum scan id
		\param maxid maximum scan id
	*/
	void updateScanIDInterval(int minid, int maxid);


	/**
		\brief A chromatographic peak was double clicked.
		\param scanid id of a scan
	*/
	void chromatogramWidgetDoubleClicked(int scanid);


private slots:


	void zoomIn();


	void zoomOut();


	void normalSize();


	void retentionTimeStateChanged(bool state);


	void absoluteIntensityStateChanged(bool state);


	void rawDataStateChanged(bool state);


	void setRetentionTimeInterval(double mintime, double maxtime);


	void resetRetentionTimeInterval();


	void setScanIDInterval(int minid, int maxid);


	void resetScanIDInterval();


	void enableMouseSelectionTool(bool enable);


	void hideTIC(bool state);


	void hideEIC(bool state);


	void hideLabels(bool state);


	void peakShapeChanged(int shape);
		
};

#endif
