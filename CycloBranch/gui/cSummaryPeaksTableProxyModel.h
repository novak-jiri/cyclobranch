/**
	\file cSummaryPeaksTableProxyModel.h
	\brief A table proxy model.
*/


#ifndef _CSUMMARYPEAKSTABLEPROXYMODEL_H
#define _CSUMMARYPEAKSTABLEPROXYMODEL_H

#include <QString>
#include <QVariant>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include "core/cParameters.h"


/**
	\brief A table proxy model.
*/
class cSummaryPeaksTableProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

private:

	eModeType mode;
	ePeakListFileFormat peaklistfileformat;
	bool generateisotopepattern;
	QComboBox* rowsfiltercombobox;
	QComboBox* rowsfiltercomparatorcombobox;
	bool wholeword;

public:

	/**
		\brief The minimum X coordinate.
	*/ 
	int xmin;


	/**
		\brief The maximum X coordinate.
	*/ 
	int xmax;


	/**
		\brief The minimum Y coordinate.
	*/ 
	int ymin;


	/**
		\brief The maximum Y coordinate.
	*/ 
	int ymax;


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cSummaryPeaksTableProxyModel(QObject *parent = 0);


	/**
		\brief Initialize the model.
		\param mode program mode
		\param peaklistfileformat peaklist format
		\param generateisotopepattern true if full isotope patterns are generated; false otherwise
		\param rowsfiltercombobox reference to the combobox with selection of columns
		\param rowsfiltercomparatorcombobox reference to the combobox with selection of comparator
	*/ 
	void initialize(eModeType mode, ePeakListFileFormat peaklistfileformat, bool generateisotopepattern, QComboBox* rowsfiltercombobox, QComboBox* rowsfiltercomparatorcombobox);


	/**
		\brief Set the region for filtering.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/ 
	void setRegion(int xmin, int xmax, int ymin, int ymax);


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

