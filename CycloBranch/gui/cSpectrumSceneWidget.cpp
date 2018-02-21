#include "gui/cSpectrumSceneWidget.h"

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


cSpectrumSceneWidget::cSpectrumSceneWidget(QWidget* parent) {
	this->parent = parent;
	parameters = 0;
	theoreticalspectrum = 0;
	origwidth = 500;
	origheight = 200;
	currentscale = 1;
	absoluteintensity = false;
	hideunmatched = false;
	hidematched = false;
	hidescrambled = false;
	factor = 0.2;
	coloredrotationid = -1;
	coloredrotationstring = "";
	coloredtrotationid = -1;
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

	scene = new QGraphicsScene(this);

	zoomgroup = new QGraphicsItemGroup();
	zoomrect = new QGraphicsRectItem();
	zoomgroup->addToGroup(zoomrect);
	zoomsimpletextitem = new QGraphicsSimpleTextItem();
	zoomgroup->addToGroup(zoomsimpletextitem);
	scene->addItem(zoomgroup);
}


cSpectrumSceneWidget::~cSpectrumSceneWidget() {
	delete scene;
}


void cSpectrumSceneWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;

	if (parameters->mode == dereplication) {
		origwidth = 800;
	}

	minmzratio = 0;
	maxmzratio = theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio();
	emit updateMZInterval(minmzratio, maxmzratio);

	setScene(scene);
	redrawScene();
	originalmatrix = matrix();
}


