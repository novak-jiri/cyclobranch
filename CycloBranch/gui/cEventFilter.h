/**
	\file cEventFilter.h
	\brief An event filter.
*/


#ifndef _CEVENTFILTER_H
#define _CEVENTFILTER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>


/**
	\brief An event filter.
*/
class cEventFilter : public QObject {

	Q_OBJECT

 protected:
 
	/**
		\brief Filter events.
		\param obj pointer to QObject
		\param event pointer to QKeyEvent
	*/ 
	bool eventFilter(QObject *obj, QEvent *event);
 
};


#endif

