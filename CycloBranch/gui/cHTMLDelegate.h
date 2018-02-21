/**
	\file cHTMLDelegate.h
	\brief A delegate to display rich text.
*/


#ifndef _CHTMLDELEGATE_H
#define _CHTMLDELEGATE_H

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
#include <QTextDocument>


/**
	\brief A delegate to display rich text.
*/
class cHTMLDelegate : public QStyledItemDelegate {

	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cHTMLDelegate(QObject* parent = (QObject *)0);


	/**
		\brief Reimplementation of paint.
		\param painter QPainter
		\param option QStyleOptionViewItem
		\param index QModelIndex
	*/
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;


	//QSize sizeHint (const QStyleOptionViewItem& option, const QModelIndex& index) const;


};


#endif

