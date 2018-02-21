#include "gui/cImageWindowWidget.h"

#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QMessageBox>


cImageWindowWidget::cImageWindowWidget(QWidget* parent) {
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	pixmap = new QPixmap();
	scene = new QGraphicsScene(this);
	setScene(scene);
	coordinates.clear();
	maxx = 0;
	maxy = 0;
	leftmargin = 0;
	topmargin = 0;
	currentscale = 1;
	factor = 0.1;
	ispixmapdefined = false;
	ismaxxydefined = false;
	enablemouseselection = true;

	pressedx = -1;
	pressedy = -1;
	currentx = 0;
	currenty = 0;
	currentwidth = 1;
	currentheight = 1;

	selectiongroup = new QGraphicsItemGroup();
	selectionrect = new QGraphicsRectItem();
	selectiongroup->addToGroup(selectionrect);
	selectionsimpletextitem = new QGraphicsSimpleTextItem();
	selectiongroup->addToGroup(selectionsimpletextitem);
	selectiongroup->setZValue(1);
	scene->addItem(selectiongroup);
}


cImageWindowWidget::~cImageWindowWidget() {
	delete pixmap;
	delete scene;
}


void cImageWindowWidget::setPixmap(QImage* image) {
	*pixmap = QPixmap::fromImage(*image);
	currentscale = 1;
	ispixmapdefined = true;
	redrawScene();
}


QImage cImageWindowWidget::getImage() {
	QImage image(sceneRect().size().toSize(), QImage::Format_ARGB32);
	image.fill(Qt::white);

	QPainter painter;
	if (!painter.begin(&image)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return image;
	}

	scene->render(&painter, sceneRect(), sceneRect(), Qt::KeepAspectRatio);
	painter.end();
	
	return image;
}


void cImageWindowWidget::setCoordinates(vector<cCoordinates>& coordinates) {
	this->coordinates = coordinates;
	redrawScene();
}


void cImageWindowWidget::setMaxXY(int maxx, int maxy, int leftmargin, int topmargin) {
	if (leftmargin >= maxx) {
		leftmargin = 0;
	}
	if (topmargin >= maxy) {
		topmargin = 0;
	}
	this->maxx = maxx - leftmargin;
	this->maxy = maxy - topmargin;
	this->leftmargin = leftmargin;
	this->topmargin = topmargin;
	ismaxxydefined = true;
	redrawScene();
}


void cImageWindowWidget::wheelEvent(QWheelEvent *event) {
	if (event->delta() > 0) {
		zoomIn();
	}
	else {
		zoomOut();
	}

	event->accept();
}


void cImageWindowWidget::mousePressEvent(QMouseEvent *event) {
	QGraphicsView::mousePressEvent(event);

	if (enablemouseselection && ispixmapdefined && ismaxxydefined) {
		if (event->button() == Qt::LeftButton) {
			QPointF p = mapToScene(event->x(), event->y());
			pressedx = (int)p.x();
			pressedy = (int)p.y();

			currentx = pressedx;
			currenty = pressedy;

			updateSelectionGroup();
		}

		if (event->button() == Qt::RightButton) {
			pressedx = -1;
			pressedy = -1;

			redrawScene();
		}

		if (event->button() == Qt::MiddleButton) {
			pressedx = -1;
			pressedy = -1;

			normalSize();

			redrawScene();
		}
	}

	event->accept();
}


void cImageWindowWidget::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);

	if (enablemouseselection && ispixmapdefined && ismaxxydefined) {
		QPointF p = mapToScene(event->x(), event->y());
		currentx = (int)p.x();
		currenty = (int)p.y();
		
		updateSelectionGroup();
	}

	event->accept();
}


