/**
	\file cImageWindowWidget.h
	\brief Image window widget.
*/


#ifndef _CIMAGEWINDOWWIDGET_H
#define _CIMAGEWINDOWWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QPixmap>
#include <QImage>
#include <unordered_set>
#include "core/utilities.h"


/**
	\brief Image window widget.
*/
class cImageWindowWidget : public QGraphicsView
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cImageWindowWidget();


	/**
		\brief The destructor.
	*/ 
	~cImageWindowWidget();


	/**
		\brief Set pixmap from an image.
		\param image an image
	*/ 
	void setPixmap(QImage* image);


	/**
		\brief Get the image.
		\retval QImage image
	*/ 
	QImage getImage();


	/**
		\brief Set options which was used to filter the points.
		\param coordinates a vector of coordinates
		\param filteredstring a string used to filter the points
		\param casesensitive true if the string was used as a casesensitive, false otherwise
	*/ 
	void setFilterOptions(vector<cCoordinates>& coordinates, string& filteredstring, bool casesensitive);


	/**
		\brief Set the maximum X and Y coordinates.
		\param minx minimum X coordinate
		\param miny minimum Y coordinate
		\param maxx maximum X coordinate
		\param maxy maximum Y coordinate
	*/
	void setMaxXY(int minx, int miny, int maxx, int maxy);


	/**
		\brief Set the default maximum X and Y coordinates - parsed from the imzML file.
		\param maxcountx maximum X coordinate
		\param maxcounty maximum Y coordinate
		\param vendor vendor type
	*/
	void setDefaultMaxXY(int maxcountx, int maxcounty, eVendorType vendor);


protected:


	/**
		\brief Handle the mouse wheel event.
		\param event pointer to QWheelEvent
	*/ 
	void wheelEvent(QWheelEvent *event);


	/**
		\brief Handle the mouse press event.
		\param event pointer to QMouseEvent
	*/ 
	void mousePressEvent(QMouseEvent *event);


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


signals:


	/**
		\brief The signal is emitted when the selection of points has been changed.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/
	void updateFilter(int xmin, int xmax, int ymin, int ymax);


	/**
		\brief The signal is emitted when the points area in the optical image has been changed.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/
	void updateCorrelation(int xmin, int xmax, int ymin, int ymax);


private:

	QWidget* parent;
	QGraphicsScene* scene;
	QPixmap* pixmap;
	vector<cCoordinates> coordinates;
	string filteredstring;
	bool casesensitive;
	bool colorscale;
	bool absoluteintensity;

	int minx;
	int miny;
	int maxx;
	int maxy;
	int maxcountx;
	int maxcounty;
	eVendorType vendor;
	qreal currentscale;
	qreal factor;
	bool ispixmapdefined;
	bool enablecorrelateselectiontool;

	int pressedx;
	int pressedy;
	int currentx;
	int currenty;
	int currentwidth;
	int currentheight;

	QGraphicsItemGroup* selectiongroup;
	QGraphicsRectItem* selectionrect;
	QGraphicsSimpleTextItem* selectionsimpletextitem;

	void redrawScene();

	void updateSelectionGroup();


private slots:


	void zoomIn();


	void zoomOut();


	void normalSize();


	void scrollbarValueChanged(int value);

	
	void absoluteIntensityStateChanged(bool state);


	void colorScaleStateChanged(bool state);


	void enableCorrelateSelectionTool(bool enable);

};

#endif
