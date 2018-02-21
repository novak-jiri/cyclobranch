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


using namespace std;


/**
	\brief Chromatogram window widget.
*/
class cChromatogramWindowWidget : public QGraphicsView
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param theoreticalspectrumlist list of spectra
		\param parent pointer to a parent widget
	*/ 
	cChromatogramWindowWidget(cTheoreticalSpectrumList& theoreticalspectrumlist, QWidget* parent = 0);


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
	*/
	void recalculateTICChromatogram();


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
		\brief Handle the widget show event.
		\param event pointer to QShowEvent
	*/
	void showEvent(QShowEvent *event);


private:

	cTheoreticalSpectrumList* theoreticalspectrumlist;
	cPeaksList ticchromatogram;
	cPeaksList eicchromatogram;

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

	int minscan;
	int maxscan;

	int topmargin;
	int bottommargin;
	int leftmargin;
	int rightmargin;

	bool firstshow;
	bool enablemouseselection;

	bool absoluteintensity;
	bool hidetic;
	bool hideeic;


	int getScanIDFromXPosition(int x, int w);


	int getXPositionFromScanID(int scanid, int w);


	void redrawScene();


	void updateZoomGroup();


signals:


	/**
		\brief The signal is emitted when the range of scans has been changed.
		\param minid minimum scan id
		\param maxid maximum scan id
	*/
	void updateScanIDInterval(int minid, int maxid);



private slots:


	void zoomIn();


	void zoomOut();


	void normalSize();


	void absoluteIntensityStateChanged(bool state);


	void setScanIDInterval(int minid, int maxid);


	void resetScanIDInterval();


	void enableMouseSelectionTool(bool enable);


	void hideTIC(bool state);


	void hideEIC(bool state);
		
};

#endif
