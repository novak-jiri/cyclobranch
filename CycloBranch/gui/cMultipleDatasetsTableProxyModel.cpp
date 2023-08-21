#include "gui/cMultipleDatasetsTableProxyModel.h"


cMultipleDatasetsTableProxyModel::cMultipleDatasetsTableProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
	// nothing to do
}


bool cMultipleDatasetsTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if (leftData.toString().isEmpty()) {
		return (sortOrder() == Qt::AscendingOrder) ? false : true;
	}

	if (rightData.toString().isEmpty()) {
		return (sortOrder() == Qt::AscendingOrder) ? true : false;
	}

	if (leftData.type() != rightData.type()) {
		if (leftData.type() == QVariant::String) {
			return (sortOrder() == Qt::AscendingOrder) ? false : true;
		}

		if (rightData.type() == QVariant::String) {
			return (sortOrder() == Qt::AscendingOrder) ? true : false;
		}
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