void cImageWindowWidget::mouseReleaseEvent(QMouseEvent *event) {
	QGraphicsView::mouseReleaseEvent(event);

	if (enablemouseselection && ispixmapdefined && ismaxxydefined) {
		if (pressedx == currentx) {
			pressedx = -1;

			redrawScene();
		}
    
		if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {
			if (pressedx < 0) {
				pressedx = 0;
			}

			if (pressedx > currentwidth) {
				pressedx = currentwidth;
			}

			if (currentx < 0) {
				currentx = 0;
			}

			if (currentx > currentwidth) {
				currentx = currentwidth;
			}

			if (pressedy < 0) {
				pressedy = 0;
			}

			if (pressedy > currentheight) {
				pressedy = currentheight;
			}

			if (currenty < 0) {
				currenty = 0;
			}

			if (currenty > currentheight) {
				currenty = currentheight;
			}

			int xmin, xmax, ymin, ymax;
			xmin = max(0, (currentx < pressedx?currentx:pressedx)*(maxx + 1)/max(1, currentwidth));
			xmax = min(maxx, (currentx < pressedx?pressedx:currentx)*(maxx + 1)/max(1, currentwidth));
			ymin = max(0, (currenty < pressedy?currenty:pressedy)*(maxy + 1)/max(1, currentheight));
			ymax = min(maxy, (currenty < pressedy?pressedy:currenty)*(maxy + 1)/max(1, currentheight));

			pressedx = -1;
			pressedy = -1;

			redrawScene();

			emit updateFilter(xmin, xmax, ymin, ymax);
		}
	}

	event->accept();
}


void cImageWindowWidget::redrawScene() {
	int xmin, xmax, ymin, ymax;
	xmin = 0;
	ymin = 0;
	xmax = maxx;
	ymax = maxy;

	scene->removeItem(selectiongroup);
	scene->clear();
	selectiongroup->setVisible(false);
	scene->addItem(selectiongroup);


	if (!ispixmapdefined && !ismaxxydefined) {
		scene->addText("Please, open an image and set up the maximum X (Max X) and Y (Max Y) coordinates of the image bottom-right corner.");
		return;
	}

	if (!ispixmapdefined) {
		scene->addText("The maximum X (Max X) and Y (Max Y) coordinates have been set up. Please, open an image.");
		return;
	}

	if (!ismaxxydefined) {
		scene->addText("The image has been successfully opened. Please, set up the maximum X and Y coordinates of the image bottom-right corner.");
		return;
	}

	QRect rect(pixmap->width()*leftmargin/(leftmargin + maxx), pixmap->height()*topmargin/(topmargin + maxy), pixmap->width()*maxx, pixmap->height()*maxy);
	QPixmap croppedpixmap = pixmap->copy(rect);

	QPixmap scaledpixmap = croppedpixmap.scaledToHeight(croppedpixmap.height()*currentscale);
	currentwidth = scaledpixmap.width();
	currentheight = scaledpixmap.height();
	scene->addPixmap(scaledpixmap);

	unordered_set<cCoordinates, hash_cCoordinates> reduced_coordinates;
	for (int i = 0; i < (int)coordinates.size(); i++) {
		if (reduced_coordinates.count(coordinates[i]) == 1) {
			auto it = reduced_coordinates.find(coordinates[i]);
			cCoordinates coord = *it;
			coord.mzratio = 0;
			coord.intensity += coordinates[i].intensity;
			coord.name += "\n" + coordinates[i].name;
			reduced_coordinates.erase(it);
			reduced_coordinates.insert(coord);

		}
		else {
			reduced_coordinates.insert(coordinates[i]);
		}
	}

	double maximumintensity = 0;
	for (auto it = reduced_coordinates.begin(); it != reduced_coordinates.end(); ++it) {
		if (it->intensity > maximumintensity) {
			maximumintensity = it->intensity;
		}
	}

	if (maximumintensity > 0) {

		for (auto it = reduced_coordinates.begin(); it != reduced_coordinates.end(); ++it) {

			QGraphicsRectItem* rectitem = new QGraphicsRectItem((it->x * currentwidth) / (maxx + 1), (it->y * currentheight) / (maxy + 1),  currentwidth / (maxx + 1), currentheight / (maxy + 1));
			rectitem->setPen(Qt::NoPen);
			QColor color;
			color.setHslF(1 - it->intensity/maximumintensity, 0.5, 0.5, 0.75);
			rectitem->setBrush(QBrush(color));
			string tooltip;
			if (it->mzratio == 0) {
				tooltip = it->name + "\nID: " + to_string(it->id) + "\nX: " + to_string(it->x) + "\nY: " + to_string(it->y);
			}
			else {
				tooltip = it->name + "\nID: " + to_string(it->id) + "\nX: " + to_string(it->x) + "\nY: " + to_string(it->y) + "\nm/z: " + to_string(it->mzratio) + "\nintensity: " + to_string(it->intensity) + "%";
			}
			rectitem->setToolTip(tooltip.c_str());
			scene->addItem(rectitem);

			if (it == reduced_coordinates.begin()) {
				xmin = it->x;
				xmax = it->x;
				ymin = it->y;
				ymax = it->y;
			}
			else {
				if (it->x < xmin) {
					xmin = it->x;
				}
				if (it->x > xmax) {
					xmax = it->x;
				}
				if (it->y < ymin) {
					ymin = it->y;
				}
				if (it->y > ymax) {
					ymax = it->y;
				}
			}
		}

		QString qstr = "Minimum Bounding Region\nX: " + QString::number(xmin) + "-" + QString::number(xmax) + "; Y: " + QString::number(ymin) + "-" + QString::number(ymax); 
		QGraphicsTextItem* text = scene->addText(qstr);
		text->setPos(2, currentheight + 2);

	}

	setSceneRect(0, 0, currentwidth, currentheight + 50);

	updateSelectionGroup();
}


