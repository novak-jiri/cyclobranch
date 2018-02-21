#include "gui/cSequenceDatabaseProxyModel.h"


cSequenceDatabaseProxyModel::cSequenceDatabaseProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
}


bool cSequenceDatabaseProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	int size = 10;

	for (int i = 2; i < size; i++) {
		if (sourceModel()->data(sourceModel()->index(sourceRow, i, sourceParent)).toString().contains(filterRegExp())) {
			return true;
		}
	}

    return false;
}


bool cSequenceDatabaseProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if (left.column() == 0) {
		return leftData.toInt() < rightData.toInt();
	}

	if (left.column() == 1) {
		return getStringFromPeptideType((ePeptideType)leftData.toInt()) < getStringFromPeptideType((ePeptideType)rightData.toInt());
	}

	if (leftData.type() == QVariant::ByteArray) {
		return leftData.toDouble() < rightData.toDouble();
	}

	if (leftData.type() == QVariant::String) {
		return leftData.toString() < rightData.toString();
	}

	return QSortFilterProxyModel::lessThan(left, right);
}

