#include "gui/cBricksDatabaseProxyModel.h"


cBricksDatabaseProxyModel::cBricksDatabaseProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
	wholeword = false;
}


void cBricksDatabaseProxyModel::initialize(QComboBox* rowsfilteroperator, QComboBox* rowsfiltercombobox1, QComboBox* rowsfiltercomparatorcombobox1, QLineEdit* rowsfilterline1, QComboBox* rowsfiltercombobox2, QComboBox* rowsfiltercomparatorcombobox2, QLineEdit* rowsfilterline2) {
	filteroperator = rowsfilteroperator;
	filtercombobox1 = rowsfiltercombobox1;
	filtercomparatorcombobox1 = rowsfiltercomparatorcombobox1;
	filterline1 = rowsfilterline1;
	filtercombobox2 = rowsfiltercombobox2;
	filtercomparatorcombobox2 = rowsfiltercomparatorcombobox2;
	filterline2 = rowsfilterline2;
}


void cBricksDatabaseProxyModel::setWholeWord(bool wholeword) {
	this->wholeword = wholeword;
}


bool cBricksDatabaseProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	if (filterline1->text().isEmpty() && filterline2->text().isEmpty()) {
		return true;
	}

	int col1 = filtercombobox1->currentIndex() + 1;
	int col2 = filtercombobox2->currentIndex() + 1;

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


bool cBricksDatabaseProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	if (left.column() == 7) {
		return false;
	}

	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if (leftData.toString().isEmpty()) {
		return (sortOrder() == Qt::AscendingOrder) ? false : true;
	}

	if (rightData.toString().isEmpty()) {
		return (sortOrder() == Qt::AscendingOrder) ? true : false;
	}

	if (left.column() == 0) {
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

