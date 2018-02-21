#include "gui/cSummaryPeaksTableProxyModel.h"


cSummaryPeaksTableProxyModel::cSummaryPeaksTableProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
	xmin = -1;
	xmax = -1;
	ymin = -1;
	ymax = -1;
}


void cSummaryPeaksTableProxyModel::initialize(eModeType mode, ePeakListFileFormat peaklistfileformat, bool generateisotopepattern) {
	this->mode = mode;
	this->peaklistfileformat = peaklistfileformat;
	this->generateisotopepattern = generateisotopepattern;
}


void cSummaryPeaksTableProxyModel::setRegion(int xmin, int xmax, int ymin, int ymax) {
	this->xmin = xmin;
	this->xmax = xmax;
	this->ymin = ymin;
	this->ymax = ymax;
}


bool cSummaryPeaksTableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	int size, x, y;

	if (mode == dereplication) {
		if ((peaklistfileformat == mis) || (peaklistfileformat == imzML)) {
			if (generateisotopepattern) {
				size = 12;
			}
			else {
				size = 11;
			}
		}
		else {
			if (generateisotopepattern) {
				size = 10;
			}
			else {
				size = 9;
			}
		}
	}
	else if (mode == denovoengine) {
		size = 9;
	}
	else {
		size = 8;
	}

	if ((mode == dereplication) && ((peaklistfileformat == mis) || (peaklistfileformat == imzML))) {
		x = sourceModel()->data(sourceModel()->index(sourceRow, 1)).toInt();
		y = sourceModel()->data(sourceModel()->index(sourceRow, 2)).toInt();

		if (((xmin == -1) && (xmax == -1) && (ymin == -1) && (ymax == -1)) || ((x >= xmin) && (x <= xmax) && (y >= ymin) && (y <= ymax))) {
			for (int i = 0; i < size; i++) {
				if (sourceModel()->data(sourceModel()->index(sourceRow, i, sourceParent)).toString().contains(filterRegExp())) {
					return true;
				}
			}
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			if (sourceModel()->data(sourceModel()->index(sourceRow, i, sourceParent)).toString().contains(filterRegExp())) {
				return true;
			}
		}
	}

    return false;
}


bool cSummaryPeaksTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if (leftData.type() == QVariant::ByteArray) {
		return leftData.toDouble() < rightData.toDouble();
	}

	if (leftData.type() == QVariant::String) {
		return leftData.toString() < rightData.toString();
	}

	if (leftData.type() == QVariant::Int) {
		return leftData.toInt() < rightData.toInt();
	}

	return QSortFilterProxyModel::lessThan(left, right);
}

