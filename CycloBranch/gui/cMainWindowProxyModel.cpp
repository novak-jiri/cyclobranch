#include "gui/cMainWindowProxyModel.h"


cMainWindowProxyModel::cMainWindowProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
}


bool cMainWindowProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	for (int i = 0; i < sourceModel()->columnCount(); i++) {
		if (sourceModel()->data(sourceModel()->index(sourceRow, i, sourceParent)).toString().contains(filterRegExp())) {
			return true;
		}
	}
    return false;
}


bool cMainWindowProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
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

