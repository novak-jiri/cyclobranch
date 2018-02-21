#include "gui/cSummaryPeaksTableProxyModel.h"


cSummaryPeaksTableProxyModel::cSummaryPeaksTableProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
	xmin = -1;
	xmax = -1;
	ymin = -1;
	ymax = -1;
	wholeword = false;
}


void cSummaryPeaksTableProxyModel::initialize(eModeType mode, ePeakListFileFormat peaklistfileformat, bool generateisotopepattern, QComboBox* rowsfiltercombobox, QComboBox* rowsfiltercomparatorcombobox) {
	this->mode = mode;
	this->peaklistfileformat = peaklistfileformat;
	this->generateisotopepattern = generateisotopepattern;
	this->rowsfiltercombobox = rowsfiltercombobox;
	this->rowsfiltercomparatorcombobox = rowsfiltercomparatorcombobox;
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

	if ((mode == dereplication) && ((peaklistfileformat == mis) || (peaklistfileformat == imzML))) {
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

	if (filterRegExp().isEmpty()) {
		return true;
	}

	int col = rowsfiltercombobox->currentIndex();

	if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).type() == QVariant::ByteArray) {
		switch (rowsfiltercomparatorcombobox->currentIndex()) {
			case 0:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toDouble() == filterRegExp().pattern().toDouble()) {
					return true;
				}
				break;
			case 1:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toDouble() < filterRegExp().pattern().toDouble()) {
					return true;
				}
				break;
			case 2:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toDouble() <= filterRegExp().pattern().toDouble()) {
					return true;
				}
				break;
			case 3:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toDouble() > filterRegExp().pattern().toDouble()) {
					return true;
				}
				break;
			case 4:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toDouble() >= filterRegExp().pattern().toDouble()) {
					return true;
				}
				break;
			default:
				break;
		}
	}
	
	if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).type() == QVariant::String) {

		string itemstr = sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toString().toStdString();
		QString qstr = stripHTML(itemstr).c_str();

		switch (rowsfiltercomparatorcombobox->currentIndex()) {
			case 0:
				if (wholeword) {
					if (qstr.compare(filterRegExp().pattern(), filterCaseSensitivity()) == 0) {
						return true;
					}
				}
				else {
					if (qstr.contains(filterRegExp())) {
						return true;
					}
				}
				break;
			case 1:
				if (qstr.compare(filterRegExp().pattern(), filterCaseSensitivity()) < 0) {
					return true;
				}
				break;
			case 2:
				if (qstr.compare(filterRegExp().pattern(), filterCaseSensitivity()) <= 0) {
					return true;
				}
				break;
			case 3:
				if (qstr.compare(filterRegExp().pattern(), filterCaseSensitivity()) > 0) {
					return true;
				}
				break;
			case 4:
				if (qstr.compare(filterRegExp().pattern(), filterCaseSensitivity()) >= 0) {
					return true;
				}
				break;
			default:
				break;
		}
	}

	if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).type() == QVariant::Int) {
		switch (rowsfiltercomparatorcombobox->currentIndex()) {
			case 0:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toInt() == filterRegExp().pattern().toInt()) {
					return true;
				}
				break;
			case 1:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toInt() < filterRegExp().pattern().toInt()) {
					return true;
				}
				break;
			case 2:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toInt() <= filterRegExp().pattern().toInt()) {
					return true;
				}
				break;
			case 3:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toInt() > filterRegExp().pattern().toInt()) {
					return true;
				}
				break;
			case 4:
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toInt() >= filterRegExp().pattern().toInt()) {
					return true;
				}
				break;
			default:
				break;
		}
	}

    return false;
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

