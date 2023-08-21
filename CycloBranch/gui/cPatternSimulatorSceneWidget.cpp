#include "gui/cPatternSimulatorSceneWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QPrinter>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QMatrix>
#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QSvgGenerator>
#include <QImage>


double computeGaussFunction(double x, double a, double ny, double sigma) {
	if (sigma == 0) {
		return 0;
	}

	double value = x - ny;
	value *= value;
	value /= 2.0 * sigma * sigma;
	value = -value;
	value = pow(econst, value);
	return a * value;
}


double computeGaussHeight(double x, double y, double ny, double sigma) {
	if (sigma == 0) {
		return 0;
	}

	double value = x - ny;
	value *= value;
	value /= 2.0 * sigma * sigma;
	value = -value;
	value = pow(econst, value);

	if (value != 0) {
		return y / value;
	}

	return 0;
}


cPatternSimulatorSceneWidget::cPatternSimulatorSceneWidget(QWidget* parent) {
	this->parent = parent;
	isotopicpattern.clear();
	origwidth = 0;
	origheight = 0;
	calledbyresizeevent = false;
	oldwidth.clear();
	oldheight.clear();
	currentscale = 1;
	absoluteintensity = false;
	rawdatastate = true;
	hidelabels = true;
	factor = 0.2;
	pressedx = -1;
	pressedy = -1;
	currentx = 0;
	currenty = 0;
	minmzratio = 0;
	maxmzratio = 0;

	topmargin = 0;
	bottommargin = 0;
	leftmargin = 0;
	rightmargin = 0;

	firstshow = true;
	enablemousemzselection = true;

	mzoverhead = 1.0;
	fwhm = 0.05;

	scene = new QGraphicsScene(this);

	zoomgroup = new QGraphicsItemGroup();
	zoomrect = new QGraphicsRectItem();
	zoomgroup->addToGroup(zoomrect);
	zoomsimpletextitem = new QGraphicsSimpleTextItem();
	zoomgroup->addToGroup(zoomsimpletextitem);
	scene->addItem(zoomgroup);
	
	cursorsimpletextitem = new QGraphicsSimpleTextItem();
	scene->addItem(cursorsimpletextitem);
}


cPatternSimulatorSceneWidget::~cPatternSimulatorSceneWidget() {
	delete scene;
}


void cPatternSimulatorSceneWidget::initialize(cPeaksList isotopicpattern, double fwhm) {
	this->isotopicpattern = isotopicpattern;
	this->fwhm = fwhm;

	minmzratio = max(isotopicpattern.getMinimumMZRatio() - mzoverhead, 0.0);
	maxmzratio = isotopicpattern.getMaximumMZRatio() + mzoverhead;
	emit updateMZInterval(minmzratio, maxmzratio);

	setScene(scene);

	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	originalmatrix = matrix();

	redrawScene();
}


