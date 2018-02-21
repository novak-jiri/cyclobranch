/**
	\file cViewButtonDelegate.h
	\brief A delegate to create a reference view button.
*/


#ifndef _CVIEWBUTTONDELEGATE_H
#define _CVIEWBUTTONDELEGATE_H

#include <QString>
#include <QLocale>
#include <QVariant>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QEvent>
#include <QAbstractItemModel>


/**
	\brief A delegate to create a reference view button.
*/
class cViewButtonDelegate : public QStyledItemDelegate {

	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cViewButtonDelegate(QObject* parent = (QObject *)0);


	/**
		\brief Reimplementation of displayText.
		\param value a value
		\param locale a locale
		\retval QString the value as a string
	*/
	QString displayText(const QVariant& value, const QLocale& locale) const;


	/**
		\brief Reimplementation of paint.
		\param painter QPainter
		\param option QStyleOptionViewItem
		\param index QModelIndex
	*/
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;


protected:


	/**
		\brief Reimplementation of editorEvent.
		\param event QEvent
		\param model QAbstractItemModel
		\param option QStyleOptionViewItem
		\param index QModelIndex
	*/
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
	
};


#endif

