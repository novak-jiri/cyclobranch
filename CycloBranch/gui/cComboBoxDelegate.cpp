#include "gui/cComboBoxDelegate.h"


cComboBoxDelegate::cComboBoxDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}


void cComboBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionComboBox combobox;
	combobox.state = QStyle::State_Enabled;
	combobox.rect = option.rect;
	combobox.currentText = QString(getStringFromPeptideType((ePeptideType)index.data(Qt::DisplayRole).toInt()).c_str());
	// visualization in Linux does not work well	
	// QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &combobox, painter, 0);
	QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &combobox, painter, 0);
}


void cComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
	QComboBox* combobox = static_cast<QComboBox*>(editor);
	combobox->setCurrentIndex(index.data(Qt::DisplayRole).toInt());
}


void cComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
	QComboBox* combobox = qobject_cast<QComboBox*>(editor);
	model->setData(index, combobox->currentIndex(), Qt::DisplayRole);
}


QWidget* cComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QComboBox* combo = new QComboBox(parent);
	combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	for (int j = 0; j <= (int)other; j++) {
		combo->addItem(QString(getStringFromPeptideType((ePeptideType)j).c_str()));
	}
	return combo;
}