void cPatternSimulatorSceneWidget::exportToPDF(QString filename, bool postscript) {
	QPrinter printer;
	if (postscript) {
		printer.setPaperSize(QSizeF(scene->width() + 100, scene->height() + 100), QPrinter::DevicePixel);
		printer.setPageMargins (50, 50, 50, 50, QPrinter::DevicePixel);
	}
	else {
		printer.setPaperSize(QSizeF(scene->width(), scene->height()), QPrinter::DevicePixel);
		printer.setPageMargins (0, 0, 0, 0, QPrinter::DevicePixel);
	}
	printer.setOutputFormat(QPrinter::NativeFormat);
	printer.setOutputFileName(filename);

	QPainter painter;
	if (!painter.begin(&printer)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		#if OS_TYPE == UNX
			errstr += "\nDo you have a properly configured print server (sudo apt-get install cups-pdf) ?";
		#endif
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
	scene->render(&painter);
	painter.end();
}


void cPatternSimulatorSceneWidget::exportToSVG(QString filename) {
	QSvgGenerator generator;
	generator.setFileName(filename);
	generator.setSize(QSize(scene->width(), scene->height()));
	generator.setViewBox(QRect(0, 0, scene->width(), scene->height()));

	QPainter painter;
	if (!painter.begin(&generator)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.fillRect(QRect(0, 0, scene->width(), scene->height()), Qt::white);
	scene->render(&painter);
	painter.end();
}


void cPatternSimulatorSceneWidget::exportToPNG(QString filename) {
	QImage image(scene->width(), scene->height(), QImage::Format_ARGB32);
	image.fill(Qt::white);

	QPainter painter;
	if (!painter.begin(&image)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	scene->render(&painter);
	painter.end();
	image.save(filename);
}


void cPatternSimulatorSceneWidget::hidePeakLabels(bool state) {
	hidelabels = state;
	redrawScene();
}


void cPatternSimulatorSceneWidget::wheelEvent(QWheelEvent *event) {
	double part, newmin, newmax;

	if (event->delta() > 0) {
		part = fabs(maxmzratio - minmzratio) / 10.0;
		newmin = minmzratio + part;
		newmax = maxmzratio - part;
	}
	else {
		part = fabs(maxmzratio - minmzratio) / 8.0;
		newmin = max(minmzratio - part, 0.0);
		newmax = min(maxmzratio + part, isotopicpattern.getMaximumMZRatio() + mzoverhead);
	}

	if (newmin < newmax) {
		minmzratio = newmin;
		maxmzratio = newmax;
		emit updateMZInterval(minmzratio, maxmzratio);
		redrawScene();
		viewport()->update();
	}
	
	event->accept();
}


void cPatternSimulatorSceneWidget::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);

	QPointF p = mapToScene(event->x(), event->y());

	QPointF curpos;
	curpos.setX(p.x() + 15);
	curpos.setY(p.y() - 2);

	double mz = getMZRatioFromXPosition((int)p.x(), origwidth);
	double intensity = getIntensityFromYPosition((int)p.y(), origheight);
	QString curtext = "m/z: " + QString::number(mz) + ", int: " + QString::number(intensity);
	if (!absoluteintensity) {
		curtext += " %";
	}

	cursorsimpletextitem->setPos(curpos);
	cursorsimpletextitem->setText(curtext);

	if ((pressedx != -1) && (pressedy != -1)) {
		cursorsimpletextitem->setVisible(false);

		currentx = (int)p.x();
		currenty = (int)p.y();

		if (enablemousemzselection) {
			updateZoomGroup();
		}
		else {
			calculateMinMaxMZ();

			emit updateMZInterval(minmzratio, maxmzratio);

			pressedx = currentx;
			pressedy = currenty;

			redrawScene();
		}
	}
	else {
		if (((int)p.x() >= leftmargin) && ((int)p.x() <= origwidth - rightmargin) && ((int)p.y() >= topmargin) && ((int)p.y() <= origheight - bottommargin)) {
			cursorsimpletextitem->setVisible(true);
		}
		else {
			cursorsimpletextitem->setVisible(false);
		}
	}

	event->accept();
}


void cPatternSimulatorSceneWidget::mouseReleaseEvent(QMouseEvent *event) {
	QGraphicsView::mouseReleaseEvent(event);

	if (pressedx == currentx) {
		pressedx = -1;
		currentx = -1;

		redrawScene();
	}
    
	if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {
		calculateMinMaxMZ();

		emit updateMZInterval(minmzratio, maxmzratio);

		pressedx = -1;
		pressedy = -1;

		redrawScene();
	}

	QPointF p = mapToScene(event->x(), event->y());

	QPointF curpos;
	curpos.setX(p.x() + 15);
	curpos.setY(p.y() - 2);

	double mz = getMZRatioFromXPosition((int)p.x(), origwidth);
	double intensity = getIntensityFromYPosition((int)p.y(), origheight);
	QString curtext = "m/z: " + QString::number(mz) + ", int: " + QString::number(intensity);
	if (!absoluteintensity) {
		curtext += " %";
	}

	cursorsimpletextitem->setPos(curpos);
	cursorsimpletextitem->setText(curtext);

	if (((int)p.x() >= leftmargin) && ((int)p.x() <= origwidth - rightmargin) && ((int)p.y() >= topmargin) && ((int)p.y() <= origheight - bottommargin)) {
		cursorsimpletextitem->setVisible(true);
	}
	else {
		cursorsimpletextitem->setVisible(false);
	}

	event->accept();
}


void cPatternSimulatorSceneWidget::mousePressEvent(QMouseEvent *event) {
	QGraphicsView::mousePressEvent(event);

	cursorsimpletextitem->setVisible(false);

	if (event->button() == Qt::LeftButton) {
		QPointF p = mapToScene(event->x(), event->y());
		pressedx = (int)p.x();
		pressedy = (int)p.y();

		currentx = pressedx;
		currenty = pressedy;

		if (enablemousemzselection) {
			updateZoomGroup();
		}
	}

	if (event->button() == Qt::RightButton) {
		pressedx = -1;
		pressedy = -1;

		redrawScene();
	}

	//if (enablemousemzselection) {
		if (event->button() == Qt::MiddleButton) {
			pressedx = -1;
			pressedy = -1;

			resetMZInterval();

			redrawScene();
		}
	//}
	
	event->accept();
}


void cPatternSimulatorSceneWidget::showEvent(QShowEvent *event) {
	QGraphicsView::showEvent(event);

	if (firstshow) {
		normalSize();
		firstshow = false;
	}
}


void cPatternSimulatorSceneWidget::resizeEvent(QResizeEvent *event) {
	QGraphicsView::resizeEvent(event);
	
	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	calledbyresizeevent = true;

	redrawScene();	
}


double cPatternSimulatorSceneWidget::getMZRatioFromXPosition(int x, int w) {
	double mz = (double)(x - leftmargin) / (double)(w - leftmargin - rightmargin) * (maxmzratio - minmzratio) + minmzratio;
	return max(0.0, mz);
}


int cPatternSimulatorSceneWidget::getXPositionFromMZRatio(double mzratio, int w) {
	double val = mzratio - minmzratio;
	val /= maxmzratio - minmzratio;
	val *= double(w - leftmargin - rightmargin);
	return (int)val + leftmargin;
}


double cPatternSimulatorSceneWidget::getIntensityFromYPosition(int y, int h) {
	double maximumintensity = getMaximumIntensity();
	double intensity = (double)(y - topmargin) / (double)(h - topmargin - bottommargin) * maximumintensity;
	intensity = maximumintensity - intensity;
	return max(0.0, intensity);
}


int cPatternSimulatorSceneWidget::getYPositionFromIntensity(double intensity, int h) {
	double maximumintensity = getMaximumIntensity();
	double val = maximumintensity - intensity;
	val /= maximumintensity;
	val *= double(h - topmargin - bottommargin);
	return (int)val + topmargin;
}


void cPatternSimulatorSceneWidget::redrawScene() {
	QGraphicsSimpleTextItem* simpletext;
	QGraphicsTextItem* text;
	QGraphicsLineItem* line;

	int x;
	double y;
	qreal tx, ty, tw, th, sumh;

	char tmpbuf[500];
	string s;

	int xstep, ystep;
	int w = origwidth;
	int h = origheight;
	int rulergranularity = 10;

	cPeaksList visiblepeaks;
	visiblepeaks.clear();

	QList<QGraphicsItem *> hiddenitems;

	vector<string> hits;
	size_t pos1, pos2;
	string tmpsubstr;

	QFont myFont("Arial", 8);
	QFontMetrics fm(myFont);
		
	double maxintensity = getMaximumIntensity();

	scene->removeItem(zoomgroup);
	scene->removeItem(cursorsimpletextitem);

	scene->clear();

	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);

	cursorsimpletextitem->setVisible(false);
	scene->addItem(cursorsimpletextitem);


	// x axis
	line = scene->addLine(leftmargin, h - bottommargin, w - rightmargin, h - bottommargin, QPen(Qt::black, 2, Qt::SolidLine));
	line->setZValue(1);

	// x axis ruler
	xstep = (w - leftmargin - rightmargin) / rulergranularity;
	for (int i = 0; i < rulergranularity; i++) {
		line = scene->addLine(leftmargin + xstep * i, h - bottommargin, leftmargin + xstep * i, h - bottommargin + 10, QPen(Qt::black, 2, Qt::SolidLine));
		line->setZValue(1);
	}	
	line = scene->addLine(w - rightmargin, h - bottommargin, w - rightmargin, h - bottommargin + 10, QPen(Qt::black, 2, Qt::SolidLine));
	line->setZValue(1);

	simpletext = scene->addSimpleText(QString::number(minmzratio), myFont);
	simpletext->setPos(QPointF(leftmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));
	simpletext->setZValue(1);

	if (maxmzratio - minmzratio > 0.01) {
		xstep = (w - leftmargin - rightmargin) / rulergranularity;
		for (int i = 1; i < rulergranularity; i++) {
			simpletext = scene->addSimpleText(QString::number(minmzratio + (maxmzratio - minmzratio) / (double)rulergranularity * (double)i), myFont);
			simpletext->setPos(QPointF(leftmargin + xstep * i - simpletext->boundingRect().width() / 2, h - bottommargin + 12));
			simpletext->setZValue(1);
		}
	}

	simpletext = scene->addSimpleText(QString::number(maxmzratio), myFont);
	simpletext->setPos(QPointF(w - rightmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));
	simpletext->setZValue(1);


	// y axis
	line = scene->addLine(leftmargin, h - bottommargin, leftmargin, h - bottommargin - std::max(h - topmargin - bottommargin, 0), QPen(Qt::black, 2, Qt::SolidLine));
	line->setZValue(1);
	
	// y axis ruler
	ystep = (h - topmargin - bottommargin) / rulergranularity;
	for (int i = 0; i < rulergranularity; i++) {
		line = scene->addLine(leftmargin - 10, h - bottommargin - ystep * i, leftmargin, h - bottommargin - ystep * i, QPen(Qt::black, 2, Qt::SolidLine));
		line->setZValue(1);
	}
	line = scene->addLine(leftmargin - 10, h - bottommargin - std::max(h - topmargin - bottommargin, 0), leftmargin, h - bottommargin - std::max(h - topmargin - bottommargin, 0), QPen(Qt::black, 2, Qt::SolidLine));
	line->setZValue(1);

	if (absoluteintensity) {
		simpletext = scene->addSimpleText(QString::number(0), myFont);
	}
	else {
		simpletext = scene->addSimpleText(QString::number(0) + " %", myFont);
	}
	simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2));
	simpletext->setZValue(1);

	ystep = (h - topmargin - bottommargin) / rulergranularity;
	if (absoluteintensity) {
		if (maxintensity > rulergranularity) {
			for (int i = 1; i < rulergranularity; i++) {
				simpletext = scene->addSimpleText(QString::number((unsigned long long)maxintensity / (unsigned long long)rulergranularity * (unsigned long long)i), myFont);
				simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2 - ystep * i));
				simpletext->setZValue(1);
			}
		}
	}
	else {
		if (maxintensity > 0) {
			for (int i = 1; i < rulergranularity; i++) {
				simpletext = scene->addSimpleText(QString::number(maxintensity / (double)rulergranularity * (double)i) + " %", myFont);
				simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2 - ystep * i));
				simpletext->setZValue(1);
			}
		}
	}

	if (absoluteintensity) {
		simpletext = scene->addSimpleText(QString::number((unsigned long long)maxintensity), myFont);
	}
	else {
		simpletext = scene->addSimpleText(QString::number(maxintensity) + " %", myFont);
	}
	simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - std::max(h - topmargin - bottommargin, 0) - simpletext->boundingRect().height() / 2));
	simpletext->setZValue(1);


	// select visible peaks
	for (int i = 0; i < isotopicpattern.size(); i++) {
		// skip peaks which are out of range
		if ((isotopicpattern[i].mzratio < minmzratio) || (isotopicpattern[i].mzratio > maxmzratio)) {
			continue;
		}
		visiblepeaks.add(isotopicpattern[i]);
	}
	visiblepeaks.sortbyRelativeIntensityDesc();


	// visible peaks only
	for (int i = 0; i < (int)visiblepeaks.size(); i++) {

		x = getXPositionFromMZRatio(visiblepeaks[i].mzratio, w);

		if (absoluteintensity) {
			y = visiblepeaks[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
		}
		else {
			y = visiblepeaks[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
		}

		hits.clear();

		pos1 = 0;
		pos2 = visiblepeaks[i].description.find(";");
		while (pos2 != string::npos) {
			tmpsubstr = visiblepeaks[i].description.substr(pos1, pos2 - pos1);
	
			if (!tmpsubstr.empty()) {
				hits.push_back(tmpsubstr);
			}
			else {
				break;
			}

			pos1 = pos2 + 1;
			pos2 = visiblepeaks[i].description.find(";", pos1);
		}

		sprintf_s(tmpbuf,"%.5f\0",visiblepeaks[i].mzratio);
		s = tmpbuf;
		hits.push_back(s);

		if (hits.size() > 1) {
			line = scene->addLine(x, h - bottommargin - 2, x, h - bottommargin - std::max((int)y, 2), QPen(Qt::red, 2, Qt::SolidLine));
			line->setZValue(2);
			
			hiddenitems.clear();
			sumh = 0;
			for (vector<string>::reverse_iterator rit = hits.rbegin(); rit != hits.rend(); ++rit) {
				text = scene->addText("");
				text->setDefaultTextColor(QColor(Qt::red));
				text->setFont(myFont);
				//if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				//	text->setTextInteractionFlags(Qt::TextBrowserInteraction);
				//	text->setOpenExternalLinks(true);
				//}
				text->setHtml(rit->c_str());
				tw = text->boundingRect().width();
				th = text->boundingRect().height();
				sumh += th + 1;
				tx = x - 2 - 4;
				ty = h - bottommargin - std::max((int)y, 2) - sumh - 4;
				text->setPos(tx, ty);
				text->setZValue(2);

				hiddenitems.append(text);
				
				if (scene->items(tx, ty, tw, th, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder).size() > 1) {
					for (int k = 0; k < (int)hiddenitems.size(); k++) {
						scene->removeItem(hiddenitems[k]);
					}
					break;
				}

				if (hidelabels) {
					break;
				}
			}
		}
		else {
			line = scene->addLine(x, h - bottommargin - 2, x, h - bottommargin - std::max((int)y, 2), QPen(Qt::black, 2, Qt::SolidLine));
			line->setZValue(1);

			simpletext = scene->addSimpleText(hits[0].c_str(), myFont);
			tw = simpletext->boundingRect().width();
			th = simpletext->boundingRect().height();
			tx = x - 2;
			ty = h - bottommargin - std::max((int)y, 2) - th - 1 - 4;
			simpletext->setPos(tx, ty);
			simpletext->setBrush(Qt::black);
			simpletext->setZValue(1);
	
			if (scene->items(tx, ty, tw, th, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder).size() > 1) {
				scene->removeItem(simpletext);
			}
		}

	}


	// draw profile spectrum
	if (rawdatastate) {

		QPainterPath rpath;

		visiblepeaks.sortbyMass();

		const int step = 1;

		int xmin = getXPositionFromMZRatio(minmzratio, w);
		int xmax = getXPositionFromMZRatio(maxmzratio, w);
		int a, ny, yval;
		double sigma;
		double relint;

		for (int i = xmin; i < xmax; i += step) {
			relint = 0;
			for (int j = 0; j < visiblepeaks.size(); j++) {
				if (absoluteintensity) {
					a = getYPositionFromIntensity(visiblepeaks[j].absoluteintensity, h);
				}
				else {
					a = getYPositionFromIntensity(visiblepeaks[j].relativeintensity, h);
				}
				ny = getXPositionFromMZRatio(visiblepeaks[j].mzratio, w);

				sigma = fwhm / (2.0 * sqrt(2.0 * log(2.0)));
				sigma /= maxmzratio - minmzratio;
				sigma *= double(w - leftmargin - rightmargin);

				a = h - topmargin - bottommargin - a;
							   
				relint += computeGaussFunction((double)i, (double)a, (double)ny, sigma);
			}

			yval = h - topmargin - bottommargin - (int)relint;

			if (i == xmin) {
				rpath.moveTo(i, yval);
			}
			else {
				rpath.lineTo(i, yval);
			}
		}

		scene->addPath(rpath, QPen(Qt::gray, 1, Qt::SolidLine));

	}


	scene->removeItem(zoomgroup);
	scene->removeItem(cursorsimpletextitem);

	if (calledbyresizeevent) {
		oldwidth.push_back(origwidth);
		oldheight.push_back(origheight);
	}
	else {
		oldwidth.clear();
		oldheight.clear();
	}

	bool blocksignal = false;
	// detection of cyclic calls redrawScene<->resizeEvent
	if ((oldwidth.size() == 4) && (oldheight.size() == 4)) {
		if ((oldwidth[3] == oldwidth[1]) && (oldheight[3] == oldheight[1]) && (oldwidth[2] == oldwidth[0]) && (oldheight[2] == oldheight[0])) {
			blocksignal = true;
		}
	}
	
	if (blocksignal) {
		scene->blockSignals(true);
	}
	
	scene->setSceneRect(scene->itemsBoundingRect());
	
	if (blocksignal) {
		scene->blockSignals(false);
	}

	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);

	cursorsimpletextitem->setVisible(false);
	scene->addItem(cursorsimpletextitem);

	while (oldwidth.size() > 3) {
		oldwidth.pop_front();
	}

	while (oldheight.size() > 3) {
		oldheight.pop_front();
	}

	calledbyresizeevent = false;

}


