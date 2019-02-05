#include "gui/cViewButtonDelegate.h"

#include <QApplication>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardItemModel>


cViewButtonDelegate::cViewButtonDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}


QString cViewButtonDelegate::displayText(const QVariant& value, const QLocale& locale) const {
	return "";
}


void cViewButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	if (index.data(Qt::DisplayRole).toString().isEmpty()) {
		return;
	}

	QStyleOptionButton button;
	button.rect = option.rect;
	if (index.data(Qt::DisplayRole).toString().contains("https://www.ncbi.nlm.nih.gov/pccompound?term=")) {
		button.text = "search";
	}
	else {
		button.text = "view";
	}
	button.state = QStyle::State_Enabled;

	QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}


bool cViewButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
	if (event->type() == QEvent::MouseButtonRelease) {
		if (index.data(Qt::DisplayRole).toString().isEmpty()) {
			return false;
		}
		QDesktopServices::openUrl(QUrl(index.data(Qt::DisplayRole).toString()));
		return true;
	}
	return false;
}

