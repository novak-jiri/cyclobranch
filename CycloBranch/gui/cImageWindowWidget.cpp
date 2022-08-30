#include "gui/cImageWindowWidget.h"

#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QMessageBox>
#include <QScrollBar>


cImageWindowWidget::cImageWindowWidget() {
	setAlignment(Qt::AlignLeft | Qt::AlignTop);

	scene = new QGraphicsScene(this);
	setScene(scene);

	coordinateinfo.clear();
	operatortype = 0;
	columnname1 = "";
	comparatorname1 = "";
	filterstring1 = "";
	columnname2 = "";
	comparatorname2 = "";
	filterstring2 = "";
	casesensitive = false;
	wholeword = false;

	colorscale = true;
	absoluteintensity = false;

	layersvector.clear();
	activelayer = 0;

	xfrom = 0;
	xto = 0;
	yfrom = 0;
	yto = 0;

	leftshift = 0;
	topshift = 0;
	maxx = 1;
	maxy = 1;
	pixelsizex = 1;
	pixelsizey = 1;

	defaultmaxx = 1;
	defaultmaxy = 1;
	defaultpixelsizex = 1;
	defaultpixelsizey = 1;
	vendor = unknownvendor;

	currentscale = 1;
	factor = 2.0;

	rulersize = 250;
	rulervalue = 1000;

	pressedx = -1;
	pressedy = -1;
	currentx = 0;
	currenty = 0;
	currentwidth = 1;
	currentheight = 1;

	showselection = false;
	keepaspectratio = false;

	microscopynavigationcenterx = 0;
	microscopynavigationcentery = 0;

	selectionrect = new QGraphicsRectItem();
	selectionrect->setZValue(1000);
	scene->addItem(selectionrect);

	selectionsimpletextitem = new QGraphicsSimpleTextItem();
	selectionsimpletextitem->setZValue(1000);
	scene->addItem(selectionsimpletextitem);

	currentrect = new QGraphicsRectItem();
	currentrect->setRect(selectionrect->rect());
	currentrect->setTransform(selectionrect->transform());

	setCursor(Qt::ArrowCursor);
	cursoractivity = cursoractivity_none;

	connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollbarValueChanged(int)));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollbarValueChanged(int)));
}


cImageWindowWidget::~cImageWindowWidget() {
	delete currentrect;
	delete scene;
}


void cImageWindowWidget::setOpticalImage(QImage* image) {
	*layersvector[1].pixmap = QPixmap::fromImage(image->convertToFormat(QImage::Format_ARGB32_Premultiplied));
	
	currentscale = 1;
	rulervalue = recalculateRulerValue(currentscale);

	emit updateRuler(rulervalue);

	layersvector[1].ispixmapdefined = true;
	redrawScene();
}


void cImageWindowWidget::setHistologyImage(QImage* histologyimage) {
	*layersvector[2].pixmap = QPixmap::fromImage(histologyimage->convertToFormat(QImage::Format_ARGB32_Premultiplied));
	layersvector[2].ispixmapdefined = true;
	redrawScene();
}


void cImageWindowWidget::setMicroscopyImage(int layer, QImage* microscopyimage) {
	*layersvector[layer].pixmap = QPixmap::fromImage(microscopyimage->convertToFormat(QImage::Format_ARGB32_Premultiplied));
	layersvector[layer].ispixmapdefined = true;
	redrawScene();
}


QImage cImageWindowWidget::getImage() {
	QRect rect_viewport(0, 0, viewport()->width(), viewport()->height());
	QRectF rect_scene = mapToScene(rect_viewport).boundingRect();

	QImage image(rect_scene.size().toSize(), QImage::Format_ARGB32);
	image.fill(Qt::white);

	QPainter painter;
	if (!painter.begin(&image)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return image;
	}

	scene->render(&painter, rect_viewport, rect_scene, Qt::KeepAspectRatio);
	painter.end();
	
	return image;
}


void cImageWindowWidget::setFilterOptions(vector<cCoordinateInfo>& coordinateinfo, bool operatortype, string& columnname1, string& comparatorname1, string& filterstring1, string& columnname2, string& comparatorname2, string& filterstring2, bool casesensitive, bool wholeword) {
	this->coordinateinfo = coordinateinfo;
	this->operatortype = operatortype;
	this->columnname1 = columnname1;
	this->comparatorname1 = comparatorname1;
	this->filterstring1 = filterstring1;
	this->columnname2 = columnname2;
	this->comparatorname2 = comparatorname2;
	this->filterstring2 = filterstring2;
	this->casesensitive = casesensitive;
	this->wholeword = wholeword;
	redrawScene();
}


void cImageWindowWidget::setCorrelationValues(int leftshift, int topshift, int maxx, int maxy, int pixelsizex, int pixelsizey) {
	this->leftshift = leftshift;
	this->topshift = topshift;
	this->maxx = maxx;
	this->maxy = maxy;
	this->pixelsizex = pixelsizex;
	this->pixelsizey = pixelsizey;

	emit updateRuler(recalculateRulerValue(currentscale));
}


void cImageWindowWidget::setDefaultMaxXY(int defaultmaxx, int defaultmaxy, int defaultpixelsizex, int defaultpixelsizey, eVendorType vendor) {
	this->defaultmaxx = defaultmaxx;
	this->defaultmaxy = defaultmaxy;
	this->defaultpixelsizex = defaultpixelsizex;
	this->defaultpixelsizey = defaultpixelsizey;
	this->vendor = vendor;
}


void cImageWindowWidget::setHistologyPosition(int x, int y, int width, int height, double angle) {
	layersvector[2].x = x;
	layersvector[2].y = y;
	layersvector[2].width = width;
	layersvector[2].height = height;
	layersvector[2].angle = angle;
}


void cImageWindowWidget::setMicroscopyPosition(int layer, bool flipx, bool flipy, double x, double y, double width, double height, double angle) {
	layersvector[layer].fliphorizontally = flipx;
	layersvector[layer].flipvertically = flipy;
	layersvector[layer].x = x;
	layersvector[layer].y = y;
	layersvector[layer].width = width;
	layersvector[layer].height = height;
	layersvector[layer].angle = angle;
}


void cImageWindowWidget::goToMicroscopyPosition(int layer) {
	if (!layersvector[1].ispixmapdefined || !layersvector[layer].ispixmapdefined) {
		return;
	}

	horizontalScrollBar()->setValue(max(0.0, layersvector[layer].lastx - 250.0));
	verticalScrollBar()->setValue(max(0.0, layersvector[layer].lasty - 100.0));
}


void cImageWindowWidget::setSelectedRegion(int xfrom, int xto, int yfrom, int yto) {
	this->xfrom = xfrom;
	this->xto = xto;
	this->yfrom = yfrom;
	this->yto = yto;
}


void cImageWindowWidget::redraw() {
	redrawScene();
}


void cImageWindowWidget::clearLayers() {
	activelayer = 0;

	size_t size = layersvector.size();
	layersvector.clear();
	layersvector.resize(size);
}


void cImageWindowWidget::clearLayer(int layer) {
	if ((layer >= 0) && (layer < layersvector.size())) {
		layersvector[layer].clear();
	}
}


void cImageWindowWidget::setKeepAspectRatio(bool state) {
	keepaspectratio = state;

	// simulate mouseMoveEvent
	if ((pressedx != -1) && (pressedy != -1)) {
		if (layersvector[1].ispixmapdefined) {
			updateSelectionGroup();
		}
	}
}


