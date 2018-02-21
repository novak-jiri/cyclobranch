/**
	\file cComboBoxDelegate.h
	\brief A combobox item delegate.
*/


#ifndef _CCOMBOBOXDELEGATE_H
#define _CCOMBOBOXDELEGATE_H

#include <QApplication>
#include <QString>
#include <QLocale>
#include <QVariant>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QEvent>
#include <QAbstractItemModel>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QComboBox>
#include "core/cFragmentIons.h"

/**
	\brief A combobox item delegate.
*/
class cComboBoxDelegate : public QStyledItemDelegate {

	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cComboBoxDelegate(QObject* parent = (QObject *)0);


	/**
		\brief Reimplementation of paint.
		\param painter QPainter
		\param option QStyleOptionViewItem
		\param index QModelIndex
	*/
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;


	/**
		\brief Reimplementation of setEditorData.
		\param editor QWidget
		\param index QModelIndex
	*/
	void setEditorData(QWidget* editor, const QModelIndex& index) const;


	/**
		\brief Reimplementation of setModelData.
		\param editor QWidget
		\param model QAbstractItemModel
		\param index QModelIndex
	*/
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;


	/**
		\brief Reimplementation of createEditor.
		\param parent QWidget
		\param option QStyleOptionViewItem
		\param index QModelIndex
	*/
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

};


#endif

