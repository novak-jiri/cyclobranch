#include "gui/cBricksDatabaseProxyModel.h"


cBricksDatabaseProxyModel::cBricksDatabaseProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
}


bool cBricksDatabaseProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	int size = 6;

	for (int i = 1; i < size; i++) {
		if (sourceModel()->data(sourceModel()->index(sourceRow, i, sourceParent)).toString().contains(filterRegExp())) {
			return true;
		}
	}

    return false;
}


bool cBricksDatabaseProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	if (left.column() == 6) {
		return false;
	}

	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if (left.column() == 0) {
		return leftData.toInt() < rightData.toInt();
	}

	if (leftData.type() == QVariant::ByteArray) {
		return leftData.toDouble() < rightData.toDouble();
	}

	if (leftData.type() == QVariant::String) {
		return leftData.toString() < rightData.toString();
	}

	return QSortFilterProxyModel::lessThan(left, right);
}

