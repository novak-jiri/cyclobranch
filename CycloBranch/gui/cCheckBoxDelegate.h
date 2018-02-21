/**
	\file cCheckBoxDelegate.h
	\brief A checkbox item delegate.
*/


#ifndef _CCHECKBOXDELEGATE_H
#define _CCHECKBOXDELEGATE_H

#include <QApplication>
#include <QString>
#include <QVariant>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QEvent>
#include <QAbstractItemModel>
#include <QCheckBox>
#include <QMouseEvent>


/**
	\brief A checkbox item delegate.
*/
class cCheckBoxDelegate : public QStyledItemDelegate {

	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cCheckBoxDelegate(QObject* parent = (QObject *)0);


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

