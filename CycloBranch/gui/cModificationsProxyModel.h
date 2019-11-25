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
#include <QLineEdit>
#include "core/utilities.h"


/**
	\brief A proxy model for editor of database of modifications.
*/
class cModificationsProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

private:

	QComboBox* filteroperator;
	QComboBox* filtercombobox1;
	QComboBox* filtercomparatorcombobox1;
	QLineEdit* filterline1;
	QComboBox* filtercombobox2;
	QComboBox* filtercomparatorcombobox2;
	QLineEdit* filterline2;
	bool wholeword;

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cModificationsProxyModel(QObject *parent = 0);


	/**
		\brief Initialize the model.
		\param rowsfilteroperator reference to the combobox with operator selection
		\param rowsfiltercombobox1 reference to the combobox with selection of columns
		\param rowsfiltercomparatorcombobox1 reference to the combobox with selection of comparator
		\param rowsfilterline1 reference to a text field
		\param rowsfiltercombobox2 reference to the combobox with selection of columns
		\param rowsfiltercomparatorcombobox2 reference to the combobox with selection of comparator
		\param rowsfilterline2 reference to a text field
	*/
	void initialize(QComboBox* rowsfilteroperator, QComboBox* rowsfiltercombobox1, QComboBox* rowsfiltercomparatorcombobox1, QLineEdit* rowsfilterline1, QComboBox* rowsfiltercombobox2, QComboBox* rowsfiltercomparatorcombobox2, QLineEdit* rowsfilterline2);


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

