/**
	\file cMultipleDatasetsTableProxyModel.h
	\brief A table proxy model.
*/


#ifndef _CMULTIPLEDATASETSTABLEPROXYMODEL_H
#define _CMULTIPLEDATASETSTABLEPROXYMODEL_H

#include <QString>
#include <QVariant>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QLineEdit>
#include "core/cParameters.h"


/**
	\brief A table proxy model.
*/
class cMultipleDatasetsTableProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

public:

	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cMultipleDatasetsTableProxyModel(QObject *parent = 0);

protected: 

	/**
		\brief Reimplementation of lessThan.
		\param left QModelIndex
		\param right QModelIndex
		\retval bool bool
	*/
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;    
	
};


#endif

