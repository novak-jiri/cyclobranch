/**
	\file cCalibrationChartProxyModel.h
	\brief A table proxy model.
*/


#ifndef _CCALIBRATIONCHARTPROXYMODEL_H
#define _CCALIBRATIONCHARTPROXYMODEL_H

#include <QString>
#include <QVariant>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QLineEdit>
#include "core/cParameters.h"


/**
	\brief A table proxy model.
*/
class cCalibrationChartProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

public:

	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cCalibrationChartProxyModel(QObject *parent = 0);

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

