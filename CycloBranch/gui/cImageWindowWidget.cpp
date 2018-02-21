#include "gui/cImageWindowWidget.h"


cImageWindowWidget::cImageWindowWidget(QWidget* parent) {
	pixmap = new QPixmap();
}


cImageWindowWidget::~cImageWindowWidget() {
	delete pixmap;
}


void cImageWindowWidget::setPixmap(QImage* image) {
	*pixmap = QPixmap::fromImage(*image);
}


void cImageWindowWidget::paint(QPainter& painter) {
	painter.drawPixmap(0, 0, *pixmap);
}


void cImageWindowWidget::paintEvent(QPaintEvent *event) {
	QPainter painter;
	painter.begin(this);
	paint(painter);
	painter.end();
}