void cImageWindowWidget::updateSelectionGroup() {
	qreal rx1, rx2, ry1, ry2;
	QFont myFont("Arial", 8);
	int xmin, xmax, ymin, ymax;
	QString qstr;

	if ((pressedx != -1) && (pressedy != -1)) {

		if (currentx < pressedx) {
			rx1 = currentx;
			rx2 = pressedx;
		}
		else {
			rx1 = pressedx;
			rx2 = currentx;
		}

		if (currenty < pressedy) {
			ry1 = currenty;
			ry2 = pressedy;
		}
		else {
			ry1 = pressedy;
			ry2 = currenty;
		}

		selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
		selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

		xmin = max(0, (currentx < pressedx?currentx:pressedx)*(maxx + 1)/max(1, currentwidth));
		xmax = min(maxx, (currentx < pressedx?pressedx:currentx)*(maxx + 1)/max(1, currentwidth));
		ymin = max(0, (currenty < pressedy?currenty:pressedy)*(maxy + 1)/max(1, currentheight));
		ymax = min(maxy, (currenty < pressedy?pressedy:currenty)*(maxy + 1)/max(1, currentheight));
	
		qstr = "X: " + QString::number(xmin) + "-" + QString::number(xmax) + "; Y: " + QString::number(ymin) + "-" + QString::number(ymax);
		
		selectionsimpletextitem->setFont(myFont);
		selectionsimpletextitem->setText(qstr);
		selectionsimpletextitem->setBrush(QBrush(Qt::green));
		selectionsimpletextitem->setPos(QPointF(pressedx + 2, pressedy));

		selectiongroup->setVisible(true);
		selectionrect->setVisible(true);
		selectionsimpletextitem->setVisible(true);

	} 
	else {

		xmin = currentx*(maxx + 1)/max(1, currentwidth);
		ymin = currenty*(maxy + 1)/max(1, currentheight);

		if ((currentx > 0) && (currenty > 0) && (xmin <= maxx) && (ymin <= maxy)) {
			qstr = "X: " + QString::number(xmin) + " Y: " + QString::number(ymin);
	
			selectionsimpletextitem->setFont(myFont);
			selectionsimpletextitem->setText(qstr);
			selectionsimpletextitem->setBrush(QBrush(Qt::green));
			selectionsimpletextitem->setPos(QPointF(currentx, currenty - 10));

			selectiongroup->setVisible(true);
			selectionrect->setVisible(false);
			selectionsimpletextitem->setVisible(true);
		}
		else {
			selectiongroup->setVisible(false);
		}
	
	}
}


void cImageWindowWidget::zoomIn() {
	currentscale += factor;
	redrawScene();
}


void cImageWindowWidget::zoomOut() {
	if (currentscale > 2*factor) {
		currentscale -= factor;
	}
	redrawScene();
}


void cImageWindowWidget::normalSize() {
	currentscale = 1;
	redrawScene();
}

