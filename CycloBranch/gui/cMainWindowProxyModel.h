/**
	\file cMainWindowProxyModel.h
	\brief A table proxy model.
*/


#ifndef _CMAINWINDOWPROXYMODEL_H
#define _CMAINWINDOWPROXYMODEL_H

#include <QString>
#include <QVariant>
#include <QSortFilterProxyModel>
#include <QComboBox>


/**
	\brief A table proxy model.
*/
class cMainWindowProxyModel : public QSortFilterProxyModel {

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
	cMainWindowProxyModel(QObject *parent = 0);


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

