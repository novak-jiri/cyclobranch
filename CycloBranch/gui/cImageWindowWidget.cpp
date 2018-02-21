#include "gui/cImageWindowWidget.h"

#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QMessageBox>
#include <QScrollBar>


cImageWindowWidget::cImageWindowWidget() {
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	pixmap = new QPixmap();
	scene = new QGraphicsScene(this);
	setScene(scene);
	coordinates.clear();
	filteredstring = "";
	casesensitive = false;
	colorscale = true;
	absoluteintensity = false;

	minx = 0;
	miny = 0;
	maxx = 1;
	maxy = 1;
	maxcountx = 1;
	maxcounty = 1;
	vendor = unknownvendor;
	currentscale = 1;
	factor = 0.1;
	ispixmapdefined = false;
	enablecorrelateselectiontool = false;

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

	connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollbarValueChanged(int)));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollbarValueChanged(int)));
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


void cImageWindowWidget::setFilterOptions(vector<cCoordinates>& coordinates, string& filteredstring, bool casesensitive) {
	this->coordinates = coordinates;
	this->filteredstring = filteredstring;
	this->casesensitive = casesensitive;
	redrawScene();
}


void cImageWindowWidget::setMaxXY(int minx, int miny, int maxx, int maxy) {
	this->minx = minx;
	this->miny = miny;
	this->maxx = maxx;
	this->maxy = maxy;
	redrawScene();
}


