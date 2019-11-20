#include "gui/cChromatogramWindowWidget.h"


cChromatogramWindowWidget::cChromatogramWindowWidget(cTheoreticalSpectrumList& theoreticalspectrumlist, QWidget* parent) {
	this->theoreticalspectrumlist = &theoreticalspectrumlist;
	this->parent = parent;

	ticchromatogram.clear();
	eicchromatogram.clear();
	rtimes.clear();

	currentscale = 1;
	factor = 0.2;

	pressedx = -1;
	pressedy = -1;
	currentx = 0;
	currenty = 0;
	mintime = 0;
	maxtime = 0;
	minscan = 0;
	maxscan = 0;

	topmargin = 0;
	bottommargin = 0;
	leftmargin = 0;
	rightmargin = 0;

	firstshow = true;
	enablemouseselection = true;

	retentiontime = true;
	absoluteintensity = true;
	hidetic = false;
	hideeic = false;

	scene = new QGraphicsScene(this);

	zoomgroup = new QGraphicsItemGroup();
	zoomrect = new QGraphicsRectItem();
	zoomgroup->addToGroup(zoomrect);
	zoomsimpletextitem = new QGraphicsSimpleTextItem();
	zoomgroup->addToGroup(zoomsimpletextitem);
	scene->addItem(zoomgroup);

	setScene(scene);
	
	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	originalmatrix = matrix();
}


cChromatogramWindowWidget::~cChromatogramWindowWidget() {
	delete scene;
}