void cImageWindowWidget::setNavigationLayer(int layer, int navigation) {
	layersvector[layer].navigationlayer = navigation;
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

	if (layersvector[1].ispixmapdefined) {

		if (event->button() == Qt::LeftButton) {
			QPointF p = mapToScene(event->x(), event->y());
			pressedx = (int)p.x();
			pressedy = (int)p.y();

			currentx = pressedx;
			currenty = pressedy;

			int transformedx = currentx;
			int transformedy = currenty;
			double tmpangle;

			if (activelayer >= 2) {
				QTransform transform;
				tmpangle = prepareTransformation(selectionrect->rect(), transform, layersvector[activelayer].fliphorizontally, layersvector[activelayer].flipvertically, layersvector[activelayer].angle, true);
				transform.map(currentx, currenty, &transformedx, &transformedy);
			}

			if (showselection) {
				bool rotatedimage = ((tmpangle >= 45) && (tmpangle <= 135)) || ((tmpangle <= -45) && (tmpangle >= -135));

				if ((abs(transformedx - selectionrect->rect().x()) < 5) && (abs(transformedy - selectionrect->rect().y()) < 5)) {
					setCursor(Qt::SizeFDiagCursor);
					cursoractivity = cursoractivity_resize_top_left;
					if (activelayer >= 2) {
						if (rotatedimage) {
							setCursor(Qt::SizeBDiagCursor);
						}
					}
				}
				else if ((abs(transformedx - selectionrect->rect().x() - selectionrect->rect().width()) < 5) && (abs(transformedy - selectionrect->rect().y()) < 5)) {
					setCursor(Qt::SizeBDiagCursor);
					cursoractivity = cursoractivity_resize_top_right;
					if (activelayer >= 2) {
						if (rotatedimage) {
							setCursor(Qt::SizeFDiagCursor);
						}
					}
				}
				else if ((abs(transformedx - selectionrect->rect().x()) < 5) && (abs(transformedy - selectionrect->rect().y() - selectionrect->rect().height()) < 5)) {
					setCursor(Qt::SizeBDiagCursor);
					cursoractivity = cursoractivity_resize_bottom_left;
					if (activelayer >= 2) {
						if (rotatedimage) {
							setCursor(Qt::SizeFDiagCursor);
						}
					}
				}
				else if ((abs(transformedx - selectionrect->rect().x() - selectionrect->rect().width()) < 5) && (abs(transformedy - selectionrect->rect().y() - selectionrect->rect().height()) < 5)) {
					setCursor(Qt::SizeFDiagCursor);
					cursoractivity = cursoractivity_resize_bottom_right;
					if (activelayer >= 2) {
						if (rotatedimage) {
							setCursor(Qt::SizeBDiagCursor);
						}
					}
				}
				else if ((abs(transformedx - selectionrect->rect().x()) < 5) && (transformedy >= selectionrect->rect().y()) && (transformedy <= selectionrect->rect().y() + selectionrect->rect().height())) {
					setCursor(Qt::SizeHorCursor);
					cursoractivity = cursoractivity_resize_left;
					if (activelayer >= 2) {
						if (rotatedimage) {
							setCursor(Qt::SizeVerCursor);
						}
					}
				}
				else if ((abs(transformedx - selectionrect->rect().x() - selectionrect->rect().width()) < 5) && (transformedy >= selectionrect->rect().y()) && (transformedy <= selectionrect->rect().y() + selectionrect->rect().height())) {
					setCursor(Qt::SizeHorCursor);
					cursoractivity = cursoractivity_resize_right;
					if (activelayer >= 2) {
						if (rotatedimage) {
							setCursor(Qt::SizeVerCursor);
						}
					}
				}
				else if ((abs(transformedy - selectionrect->rect().y()) < 5) && (transformedx >= selectionrect->rect().x()) && (transformedx <= selectionrect->rect().x() + selectionrect->rect().width())) {
					setCursor(Qt::SizeVerCursor);
					cursoractivity = cursoractivity_resize_top;
					if (activelayer >= 2) {
						if (rotatedimage) {
							setCursor(Qt::SizeHorCursor);
						}
					}
				}
				else if ((abs(transformedy - selectionrect->rect().y() - selectionrect->rect().height()) < 5) && (transformedx >= selectionrect->rect().x()) && (transformedx <= selectionrect->rect().x() + selectionrect->rect().width())) {
					setCursor(Qt::SizeVerCursor);
					cursoractivity = cursoractivity_resize_bottom;
					if (activelayer >= 2) {
						if (rotatedimage) {
							setCursor(Qt::SizeHorCursor);
						}
					}
				}
				else if ((transformedx > selectionrect->rect().x()) && (transformedx < selectionrect->rect().x() + selectionrect->rect().width())
					&& (transformedy > selectionrect->rect().y()) && (transformedy < selectionrect->rect().y() + selectionrect->rect().height())) {
					setCursor(Qt::SizeAllCursor);
					cursoractivity = cursoractivity_move;
				}
				else {
					setCursor(Qt::ArrowCursor);
					cursoractivity = cursoractivity_none;
				}
			}
			else {
				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;
			}

			updateSelectionGroup();
		}

		if (event->button() == Qt::RightButton) {
			pressedx = -1;
			pressedy = -1;

			setCursor(Qt::ArrowCursor);
			cursoractivity = cursoractivity_none;

			redrawScene();
		}

		if (event->button() == Qt::MiddleButton) {
			pressedx = -1;
			pressedy = -1;

			setCursor(Qt::ArrowCursor);
			cursoractivity = cursoractivity_none;

			normalSize();

			redrawScene();
		}

	}

	event->accept();
}


void cImageWindowWidget::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);

	if (layersvector[1].ispixmapdefined) {

		QPointF p = mapToScene(event->x(), event->y());
		currentx = (int)p.x();
		currenty = (int)p.y();

		updateSelectionGroup();

	}

	event->accept();
}


void cImageWindowWidget::mouseReleaseEvent(QMouseEvent *event) {
	QGraphicsView::mouseReleaseEvent(event);

	if (layersvector[1].ispixmapdefined) {

		int xmin, xmax, ymin, ymax;
		qreal rx1, ry1, rx2, ry2;

		// select region tool
		if (activelayer == 0) {
			if (pressedx == currentx) {
				pressedx = -1;

				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;

				redrawScene();
			}

			if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {

				getRectanglePoints(rx1, ry1, rx2, ry2);

				if (vendor == bruker) {
					xfrom = max(0, (int)rx1*(maxx + 1) / max(1, currentwidth) - leftshift);
					xto = min(maxx, (int)rx2*(maxx + 1) / max(1, currentwidth) - leftshift);
					yfrom = max(0, (int)ry1*(maxy + 1) / max(1, currentheight) - topshift);
					yto = min(maxy, (int)ry2*(maxy + 1) / max(1, currentheight) - topshift);
				}
				else {
					xfrom = max(0, (int)rx1*maxx / max(1, currentwidth) - leftshift) + 1;
					xto = min(maxx, (int)rx2*maxx / max(1, currentwidth) - leftshift) + 1;
					yfrom = max(0, (int)ry1*maxy / max(1, currentheight) - topshift) + 1;
					yto = min(maxy, (int)ry2*maxy / max(1, currentheight) - topshift) + 1;
				}

				pressedx = -1;
				pressedy = -1;

				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;

				redrawScene();

				emit updateFilter(xfrom, xto, yfrom, yto);
			}
		}

		// correlate image
		if (activelayer == 1) {
			if (pressedx == currentx) {
				pressedx = -1;

				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;

				redrawScene();
			}

			if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {

				double pointwidth, pointheight;

				getRectanglePoints(rx1, ry1, rx2, ry2);

				if (vendor == bruker) {
					pointwidth = max((double)abs(rx2 - rx1), 1.0) / (double)(defaultmaxx + 1);
					pointheight = max((double)abs(ry2 - ry1), 1.0) / (double)(defaultmaxy + 1);
				}
				else {
					pointwidth = max((double)abs(rx2 - rx1), 1.0) / (double)(max(defaultmaxx, 1));
					pointheight = max((double)abs(ry2 - ry1), 1.0) / (double)(max(defaultmaxy, 1));
				}

				xmax = (double)currentwidth / pointwidth;
				ymax = (double)currentheight / pointheight;

				xmin = (double)rx1 / pointwidth;
				ymin = (double)ry1 / pointheight;

				pressedx = -1;
				pressedy = -1;

				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;

				redrawScene();

				emit updateCorrelation(xmin, xmax, ymin, ymax);
			}
		}

		// set histology
		if (activelayer == 2) {
			if (pressedx == currentx) {
				pressedx = -1;

				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;

				redrawScene();
			}

			if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {
				getRectanglePoints(rx1, ry1, rx2, ry2);

				int x = (double)rx1 / currentscale;
				int y = (double)ry1 / currentscale;
				int width = abs(rx2 - rx1) / currentscale;
				int height = abs(ry2 - ry1) / currentscale;

				pressedx = -1;
				pressedy = -1;

				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;

				redrawScene();

				emit updateHistologyPosition(x, y, width, height, layersvector[2].angle);
			}
		}

		// set microscopy
		if (activelayer >= 3) {
			if (pressedx == currentx) {
				pressedx = -1;

				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;

				redrawScene();
			}

			if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {
				getRectanglePoints(rx1, ry1, rx2, ry2);

				int micrometerswidth, micrometersheight;
				switch (vendor) {
					case unknownvendor:
						micrometerswidth = maxx*pixelsizex;
						micrometersheight = maxy*pixelsizey;
						break;
					case bruker:
						micrometerswidth = (maxx + 1)*pixelsizex;
						micrometersheight = (maxy + 1)*pixelsizey;
						break;
					case waters:
						micrometerswidth = maxx*pixelsizex;
						micrometersheight = maxy*pixelsizey;
						break;
					default:
						break;
				}

				layersvector[activelayer].x = (double)rx1 * (double)micrometerswidth / (double)currentwidth;
				layersvector[activelayer].y = (double)ry1 * (double)micrometersheight / (double)currentheight;
				layersvector[activelayer].width = abs(rx2 - rx1) * (double)micrometerswidth / (double)currentwidth;
				layersvector[activelayer].height = abs(ry2 - ry1) * (double)micrometersheight / (double)currentheight;

				pressedx = -1;
				pressedy = -1;

				setCursor(Qt::ArrowCursor);
				cursoractivity = cursoractivity_none;

				redrawScene();

				emit updateMicroscopyPosition(layersvector[activelayer].fliphorizontally, layersvector[activelayer].flipvertically, layersvector[activelayer].x, layersvector[activelayer].y, layersvector[activelayer].width, layersvector[activelayer].height, layersvector[activelayer].angle);
			}
		}

	}

	event->accept();
}


