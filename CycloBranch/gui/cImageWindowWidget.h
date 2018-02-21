/**
	\file cImageWindowWidget.h
	\brief Image window widget.
*/


#ifndef _CIMAGEWINDOWWIDGET_H
#define _CIMAGEWINDOWWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QImage>


/**
	\brief Image window widget.
*/
class cImageWindowWidget : public QWidget
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


private:

	QPixmap* pixmap;


	void paint(QPainter& painter);


protected:


	/**
		\brief Handle the paint event.
		\param event pointer to QPaintEvent
	*/ 
	void paintEvent(QPaintEvent *event);

};

#endif
