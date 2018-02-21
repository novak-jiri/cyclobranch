#include "gui/cSpectrumDetailProxyModel.h"


cSpectrumDetailProxyModel::cSpectrumDetailProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
	hidematched = false;
	hideunmatched = false;
	hidescrambled = false;
	hiderotations = false;
	hidetrotations = false;
}


void cSpectrumDetailProxyModel::setFlags(bool hidematched, bool hideunmatched, bool hidescrambled, bool hiderotations, bool hidetrotations) {
	this->hidematched = hidematched;
	this->hideunmatched = hideunmatched;
	this->hidescrambled = hidescrambled;
	this->hiderotations = hiderotations;
	this->hidetrotations = hidetrotations;
}


bool cSpectrumDetailProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	bool show = true;

	if (hidematched 
		&& ((QStandardItemModel *)sourceModel())->item(sourceRow, 1) 
		&& ((QStandardItemModel *)sourceModel())->item(sourceRow, 2)
		&& ((QStandardItemModel *)sourceModel())->item(sourceRow, 3)
		&& !((QStandardItemModel *)sourceModel())->item(sourceRow, 1)->data(Qt::DisplayRole).toString().isEmpty()
		&& !((QStandardItemModel *)sourceModel())->item(sourceRow, 2)->data(Qt::DisplayRole).toString().isEmpty()
		&& !((QStandardItemModel *)sourceModel())->item(sourceRow, 3)->data(Qt::DisplayRole).toString().isEmpty()
		) {
		show = false;
	}
		
	if (hideunmatched && (
		(((QStandardItemModel *)sourceModel())->item(sourceRow, 1) == 0) 
		|| (((QStandardItemModel *)sourceModel())->item(sourceRow, 2) == 0)
		|| (((QStandardItemModel *)sourceModel())->item(sourceRow, 3) == 0)
		|| ((QStandardItemModel *)sourceModel())->item(sourceRow, 1)->data(Qt::DisplayRole).toString().isEmpty()
		|| ((QStandardItemModel *)sourceModel())->item(sourceRow, 2)->data(Qt::DisplayRole).toString().isEmpty()
		|| ((QStandardItemModel *)sourceModel())->item(sourceRow, 3)->data(Qt::DisplayRole).toString().isEmpty()
		)) {
		show = false;
	}
		
	if ((hidescrambled || hiderotations || hidetrotations) && 
		((QStandardItemModel *)sourceModel())->item(sourceRow, 0) && (!sourceModel()->data(sourceModel()->index(sourceRow, 0, sourceParent)).toString().contains(filterRegExp()))) {
		show = false;
	}
		
    return show;
}


bool cSpectrumDetailProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
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

	return QSortFilterProxyModel::lessThan(left, right);
}