void cSpectrumSceneWidget::exportToPDF(QString filename, bool postscript) {
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


void cSpectrumSceneWidget::exportToSVG(QString filename) {
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


void cSpectrumSceneWidget::exportToPNG(QString filename) {
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


void cSpectrumSceneWidget::hideUnmatchedPeaks(bool state) {
	if (state == false) {
		hideunmatched = false;
	}
	else {
		hideunmatched = true;
	}

	redrawScene();
}


void cSpectrumSceneWidget::hideMatchedPeaks(bool state) {
	if (state == false) {
		hidematched = false;
	}
	else {
		hidematched = true;
	}

	redrawScene();
}


void cSpectrumSceneWidget::hideScrambledPeaks(bool state) {
	if (state == false) {
		hidescrambled = false;
	}
	else {
		hidescrambled = true;
	}

	redrawScene();
}


void cSpectrumSceneWidget::wheelEvent(QWheelEvent *event) {
	if (event->delta() > 0) {
		zoomIn();
	}
	else {
		zoomOut();
	}

	event->accept();
}


void cSpectrumSceneWidget::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);

	if ((pressedx != -1) && (pressedy != -1)) {
		QPointF p = mapToScene(event->x(), event->y());
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

	event->accept();
}


void cSpectrumSceneWidget::mouseReleaseEvent(QMouseEvent *event) {
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

	event->accept();
}


void cSpectrumSceneWidget::mousePressEvent(QMouseEvent *event) {
	QGraphicsView::mousePressEvent(event);

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

	if (enablemousemzselection) {
		if (event->button() == Qt::MiddleButton) {
			pressedx = -1;
			pressedy = -1;

			resetMZInterval();

			redrawScene();
		}
	}
	
	event->accept();
}


void cSpectrumSceneWidget::showEvent(QShowEvent *event) {
	if (firstshow) {
		normalSize();
		firstshow = false;
	}

	event->accept();
}


double cSpectrumSceneWidget::getMZRatioFromXPosition(int x, int w) {
	double mz = (double)(x - leftmargin)/(double)(w - leftmargin - rightmargin)*(maxmzratio - minmzratio) + minmzratio;
	return max(0.0, mz);
}


int cSpectrumSceneWidget::getXPositionFromMZRatio(double mzratio, int w) {
	double val = mzratio - minmzratio;
	val /= maxmzratio - minmzratio;
	val *= double(w - leftmargin - rightmargin);
	return (int)val + leftmargin;
}


void cSpectrumSceneWidget::redrawScene() {

	QGraphicsSimpleTextItem* simpletext;
	QGraphicsTextItem* text;

	int x;
	double y;
	qreal tx, ty, tw, th, sumh;

	char tmpbuf[30];
	string s;

	int xstep, ystep;
	int w = origwidth;
	int h = origheight;
	int rulergranularity = 10;

	cPeaksList visiblepeaks;
	visiblepeaks.clear();

	QList<QGraphicsItem *> hiddenitems;

	vector<string> hits;
	set<int> experimentalmatches;

	QFont myFont("Arial", 8);
	QFontMetrics fm(myFont);


	// maximum intensity in the interval <minmzratio, maxmzratio>
	double maxintensity;
	if (absoluteintensity) {
		maxintensity = theoreticalspectrum->getExperimentalSpectrum().getMaximumAbsoluteIntensityFromMZInterval(minmzratio, maxmzratio, hidematched, hideunmatched, parameters->peptidetype, hidescrambled);
	}
	else {
		maxintensity = theoreticalspectrum->getExperimentalSpectrum().getMaximumRelativeIntensityFromMZInterval(minmzratio, maxmzratio, hidematched, hideunmatched, parameters->peptidetype, hidescrambled);
	}


	scene->removeItem(zoomgroup);
	scene->clear();
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

	simpletext = scene->addSimpleText(QString::number(minmzratio), myFont);
	simpletext->setPos(QPointF(leftmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));

	if (maxmzratio - minmzratio > 0.01) {
		xstep = (w - leftmargin - rightmargin) / rulergranularity;
		for (int i = 1; i < rulergranularity; i++) {
			simpletext = scene->addSimpleText(QString::number(minmzratio + (maxmzratio - minmzratio) / (double)rulergranularity * (double)i), myFont);
			simpletext->setPos(QPointF(leftmargin + xstep * i - simpletext->boundingRect().width() / 2, h - bottommargin + 12));
		}
	}

	simpletext = scene->addSimpleText(QString::number(maxmzratio), myFont);
	simpletext->setPos(QPointF(w - rightmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));


	// y axis
	scene->addLine(leftmargin, h - bottommargin, leftmargin, h - bottommargin - std::max(h - topmargin - bottommargin, 5), QPen(Qt::black, 2, Qt::SolidLine));
	
	// y axis ruler
	ystep = (h - topmargin - bottommargin) / rulergranularity;
	for (int i = 0; i < rulergranularity; i++) {
		scene->addLine(leftmargin - 10, h - bottommargin - ystep * i, leftmargin, h - bottommargin - ystep * i, QPen(Qt::black, 2, Qt::SolidLine));
	}
	scene->addLine(leftmargin - 10, h - bottommargin - std::max(h - topmargin - bottommargin, 5), leftmargin, h - bottommargin - std::max(h - topmargin - bottommargin, 5), QPen(Qt::black, 2, Qt::SolidLine));

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
	simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - std::max(h - topmargin - bottommargin, 5) - simpletext->boundingRect().height() / 2));


	cPeaksList thpeaks = *theoreticalspectrum->getTheoreticalPeaks();
	thpeaks.sortbyOrderId();

	// all experimental peaks
	int visiblepeakscount = 0;
	for (int i = 0; i < (int)theoreticalspectrum->getExperimentalSpectrum().size(); i++) {

		// skip peaks which are out of range
		if ((theoreticalspectrum->getExperimentalSpectrum()[i].mzratio < minmzratio) || (theoreticalspectrum->getExperimentalSpectrum()[i].mzratio > maxmzratio)) {
			continue;
		}

		// hide unmatched peaks
		if (hideunmatched && (theoreticalspectrum->getExperimentalSpectrum()[i].matched <= 0)) {
			continue;
		}

		// hide matched peaks
		if (hidematched && (theoreticalspectrum->getExperimentalSpectrum()[i].matched > 0)) {
			continue;
		}

		// hide scrambled peaks
		if ((parameters->peptidetype == cyclic) && hidescrambled && theoreticalspectrum->getExperimentalSpectrum()[i].scrambled) {
			continue;
		}

		visiblepeaks.add(theoreticalspectrum->getExperimentalSpectrum()[i]);
		visiblepeaks[visiblepeakscount].description.clear();

		experimentalmatches = theoreticalspectrum->getExperimentalMatches(i);
		for (auto it = experimentalmatches.begin(); it != experimentalmatches.end(); ++it) {
			if (visiblepeaks[visiblepeakscount].description.compare("") != 0) {
				visiblepeaks[visiblepeakscount].description += "@";
			}

			if (parameters->mode == dereplication) {
				visiblepeaks[visiblepeakscount].description += parameters->peakidtodesc[thpeaks[*it].descriptionid].substr(0, parameters->peakidtodesc[thpeaks[*it].descriptionid].rfind(':'));
			}
			else {
				visiblepeaks[visiblepeakscount].description += parameters->peakidtodesc[thpeaks[*it].descriptionid].substr(0, parameters->peakidtodesc[thpeaks[*it].descriptionid].find(':'));
			}
		}

		visiblepeakscount++;

	}

	visiblepeaks.sortbyRelativeIntensityDesc();


	string coloredtrotationstring;
	if (coloredtrotationid != -1) {
		if (parameters->peptidetype == branched) {
			coloredtrotationstring = " " + to_string(coloredtrotationid + 1) + "_";
		}
		if (parameters->peptidetype == branchcyclic) {
			coloredtrotationstring = "_" + to_string(coloredtrotationid + 1) + "_";
		}
	}


	// visible peaks only
	for (int i = 0; i < (int)visiblepeaks.size(); i++) {

		x = getXPositionFromMZRatio(visiblepeaks[i].mzratio, origwidth);

		if (absoluteintensity) {
			y = visiblepeaks[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
		}
		else {
			y = visiblepeaks[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
		}

		hits.clear();

		s = visiblepeaks[i].description;
		int position = (int)s.find('@');
		while (position != (int)string::npos) {
			hits.push_back(s.substr(0, position));
			s = s.substr(position + 1);

			if (parameters->peptidetype == cyclic) {
				if (visiblepeaks[i].scrambled) {
					if (hidescrambled) {
						hits.pop_back();
					}
				}
				else {
					if ((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) {
						hits.pop_back();
					}
				}	
			}

			if ((parameters->peptidetype == branched) && (coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)) {
				hits.pop_back();
			}

			if ((parameters->peptidetype == branchcyclic) && (((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) || ((coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)))) {
				hits.pop_back();
			}

			position = (int)s.find('@');
		}

		if (s.size() > 0) {
			hits.push_back(s);

			if (parameters->peptidetype == cyclic) {
				if (visiblepeaks[i].scrambled) {
					if (hidescrambled) {
						hits.pop_back();
					}
				}
				else {
					if ((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) {
						hits.pop_back();
					}
				}	
			}

			if ((parameters->peptidetype == branched) && (coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)) {
				hits.pop_back();
			}

			if ((parameters->peptidetype == branchcyclic) && (((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) || ((coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)))) {
				hits.pop_back();
			}
		}


		sprintf_s(tmpbuf,"%.5f\0",visiblepeaks[i].mzratio);
		s = tmpbuf;
		hits.push_back(s);
		QGraphicsLineItem* line;

		if (hits.size() > 1) {
			line = scene->addLine(x, h - bottommargin - 2, x, h - bottommargin - std::max((int)y, 5), QPen(Qt::red, 2, Qt::SolidLine));
			line->setZValue(1);
			
			hiddenitems.clear();
			sumh = 0;
			for (vector<string>::reverse_iterator rit = hits.rbegin(); rit != hits.rend(); ++rit) {
				text = scene->addText("");
				text->setDefaultTextColor(QColor(Qt::red));
				text->setFont(myFont);
				if (parameters->mode == dereplication) {
					text->setTextInteractionFlags(Qt::TextBrowserInteraction);
					text->setOpenExternalLinks(true);
				}
				text->setHtml(rit->c_str());
				tw = text->boundingRect().width();
				th = text->boundingRect().height();
				sumh += th + 1;
				tx = x - 2 - 4;
				ty = h - bottommargin - std::max((int)y, 5) - sumh - 4;
				text->setPos(tx, ty);

				hiddenitems.append(text);
				
				if (scene->items(tx, ty, tw, th, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder).size() > 1) {
					for (int k = 0; k < (int)hiddenitems.size(); k++) {
						scene->removeItem(hiddenitems[k]);
					}
					break;
				}
			}
		}
		else {
			scene->addLine(x, h - bottommargin - 2, x, h - bottommargin - std::max((int)y, 5), QPen(Qt::black, 2, Qt::SolidLine));

			simpletext = scene->addSimpleText(hits[0].c_str(), myFont);
			tw = simpletext->boundingRect().width();
			th = simpletext->boundingRect().height();
			tx = x - 2;
			ty = h - bottommargin - std::max((int)y, 5) - th - 1 - 4;
			simpletext->setPos(tx, ty);
			simpletext->setBrush(Qt::black);
	
			if (scene->items(tx, ty, tw, th, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder).size() > 1) {
				scene->removeItem(simpletext);
			}
		}

	}


	scene->removeItem(zoomgroup);
	scene->setSceneRect(scene->itemsBoundingRect());
	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);

}


void cSpectrumSceneWidget::updateZoomGroup() {
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


void cSpectrumSceneWidget::calculateMinMaxMZ() {
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
			minmzratio = min(minmzratio + tmpmaxmzratio - tmpminmzratio, theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio());
			maxmzratio = min(maxmzratio + tmpmaxmzratio - tmpminmzratio, theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio());
		}
		else {
			minmzratio = max(0.0, minmzratio - tmpmaxmzratio + tmpminmzratio);
			maxmzratio = max(0.0, maxmzratio - tmpmaxmzratio + tmpminmzratio);
		}
	}
}


void cSpectrumSceneWidget::zoomIn() {
	if (currentscale < 32) {
		currentscale += factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cSpectrumSceneWidget::zoomOut() {
	if (currentscale > 1) {
		currentscale -= factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cSpectrumSceneWidget::normalSize() {
	currentscale = 1;
	setMatrix(originalmatrix);
	verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
}


void cSpectrumSceneWidget::absoluteIntensityStateChanged(bool state) {
	absoluteintensity = state;
	redrawScene();
}


void cSpectrumSceneWidget::setMZInterval(double minmz, double maxmz) {
	if (maxmz < minmz) {
		double tmp = maxmz;
		maxmz = minmz;
		minmz = tmp;
	}

	minmzratio = std::max(0.0, minmz);
	maxmzratio = std::min(maxmz, theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio());
	emit updateMZInterval(minmzratio, maxmzratio);

	redrawScene();
}


void cSpectrumSceneWidget::resetMZInterval() {
	minmzratio = 0;
	maxmzratio = theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio();
	emit updateMZInterval(minmzratio, maxmzratio);

	redrawScene();
}


void cSpectrumSceneWidget::rotationChanged(int index) {
	coloredrotationid = index - 1;
	redrawScene();
}


void cSpectrumSceneWidget::trotationChanged(int index) {
	coloredtrotationid = index - 1;
	redrawScene();
}


void cSpectrumSceneWidget::rotationChanged(QString text) {
	coloredrotationstring = " " + text.toStdString() + "_";
	redrawScene();
}


void cSpectrumSceneWidget::enableMouseMzSelectionTool(bool enable) {
	enablemousemzselection = enable;
	pressedx = -1;
	pressedy = -1;

	redrawScene();
}

