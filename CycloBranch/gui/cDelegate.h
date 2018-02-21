/**
	\file cDelegate.h
	\brief A delegate for visualization of doubles in QTableWidget.
*/


#ifndef _CDELEGATE_H
#define _CDELEGATE_H

#include <QString>
#include <QVariant>
#include <QLocale>
#include <QStyledItemDelegate>


/**
	\brief A delegate for visualization of doubles in QTableWidget.
*/
class cDelegate: public QStyledItemDelegate {

	Q_OBJECT

public:


	/**
		\brief Reimplementation of displayText.
		\param value a value
		\param locale a locale
		\retval QString the value as a string
	*/
	QString displayText(const QVariant &value, const QLocale &locale) const;

};


#endif

