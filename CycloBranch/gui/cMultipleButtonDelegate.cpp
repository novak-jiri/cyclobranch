#include "gui/cMultipleButtonDelegate.h"

#include <QApplication>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardItemModel>
#include <QStringList>


cMultipleButtonDelegate::cMultipleButtonDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}


QString cMultipleButtonDelegate::displayText(const QVariant& value, const QLocale& locale) const {
	return "";
}


void cMultipleButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QString acronyms = index.model()->index(index.row(), 2).data(Qt::DisplayRole).toString();

	if (acronyms.isEmpty()) {
		return;
	}

	QStringList acronymslist = acronyms.split("/");
	int count = acronymslist.size();
	vector<QStyleOptionButton> buttons;
	buttons.resize(count);
	for (int i = 0; i < count; i++) {
		buttons[i].rect = option.rect;
		buttons[i].rect.setX(buttons[i].rect.x() + i*(option.rect.width()/count));
		buttons[i].rect.setWidth(option.rect.width()/count);
		buttons[i].text = acronymslist[i];
		buttons[i].state = QStyle::State_Enabled;
		QApplication::style()->drawControl(QStyle::CE_PushButton, &(buttons[i]), painter);
	}
}


bool cMultipleButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
	if (event->type() == QEvent::MouseButtonRelease) {
		QString acronyms = index.model()->index(index.row(), 2).data(Qt::DisplayRole).toString();
		QString references = index.model()->index(index.row(), 7).data(Qt::DisplayRole).toString();

		if (acronyms.isEmpty() || references.isEmpty()) {
			return false;
		}

		QMouseEvent *mouseevent = static_cast<QMouseEvent*>(event);
		QStringList acronymslist = acronyms.split("/");
		QStringList referenceslist = references.split("\"");
		int count = acronymslist.size();

		for (int i = 0; i < count; i++) {
			if ((mouseevent->x() >= option.rect.x() + i*(option.rect.width()/count)) && (mouseevent->x() < option.rect.x() + (i + 1)*(option.rect.width()/count))) {
				QString link = (2*i + 1 < referenceslist.size())?referenceslist[2*i + 1]:"";
				QDesktopServices::openUrl(QUrl(link));
				break;
			}
		}

		return true;
	}
	return false;
}