void cChromatogramWindowWidget::exportToPDF(QString filename, bool postscript) {
	QPrinter printer;
	if (postscript) {
		printer.setPaperSize(QSizeF(scene->width() + 100, scene->height() + 100), QPrinter::DevicePixel);
		printer.setPageMargins(50, 50, 50, 50, QPrinter::DevicePixel);
	}
	else {
		printer.setPaperSize(QSizeF(scene->width(), scene->height()), QPrinter::DevicePixel);
		printer.setPageMargins(0, 0, 0, 0, QPrinter::DevicePixel);
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

	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
	scene->render(&painter);
	painter.end();
}


void cChromatogramWindowWidget::exportToSVG(QString filename) {
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


void cChromatogramWindowWidget::exportToPNG(QString filename) {
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


void cChromatogramWindowWidget::recalculateTICChromatogram() {
	int spectrumcount, peakscount;
	unsigned long long intensity;
	cPeak chromatogrampeak;

	ticchromatogram.clear();
	spectrumcount = theoreticalspectrumlist->size();

	rtimes.clear();
	
	QProgressDialog progress("Updating...", /*"Cancel"*/0, 0, spectrumcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);

	rtimes.resize(spectrumcount);
	
	for (int i = 0; i < spectrumcount; i++) {
		intensity = 0;
		peakscount = (*theoreticalspectrumlist)[i].getExperimentalSpectrum().size();
		for (int j = 0; j < peakscount; j++) {
			intensity += (*theoreticalspectrumlist)[i].getExperimentalSpectrum()[j].absoluteintensity;
		}
		chromatogrampeak.clear();
		chromatogrampeak.mzratio = (double)(ticchromatogram.size() + 1);
		chromatogrampeak.absoluteintensity = intensity;
		ticchromatogram.add(chromatogrampeak);

		rtimes[i] = (*theoreticalspectrumlist)[i].getExperimentalSpectrum().getRetentionTime();

		progress.setValue(i);
	}

	if ((spectrumcount > 0) && (rtimes[rtimes.size() - 1] == 0)) {
		for (int i = 0; i < spectrumcount; i++) {
			rtimes[i] = i + 1;
		}
	}

	ticchromatogram.normalizeIntenzity();

	progress.setValue(spectrumcount);

	resetScanIDInterval();
}


void cChromatogramWindowWidget::setEICChromatogram(cPeaksList eic) {
	eicchromatogram = eic;
	redrawScene();
}


void cChromatogramWindowWidget::wheelEvent(QWheelEvent *event) {
	int part, newmin, newmax;
	if (event->delta() > 0) {
		part = max(abs(maxscan - minscan) / 10, 1);
		newmin = minscan + part;
		newmax = maxscan - part;
	}
	else {
		part = max(abs(maxscan - minscan) / 8, 1);
		newmin = max(minscan - part, 1);
		newmax = min(maxscan + part, ticchromatogram.size());
	}

	if (newmin < newmax) {
		minscan = newmin;
		maxscan = newmax;

		emit updateScanIDInterval(minscan, maxscan);

		if ((rtimes.size() > 0) && (rtimes.size() == ticchromatogram.size())) {
			mintime = rtimes[minscan - 1];
			maxtime = rtimes[maxscan - 1];
			emit updateRetentionTimeInterval(mintime, maxtime);
		}


		redrawScene();
	}

	event->accept();
}


void cChromatogramWindowWidget::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);

	if ((pressedx != -1) && (pressedy != -1)) {
		QPointF p = mapToScene(event->x(), event->y());
		currentx = (int)p.x();
		currenty = (int)p.y();

		if (enablemouseselection) {
			updateZoomGroup();
		}
		else {
			calculateMinMaxScan();

			emit updateScanIDInterval(minscan, maxscan);

			if ((rtimes.size() > 0) && (rtimes.size() == ticchromatogram.size())) {
				mintime = rtimes[minscan - 1];
				maxtime = rtimes[maxscan - 1];
				emit updateRetentionTimeInterval(mintime, maxtime);
			}

			pressedx = currentx;
			pressedy = currenty;

			redrawScene();
		}
	}

	event->accept();
}


void cChromatogramWindowWidget::mouseReleaseEvent(QMouseEvent *event) {
	QGraphicsView::mouseReleaseEvent(event);

	if (pressedx == currentx) {
		pressedx = -1;
		currentx = -1;

		redrawScene();
	}

	if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {
		calculateMinMaxScan();

		emit updateScanIDInterval(minscan, maxscan);

		if ((rtimes.size() > 0) && (rtimes.size() == ticchromatogram.size())) {
			mintime = rtimes[minscan - 1];
			maxtime = rtimes[maxscan - 1];
			emit updateRetentionTimeInterval(mintime, maxtime);
		}

		pressedx = -1;
		pressedy = -1;

		redrawScene();
	}

	event->accept();
}


void cChromatogramWindowWidget::mousePressEvent(QMouseEvent *event) {
	QGraphicsView::mousePressEvent(event);

	if (event->button() == Qt::LeftButton) {
		QPointF p = mapToScene(event->x(), event->y());
		pressedx = (int)p.x();
		pressedy = (int)p.y();

		currentx = pressedx;
		currenty = pressedy;

		if (enablemouseselection) {
			updateZoomGroup();
		}
	}

	if (event->button() == Qt::RightButton) {
		pressedx = -1;
		pressedy = -1;

		redrawScene();
	}

	if (enablemouseselection) {
		if (event->button() == Qt::MiddleButton) {
			pressedx = -1;
			pressedy = -1;

			resetScanIDInterval();

			redrawScene();
		}
	}

	event->accept();
}


void cChromatogramWindowWidget::mouseDoubleClickEvent(QMouseEvent *event) {
	QGraphicsView::mouseDoubleClickEvent(event);
	int currentscan;

	if (event->button() == Qt::LeftButton) {
		QPointF p = mapToScene(event->x(), event->y());
		pressedx = (int)p.x();
		pressedy = (int)p.y();

		currentx = pressedx;
		currenty = pressedy;

		currentscan = getScanIDFromXPosition(currentx + origwidth / max(1, 2 * (maxscan - minscan + 1)), origwidth);

		emit chromatogramWidgetDoubleClicked(currentscan - 1);

		pressedx = -1;
		currentx = -1;

		redrawScene();
	}

	event->accept();
}


void cChromatogramWindowWidget::showEvent(QShowEvent *event) {
	QGraphicsView::showEvent(event);

	if (firstshow) {
		normalSize();
		firstshow = false;
	}
}


void cChromatogramWindowWidget::resizeEvent(QResizeEvent *event) {
	QGraphicsView::resizeEvent(event);

	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	redrawScene();
}


int cChromatogramWindowWidget::getScanIDFromXPosition(int x, int w) {
	int scanid = (double)(x - leftmargin) / (double)(w - leftmargin - rightmargin)*(maxscan - minscan + 1) + minscan - 1;
	return min(max(1, scanid), (int)ticchromatogram.size());
}


int cChromatogramWindowWidget::getXPositionFromScanID(int scanid, int w) {
	double val = scanid - minscan + 1;
	val /= maxscan - minscan + 1;
	val *= double(w - leftmargin - rightmargin);
	return (int)val + leftmargin;
}


void cChromatogramWindowWidget::redrawScene() {

	QGraphicsSimpleTextItem* simpletext;
	QGraphicsLineItem* line;
	qreal tx, ty;
	int x;
	double y;

	int xstep, ystep;

	int w = origwidth;
	int h = origheight;
	int rulergranularity = 10;

	QFont myFont("Arial", 8);
	QFontMetrics fm(myFont);

	double maxintensitytic = 0;
	double maxintensityeic = 0;

	int peakscount = 0;

	int printscan;
	bool printintensity;

	if (!hidetic) {
		if (absoluteintensity) {
			maxintensitytic = ticchromatogram.getMaximumAbsoluteIntensityFromMZInterval((double)minscan, (double)maxscan, false, false, other, false);
		}
		else {
			maxintensitytic = ticchromatogram.getMaximumRelativeIntensityFromMZInterval((double)minscan, (double)maxscan, false, false, other, false);
		}
	}

	if (!hideeic) {
		if (absoluteintensity) {
			maxintensityeic = eicchromatogram.getMaximumAbsoluteIntensityFromMZInterval((double)minscan, (double)maxscan, false, false, other, false);
		}
		else {
			maxintensityeic = eicchromatogram.getMaximumRelativeIntensityFromMZInterval((double)minscan, (double)maxscan, false, false, other, false);
		}
	}

	double maxintensity = max(maxintensitytic, maxintensityeic);

	scene->removeItem(zoomgroup);
	scene->clear();

	if ((ticchromatogram.size() < 2) || (hidetic && (eicchromatogram.size() < 2))) {
		simpletext = scene->addSimpleText("The chromatogram is not available.", myFont);
		simpletext->setPos(0, 0);
		scene->setSceneRect(scene->itemsBoundingRect());

		zoomgroup->setVisible(false);
		scene->addItem(zoomgroup);

		return;
	}

	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);

	// x axis
	scene->addLine(leftmargin, h - bottommargin, w - rightmargin, h - bottommargin, QPen(Qt::black, 2, Qt::SolidLine));

	// x axis ruler
	xstep = (w - leftmargin - rightmargin) / rulergranularity;
	for (int i = 0; i < rulergranularity; i++) {
		scene->addLine(leftmargin + xstep * i, h - bottommargin, leftmargin + xstep * i, h - bottommargin + 10, QPen(Qt::black, 2, Qt::SolidLine));
	}
	scene->addLine(w - rightmargin, h - bottommargin, w - rightmargin, h - bottommargin + 10, QPen(Qt::black, 2, Qt::SolidLine));

	printscan = minscan - 1;
	if (retentiontime) {
		if (printscan - 1 >= 0) {
			simpletext = scene->addSimpleText(QString::number(rtimes[printscan - 1]), myFont);
		}
		else {
			simpletext = scene->addSimpleText(QString::number(0), myFont);
		}
	}
	else {
		simpletext = scene->addSimpleText(QString::number(printscan), myFont);
	}
	simpletext->setPos(QPointF(leftmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));

	if (maxscan - minscan > rulergranularity) {
		xstep = (w - leftmargin - rightmargin) / rulergranularity;
		for (int i = 1; i < rulergranularity; i++) {
			printscan = minscan + (maxscan - minscan) / rulergranularity * i;
			if (retentiontime) {
				if (printscan - 1 >= 0) {
					simpletext = scene->addSimpleText(QString::number(rtimes[printscan - 1]), myFont);
				}
				else {
					simpletext = scene->addSimpleText(QString::number(0), myFont);
				}
			}
			else {
				simpletext = scene->addSimpleText(QString::number(printscan), myFont);
			}
			simpletext->setPos(QPointF(leftmargin + xstep * i - simpletext->boundingRect().width() / 2, h - bottommargin + 12));
		}
	}

	printscan = maxscan;
	if (retentiontime) {
		if (printscan - 1 >= 0) {
			simpletext = scene->addSimpleText(QString::number(rtimes[printscan - 1]), myFont);
		}
		else {
			simpletext = scene->addSimpleText(QString::number(0), myFont);
		}
	}
	else {
		simpletext = scene->addSimpleText(QString::number(printscan), myFont);
	}
	simpletext->setPos(QPointF(w - rightmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));

	// y axis
	scene->addLine(leftmargin, h - bottommargin, leftmargin, h - bottommargin - std::max(h - topmargin - bottommargin, 0), QPen(Qt::black, 2, Qt::SolidLine));

	// y axis ruler
	ystep = (h - topmargin - bottommargin) / rulergranularity;
	for (int i = 0; i < rulergranularity; i++) {
		scene->addLine(leftmargin - 10, h - bottommargin - ystep * i, leftmargin, h - bottommargin - ystep * i, QPen(Qt::black, 2, Qt::SolidLine));
	}
	scene->addLine(leftmargin - 10, h - bottommargin - std::max(h - topmargin - bottommargin, 0), leftmargin, h - bottommargin - std::max(h - topmargin - bottommargin, 0), QPen(Qt::black, 2, Qt::SolidLine));

	if (absoluteintensity) {
		simpletext = scene->addSimpleText(QString::number(0), myFont);
	}
	else {
		simpletext = scene->addSimpleText(QString::number(0) + " %", myFont);
	}
	simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2));

	ystep = (h - topmargin - bottommargin) / rulergranularity;
	if (absoluteintensity) {
		if (maxintensity > rulergranularity) {
			for (int i = 1; i < rulergranularity; i++) {
				simpletext = scene->addSimpleText(QString::number((unsigned long long)maxintensity / (unsigned long long)rulergranularity * (unsigned long long)i), myFont);
				simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2 - ystep * i));
			}
		}
	}
	else {
		if (maxintensity > 0) {
			for (int i = 1; i < rulergranularity; i++) {
				simpletext = scene->addSimpleText(QString::number(maxintensity / (double)rulergranularity * (double)i) + " %", myFont);
				simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2 - ystep * i));
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

	// TIC peaks
	if (!hidetic) {

		for (int i = 0; i < ticchromatogram.size(); i++) {

			if ((ticchromatogram[i].mzratio < (double)minscan) || (ticchromatogram[i].mzratio >(double)maxscan)) {
				continue;
			}

			x = getXPositionFromScanID((int)ticchromatogram[i].mzratio, w);

			printintensity = true;
			if (absoluteintensity) {
				y = ticchromatogram[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
				if (ticchromatogram[i].absoluteintensity == 0) {
					printintensity = false;
				}
			}
			else {
				y = ticchromatogram[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
				if (ticchromatogram[i].relativeintensity == 0) {
					printintensity = false;
				}
			}

			if (printintensity) {
				line = scene->addLine(x, h - bottommargin - 2, x, h - bottommargin - std::max((int)y, 2), QPen(Qt::black, 2, Qt::SolidLine));
				peakscount++;
			}

		}

	}

	// EIC peaks
	if (!hideeic) {

		for (int i = 0; i < eicchromatogram.size(); i++) {

			if ((eicchromatogram[i].mzratio < (double)minscan) || (eicchromatogram[i].mzratio >(double)maxscan)) {
				continue;
			}

			x = getXPositionFromScanID((int)eicchromatogram[i].mzratio, w);

			printintensity = true;
			if (absoluteintensity) {
				y = eicchromatogram[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
				if (eicchromatogram[i].absoluteintensity == 0) {
					printintensity = false;
				}
			}
			else {
				y = eicchromatogram[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
				if (eicchromatogram[i].relativeintensity == 0) {
					printintensity = false;
				}
			}

			if (printintensity) {
				line = scene->addLine(x, h - bottommargin - 2, x, h - bottommargin - std::max((int)y, 2), QPen(Qt::red, 2, Qt::SolidLine));
				line->setZValue(1);
				peakscount++;
			}

		}

	}

	// TIC descriptions
	if (!hidetic && (peakscount < 500)) {

		for (int i = 0; i < ticchromatogram.size(); i++) {

			if ((ticchromatogram[i].mzratio < (double)minscan) || (ticchromatogram[i].mzratio > (double)maxscan)) {
				continue;
			}

			x = getXPositionFromScanID((int)ticchromatogram[i].mzratio, w);

			printintensity = true;
			if (absoluteintensity) {
				y = ticchromatogram[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
				if (ticchromatogram[i].absoluteintensity == 0) {
					printintensity = false;
				}
			}
			else {
				y = ticchromatogram[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
				if (ticchromatogram[i].relativeintensity == 0) {
					printintensity = false;
				}
			}

			if (printintensity) {
			
				if (retentiontime) {
					simpletext = scene->addSimpleText(QString::number(rtimes[i]) + " ", myFont);
				}
				else {
					simpletext = scene->addSimpleText(QString::number((int)ticchromatogram[i].mzratio) + " ", myFont);
				}
				tx = x - 2;
				ty = h - bottommargin - std::max((int)y, 2) - simpletext->boundingRect().height() - 1 - 4;
				simpletext->setPos(tx, ty);
				simpletext->setBrush(Qt::black);

				if (scene->collidingItems(simpletext, Qt::IntersectsItemBoundingRect).size() > 0) {
					scene->removeItem(simpletext);
				}

			}

		}

	}

	// EIC descriptions
	if (!hideeic && (peakscount < 500)) {

		for (int i = 0; i < eicchromatogram.size(); i++) {

			if ((eicchromatogram[i].mzratio < (double)minscan) || (eicchromatogram[i].mzratio >(double)maxscan)) {
				continue;
			}

			x = getXPositionFromScanID((int)eicchromatogram[i].mzratio, w);

			printintensity = true;
			if (absoluteintensity) {
				y = eicchromatogram[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
				if (eicchromatogram[i].absoluteintensity == 0) {
					printintensity = false;
				}
			}
			else {
				y = eicchromatogram[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
				if (eicchromatogram[i].relativeintensity == 0) {
					printintensity = false;
				}
			}

			if (printintensity) {

				if (retentiontime) {
					simpletext = scene->addSimpleText(QString::number(rtimes[i]) + " ", myFont);
				}
				else {
					simpletext = scene->addSimpleText(QString::number((int)eicchromatogram[i].mzratio) + " ", myFont);
				}
				tx = x - 2;
				ty = h - bottommargin - std::max((int)y, 2) - simpletext->boundingRect().height() - 1 - 4;
				simpletext->setPos(tx, ty);
				simpletext->setBrush(Qt::red);
				simpletext->setZValue(1);

				if (scene->collidingItems(simpletext, Qt::IntersectsItemBoundingRect).size() > 0) {
					scene->removeItem(simpletext);
				}

			}

		}

	}

	scene->removeItem(zoomgroup);
	QRectF brect = scene->itemsBoundingRect();

	if (!hidetic) {
		simpletext = scene->addSimpleText("TIC", myFont);
		simpletext->setPos(brect.x() - 50, brect.y() - 50);
		simpletext->setBrush(Qt::black);
	}

	if (!hideeic) {
		simpletext = scene->addSimpleText("EIC", myFont);
		simpletext->setPos(brect.x() - 50, brect.y() - 35);
		simpletext->setBrush(Qt::red);
	}

	scene->setSceneRect(scene->itemsBoundingRect());
	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);

}


void cChromatogramWindowWidget::updateZoomGroup() {
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

	int from = getScanIDFromXPosition((pressedx < currentx) ? pressedx : currentx, origwidth);
	int to = getScanIDFromXPosition((pressedx < currentx) ? currentx : pressedx, origwidth);

	QString qstr;

	if (retentiontime) {
		if ((from > 0) && (from <= (int)rtimes.size()) && (to > 0) && (to <= (int)rtimes.size())) {
			qstr += "Time: ";
			qstr += QString::number(rtimes[from - 1]);
			qstr += "-";
			qstr += QString::number(rtimes[to - 1]);
			qstr += "\ndiff: ";
			qstr += QString::number(rtimes[to - 1] - rtimes[from - 1]);
		}
	}
	else {
		qstr += "Spectrum ID: ";
		qstr += QString::number(from);
		qstr += "-";
		qstr += QString::number(to);
		qstr += "\ndiff: ";
		qstr += QString::number(to - from);
	}

	zoomsimpletextitem->setFont(myFont);
	zoomsimpletextitem->setText(qstr);
	zoomsimpletextitem->setPos(QPointF(pressedx, pressedy - 2));

	zoomgroup->setVisible(true);
}


void cChromatogramWindowWidget::calculateMinMaxScan() {
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

	int tmpminscan = getScanIDFromXPosition((pressedx < currentx) ? pressedx : currentx, origwidth);
	int tmpmaxscan = getScanIDFromXPosition((pressedx < currentx) ? currentx : pressedx, origwidth);

	if (enablemouseselection) {
		if (tmpminscan != tmpmaxscan) {
			minscan = tmpminscan;
			maxscan = tmpmaxscan;
		}
	}
	else {
		if (pressedx > currentx) {
			minscan = min(minscan + tmpmaxscan - tmpminscan, max(1, ticchromatogram.size()));
			maxscan = min(maxscan + tmpmaxscan - tmpminscan, max(1, ticchromatogram.size()));
		}
		else {
			minscan = max(1, minscan - tmpmaxscan + tmpminscan);
			maxscan = max(1, maxscan - tmpmaxscan + tmpminscan);
		}
	}
}


void cChromatogramWindowWidget::zoomIn() {
	if (currentscale < 32) {
		currentscale += factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cChromatogramWindowWidget::zoomOut() {
	if (currentscale > 1) {
		currentscale -= factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cChromatogramWindowWidget::normalSize() {
	currentscale = 1;
	setMatrix(originalmatrix);
	verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
}


void cChromatogramWindowWidget::retentionTimeStateChanged(bool state) {
	retentiontime = state;
	redrawScene();
}


void cChromatogramWindowWidget::absoluteIntensityStateChanged(bool state) {
	absoluteintensity = state;
	redrawScene();
}


void cChromatogramWindowWidget::setRetentionTimeInterval(double mintime, double maxtime) {
	if (rtimes.size() == 0) {
		return;
	}

	int minid = 0;
	int maxid = 0;

	int size = (int)rtimes.size();
	for (int i = 0; i < size; i++) {
		if (fabs(rtimes[i] - mintime) < fabs(rtimes[minid] - mintime)) {
			minid = i;
		}
		if (fabs(rtimes[i] - maxtime) < fabs(rtimes[maxid] - maxtime)) {
			maxid = i;
		}
	}
	
	setScanIDInterval(minid + 1, maxid + 1);
}


void cChromatogramWindowWidget::resetRetentionTimeInterval() {
	resetScanIDInterval();
}


void cChromatogramWindowWidget::setScanIDInterval(int minid, int maxid) {
	if (maxid < minid) {
		int tmp = maxid;
		maxid = minid;
		minid = tmp;
	}

	minscan = max(1, minid);
	maxscan = min(maxid, ticchromatogram.size());
	emit updateScanIDInterval(minscan, maxscan);

	if ((rtimes.size() > 0) && (rtimes.size() == ticchromatogram.size())) {
		mintime = rtimes[minscan - 1];
		maxtime = rtimes[maxscan - 1];
		emit updateRetentionTimeInterval(mintime, maxtime);
	}

	redrawScene();
}


void cChromatogramWindowWidget::resetScanIDInterval() {
	minscan = 1;
	maxscan = max(1, ticchromatogram.size());
	emit updateScanIDInterval(minscan, maxscan);

	if (rtimes.size() > 0) {
		mintime = rtimes[0];
		maxtime = rtimes[rtimes.size() - 1];
		emit updateRetentionTimeInterval(mintime, maxtime);
	}

	redrawScene();
}


void cChromatogramWindowWidget::enableMouseSelectionTool(bool enable) {
	enablemouseselection = enable;
	pressedx = -1;
	pressedy = -1;

	redrawScene();
}


void cChromatogramWindowWidget::hideTIC(bool state) {
	hidetic = state;
	redrawScene();
}


void cChromatogramWindowWidget::hideEIC(bool state) {
	hideeic = state;
	redrawScene();
}

