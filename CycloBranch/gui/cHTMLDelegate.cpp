#include "gui/cHTMLDelegate.h"


cHTMLDelegate::cHTMLDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}


void cHTMLDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionViewItemV4 options = option;
	initStyleOption(&options, index);

	painter->save();

	QTextDocument doc;
	doc.setHtml(options.text);

	/* Call this to get the focus rect and selection background. */
	options.text = "";
	options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

	/* Draw using our rich text document. */
	painter->translate(options.rect.left(), options.rect.top());
	QRect clip(0, 0, options.rect.width(), options.rect.height());
	doc.drawContents(painter, clip);

	painter->restore();
}


/*
QSize cHTMLDelegate::sizeHint (const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionViewItemV4 options = option;
	initStyleOption(&options, index);

	QTextDocument doc;
	doc.setHtml(options.text);
	doc.setTextWidth(options.rect.width());
	return QSize(doc.idealWidth(), doc.size().height());
}
*/

