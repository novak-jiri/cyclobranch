#include "gui/cModificationsProxyModel.h"


cModificationsProxyModel::cModificationsProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
}


bool cModificationsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	int size = 4;

	for (int i = 1; i < size; i++) {
		if (sourceModel()->data(sourceModel()->index(sourceRow, i, sourceParent)).toString().contains(filterRegExp())) {
			return true;
		}
	}

    return false;
}


bool cModificationsProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if ((left.column() == 0) || (left.column() == 4) || (left.column() == 5)) {
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

