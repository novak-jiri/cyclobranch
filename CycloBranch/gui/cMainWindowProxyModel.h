/**
	\file cMainWindowProxyModel.h
	\brief A table proxy model.
*/


#ifndef _CMAINWINDOWPROXYMODEL_H
#define _CMAINWINDOWPROXYMODEL_H

#include <QString>
#include <QVariant>
#include <QSortFilterProxyModel>


/**
	\brief A table proxy model.
*/
class cMainWindowProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cMainWindowProxyModel(QObject *parent = 0);


protected:


	/**
		\brief Reimplementation of filterAcceptsRow.
		\param sourceRow int
		\param sourceParent QModelIndex
		\retval bool bool
	*/
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;
    

	/**
		\brief Reimplementation of lessThan.
		\param left QModelIndex
		\param right QModelIndex
		\retval bool bool
	*/
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;    


};


#endif

