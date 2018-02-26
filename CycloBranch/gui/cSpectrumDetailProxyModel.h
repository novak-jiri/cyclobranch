/**
	\file cSpectrumDetailProxyModel.h
	\brief A table proxy model.
*/


#ifndef _CSPECTRUMDETAILPROXYMODEL_H
#define _CSPECTRUMDETAILPROXYMODEL_H

#include <QString>
#include <QVariant>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>


/**
	\brief A table proxy model.
*/
class cSpectrumDetailProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

private:

	bool hidematched, hideunmatched, hidescrambled, hideionseries, hideneutralloss, hiderotations, hidetrotations;

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cSpectrumDetailProxyModel(QObject *parent = 0);


	/**
		\brief Set the flags for filtering.
		\param hidematched hide matched peaks
		\param hideunmatched hide unmatched peaks
		\param hidescrambled hide scrambled peaks
		\param hideionseries hide ion series
		\param hideneutralloss hide neutral losses
		\param hiderotations hide rotations of a cyclic peptide or branch-cyclic peptide or cyclic polyketide
		\param hidetrotations hide trotations of a branched or branch-cyclic peptide
	*/
	void setFlags(bool hidematched, bool hideunmatched, bool hidescrambled, bool hideionseries, bool hideneutralloss, bool hiderotations, bool hidetrotations);


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

