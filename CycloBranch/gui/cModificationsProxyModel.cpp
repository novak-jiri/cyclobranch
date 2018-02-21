#include "gui/cModificationsProxyModel.h"


cModificationsProxyModel::cModificationsProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
	wholeword = false;
}


void cModificationsProxyModel::initialize(QComboBox* rowsfiltercombobox, QComboBox* rowsfiltercomparatorcombobox) {
	this->rowsfiltercombobox = rowsfiltercombobox;
	this->rowsfiltercomparatorcombobox = rowsfiltercomparatorcombobox;
}


void cModificationsProxyModel::setWholeWord(bool wholeword) {
	this->wholeword = wholeword;
}


bool cModificationsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	if (filterRegExp().isEmpty()) {
		return true;
	}

	int col = rowsfiltercombobox->currentIndex() + 1;

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
		switch (rowsfiltercomparatorcombobox->currentIndex()) {
		case 0:
			if (wholeword) {
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toString().compare(filterRegExp().pattern(), filterCaseSensitivity()) == 0) {
					return true;
				}
			}
			else {
				if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toString().contains(filterRegExp())) {
					return true;
				}
			}
			break;
		case 1:
			if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toString().compare(filterRegExp().pattern(), filterCaseSensitivity()) < 0) {
				return true;
			}
			break;
		case 2:
			if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toString().compare(filterRegExp().pattern(), filterCaseSensitivity()) <= 0) {
				return true;
			}
			break;
		case 3:
			if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toString().compare(filterRegExp().pattern(), filterCaseSensitivity()) > 0) {
				return true;
			}
			break;
		case 4:
			if (sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toString().compare(filterRegExp().pattern(), filterCaseSensitivity()) >= 0) {
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


bool cModificationsProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if (leftData.toString().isEmpty()) {
		return (sortOrder() == Qt::AscendingOrder) ? false : true;
	}

	if (rightData.toString().isEmpty()) {
		return (sortOrder() == Qt::AscendingOrder) ? true : false;
	}

	if ((left.column() == 0) || (left.column() == 4) || (left.column() == 5)) {
		return leftData.toInt() < rightData.toInt();
	}

	if (leftData.type() == QVariant::ByteArray) {
		return leftData.toDouble() < rightData.toDouble();
	}

	if (leftData.type() == QVariant::String) {
		return leftData.toString().toLower() < rightData.toString().toLower();
	}

	return QSortFilterProxyModel::lessThan(left, right);
}