void cPatternSimulatorSceneWidget::updateZoomGroup() {
	qreal rx1, rx2, ry1, ry2;
	QFont myFont("Arial", 8);

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

	zoomrect->setPen(QPen(Qt::black, 1, Qt::DashLine));
	zoomrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));
		
	QString qstr = "m/z: ";
	qstr += QString::number(getMZRatioFromXPosition((pressedx < currentx)?pressedx:currentx, origwidth));
	qstr += "-";
	qstr += QString::number(getMZRatioFromXPosition((pressedx < currentx)?currentx:pressedx, origwidth));
	qstr += "\ndiff: ";
	qstr += QString::number(getMZRatioFromXPosition((pressedx < currentx)?currentx:pressedx, origwidth) - getMZRatioFromXPosition((pressedx < currentx)?pressedx:currentx, origwidth));
		
	zoomsimpletextitem->setFont(myFont);
	zoomsimpletextitem->setText(qstr);
	zoomsimpletextitem->setPos(QPointF(pressedx, pressedy - 2));

	zoomgroup->setVisible(true);
}


void cPatternSimulatorSceneWidget::calculateMinMaxMZ() {
	if (pressedx < leftmargin) {
		pressedx = leftmargin;
	}

	if (pressedx > origwidth - rightmargin) {
		pressedx = origwidth - rightmargin;
	}

	if (currentx < leftmargin) {
		currentx = leftmargin;
	}

	if (currentx > origwidth - rightmargin) {
		currentx = origwidth - rightmargin;
	}

	double tmpminmzratio = getMZRatioFromXPosition((pressedx < currentx) ? pressedx : currentx, origwidth);
	double tmpmaxmzratio = getMZRatioFromXPosition((pressedx < currentx) ? currentx : pressedx, origwidth);

	if (enablemousemzselection) {
		minmzratio = tmpminmzratio;
		maxmzratio = tmpmaxmzratio;
	}
	else {
		if (pressedx > currentx) {
			minmzratio = min(minmzratio + tmpmaxmzratio - tmpminmzratio, isotopicpattern.getMaximumMZRatio() + mzoverhead);
			maxmzratio = min(maxmzratio + tmpmaxmzratio - tmpminmzratio, isotopicpattern.getMaximumMZRatio() + mzoverhead);
		}
		else {
			minmzratio = max(0.0, minmzratio - tmpmaxmzratio + tmpminmzratio);
			maxmzratio = max(0.0, maxmzratio - tmpmaxmzratio + tmpminmzratio);
		}
	}
}