void cImageWindowWidget::mouseDoubleClickEvent(QMouseEvent *event) {
	QGraphicsView::mouseDoubleClickEvent(event);
	int x, y, spectrumid;

	if (event->button() == Qt::LeftButton) {
		QPointF p = mapToScene(event->x(), event->y());
		pressedx = (int)p.x();
		pressedy = (int)p.y();

		currentx = pressedx;
		currenty = pressedy;

		if (vendor == bruker) {
			x = currentx * (maxx + 1) / max(1, currentwidth) - leftshift;
			y = currenty * (maxy + 1) / max(1, currentheight) - topshift;
		}
		else {
			x = currentx * maxx / max(1, currentwidth) - leftshift + 1;
			y = currenty * maxy / max(1, currentheight) - topshift + 1;
		}

		if ((currentx > 0) && (currenty > 0) && (currentx < currentwidth) && (currenty < currentheight)) {
				
			spectrumid = -1;
			for (auto& it : coordinateinfo) {
				if ((it.x == x) && (it.y == y)) {
					spectrumid = it.id;
					break;
				}
			}

			if (spectrumid != -1) {
				emit imageWidgetDoubleClicked(spectrumid - 1);
			}
		}

		pressedx = -1;
		pressedy = -1;

		setCursor(Qt::ArrowCursor);
		cursoractivity = cursoractivity_none;

		redrawScene();

	}

	event->accept();
}


