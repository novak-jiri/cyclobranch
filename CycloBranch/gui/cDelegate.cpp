#include "gui/cDelegate.h"


QString cDelegate::displayText(const QVariant &value, const QLocale &locale) const {
	if (value.type() == QVariant::Double) {
		return locale.toString(value.toDouble(), 'f', 6);
	}
	else {
		return QStyledItemDelegate::displayText(value, locale);
	}
}

