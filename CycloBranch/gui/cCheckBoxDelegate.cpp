#include "gui/cCheckBoxDelegate.h"


cCheckBoxDelegate::cCheckBoxDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}


void cCheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionButton button;
	button.state = QStyle::State_Enabled;

	if (index.data(Qt::DisplayRole).toBool()) {
		button.state |= QStyle::State_On;
	}
	else {
		button.state |= QStyle::State_Off;
	}

	button.direction = QApplication::layoutDirection();
	button.rect = option.rect;
	QApplication::style()->drawControl(QStyle::CE_CheckBox, &button, painter);
}


bool cCheckBoxDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
	if (event->type() == QEvent::MouseButtonRelease) {
		model->setData(model->index(index.row(), index.column()), QVariant::fromValue(!index.data(Qt::DisplayRole).toBool()), Qt::DisplayRole);
		return true;
	}
	return false;
}

