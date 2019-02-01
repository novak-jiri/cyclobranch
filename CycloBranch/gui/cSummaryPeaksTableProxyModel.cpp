#include "gui/cSummaryPeaksTableProxyModel.h"


cSummaryPeaksTableProxyModel::cSummaryPeaksTableProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
	xmin = -1;
	xmax = -1;
	ymin = -1;
	ymax = -1;
	wholeword = false;
}


void cSummaryPeaksTableProxyModel::initialize(eModeType mode, ePeakListFileFormat peaklistfileformat, bool generateisotopepattern, QComboBox* rowsfilteroperator, QComboBox* rowsfiltercombobox1, QComboBox* rowsfiltercomparatorcombobox1, QLineEdit* rowsfilterline1, QComboBox* rowsfiltercombobox2, QComboBox* rowsfiltercomparatorcombobox2, QLineEdit* rowsfilterline2) {
	this->mode = mode;
	this->peaklistfileformat = peaklistfileformat;
	this->generateisotopepattern = generateisotopepattern;
	this->filteroperator = rowsfilteroperator;
	this->filtercombobox1 = rowsfiltercombobox1;
	this->filtercomparatorcombobox1 = rowsfiltercomparatorcombobox1;
	this->filterline1 = rowsfilterline1;
	this->filtercombobox2 = rowsfiltercombobox2;
	this->filtercomparatorcombobox2 = rowsfiltercomparatorcombobox2;
	this->filterline2 = rowsfilterline2;
}


void cSummaryPeaksTableProxyModel::setRegion(int xmin, int xmax, int ymin, int ymax) {
	this->xmin = xmin;
	this->xmax = xmax;
	this->ymin = ymin;
	this->ymax = ymax;
}


void cSummaryPeaksTableProxyModel::setWholeWord(bool wholeword) {
	this->wholeword = wholeword;
}


bool cSummaryPeaksTableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	bool checkrow = false;
	int x, y;

	if (((mode == dereplication) || (mode == compoundsearch)) && ((peaklistfileformat == mis) || (peaklistfileformat == imzML))) {
		x = sourceModel()->data(sourceModel()->index(sourceRow, 1)).toInt();
		y = sourceModel()->data(sourceModel()->index(sourceRow, 2)).toInt();

		if (((xmin == -1) && (xmax == -1) && (ymin == -1) && (ymax == -1)) || ((x >= xmin) && (x <= xmax) && (y >= ymin) && (y <= ymax))) {
			checkrow = true;
		}
	}
	else {
		checkrow = true;
	}

	if (!checkrow) {
		return false;
	}

	if (filterline1->text().isEmpty() && filterline2->text().isEmpty()) {
		return true;
	}

	int col1 = filtercombobox1->currentIndex();
	int col2 = filtercombobox2->currentIndex();

	bool result1 = false;
	bool result2 = false;

	if (!filterline1->text().isEmpty()) {
		if (sourceModel()->data(sourceModel()->index(sourceRow, col1, sourceParent)).type() == QVariant::ByteArray) {
			result1 = proxyModelCheckDouble(sourceModel(), filtercomparatorcombobox1->currentIndex(), sourceRow, col1, filterline1->text(), sourceParent);
		}

		if (sourceModel()->data(sourceModel()->index(sourceRow, col1, sourceParent)).type() == QVariant::String) {
			QString qstr = sourceModel()->data(sourceModel()->index(sourceRow, col1, sourceParent)).toString();
			result1 = proxyModelCheckString(sourceModel(), filtercomparatorcombobox1->currentIndex(), sourceRow, col1, qstr, filterline1->text(), sourceParent, wholeword, filterCaseSensitivity());
		}

		if (sourceModel()->data(sourceModel()->index(sourceRow, col1, sourceParent)).type() == QVariant::Int) {
			result1 = proxyModelCheckInt(sourceModel(), filtercomparatorcombobox1->currentIndex(), sourceRow, col1, filterline1->text(), sourceParent);
		}
	}

	if (!filterline2->text().isEmpty()) {
		if (sourceModel()->data(sourceModel()->index(sourceRow, col2, sourceParent)).type() == QVariant::ByteArray) {
			result2 = proxyModelCheckDouble(sourceModel(), filtercomparatorcombobox2->currentIndex(), sourceRow, col2, filterline2->text(), sourceParent);
		}

		if (sourceModel()->data(sourceModel()->index(sourceRow, col2, sourceParent)).type() == QVariant::String) {
			QString qstr = sourceModel()->data(sourceModel()->index(sourceRow, col2, sourceParent)).toString();
			result2 = proxyModelCheckString(sourceModel(), filtercomparatorcombobox2->currentIndex(), sourceRow, col2, qstr, filterline2->text(), sourceParent, wholeword, filterCaseSensitivity());
		}

		if (sourceModel()->data(sourceModel()->index(sourceRow, col2, sourceParent)).type() == QVariant::Int) {
			result2 = proxyModelCheckInt(sourceModel(), filtercomparatorcombobox2->currentIndex(), sourceRow, col2, filterline2->text(), sourceParent);
		}
	}

	if (!filterline1->text().isEmpty() && filterline2->text().isEmpty()) {
		return result1;
	}

	if (filterline1->text().isEmpty() && !filterline2->text().isEmpty()) {
		return result2;
	}

	if (filteroperator->currentIndex() == 0) {
		return result1 || result2;
	}

	return result1 && result2;
}


bool cSummaryPeaksTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if (leftData.toString().isEmpty()) {
		return (sortOrder() == Qt::AscendingOrder) ? false : true;
	}

	if (rightData.toString().isEmpty()) {
		return (sortOrder() == Qt::AscendingOrder) ? true : false;
	}

	if (leftData.type() == QVariant::ByteArray) {
		return leftData.toDouble() < rightData.toDouble();
	}

	if (leftData.type() == QVariant::String) {
		return leftData.toString().toLower() < rightData.toString().toLower();
	}

	if (leftData.type() == QVariant::Int) {
		return leftData.toInt() < rightData.toInt();
	}

	return QSortFilterProxyModel::lessThan(left, right);
}