double cPatternSimulatorSceneWidget::getMaximumIntensity() {
	double maxintensity = 0;

	if (absoluteintensity) {
		maxintensity = isotopicpattern.getMaximumAbsoluteIntensityFromMZInterval(minmzratio, maxmzratio, false, false, other, false);
	}
	else {
		maxintensity = isotopicpattern.getMaximumRelativeIntensityFromMZInterval(minmzratio, maxmzratio, false, false, other, false);
	}

	return maxintensity;
}


void cPatternSimulatorSceneWidget::zoomIn() {
	if (currentscale < 32) {
		currentscale += factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cPatternSimulatorSceneWidget::zoomOut() {
	if (currentscale > 1) {
		currentscale -= factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cPatternSimulatorSceneWidget::normalSize() {
	currentscale = 1;
	setMatrix(originalmatrix);
	verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
}


void cPatternSimulatorSceneWidget::absoluteIntensityStateChanged(bool state) {
	absoluteintensity = state;
	redrawScene();
}


void cPatternSimulatorSceneWidget::rawDataStateChanged(bool state) {
	rawdatastate = state;
	redrawScene();
}


void cPatternSimulatorSceneWidget::setMZInterval(double minmz, double maxmz) {
	if (maxmz < minmz) {
		double tmp = maxmz;
		maxmz = minmz;
		minmz = tmp;
	}

	minmzratio = std::max(0.0, minmz);
	maxmzratio = std::min(maxmz, isotopicpattern.getMaximumMZRatio() + mzoverhead);
	emit updateMZInterval(minmzratio, maxmzratio);

	redrawScene();
	viewport()->update();
}


void cPatternSimulatorSceneWidget::resetMZInterval() {
	minmzratio = max(isotopicpattern.getMinimumMZRatio() - mzoverhead, 0.0);
	maxmzratio = isotopicpattern.getMaximumMZRatio() + mzoverhead;
	emit updateMZInterval(minmzratio, maxmzratio);

	redrawScene();
}


void cPatternSimulatorSceneWidget::enableMouseMzSelectionTool(bool enable) {
	enablemousemzselection = enable;
	pressedx = -1;
	pressedy = -1;

	redrawScene();
}