void cImageWindowWidget::resizeEvent(QResizeEvent *event) {
	QGraphicsView::resizeEvent(event);
	redrawScene();
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

	scene->removeItem(selectionrect);
	scene->removeItem(selectionsimpletextitem);

	scene->clear();

	selectionrect->setVisible(false);
	selectionsimpletextitem->setVisible(false);

	scene->addItem(selectionrect);
	scene->addItem(selectionsimpletextitem);

	if (!((int)layersvector.size() >= 2) || !layersvector[1].ispixmapdefined) {
		scene->addText("Please, open an optical image ('File->Open Image', CTRL + O) or load a previously saved configuration ('File->Load Layers', CTRL + L).");
		return;
	}

	QRect rect_viewport(0, 0, viewport()->width(), viewport()->height());
	QRectF rect_scene = mapToScene(rect_viewport).boundingRect();

	QPixmap scaledpixmap = layersvector[1].pixmap->copy(rect_scene.x() / currentscale, rect_scene.y() / currentscale, rect_scene.width() / currentscale + 1, rect_scene.height() / currentscale + 1);

	if (scaledpixmap.isNull()) {
		return;
	}

	if (scaledpixmap.width() * currentscale * scaledpixmap.height() * currentscale > 100000000) {
		// too many pixels
		return;
	}

	scaledpixmap = scaledpixmap.scaledToHeight(scaledpixmap.height()*currentscale);

	if (scaledpixmap.isNull()) {
		return;
	}

	currentwidth = layersvector[1].pixmap->width()*currentscale;
	currentheight = layersvector[1].pixmap->height()*currentscale;

	if (((int)layersvector.size() >= 2) && layersvector[1].checked) {
		QPixmap alphapixmap(scaledpixmap.size());
		alphapixmap.fill(Qt::transparent);

		QPainter painter(&alphapixmap);
		painter.setOpacity((double)layersvector[1].alpha / 100.0);
		painter.drawPixmap(0, 0, scaledpixmap);
		painter.end();

		QGraphicsPixmapItem* pixmapitem = scene->addPixmap(alphapixmap);
		pixmapitem->setPos(rect_scene.x(), rect_scene.y());
		pixmapitem->setZValue(layersvector[1].zvalue);
	}

	if (((int)layersvector.size() >= 3) && layersvector[2].ispixmapdefined) {
		if (layersvector[2].checked) {
			if ((layersvector[2].width * currentscale > 1) && (layersvector[2].height * currentscale > 1)) {

				QPixmap scaledhistologypixmap = layersvector[2].pixmap->scaled((int)layersvector[2].width, (int)layersvector[2].height);
				
				int historigcenterx = scaledhistologypixmap.rect().center().x();
				int historigcentery = scaledhistologypixmap.rect().center().y();

				QTransform transform;
				transform.translate((qreal)historigcenterx, (qreal)historigcentery);
				transform.scale(layersvector[2].flipvertically ? -1 : 1, layersvector[2].fliphorizontally ? -1 : 1);
				transform.rotate(layersvector[2].angle);
				transform.translate(-(qreal)historigcenterx, -(qreal)historigcentery);
				scaledhistologypixmap = scaledhistologypixmap.transformed(transform);

				int histnewcenterx = scaledhistologypixmap.rect().center().x();
				int histnewcentery = scaledhistologypixmap.rect().center().y();
				int histnewwidth = scaledhistologypixmap.width();
				int histnewheight = scaledhistologypixmap.height();

				if (((qreal)(layersvector[2].x + histnewwidth + historigcenterx - histnewcenterx) * currentscale > rect_scene.x()) && ((qreal)(layersvector[2].y + histnewheight + historigcentery - histnewcentery) * currentscale > rect_scene.y())) {
					scaledhistologypixmap = scaledhistologypixmap.copy(max(rect_scene.x() / currentscale - (qreal)(layersvector[2].x + historigcenterx - histnewcenterx), 0.0), max(rect_scene.y() / currentscale - (qreal)(layersvector[2].y + historigcentery - histnewcentery), 0.0), rect_scene.width() / currentscale + 1, rect_scene.height() / currentscale + 1);
					scaledhistologypixmap = scaledhistologypixmap.scaled(scaledhistologypixmap.width()*currentscale, scaledhistologypixmap.height()*currentscale);
					
					QPixmap alphapixmap(scaledhistologypixmap.size());
					alphapixmap.fill(Qt::transparent);

					QPainter painter(&alphapixmap);
					painter.setOpacity((double)layersvector[2].alpha / 100.0);
					painter.drawPixmap(0, 0, scaledhistologypixmap);
					painter.end();

					QGraphicsPixmapItem* pixmapitem = scene->addPixmap(alphapixmap);
					pixmapitem->setPos(max(rect_scene.x(), (qreal)(layersvector[2].x + historigcenterx - histnewcenterx) * currentscale), max(rect_scene.y(), (qreal)(layersvector[2].y + historigcentery - histnewcentery) * currentscale));
					pixmapitem->setZValue(layersvector[2].zvalue);
				}

			}
		}
	}

	microscopynavigationcenterx = 0;
	microscopynavigationcentery = 0;

	int size = (int)layersvector.size();
	for (int i = 3; i < size; i++) {
		if (((int)layersvector.size() >= i + 1) && layersvector[i].ispixmapdefined) {
			if ((i == 3) || (layersvector[i].checked)) {
				drawMicroscopyImage(i, rect_scene, currentwidth, currentheight);
			}
		}
	}

	double minimumintensity = DBL_MAX;
	double maximumintensity = 0;
	double averageintensity = 0;
	vector<double> medianvector;
	for (auto it = coordinateinfo.begin(); it != coordinateinfo.end(); ++it) {
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

	if (coordinateinfo.size() > 0) {
		averageintensity /= (double)coordinateinfo.size();
	}

	double medianintensity = median(medianvector);
	stringstream scalemin;
	stringstream scalemax;

	if (((int)layersvector.size() >= 1) && layersvector[0].checked) {

		if (maximumintensity > 0) {

			double hue_min = 0.2;
			double hue_max = 1;
			qreal x, y, w, h;

			for (auto it = coordinateinfo.begin(); it != coordinateinfo.end(); ++it) {

				if (it == coordinateinfo.begin()) {
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
					if ((it->x > maxx + 1 - leftshift) || (it->y > maxy + 1 - topshift)) {
						continue;
					}

					x = ((it->x + leftshift) * currentwidth) / (maxx + 1);
					y = ((it->y + topshift) * currentheight) / (maxy + 1);
					w = currentwidth / (maxx + 1);
					h = currentheight / (maxy + 1);
				}
				else {
					if ((it->x > maxx - leftshift) || (it->y > maxy - topshift)) {
						continue;
					}

					x = ((it->x + leftshift - 1) * currentwidth) / max(maxx, 1);
					y = ((it->y + topshift - 1) * currentheight) / max(maxy, 1);
					w = currentwidth / max(maxx, 1);
					h = currentheight / max(maxy, 1);
				}

				QGraphicsRectItem* rectitem = new QGraphicsRectItem(x, y, w, h);
				rectitem->setPen(Qt::NoPen);

				QColor color;
				double alpha = (double)layersvector[0].alpha / 100.0;

				if (!absoluteintensity) {
					color.setHslF(min(max((double)1 - (it->relativeintensity*(hue_max - hue_min) / maximumintensity + hue_min), 0.0), 1.0), 0.5, 0.5, alpha);
				}
				else {
					color.setHslF(min(max((double)1 - (it->absoluteintensity*(hue_max - hue_min) / maximumintensity + hue_min), 0.0), 1.0), 0.5, 0.5, alpha);
				}
				rectitem->setBrush(QBrush(color));

				string tooltip = it->description + "\nID: " + to_string(it->id) + "\nX: " + to_string(it->x) + "\nY: " + to_string(it->y) + "\nsum of rel. intensities: " + to_string(it->relativeintensity) + "%\nsum of abs. intensities: " + QVariant(cropDecimalsByteArray(it->absoluteintensity)).toString().toStdString();
				rectitem->setToolTip(tooltip.c_str());

				rectitem->setZValue(layersvector[0].zvalue);
				scene->addItem(rectitem);
			}

			int scalegranularity = min(500, currentheight - 50);
			if (colorscale) {
				for (int i = 0; i <= scalegranularity; i++) {
					QGraphicsLineItem* lineitem = new QGraphicsLineItem(QLineF(mapToScene(25, i + 25), mapToScene(75, i + 25)));
					QColor color;
					color.setHslF(min(max((double)1 - ((double)(scalegranularity - i)*(hue_max - hue_min) / (double)scalegranularity + hue_min), 0.0), 1.0), 0.5, 0.5, 1);
					lineitem->setPen(QPen(color));
					lineitem->setZValue(1001);
					scene->addItem(lineitem);
				}
			}

			scalemin << std::fixed;
			if (!absoluteintensity) {
				scalemin << std::setprecision(2);
			}
			else {
				scalemin << std::setprecision(0);
			}
			scalemin << minimumintensity;

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

			if (colorscale) {
				QString html;
				text = scene->addText("");
				text->setPos(mapToScene(75, 15));
				text->setDefaultTextColor(QColor(0, 0, 0));
				html = "<span style='background-color:#FFFFFF;'>";
				html += scalemax.str().c_str();
				html += "</span>";
				text->setHtml(html);
				text->setZValue(1001);

				text = scene->addText(scalemin.str().c_str());
				text->setPos(mapToScene(75, scalegranularity + 15));
				text->setDefaultTextColor(QColor(0, 0, 0));
				html = "<span style='background-color:#FFFFFF;'>";
				html += scalemin.str().c_str();
				html += "</span>";
				text->setHtml(html);
				text->setZValue(1001);
			}

		}

		qstr = "Number of Points Selected: " + QString::number(coordinateinfo.size());
		if (!filterstring1.empty() || !filterstring2.empty()) {
			qstr += " (";
		}
		if (!filterstring1.empty()) {
			qstr += "column: \"" + QString(columnname1.c_str()) + "\", comparator: \"" + QString(comparatorname1.c_str()) + "\", text: \"" + QString(filterstring1.c_str()) + "\"";
		}
		if (!filterstring1.empty() && !filterstring2.empty()) {
			if (operatortype == 0) {
				qstr += " or ";
			}
			else {
				qstr += " and ";
			}
		}
		if (!filterstring2.empty()) {
			qstr += "column: \"" + QString(columnname2.c_str()) + "\", comparator: \"" + QString(comparatorname2.c_str()) + "\", text: \"" + QString(filterstring2.c_str()) + "\"";
		}
		if (!filterstring1.empty() || !filterstring2.empty()) {
			qstr += ", case sensitive: " + QString((casesensitive) ? "yes" : "no") + ", whole words only: " + QString((wholeword) ? "yes" : "no") + ")";
		}
		qstr += "\n";
		qstr += "Minimum Bounding Region:\nX: " + QString::number(xmin) + "-" + QString::number(xmax) + "; Y: " + QString::number(ymin) + "-" + QString::number(ymax) + "\n";

		if (maximumintensity > 0) {

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

			if (absoluteintensity) {
				qstr += "Absolute ";
			}
			else {
				qstr += "Relative ";
			}
			qstr += "intensity:\nAverage: ";
			qstr += avgint.str().c_str();
			qstr += "; Median: ";
			qstr += medint.str().c_str();
			qstr += "; Minimum: ";
			qstr += scalemin.str().c_str();
			qstr += "; Maximum: ";
			qstr += scalemax.str().c_str();
			qstr += "\n";

		}

		text = scene->addText(qstr);
		text->setPos(2, currentheight + 2);

	}

	if (colorscale) {
		int rulerwidth = 1;

		QColor color;
		QPen pen;
		color.setRgb(0, 255, 0);
		pen.setColor(color);
		pen.setWidth(rulerwidth);

		QGraphicsLineItem* ruler = new QGraphicsLineItem(QLineF(mapToScene(250, 25), mapToScene(250 + rulersize, 25)));
		ruler->setPen(pen);
		ruler->setZValue(1001);
		scene->addItem(ruler);

		QGraphicsLineItem* rulerstart = new QGraphicsLineItem(QLineF(mapToScene(250, 25), mapToScene(250, 30)));
		rulerstart->setPen(pen);
		rulerstart->setZValue(1001);
		scene->addItem(rulerstart);

		QGraphicsLineItem* rulerend = new QGraphicsLineItem(QLineF(mapToScene(250 + rulersize, 25), mapToScene(250 + rulersize, 30)));
		rulerend->setPen(pen);
		rulerend->setZValue(1001);
		scene->addItem(rulerend);

		stringstream rulerstream;
		rulerstream << std::fixed << std::setprecision(3) << rulervalue;

		QString rulerhtml = "<span style='background-color:#FFFFFF;'>";
		rulerhtml += rulerstream.str().c_str();
		rulerhtml += " um</span>";

		QGraphicsTextItem *rulertext = scene->addText("");
		rulertext->setDefaultTextColor(QColor(0, 0, 0));
		rulertext->setHtml(rulerhtml);
		rulertext->setZValue(1001);
		rulertext->setPos(mapToScene(250 + rulersize / 2 - rulertext->boundingRect().width() / 2, 5));
	}

	setSceneRect(0, 0, currentwidth + 10, currentheight + 120);

	updateSelectionGroup();
}


void cImageWindowWidget::drawMicroscopyImage(int layer, QRectF& rect_scene, int currentwidth, int currentheight) {

	int micrometerswidth, micrometersheight;
	switch (vendor) {
		case unknownvendor:
			micrometerswidth = maxx*pixelsizex;
			micrometersheight = maxy*pixelsizey;
			break;
		case bruker:
			micrometerswidth = (maxx + 1)*pixelsizex;
			micrometersheight = (maxy + 1)*pixelsizey;
			break;
		case waters:
			micrometerswidth = maxx*pixelsizex;
			micrometersheight = maxy*pixelsizey;
			break;
		default:
			break;
	}

	QPixmap scaledmicroscopypixmap = *layersvector[layer].pixmap;

	int microscopyorigcenterx = scaledmicroscopypixmap.rect().center().x();
	int microscopyorigcentery = scaledmicroscopypixmap.rect().center().y();

	QTransform transform;
	transform.translate((qreal)microscopyorigcenterx, (qreal)microscopyorigcentery);
	transform.scale(layersvector[layer].flipvertically ? -1 : 1, layersvector[layer].fliphorizontally ? -1 : 1);
	transform.rotate(layersvector[layer].angle);
	transform.translate(-(qreal)microscopyorigcenterx, -(qreal)microscopyorigcentery);

	// to do - potential bug
	scaledmicroscopypixmap = scaledmicroscopypixmap.transformed(transform);

	int microscopynewcenterx = scaledmicroscopypixmap.rect().center().x();
	int microscopynewcentery = scaledmicroscopypixmap.rect().center().y();

	qreal centershiftx = microscopyorigcenterx - microscopynewcenterx;
	qreal centershifty = microscopyorigcentery - microscopynewcentery;

	int microscopynewwidth = scaledmicroscopypixmap.width();
	int microscopynewheight = scaledmicroscopypixmap.height();
	
	int scaledmicroscopywidth = ((double)layersvector[layer].width / (double)layersvector[layer].pixmap->width() * (double)microscopynewwidth) / (double)micrometerswidth * (double)currentwidth;
	int scaledmicroscopyheight = ((double)layersvector[layer].height / (double)layersvector[layer].pixmap->height() * (double)microscopynewheight) / (double)micrometersheight * (double)currentheight;
		
	int scaledmicroscopyx = layersvector[layer].x / (double)micrometerswidth * (double)currentwidth;
	int scaledmicroscopyy = layersvector[layer].y / (double)micrometersheight * (double)currentheight;

	qreal microscopywidthratio = (double)scaledmicroscopywidth / (double)microscopynewwidth;
	qreal microscopyheightratio = (double)scaledmicroscopyheight / (double)microscopynewheight;

	qreal scaledmicroscopycenterx = ((qreal)scaledmicroscopyx / microscopywidthratio + (qreal)microscopynewwidth / 2.0 + centershiftx) * microscopywidthratio;
	qreal scaledmicroscopycentery = ((qreal)scaledmicroscopyy / microscopyheightratio + (qreal)microscopynewheight / 2.0 + centershifty) * microscopyheightratio;

	if (layer == 3) {
		microscopynavigationcenterx = scaledmicroscopycenterx;
		microscopynavigationcentery = scaledmicroscopycentery;

		if (!layersvector[layer].checked) {
			return;
		}
	}

	QTransform transformcenter;
	transformcenter.translate((qreal)microscopynavigationcenterx, (qreal)microscopynavigationcentery);
	transformcenter.rotate(layersvector[layer].angle);
	transformcenter.translate(-(qreal)microscopynavigationcenterx, -(qreal)microscopynavigationcentery);

	QPointF oldcenterpoint(scaledmicroscopycenterx, scaledmicroscopycentery);
	QPointF newcenterpoint = transformcenter.map(oldcenterpoint);
	if (layersvector[layer].navigationlayer != 3) {
		newcenterpoint = oldcenterpoint;
	}
	
	centershiftx += (newcenterpoint.x() - scaledmicroscopycenterx) / microscopywidthratio;
	centershifty += (newcenterpoint.y() - scaledmicroscopycentery) / microscopyheightratio;

	if (((qreal)((qreal)scaledmicroscopyx / microscopywidthratio + microscopynewwidth + centershiftx) * microscopywidthratio > rect_scene.x()) && ((qreal)((qreal)scaledmicroscopyy / microscopyheightratio + microscopynewheight + centershifty) * microscopyheightratio > rect_scene.y())) {
		scaledmicroscopypixmap = scaledmicroscopypixmap.copy(max(rect_scene.x() / microscopywidthratio - (qreal)((qreal)scaledmicroscopyx / microscopywidthratio + centershiftx), 0.0), max(rect_scene.y() / microscopyheightratio - (qreal)((qreal)scaledmicroscopyy / microscopyheightratio + centershifty), 0.0), rect_scene.width() / microscopywidthratio + 1, rect_scene.height() / microscopyheightratio + 1);
		scaledmicroscopypixmap = scaledmicroscopypixmap.scaled(scaledmicroscopypixmap.width() * microscopywidthratio, scaledmicroscopypixmap.height() * microscopyheightratio);

		QPixmap alphapixmap(scaledmicroscopypixmap.size());
		alphapixmap.fill(Qt::transparent);

		QPainter painter(&alphapixmap);
		painter.setOpacity((double)layersvector[layer].alpha / 100.0);
		painter.drawPixmap(0, 0, scaledmicroscopypixmap);
		painter.end();

		QGraphicsPixmapItem* pixmapitem = scene->addPixmap(alphapixmap);
		pixmapitem->setPos(max(rect_scene.x(), (qreal)((qreal)scaledmicroscopyx / microscopywidthratio + centershiftx) * microscopywidthratio), max(rect_scene.y(), (qreal)((qreal)scaledmicroscopyy / microscopyheightratio + centershifty) * microscopyheightratio));
		pixmapitem->setZValue(layersvector[layer].zvalue);
	}

	layersvector[layer].lastx = (qreal)((qreal)scaledmicroscopyx / microscopywidthratio + centershiftx) * microscopywidthratio;
	layersvector[layer].lasty = (qreal)((qreal)scaledmicroscopyy / microscopyheightratio + centershifty) * microscopyheightratio;

}


void cImageWindowWidget::updateSelectionGroup() {
	qreal rx1, ry1, rx2, ry2;
	QFont myFont("Arial", 8);
	int xmin, xmax, ymin, ymax;
	double pointwidth, pointheight;

	double testedangle = layersvector[activelayer].angle;
	bool testedfliphorizontally = layersvector[activelayer].fliphorizontally;
	bool testedflipvertically = layersvector[activelayer].flipvertically;

	int micrometerswidth, micrometersheight;
	switch (vendor) {
		case unknownvendor:
			micrometerswidth = maxx*pixelsizex;
			micrometersheight = maxy*pixelsizey;
			break;
		case bruker:
			micrometerswidth = (maxx + 1)*pixelsizex;
			micrometersheight = (maxy + 1)*pixelsizey;
			break;
		case waters:
			micrometerswidth = maxx*pixelsizex;
			micrometersheight = maxy*pixelsizey;
			break;
		default:
			break;
	}

	QString qstr;

	if ((pressedx != -1) && (pressedy != -1)) {

		getRectanglePoints(rx1, ry1, rx2, ry2);

		if (activelayer == 0) {
			selectionrect->resetTransform();

			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
			selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

			if (vendor == bruker) {
				xmin = max(0, (int)rx1*(maxx + 1) / max(1, currentwidth) - leftshift);
				xmax = min(maxx, (int)rx2*(maxx + 1) / max(1, currentwidth) - leftshift);
				ymin = max(0, (int)ry1*(maxy + 1) / max(1, currentheight) - topshift);
				ymax = min(maxy, (int)ry2*(maxy + 1) / max(1, currentheight) - topshift);
			}
			else {
				xmin = max(0, (int)rx1*maxx / max(1, currentwidth) - leftshift) + 1;
				xmax = min(maxx, (int)rx2*maxx / max(1, currentwidth) - leftshift) + 1;
				ymin = max(0, (int)ry1*maxy / max(1, currentheight) - topshift) + 1;
				ymax = min(maxy, (int)ry2*maxy / max(1, currentheight) - topshift) + 1;
			}

			qstr = "X: " + QString::number(xmin) + "-" + QString::number(xmax) + "; Y: " + QString::number(ymin) + "-" + QString::number(ymax);
		}


		if (activelayer == 1) {
			selectionrect->resetTransform();

			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
			selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

			if (vendor == bruker) {
				pointwidth = max((double)abs(rx2 - rx1), 1.0) / (double)(defaultmaxx + 1);
				pointheight = max((double)abs(ry2 - ry1), 1.0) / (double)(defaultmaxy + 1);
			}
			else {
				pointwidth = max((double)abs(rx2 - rx1), 1.0) / (double)(max(defaultmaxx, 1));
				pointheight = max((double)abs(ry2 - ry1), 1.0) / (double)(max(defaultmaxy, 1));
			}

			xmax = (double)currentwidth / pointwidth;
			ymax = (double)currentheight / pointheight;

			xmin = (double)rx1 / pointwidth;
			ymin = (double)ry1 / pointheight;

			qstr = "Horizontal Shift: " + QString::number(xmin) + ", Vertical Shift: " + QString::number(ymin) + ", Max X: " + QString::number(xmax) + ", Max Y:" + QString::number(ymax);
		}


		if (activelayer == 2) {
			selectionrect->resetTransform();

			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
			selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

			QTransform transform;
			transform.translate(selectionrect->rect().center().x(), selectionrect->rect().center().y());
			transform.scale(testedflipvertically ? -1 : 1, testedfliphorizontally ? -1 : 1);
			transform.rotate(testedangle);
			transform.translate(-selectionrect->rect().center().x(), -selectionrect->rect().center().y());
			selectionrect->setTransform(transform);

			int x = (double)rx1 / currentscale;
			int y = (double)ry1 / currentscale;
			int width = abs(rx2 - rx1) / currentscale;
			int height = abs(ry2 - ry1) / currentscale;

			qstr = "X: " + QString::number(x) + ", Y: " + QString::number(y) + ", Width: " + QString::number(width) + ", Height: " + QString::number(height);
		}
		

		if (activelayer >= 3) {
			selectionrect->resetTransform();

			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
			selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

			QTransform transform;
			if (activelayer > 3) {
				QTransform transformcenter;
				transformcenter.translate((qreal)microscopynavigationcenterx, (qreal)microscopynavigationcentery);
				transformcenter.rotate(layersvector[activelayer].angle);
				transformcenter.translate(-(qreal)microscopynavigationcenterx, -(qreal)microscopynavigationcentery);

				QPointF newcenterpoint = transformcenter.map(selectionrect->rect().center());
				if (layersvector[activelayer].navigationlayer != 3) {
					newcenterpoint = selectionrect->rect().center();
				}

				transform.translate(newcenterpoint.x() - selectionrect->rect().center().x(), newcenterpoint.y() - selectionrect->rect().center().y());
			}
			transform.translate(selectionrect->rect().center().x(), selectionrect->rect().center().y());
			transform.scale(testedflipvertically ? -1 : 1, testedfliphorizontally ? -1 : 1);
			transform.rotate(testedangle);
			transform.translate(-selectionrect->rect().center().x(), -selectionrect->rect().center().y());
			selectionrect->setTransform(transform);

			double x = (double)rx1 * (double)micrometerswidth / (double)currentwidth;
			double y = (double)ry1 * (double)micrometersheight / (double)currentheight;
			double width = abs(rx2 - rx1) * (double)micrometerswidth / (double)currentwidth;
			double height = abs(ry2 - ry1) * (double)micrometersheight / (double)currentheight;

			qstr = "X: " + QString::number(x) + " um, Y: " + QString::number(y) + " um, Width: " + QString::number(width) + " um, Height: " + QString::number(height) + " um";
		}

		selectionsimpletextitem->setFont(myFont);
		selectionsimpletextitem->setText(qstr);
		selectionsimpletextitem->setBrush(QBrush(Qt::green));

		qreal simpletextposx = currentx + 15;
		qreal simpletextposy = currenty;
		selectionsimpletextitem->setPos(QPointF(simpletextposx, simpletextposy));

		selectionrect->setVisible(true);
		selectionsimpletextitem->setVisible(true);

	} 
	else {

		int transformedx = currentx;
		int transformedy = currenty;
		double tmpangle;

		if (activelayer >= 2) {
			QTransform transform;
			tmpangle = prepareTransformation(selectionrect->rect(), transform, testedfliphorizontally, testedflipvertically, testedangle, true);
			transform.map(currentx, currenty, &transformedx, &transformedy);
		}

		if (showselection) {
			bool rotatedimage = ((tmpangle >= 45) && (tmpangle <= 135)) || ((tmpangle <= -45) && (tmpangle >= -135));

			if ((abs(transformedx - selectionrect->rect().x()) < 5) && (abs(transformedy - selectionrect->rect().y()) < 5)) {
				setCursor(Qt::SizeFDiagCursor);
				if (activelayer >= 2) {
					if (rotatedimage) {
						setCursor(Qt::SizeBDiagCursor);
					}
				}
			}
			else if ((abs(transformedx - selectionrect->rect().x() - selectionrect->rect().width()) < 5) && (abs(transformedy - selectionrect->rect().y()) < 5)) {
				setCursor(Qt::SizeBDiagCursor);
				if (activelayer >= 2) {
					if (rotatedimage) {
						setCursor(Qt::SizeFDiagCursor);
					}
				}
			}
			else if ((abs(transformedx - selectionrect->rect().x()) < 5) && (abs(transformedy - selectionrect->rect().y() - selectionrect->rect().height()) < 5)) {
				setCursor(Qt::SizeBDiagCursor);
				if (activelayer >= 2) {
					if (rotatedimage) {
						setCursor(Qt::SizeFDiagCursor);
					}
				}
			}
			else if ((abs(transformedx - selectionrect->rect().x() - selectionrect->rect().width()) < 5) && (abs(transformedy - selectionrect->rect().y() - selectionrect->rect().height()) < 5)) {
				setCursor(Qt::SizeFDiagCursor);
				if (activelayer >= 2) {
					if (rotatedimage) {
						setCursor(Qt::SizeBDiagCursor);
					}
				}
			}
			else if ((abs(transformedx - selectionrect->rect().x()) < 5) && (transformedy >= selectionrect->rect().y()) && (transformedy <= selectionrect->rect().y() + selectionrect->rect().height())) {
				setCursor(Qt::SizeHorCursor);
				if (activelayer >= 2) {
					if (rotatedimage) {
						setCursor(Qt::SizeVerCursor);
					}
				}
			}
			else if ((abs(transformedx - selectionrect->rect().x() - selectionrect->rect().width()) < 5) && (transformedy >= selectionrect->rect().y()) && (transformedy <= selectionrect->rect().y() + selectionrect->rect().height())) {
				setCursor(Qt::SizeHorCursor);
				if (activelayer >= 2) {
					if (rotatedimage) {
						setCursor(Qt::SizeVerCursor);
					}
				}
			}
			else if ((abs(transformedy - selectionrect->rect().y()) < 5) && (transformedx >= selectionrect->rect().x()) && (transformedx <= selectionrect->rect().x() + selectionrect->rect().width())) {
				setCursor(Qt::SizeVerCursor);
				if (activelayer >= 2) {
					if (rotatedimage) {
						setCursor(Qt::SizeHorCursor);
					}
				}
			}
			else if ((abs(transformedy - selectionrect->rect().y() - selectionrect->rect().height()) < 5) && (transformedx >= selectionrect->rect().x()) && (transformedx <= selectionrect->rect().x() + selectionrect->rect().width())) {
				setCursor(Qt::SizeVerCursor);
				if (activelayer >= 2) {
					if (rotatedimage) {
						setCursor(Qt::SizeHorCursor);
					}
				}
			}
			else if ((transformedx > selectionrect->rect().x()) && (transformedx < selectionrect->rect().x() + selectionrect->rect().width())
				&& (transformedy > selectionrect->rect().y()) && (transformedy < selectionrect->rect().y() + selectionrect->rect().height())) {
				setCursor(Qt::SizeAllCursor);
			}
			else {
				setCursor(Qt::ArrowCursor);
			}
		}
		else {
			setCursor(Qt::ArrowCursor);
		}

		if (activelayer == 0) {

			if (vendor == bruker) {
				xmin = currentx*(maxx + 1) / max(1, currentwidth) - leftshift;
				ymin = currenty*(maxy + 1) / max(1, currentheight) - topshift;

				pointwidth = max((double)currentwidth / (double)(maxx + 1), (double)1);
				pointheight = max((double)currentheight / (double)(maxy + 1), (double)1);

				rx1 = (xfrom + leftshift) * pointwidth;
				ry1 = (yfrom + topshift) * pointheight;
				rx2 = (xto + leftshift + 1) * pointwidth;
				ry2 = (yto + topshift + 1) * pointheight;
			}
			else {
				xmin = currentx*maxx / max(1, currentwidth) - leftshift + 1;
				ymin = currenty*maxy / max(1, currentheight) - topshift + 1;

				pointwidth = max((double)currentwidth / (double)max(maxx, 1), (double)1);
				pointheight = max((double)currentheight / (double)max(maxy, 1), (double)1);

				rx1 = (max(xfrom, 1) + leftshift - 1) * pointwidth;
				ry1 = (max(yfrom, 1) + topshift - 1) * pointheight;
				rx2 = (xto + leftshift) * pointwidth;
				ry2 = (yto + topshift) * pointheight;
			}

			rx1 = max(0.0, rx1);
			ry1 = max(0.0, ry1);
			rx2 = min(rx2, (qreal)currentwidth);
			ry2 = min(ry2, (qreal)currentheight);

			selectionrect->resetTransform();

			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
			selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

			currentrect->setRect(selectionrect->rect());
			currentrect->setTransform(selectionrect->transform());
			
			selectionrect->setVisible(showselection);

			if ((currentx > 0) && (currenty > 0) && (currentx < currentwidth) && (currenty < currentheight)) {
				qstr = "X: " + QString::number(xmin) + ", Y: " + QString::number(ymin);
				selectionsimpletextitem->setBrush(QBrush(Qt::green));
				selectionsimpletextitem->setFont(myFont);
				selectionsimpletextitem->setText(qstr);
				selectionsimpletextitem->setPos(QPointF(currentx + 15, currenty));
				selectionsimpletextitem->setVisible(true);
			}
			else {
				selectionsimpletextitem->setVisible(false);
			}

		}


		if (activelayer == 1) {

			if (vendor == bruker) {
				rx1 = (double)leftshift * (double)currentwidth / (double)(maxx + 1);
				ry1 = (double)topshift *  (double)currentheight / (double)(maxy + 1);

				rx2 = (double)currentwidth / (double)(maxx + 1) * (double)(leftshift + defaultmaxx + 2);
				ry2 = (double)currentheight / (double)(maxy + 1) * (double)(topshift + defaultmaxy + 2);
			}
			else {
				rx1 = (double)leftshift * (double)currentwidth / (double)max(maxx, 1);
				ry1 = (double)topshift * (double)currentheight / (double)max(maxy, 1);

				rx2 = (double)currentwidth / (double)max(maxx, 1) * (double)(leftshift + defaultmaxx + 1);
				ry2 = (double)currentheight / (double)max(maxy, 1) * (double)(topshift + defaultmaxy + 1);
			}

			rx2 = min(rx2, (qreal)currentwidth);
			ry2 = min(ry2, (qreal)currentheight);

			selectionrect->resetTransform();

			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
			selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

			currentrect->setRect(selectionrect->rect());
			currentrect->setTransform(selectionrect->transform());

			selectionrect->setVisible(showselection);

			if (vendor == bruker) {
				pointwidth = 1.0 / (double)(defaultmaxx + 1);
				pointheight = 1.0 / (double)(defaultmaxy + 1);
			}
			else {
				pointwidth = 1.0 / (double)(max(defaultmaxx, 1));
				pointheight = 1.0 / (double)(max(defaultmaxy, 1));
			}

			xmin = (double)currentx / pointwidth;
			ymin = (double)currenty / pointheight;

			if ((currentx > 0) && (currenty > 0) && (currentx < currentwidth) && (currenty < currentheight)) {
				qstr = "Horizontal Shift: " + QString::number(xmin) + ", Vertical Shift: " + QString::number(ymin);
				selectionsimpletextitem->setBrush(QBrush(Qt::green));
				selectionsimpletextitem->setFont(myFont);
				selectionsimpletextitem->setText(qstr);
				selectionsimpletextitem->setPos(QPointF(currentx + 15, currenty));
				selectionsimpletextitem->setVisible(true);
			}
			else {
				selectionsimpletextitem->setVisible(false);
			}

		}


		if (activelayer == 2) {

			int x = (double)currentx / currentscale;
			int y = (double)currenty / currentscale;

			rx1 = layersvector[2].x * currentscale;
			ry1 = layersvector[2].y * currentscale;
			rx2 = (layersvector[2].x + layersvector[2].width) * currentscale;
			ry2 = (layersvector[2].y + layersvector[2].height) * currentscale;

			//rx2 = min(rx2, (qreal)currentwidth);
			//ry2 = min(ry2, (qreal)currentheight);

			selectionrect->resetTransform();

			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
			selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

			QTransform transform;
			transform.translate(selectionrect->rect().center().x(), selectionrect->rect().center().y());
			transform.scale(testedflipvertically ? -1 : 1, testedfliphorizontally ? -1 : 1);
			transform.rotate(testedangle);
			transform.translate(-selectionrect->rect().center().x(), -selectionrect->rect().center().y());
			selectionrect->setTransform(transform);

			currentrect->setRect(selectionrect->rect());
			currentrect->setTransform(selectionrect->transform());

			selectionrect->setVisible(showselection);

			if ((currentx > 0) && (currenty > 0) && (currentx < currentwidth) && (currenty < currentheight)) {
				qstr = "X: " + QString::number(x) + ", Y: " + QString::number(y);
				selectionsimpletextitem->setBrush(QBrush(Qt::green));
				selectionsimpletextitem->setFont(myFont);
				selectionsimpletextitem->setText(qstr);
				selectionsimpletextitem->setPos(QPointF(currentx + 15, currenty));
				selectionsimpletextitem->setVisible(true);
			}
			else {
				selectionsimpletextitem->setVisible(false);
			}

		}


		if (activelayer >= 3) {

			double x = (double)currentx * (double)micrometerswidth / (double)currentwidth;
			double y = (double)currenty * (double)micrometersheight / (double)currentheight;

			rx1 = layersvector[activelayer].x / (double)micrometerswidth * (double)currentwidth;
			ry1 = layersvector[activelayer].y / (double)micrometersheight * (double)currentheight;
			rx2 = (layersvector[activelayer].x + layersvector[activelayer].width) / (double)micrometerswidth * (double)currentwidth;
			ry2 = (layersvector[activelayer].y + layersvector[activelayer].height) / (double)micrometersheight * (double)currentheight;

			selectionrect->resetTransform();

			selectionrect->setPen(QPen(Qt::green, 1, Qt::DashLine));
			selectionrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

			QTransform transform;
			if (activelayer > 3) {
				QTransform transformcenter;
				transformcenter.translate((qreal)microscopynavigationcenterx, (qreal)microscopynavigationcentery);
				transformcenter.rotate(layersvector[activelayer].angle);
				transformcenter.translate(-(qreal)microscopynavigationcenterx, -(qreal)microscopynavigationcentery);

				QPointF newcenterpoint = transformcenter.map(selectionrect->rect().center());
				if (layersvector[activelayer].navigationlayer != 3) {
					newcenterpoint = selectionrect->rect().center();
				}

				transform.translate(newcenterpoint.x() - selectionrect->rect().center().x(), newcenterpoint.y() - selectionrect->rect().center().y());
			}
			transform.translate(selectionrect->rect().center().x(), selectionrect->rect().center().y());
			transform.scale(testedflipvertically ? -1 : 1, testedfliphorizontally ? -1 : 1);
			transform.rotate(testedangle);
			transform.translate(-selectionrect->rect().center().x(), -selectionrect->rect().center().y());
			selectionrect->setTransform(transform);

			currentrect->setRect(selectionrect->rect());
			currentrect->setTransform(selectionrect->transform());

			selectionrect->setVisible(showselection);

			if ((currentx > 0) && (currenty > 0) && (currentx < currentwidth) && (currenty < currentheight)) {
				qstr = "X: " + QString::number(x) + " um, Y: " + QString::number(y) + " um";
				selectionsimpletextitem->setBrush(QBrush(Qt::green));
				selectionsimpletextitem->setFont(myFont);
				selectionsimpletextitem->setText(qstr);
				selectionsimpletextitem->setPos(QPointF(currentx + 15, currenty));
				selectionsimpletextitem->setVisible(true);
			}
			else {
				selectionsimpletextitem->setVisible(false);
			}

		}

	
	}
}


void cImageWindowWidget::getRectanglePoints(qreal& rx1, qreal& ry1, qreal& rx2, qreal& ry2) {
	qreal tmp;

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

	qreal untransformedcurrentx = currentx;
	qreal untransformedcurrenty = currenty;
	qreal untransformedpressedx = pressedx;
	qreal untransformedpressedy = pressedy;

	qreal tmprx1 = currentrect->rect().x();
	qreal tmpry1 = currentrect->rect().y();
	qreal tmprx2 = currentrect->rect().x() + currentrect->rect().width();
	qreal tmpry2 = currentrect->rect().y() + currentrect->rect().height();

	double testedangle = layersvector[activelayer].angle;
	bool testedfliphorizontally = layersvector[activelayer].fliphorizontally;
	bool testedflipvertically = layersvector[activelayer].flipvertically;

	if (activelayer >= 2) {
		QTransform transform;
		prepareTransformation(currentrect->rect(), transform, testedfliphorizontally, testedflipvertically, testedangle, true);
		transform.map((qreal)currentx, (qreal)currenty, &untransformedcurrentx, &untransformedcurrenty);
		transform.map((qreal)pressedx, (qreal)pressedy, &untransformedpressedx, &untransformedpressedy);
	}

	switch (cursoractivity) {
		case cursoractivity_none:
			if (untransformedcurrentx < untransformedpressedx) {
				rx1 = untransformedcurrentx;
				rx2 = untransformedpressedx;
			}
			else {
				rx1 = untransformedpressedx;
				rx2 = untransformedcurrentx;
			}
			if (untransformedcurrenty < untransformedpressedy) {
				ry1 = untransformedcurrenty;
				ry2 = untransformedpressedy;
			}
			else {
				ry1 = untransformedpressedy;
				ry2 = untransformedcurrenty;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		case cursoractivity_move:
			if (activelayer > 3) {
				if (((testedangle >= 90.0) && (testedangle <= 270.0)) || ((testedangle <= -90.0) && (testedangle >= -270.0))) {
					rx1 = tmprx1 - untransformedcurrentx + untransformedpressedx;
					ry1 = tmpry1 - untransformedcurrenty + untransformedpressedy;
					rx2 = tmprx2 - untransformedcurrentx + untransformedpressedx;
					ry2 = tmpry2 - untransformedcurrenty + untransformedpressedy;
				}
				else {
					rx1 = tmprx1 + untransformedcurrentx - untransformedpressedx;
					ry1 = tmpry1 + untransformedcurrenty - untransformedpressedy;
					rx2 = tmprx2 + untransformedcurrentx - untransformedpressedx;
					ry2 = tmpry2 + untransformedcurrenty - untransformedpressedy;
				}
			}
			else {
				rx1 = tmprx1 + (qreal)currentx - (qreal)pressedx;
				ry1 = tmpry1 + (qreal)currenty - (qreal)pressedy;
				rx2 = tmprx2 + (qreal)currentx - (qreal)pressedx;
				ry2 = tmpry2 + (qreal)currenty - (qreal)pressedy;
			}
			if (rx1 < 0) {
				rx2 = rx2 - rx1;
				rx1 = 0;
			}
			if (ry1 < 0) {
				ry2 = ry2 - ry1;
				ry1 = 0;
			}
			if (rx2 > (qreal)currentwidth) {
				rx1 = max(0.0, rx1 - rx2 + (qreal)currentwidth);
				rx2 = currentwidth;
			}
			if (ry2 > (qreal)currentheight) {
				ry1 = max(0.0, ry1 - ry2 + (qreal)currentheight);
				ry2 = currentheight;
			}
			break;
		case cursoractivity_resize_left:
			rx1 = untransformedcurrentx;
			ry1 = tmpry1;
			rx2 = tmprx2;
			ry2 = tmpry2;

			if (keepaspectratio && (activelayer >= 2) && (tmprx1 != tmprx2)) {
				ry2 = tmpry2 - (untransformedcurrentx - tmprx1) * (tmpry2 - tmpry1) / (tmprx2 - tmprx1);

				if (ry2 < ry1) {
					tmp = ry1;
					ry1 = ry2;
					ry2 = tmp;
				}
			}
			
			if (rx2 < rx1) {
				tmp = rx1;
				rx1 = rx2;
				rx2 = tmp;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		case cursoractivity_resize_right:
			rx1 = tmprx1;
			ry1 = tmpry1;
			rx2 = untransformedcurrentx;
			ry2 = tmpry2;

			if (keepaspectratio && (activelayer >= 2) && (tmprx1 != tmprx2)) {
				ry2 = tmpry2 + (untransformedcurrentx - tmprx2) * (tmpry2 - tmpry1) / (tmprx2 - tmprx1);

				if (ry2 < ry1) {
					tmp = ry1;
					ry1 = ry2;
					ry2 = tmp;
				}
			}

			if (rx2 < rx1) {
				tmp = rx1;
				rx1 = rx2;
				rx2 = tmp;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		case cursoractivity_resize_top:
			rx1 = tmprx1;
			ry1 = untransformedcurrenty;
			rx2 = tmprx2;
			ry2 = tmpry2;

			if (keepaspectratio && (activelayer >= 2) && (tmpry1 != tmpry2)) {
				rx2 = tmprx2 - (untransformedcurrenty - tmpry1) * (tmprx2 - tmprx1) / (tmpry2 - tmpry1);

				if (rx2 < rx1) {
					tmp = rx1;
					rx1 = rx2;
					rx2 = tmp;
				}
			}

			if (ry2 < ry1) {
				tmp = ry1;
				ry1 = ry2;
				ry2 = tmp;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		case cursoractivity_resize_bottom:
			rx1 = tmprx1;
			ry1 = tmpry1;
			rx2 = tmprx2;
			ry2 = untransformedcurrenty;

			if (keepaspectratio && (activelayer >= 2) && (tmpry1 != tmpry2)) {
				rx2 = tmprx2 + (untransformedcurrenty - tmpry2) * (tmprx2 - tmprx1) / (tmpry2 - tmpry1);

				if (rx2 < rx1) {
					tmp = rx1;
					rx1 = rx2;
					rx2 = tmp;
				}
			}

			if (ry2 < ry1) {
				tmp = ry1;
				ry1 = ry2;
				ry2 = tmp;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		case cursoractivity_resize_top_left:
			rx1 = untransformedcurrentx;
			ry1 = untransformedcurrenty;
			rx2 = tmprx2;
			ry2 = tmpry2;

			if (keepaspectratio && (activelayer >= 2) && (tmprx1 != tmprx2)) {
				ry1 = tmpry1 + (untransformedcurrentx - tmprx1) * (tmpry2 - tmpry1) / (tmprx2 - tmprx1);
			}

			if (rx2 < rx1) {
				tmp = rx1;
				rx1 = rx2;
				rx2 = tmp;
			}

			if (ry2 < ry1) {
				tmp = ry1;
				ry1 = ry2;
				ry2 = tmp;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		case cursoractivity_resize_top_right:
			rx1 = tmprx1;
			ry1 = untransformedcurrenty;
			rx2 = untransformedcurrentx;
			ry2 = tmpry2;

			if (keepaspectratio && (activelayer >= 2) && (tmprx1 != tmprx2)) {
				ry1 = tmpry1 - (untransformedcurrentx - tmprx2) * (tmpry2 - tmpry1) / (tmprx2 - tmprx1);
			}

			if (rx2 < rx1) {
				tmp = rx1;
				rx1 = rx2;
				rx2 = tmp;
			}

			if (ry2 < ry1) {
				tmp = ry1;
				ry1 = ry2;
				ry2 = tmp;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		case cursoractivity_resize_bottom_left:
			rx1 = untransformedcurrentx;
			ry1 = tmpry1;
			rx2 = tmprx2;
			ry2 = untransformedcurrenty;

			if (keepaspectratio && (activelayer >= 2) && (tmprx1 != tmprx2)) {
				ry2 = tmpry2 - (untransformedcurrentx - tmprx1) * (tmpry2 - tmpry1) / (tmprx2 - tmprx1);
			}

			if (rx2 < rx1) {
				tmp = rx1;
				rx1 = rx2;
				rx2 = tmp;
			}

			if (ry2 < ry1) {
				tmp = ry1;
				ry1 = ry2;
				ry2 = tmp;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		case cursoractivity_resize_bottom_right:
			rx1 = tmprx1;
			ry1 = tmpry1;
			rx2 = untransformedcurrentx;
			ry2 = untransformedcurrenty;

			if (keepaspectratio && (activelayer >= 2) && (tmprx1 != tmprx2)) {
				ry2 = tmpry2 + (untransformedcurrentx - tmprx2) * (tmpry2 - tmpry1) / (tmprx2 - tmprx1);
			}

			if (rx2 < rx1) {
				tmp = rx1;
				rx1 = rx2;
				rx2 = tmp;
			}

			if (ry2 < ry1) {
				tmp = ry1;
				ry1 = ry2;
				ry2 = tmp;
			}

			if (activelayer >= 2) {
				fixRectCenter(rx1, ry1, rx2, ry2, testedfliphorizontally, testedflipvertically, testedangle);
			}
			break;
		default:
			break;
	}
}


double cImageWindowWidget::prepareTransformation(QRectF rect, QTransform& transform, bool testedfliphorizontally, bool testedflipvertically, double testedangle, bool flipaxes) {
	double tmpangle = testedangle;
	if (tmpangle < 0) {
		tmpangle += 360.0;
	}
	if ((testedflipvertically && !testedfliphorizontally) || (!testedflipvertically && testedfliphorizontally)) {
		tmpangle = 180.0 - tmpangle;
	}
	if (tmpangle < 0) {
		tmpangle += 360.0;
	}
	if ((testedflipvertically && !testedfliphorizontally) || (!testedflipvertically && testedfliphorizontally)) {
		if ((tmpangle > 90.0) && (tmpangle < 270.0)) {
			tmpangle -= 180.0;
		}
	}
	else {
		if ((tmpangle >= 90.0) && (tmpangle <= 270.0)) {
			tmpangle -= 180.0;
		}
	}

	transform.reset();

	if (activelayer > 3) {
		QTransform transformcenter;
		transformcenter.translate((qreal)microscopynavigationcenterx, (qreal)microscopynavigationcentery);
		transformcenter.rotate(-testedangle);
		transformcenter.translate(-(qreal)microscopynavigationcenterx, -(qreal)microscopynavigationcentery);

		QPointF newcenterpoint = transformcenter.map(rect.center());
		if (layersvector[activelayer].navigationlayer != 3) {
			newcenterpoint = rect.center();
		}

		if (flipaxes && (((testedangle >= 90.0) && (testedangle <= 270.0)) || ((testedangle <= -90.0) && (testedangle >= -270.0)))) {
			transform.translate(rect.center().x() - newcenterpoint.x(), rect.center().y() - newcenterpoint.y());
		}
		else {
			transform.translate(newcenterpoint.x() - rect.center().x(), newcenterpoint.y() - rect.center().y());
		}
	}

	transform.translate(rect.center().x(), rect.center().y());
	transform.rotate(-tmpangle);
	transform.translate(-rect.center().x(), -rect.center().y());

	return tmpangle;
}


void cImageWindowWidget::fixRectCenter(qreal& rx1, qreal& ry1, qreal& rx2, qreal& ry2, bool testedfliphorizontally, bool testedflipvertically, double testedangle) {
	qreal x1, y1, x2, y2;
	QTransform transform;

	prepareTransformation(currentrect->rect(), transform, testedfliphorizontally, testedflipvertically, testedangle, true);
	transform.inverted().map(rx1, ry1, &x1, &y1);
	transform.inverted().map(rx2, ry2, &x2, &y2);

	QGraphicsLineItem gline;
	gline.setLine(x1, y1, x2, y2);

	prepareTransformation(gline.boundingRect(), transform, testedfliphorizontally, testedflipvertically, testedangle, false);
	
	transform.map(x1, y1, &rx1, &ry1);
	transform.map(x2, y2, &rx2, &ry2);
}


double cImageWindowWidget::recalculateRulerValue(qreal currentscale) {
	double value = 0;

	switch (vendor) {
		case unknownvendor:
			value = ((double)maxx*(double)pixelsizex) * (double)rulersize / ((double)layersvector[1].pixmap->width()*currentscale);
			break;
		case bruker:
			value = ((double)(maxx + 1)*(double)pixelsizex) * (double)rulersize / ((double)layersvector[1].pixmap->width()*currentscale);
			break;
		case waters:
			value = ((double)maxx*(double)pixelsizex) * (double)rulersize / ((double)layersvector[1].pixmap->width()*currentscale);
			break;
		default:
			break;
	}

	return value;
}


void cImageWindowWidget::zoomIn() {
	emit updateRuler(rulervalue / factor);
}


void cImageWindowWidget::zoomOut() {
	emit updateRuler(rulervalue * factor);
}


void cImageWindowWidget::normalSize() {
	emit updateRuler(recalculateRulerValue(1.0));
}


void cImageWindowWidget::setRulerValue(double value) {
	qreal oldscale = currentscale;

	rulervalue = value;
	switch (vendor) {
		case unknownvendor:
			currentscale = ((double)maxx*(double)pixelsizex) * (double)rulersize / ((double)layersvector[1].pixmap->width()*rulervalue);
			break;
		case bruker:
			currentscale = ((double)(maxx + 1)*(double)pixelsizex) * (double)rulersize / ((double)layersvector[1].pixmap->width()*rulervalue);
			break;
		case waters:
			currentscale = ((double)maxx*(double)pixelsizex) * (double)rulersize / ((double)layersvector[1].pixmap->width()*rulervalue);
			break;
		default:
		break;
	}

	int horizontalshift = horizontalScrollBar()->value() / oldscale * currentscale;
	int verticalshift = verticalScrollBar()->value() / oldscale * currentscale;

	if ((horizontalshift != horizontalScrollBar()->value()) || (verticalshift != verticalScrollBar()->value())) {
		horizontalScrollBar()->setValue(horizontalshift);
		if (horizontalshift != horizontalScrollBar()->value()) {
			// if horizontalshift > maximum value, setValue was ignored
			redrawScene();
		}
		verticalScrollBar()->setValue(verticalshift);
		if (verticalshift != verticalScrollBar()->value()) {
			// if verticalshift > maximum value, setValue was ignored
			redrawScene();
		}
	}
	else {
		redrawScene();
	}
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


void cImageWindowWidget::changeLayer(int layerid, bool checked, int alpha, int zvalue, bool redraw) {
	if (layerid == (int)layersvector.size()) {
		layerInfo layer;
		layer.checked = checked;
		layer.alpha = alpha;
		layer.zvalue = zvalue;
		layersvector.push_back(layer);
	}

	if (layerid < (int)layersvector.size()) {
		layersvector[layerid].checked = checked;
		layersvector[layerid].alpha = alpha;
		layersvector[layerid].zvalue = zvalue;
	}

	if (redraw) {
		redrawScene();
	}
}


void cImageWindowWidget::changeActiveLayer(int layerid) {
	activelayer = layerid;
	redrawScene();
}


void cImageWindowWidget::flipHistologyHorizontallyStateChanged(bool state) {
	layersvector[2].fliphorizontally = state;
	redrawScene();
}


void cImageWindowWidget::flipHistologyVerticallyStateChanged(bool state) {
	layersvector[2].flipvertically = state;
	redrawScene();
}


void cImageWindowWidget::showSelectionStateChanged(bool state) {
	showselection = state;
	redrawScene();
}

