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
		\param parent pointer to a parent widget
	*/ 
	cImageWindowWidget(QWidget* parent = (QWidget *)0);


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
		\brief Set points.
		\param coordinates a vector of coordinates
	*/ 
	void setCoordinates(vector<cCoordinates>& coordinates);


	/**
		\brief Set the maximum X and Y coordinates.
		\param maxx maximum X coordinate
		\param maxy maximum Y coordinate
		\param leftmargin left margin
		\param topmargin top margin
	*/ 
	void setMaxXY(int maxx, int maxy, int leftmargin, int topmargin);


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
		\brief The signal is emitted when the selection of points has changed.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/ 
	void updateFilter(int xmin, int xmax, int ymin, int ymax);


private:


	QGraphicsScene* scene;
	QPixmap* pixmap;
	vector<cCoordinates> coordinates;

	int maxx;
	int maxy;
	int leftmargin;
	int topmargin;
	qreal currentscale;
	qreal factor;
	bool ispixmapdefined;
	bool ismaxxydefined;
	bool enablemouseselection;

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


};

#endif
