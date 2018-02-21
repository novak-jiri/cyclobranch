/**
	\file cModificationsProxyModel.h
	\brief A proxy model for editor of database of modifications.
*/


#ifndef _CMODIFICATIONSPROXYMODEL_H
#define _CMODIFICATIONSPROXYMODEL_H

#include <QString>
#include <QVariant>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include "core/cFragmentIons.h"


/**
	\brief A proxy model for editor of database of modifications.
*/
class cModificationsProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

private:

	QComboBox* rowsfiltercombobox;
	QComboBox* rowsfiltercomparatorcombobox;
	bool wholeword;

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cModificationsProxyModel(QObject *parent = 0);


	/**
		\brief Initialize the model.
		\param rowsfiltercombobox reference to the combobox with selection of columns
		\param rowsfiltercomparatorcombobox reference to the combobox with selection of comparator
	*/
	void initialize(QComboBox* rowsfiltercombobox, QComboBox* rowsfiltercomparatorcombobox);


	/**
		\brief Set whole word option.
		\param wholeword true if whole words are matched, false otherwise
	*/
	void setWholeWord(bool wholeword);


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