void cImageWindowWidget::setDefaultMaxXY(int maxcountx, int maxcounty, eVendorType vendor) {
	this->maxcountx = maxcountx;
	this->maxcounty = maxcounty;
	this->vendor = vendor;
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

	if (ispixmapdefined) {

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

	if (ispixmapdefined) {

		QPointF p = mapToScene(event->x(), event->y());
		currentx = (int)p.x();
		currenty = (int)p.y();

		updateSelectionGroup();

	}

	event->accept();
}


void cImageWindowWidget::mouseReleaseEvent(QMouseEvent *event) {
	QGraphicsView::mouseReleaseEvent(event);

	if (ispixmapdefined) {

		int xmin, xmax, ymin, ymax;

		// correlate image
		if (enablecorrelateselectiontool) {
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

				double pointwidth;
				double pointheight;

				if (vendor == bruker) {
					pointwidth = max((double)abs(pressedx - currentx) / (double)(maxcountx + 1), (double)1);
					pointheight = max((double)abs(pressedy - currenty) / (double)(maxcounty + 1), (double)1);
				}
				else {
					pointwidth = max((double)abs(pressedx - currentx) / (double)(max(maxcountx, 1)), (double)1);
					pointheight = max((double)abs(pressedy - currenty) / (double)(max(maxcounty, 1)), (double)1);
				}

				xmin = (double)(currentx < pressedx ? currentx : pressedx) / pointwidth;
				xmax = (double)currentwidth / pointwidth;
				ymin = (double)(currenty < pressedy ? currenty : pressedy) / pointheight;
				ymax = (double)currentheight / pointheight;

				pressedx = -1;
				pressedy = -1;

				redrawScene();

				emit updateCorrelation(xmin, xmax, ymin, ymax);
			}
		}
		// selection tool
		else {
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

				if (vendor == bruker) {
					xmin = max(0, (currentx < pressedx ? currentx : pressedx)*(maxx + 1) / max(1, currentwidth) - minx);
					xmax = min(maxx, (currentx < pressedx ? pressedx : currentx)*(maxx + 1) / max(1, currentwidth) - minx);
					ymin = max(0, (currenty < pressedy ? currenty : pressedy)*(maxy + 1) / max(1, currentheight) - miny);
					ymax = min(maxy, (currenty < pressedy ? pressedy : currenty)*(maxy + 1) / max(1, currentheight) - miny);
				}
				else {
					xmin = max(0, (currentx < pressedx ? currentx : pressedx)*maxx / max(1, currentwidth) - minx) + 1;
					xmax = min(maxx, (currentx < pressedx ? pressedx : currentx)*maxx / max(1, currentwidth) - minx) + 1;
					ymin = max(0, (currenty < pressedy ? currenty : pressedy)*maxy / max(1, currentheight) - miny) + 1;
					ymax = min(maxy, (currenty < pressedy ? pressedy : currenty)*maxy / max(1, currentheight) - miny) + 1;
				}

				pressedx = -1;
				pressedy = -1;

				redrawScene();

				emit updateFilter(xmin, xmax, ymin, ymax);
			}
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

	QString qstr;
	QGraphicsTextItem* text;

	scene->removeItem(selectiongroup);
	scene->clear();
	selectiongroup->setVisible(false);
	scene->addItem(selectiongroup);

	if (!ispixmapdefined) {
		scene->addText("Please, open an optical image.");
		return;
	}

	QPixmap scaledpixmap = pixmap->scaledToHeight(pixmap->height()*currentscale);
	currentwidth = scaledpixmap.width();
	currentheight = scaledpixmap.height();
	scene->addPixmap(scaledpixmap);

	unordered_set<cCoordinates, hash_cCoordinates> reduced_coordinates;
	for (int i = 0; i < (int)coordinates.size(); i++) {
		if (reduced_coordinates.count(coordinates[i]) == 1) {
			auto it = reduced_coordinates.find(coordinates[i]);
			cCoordinates coord = *it;
			coord.mzratio = 0;
			coord.relativeintensity += coordinates[i].relativeintensity;
			coord.absoluteintensity += coordinates[i].absoluteintensity;
			coord.name += "\n" + coordinates[i].name;
			reduced_coordinates.erase(it);
			reduced_coordinates.insert(coord);

		}
		else {
			reduced_coordinates.insert(coordinates[i]);
		}
	}

	double minimumintensity = DBL_MAX;
	double maximumintensity = 0;
	double averageintensity = 0;
	vector<double> medianvector;
	for (auto it = reduced_coordinates.begin(); it != reduced_coordinates.end(); ++it) {
		if (!absoluteintensity) {
			if (it->relativeintensity < minimumintensity) {
				minimumintensity = it->relativeintensity;
			}
			if (it->relativeintensity > maximumintensity) {
				maximumintensity = it->relativeintensity;
			}
			averageintensity += it->relativeintensity;
			medianvector.push_back(it->relativeintensity);
		}
		else {
			if (it->absoluteintensity < minimumintensity) {
				minimumintensity = it->absoluteintensity;
			}
			if (it->absoluteintensity > maximumintensity) {
				maximumintensity = it->absoluteintensity;
			}
			averageintensity += it->absoluteintensity;
			medianvector.push_back(it->absoluteintensity);
		}
	}

	if (reduced_coordinates.size() > 0) {
		averageintensity /= (double)reduced_coordinates.size();
	}

	double medianintensity = median(medianvector);

	if (maximumintensity > 0) {

		double hue_min = 0.2;
		double hue_max = 1;
		qreal x, y, w, h;

		for (auto it = reduced_coordinates.begin(); it != reduced_coordinates.end(); ++it) {

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

			if (vendor == bruker) {
				if ((it->x > maxx + 1 - minx) || (it->y > maxy + 1 - miny)) {
					continue;
				}

				x = ((it->x + minx) * currentwidth) / (maxx + 1);
				y = ((it->y + miny) * currentheight) / (maxy + 1);
				w = currentwidth / (maxx + 1);
				h = currentheight / (maxy + 1);
			}
			else {
				if ((it->x > maxx - minx) || (it->y > maxy - miny)) {
					continue;
				}

				x = ((it->x + minx - 1) * currentwidth) / max(maxx, 1);
				y = ((it->y + miny - 1) * currentheight) / max(maxy, 1);
				w = currentwidth / max(maxx, 1);
				h = currentheight / max(maxy, 1);
			}

			QGraphicsRectItem* rectitem = new QGraphicsRectItem(x, y, w, h);
			rectitem->setPen(Qt::NoPen);
			QColor color;
			if (!absoluteintensity) {
				color.setHslF(min(max((double)1 - (it->relativeintensity*(hue_max - hue_min)/maximumintensity + hue_min), 0.0), 1.0), 0.5, 0.5, 0.75);
			}
			else {
				color.setHslF(min(max((double)1 - (it->absoluteintensity*(hue_max - hue_min)/maximumintensity + hue_min), 0.0), 1.0), 0.5, 0.5, 0.75);
			}
			rectitem->setBrush(QBrush(color));
			string tooltip;
			if (it->mzratio == 0) {
				tooltip = it->name + "\nID: " + to_string(it->id) + "\nX: " + to_string(it->x) + "\nY: " + to_string(it->y) + "\nsum of rel. intensities: " + to_string(it->relativeintensity) + "%\nsum of abs. intensities: " + QVariant(cropDecimalsByteArray(it->absoluteintensity)).toString().toStdString();
			}
			else {
				tooltip = it->name + "\nID: " + to_string(it->id) + "\nX: " + to_string(it->x) + "\nY: " + to_string(it->y) + "\nm/z: " + to_string(it->mzratio) + "\nrel. intensity: " + to_string(it->relativeintensity) + "%\nabs. intensity: " + QVariant(cropDecimalsByteArray(it->absoluteintensity)).toString().toStdString();
			}
			rectitem->setToolTip(tooltip.c_str());
			scene->addItem(rectitem);
		}

		if (colorscale) {

			int scalegranularity = min(500, currentheight - 50);
			for (int i = 0; i <= scalegranularity; i++) {
				QGraphicsLineItem* lineitem = new QGraphicsLineItem(QLineF(mapToScene(25, i + 25), mapToScene(75, i + 25)));
				QColor color;
				color.setHslF(min(max((double)1 - ((double)(scalegranularity - i)*(hue_max - hue_min)/(double)scalegranularity + hue_min), 0.0), 1.0), 0.5, 0.5, 1);
				lineitem->setPen(QPen(color));
				scene->addItem(lineitem);
			}

			stringstream scalemin;
			scalemin << std::fixed;
			if (!absoluteintensity) {
				scalemin << std::setprecision(2);
			}
			else {
				scalemin << std::setprecision(0);
			}
			scalemin << minimumintensity;

			stringstream scalemax;
			scalemax << std::fixed;
			if (!absoluteintensity) {
				scalemax << std::setprecision(2);
			}
			else {
				scalemax << std::setprecision(0);
			}
			scalemax << maximumintensity;

			if (!absoluteintensity) {
				scalemin << " %";
				scalemax << " %";
			}

			text = scene->addText(scalemax.str().c_str());
			text->setPos(mapToScene(75, 15));
			text->setDefaultTextColor(QColor(255, 255, 255));

			text = scene->addText(scalemin.str().c_str());
			text->setPos(mapToScene(75, scalegranularity + 15));
			text->setDefaultTextColor(QColor(255, 255, 255));

		}

	}

	stringstream avgint;
	avgint << std::fixed;
	if (!absoluteintensity) {
		avgint << std::setprecision(2);
	}
	else {
		avgint << std::setprecision(0);
	}
	avgint << averageintensity;
	if (!absoluteintensity) {
		avgint << " %";
	}

	stringstream medint;
	medint << std::fixed;
	if (!absoluteintensity) {
		medint << std::setprecision(2);
	}
	else {
		medint << std::setprecision(0);
	}
	medint << medianintensity;
	if (!absoluteintensity) {
		medint << " %";
	}

	qstr = "Number of Points Selected: " + QString::number(reduced_coordinates.size());
	if (!filteredstring.empty()) {
		qstr += " (filtered by text: \"" + QString(filteredstring.c_str()) + "\", case sensitive: " + QString((casesensitive)?"yes":"no") + ")";
	}
	qstr += "\n";
	qstr += "Minimum Bounding Region:\nX: " + QString::number(xmin) + "-" + QString::number(xmax) + "; Y: " + QString::number(ymin) + "-" + QString::number(ymax) + "\n";
	qstr += "Average intensity: ";
	qstr += avgint.str().c_str();
	qstr += "\n";
	qstr += "Median intensity: ";
	qstr += medint.str().c_str();
	qstr += "\n";
	text = scene->addText(qstr);
	text->setPos(2, currentheight + 2);

	setSceneRect(0, 0, currentwidth, currentheight + 75);

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

		if (enablecorrelateselectiontool) {
			selectionrect->setPen(QPen(Qt::red, 1, Qt::DashLine));
		}
		else {
			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
		}
		selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

		if (enablecorrelateselectiontool) {
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

			double pointwidth;
			double pointheight;

			if (vendor == bruker) {
				pointwidth = max((double)abs(pressedx - currentx) / (double)(maxcountx + 1), (double)1);
				pointheight = max((double)abs(pressedy - currenty) / (double)(maxcounty + 1), (double)1);
			}
			else {
				pointwidth = max((double)abs(pressedx - currentx) / (double)(max(maxcountx, 1)), (double)1);
				pointheight = max((double)abs(pressedy - currenty) / (double)(max(maxcounty, 1)), (double)1);
			}

			xmin = (double)(currentx < pressedx ? currentx : pressedx) / pointwidth;
			xmax = (double)currentwidth / pointwidth;
			ymin = (double)(currenty < pressedy ? currenty : pressedy) / pointheight;
			ymax = (double)currentheight / pointheight;

			qstr = "Left Shift: " + QString::number(xmin) + ", Top Shift: " + QString::number(ymin) + ", Max X: " + QString::number(xmax) + ", Max Y:" + QString::number(ymax);
		}
		else {

			if (vendor == bruker) {
				xmin = max(0, (currentx < pressedx ? currentx : pressedx)*(maxx + 1) / max(1, currentwidth) - minx);
				xmax = min(maxx, (currentx < pressedx ? pressedx : currentx)*(maxx + 1) / max(1, currentwidth) - minx);
				ymin = max(0, (currenty < pressedy ? currenty : pressedy)*(maxy + 1) / max(1, currentheight) - miny);
				ymax = min(maxy, (currenty < pressedy ? pressedy : currenty)*(maxy + 1) / max(1, currentheight) - miny);
			}
			else {
				xmin = max(0, (currentx < pressedx ? currentx : pressedx)*maxx / max(1, currentwidth) - minx) + 1;
				xmax = min(maxx, (currentx < pressedx ? pressedx : currentx)*maxx / max(1, currentwidth) - minx) + 1;
				ymin = max(0, (currenty < pressedy ? currenty : pressedy)*maxy / max(1, currentheight) - miny) + 1;
				ymax = min(maxy, (currenty < pressedy ? pressedy : currenty)*maxy / max(1, currentheight) - miny) + 1;
			}

			qstr = "X: " + QString::number(xmin) + "-" + QString::number(xmax) + "; Y: " + QString::number(ymin) + "-" + QString::number(ymax);
		}
		
		selectionsimpletextitem->setFont(myFont);
		selectionsimpletextitem->setText(qstr);
		if (enablecorrelateselectiontool) {
			selectionsimpletextitem->setBrush(QBrush(Qt::red));
		}
		else {
			selectionsimpletextitem->setBrush(QBrush(Qt::green));
		}
		selectionsimpletextitem->setPos(QPointF(pressedx + 2, pressedy));

		selectiongroup->setVisible(true);
		selectionrect->setVisible(true);
		selectionsimpletextitem->setVisible(true);

	} 
	else {

		if (vendor == bruker) {
			xmin = currentx*(maxx + 1) / max(1, currentwidth) - minx;
			ymin = currenty*(maxy + 1) / max(1, currentheight) - miny;
		}
		else {
			xmin = currentx*maxx / max(1, currentwidth) - minx + 1;
			ymin = currenty*maxy / max(1, currentheight) - miny + 1;
		}

		if ((currentx > 0) && (currenty > 0) && (xmin <= maxx - minx) && (ymin <= maxy - miny)) {
			if (enablecorrelateselectiontool) {
				selectionsimpletextitem->setBrush(QBrush(Qt::red));
				qstr = "Left Shift: " + QString::number(xmin) + ", Top Shift: " + QString::number(ymin);
			}
			else {
				selectionsimpletextitem->setBrush(QBrush(Qt::green));
				qstr = "X: " + QString::number(xmin) + " Y: " + QString::number(ymin);
			}
			selectionsimpletextitem->setFont(myFont);
			selectionsimpletextitem->setText(qstr);
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


void cImageWindowWidget::scrollbarValueChanged(int value) {
	redrawScene();
}


void cImageWindowWidget::absoluteIntensityStateChanged(bool state) {
	absoluteintensity = state;
	redrawScene();
}


void cImageWindowWidget::colorScaleStateChanged(bool state) {
	colorscale = state;
	redrawScene();
}


void cImageWindowWidget::enableCorrelateSelectionTool(bool enable) {
	enablecorrelateselectiontool = enable;
}

