#include "gui/cCalibrationChartScene.h"

//#include <QVBoxLayout>
//#include <QPainter>
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
//#include <QImage>


double calculateR2Value(double a, double b, vector<double> xvalues, vector<double> yvalues) {
	if (xvalues.empty() || yvalues.empty()) {
		return 0;
	}

	double avg = 0;
	for (int i = 0; i < (int)yvalues.size(); i++) {
		avg += yvalues[i];
	}
	avg /= (double)(yvalues.size());

	double sqsumres = 0;
	double sqsumtot = 0;
	double tmpval;

	for (int i = 0; i < (int)yvalues.size(); i++) {
		tmpval = a * xvalues[i] + b;
		tmpval = tmpval - yvalues[i];
		sqsumres += tmpval * tmpval;

		tmpval = yvalues[i] - avg;
		sqsumtot += tmpval * tmpval;
	}

	return 1.0 - sqsumres / sqsumtot;
}


double calculateLOD(double a, vector<double> xvalues, vector<double> yvalues) {
	if ((a == 0) || (xvalues.size() < 3) || (xvalues.size() != yvalues.size())) {
		return 0;
	}

	double slope = a;
	double stddevy = 0;

	int size = (int)xvalues.size();

	double avgx = 0;
	double avgy = 0;
	
	for (int i = 0; i < size; i++) {
		avgx += xvalues[i];
		avgy += yvalues[i];
	}
	avgx /= (double)(size);
	avgy /= (double)(size);

	double part1 = 0;
	double part2 = 0;
	double part3 = 0;
	double xdiff;
	double ydiff;

	for (int i = 0; i < size; i++) {
		xdiff = xvalues[i] - avgx;
		ydiff = yvalues[i] - avgy;
		
		part1 += ydiff * ydiff;
		part2 += xdiff * ydiff;
		part3 += xdiff * xdiff;
	}

	stddevy = sqrt(1.0 / ((double)(size - 2)) * (part1 - part2 * part2 / part3));

	return max(0.0, 3.3 * stddevy / slope);
}


double calculateLOQ(double a, vector<double> xvalues, vector<double> yvalues) {
	return 10.0 / 3.3 * calculateLOD(a, xvalues, yvalues);
}


cCalibrationChartScene::cCalibrationChartScene(QWidget* parent) {
	this->parent = parent;
	origwidth = 0;
	origheight = 0;
	calledbyresizeevent = false;
	oldwidth.clear();
	oldheight.clear();

	graphtype = 0;

	currentscale = 1;
	factor = 0.2;
	pressedx = -1;
	pressedy = -1;
	currentx = 0;
	currenty = 0;
	minxvalue = 0;
	maxxvalue = 0;

	topmargin = 0;
	bottommargin = 0;
	leftmargin = 0;
	rightmargin = 0;

	firstshow = true;
	enablemousexvalueselection = true;

	equationtype = 0;

	a = 1;
	b = 0;

	xvalues.clear();
	yvalues.clear();

	sd.clear();

	datax.clear();
	datay.clear();

	datasd.clear();
	datagroups.clear();

	datatimevalues.clear();

	hidelabels = false;

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


cCalibrationChartScene::~cCalibrationChartScene() {
	delete scene;
}


void cCalibrationChartScene::initialize() {
	if (graphtype == 0) {
		minxvalue = 0;

		maxxvalue = getMaximumConcentration();
		if (maxxvalue == 0) {
			maxxvalue = 1000;
		}
	}
	else {
		minxvalue = 0;

		maxxvalue = (double)getMaximumCollectionTime();
		if (maxxvalue == 0) {
			maxxvalue = 240;
		}
	}

	emit updateXValueInterval(minxvalue, maxxvalue);

	setScene(scene);

	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	originalmatrix = matrix();

	datax.clear();
	datay.clear();

	datasd.clear();
	datagroups.clear();

	datatimevalues.clear();

	redrawScene();
}


void cCalibrationChartScene::exportToPDF(QString filename, bool postscript) {
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


void cCalibrationChartScene::exportToSVG(QString filename) {
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


void cCalibrationChartScene::exportToPNG(QString filename) {
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


void cCalibrationChartScene::hideLabels(bool state) {
	hidelabels = state;
	redrawScene();
}


void cCalibrationChartScene::setLineParameters(int equationtype, double a, double b, vector<double> xvalues, vector<double> yvalues, vector<double> sd) {
	this->equationtype = equationtype;

	this->a = a;
	this->b = b;

	this->xvalues = xvalues;
	this->yvalues = yvalues;

	this->sd = sd;

	initialize();
}


void cCalibrationChartScene::setData(vector<double> datax, vector<double> datay, vector<double> datasd, vector<string> datagroups, vector<int> datatimevalues) {
	this->datax = datax;
	this->datay = datay;

	this->datasd = datasd;
	this->datagroups = datagroups;

	this->datatimevalues = datatimevalues;

	if (graphtype == 0) {
		minxvalue = 0;

		maxxvalue = getMaximumConcentration();
		if (maxxvalue == 0) {
			maxxvalue = 1000;
		}
	}
	else {
		minxvalue = 0;

		maxxvalue = (double)getMaximumCollectionTime();
		if (maxxvalue == 0) {
			maxxvalue = 240;
		}
	}

	redrawScene();
}


void cCalibrationChartScene::setGraphType(int type) {
	graphtype = type;
	redrawScene();
}


double cCalibrationChartScene::getMaximumConcentration() {
	double value = 0;
	for (auto it : xvalues) {
		if (it > value) {
			value = it;
		}
	}
	for (auto it : datax) {
		if (it > value) {
			value = it;
		}
	}
	return value;
}


int cCalibrationChartScene::getMaximumCollectionTime() {
	int value = 0;
	for (auto it : datatimevalues) {
		if (it > value) {
			value = it;
		}
	}
	return value;
}


void cCalibrationChartScene::wheelEvent(QWheelEvent *event) {
	double part, newmin, newmax;

	if (event->delta() > 0) {
		part = fabs(maxxvalue - minxvalue) / 10.0;
		newmin = minxvalue + part;
		newmax = maxxvalue - part;
	}
	else {
		part = fabs(maxxvalue - minxvalue) / 8.0;
		newmin = max(minxvalue - part, 0.0);
		newmax = min(maxxvalue + part, 1000000.0);
	}

	if (newmin < newmax) {
		minxvalue = newmin;
		maxxvalue = newmax;
		emit updateXValueInterval(minxvalue, maxxvalue);
		redrawScene();
		viewport()->update();
	}

	event->accept();
}


void cCalibrationChartScene::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);

	QPointF p = mapToScene(event->x(), event->y());

	QPointF curpos;
	curpos.setX(p.x() + 15);
	curpos.setY(p.y() - 2);

	double xval = getValueFromXPosition((int)p.x(), origwidth);
	double yval = getValueFromYPosition((int)p.y(), origheight);
	QString curtext = "x: " + QString::number(xval) + ", y: " + QString::number(yval);

	cursorsimpletextitem->setPos(curpos);
	cursorsimpletextitem->setText(curtext);

	if ((pressedx != -1) && (pressedy != -1)) {
		cursorsimpletextitem->setVisible(false);

		currentx = (int)p.x();
		currenty = (int)p.y();

		if (enablemousexvalueselection) {
			updateZoomGroup();
		}
		else {
			calculateMinMaxXValue();

			emit updateXValueInterval(minxvalue, maxxvalue);

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


void cCalibrationChartScene::mouseReleaseEvent(QMouseEvent *event) {
	QGraphicsView::mouseReleaseEvent(event);

	if (pressedx == currentx) {
		pressedx = -1;
		currentx = -1;

		redrawScene();
	}
    
	if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {
		calculateMinMaxXValue();

		emit updateXValueInterval(minxvalue, maxxvalue);

		pressedx = -1;
		pressedy = -1;

		redrawScene();
	}

	QPointF p = mapToScene(event->x(), event->y());

	QPointF curpos;
	curpos.setX(p.x() + 15);
	curpos.setY(p.y() - 2);

	double xval = getValueFromXPosition((int)p.x(), origwidth);
	double yval = getValueFromYPosition((int)p.y(), origheight);
	QString curtext = "x: " + QString::number(xval) + ", y: " + QString::number(yval);

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


void cCalibrationChartScene::mousePressEvent(QMouseEvent *event) {
	QGraphicsView::mousePressEvent(event);

	cursorsimpletextitem->setVisible(false);

	if (event->button() == Qt::LeftButton) {
		QPointF p = mapToScene(event->x(), event->y());
		pressedx = (int)p.x();
		pressedy = (int)p.y();

		currentx = pressedx;
		currenty = pressedy;

		if (enablemousexvalueselection) {
			updateZoomGroup();
		}
	}

	if (event->button() == Qt::RightButton) {
		pressedx = -1;
		pressedy = -1;

		redrawScene();
	}

	//if (enablemousexvalueselection) {
		if (event->button() == Qt::MiddleButton) {
			pressedx = -1;
			pressedy = -1;

			resetXValueInterval();

			redrawScene();
		}
	//}
	
	event->accept();
}


void cCalibrationChartScene::showEvent(QShowEvent *event) {
	QGraphicsView::showEvent(event);

	if (firstshow) {
		normalSize();
		firstshow = false;
	}
}


void cCalibrationChartScene::resizeEvent(QResizeEvent *event) {
	QGraphicsView::resizeEvent(event);
	
	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	calledbyresizeevent = true;

	redrawScene();	
}


double cCalibrationChartScene::getValueFromXPosition(int x, int w) {
	double value = (double)(x - leftmargin) / (double)(w - leftmargin - rightmargin) * (maxxvalue - minxvalue) + minxvalue;
	return max(0.0, value);
}


int cCalibrationChartScene::getXPositionFromValue(double value, int w) {
	double val = value - minxvalue;
	val /= maxxvalue - minxvalue;
	val *= double(w - leftmargin - rightmargin);
	return (int)val + leftmargin;
}


double cCalibrationChartScene::getValueFromYPosition(int y, int h) {
	double maximumintensity = getMaximumYValue();
	double intensity = (double)(y - topmargin) / (double)(h - topmargin - bottommargin) * maximumintensity;
	intensity = maximumintensity - intensity;
	return max(0.0, intensity);
}


int cCalibrationChartScene::getYPositionFromValue(double intensity, int h) {
	double maximumintensity = getMaximumYValue();
	double val = maximumintensity - intensity;
	val /= maximumintensity;
	val *= double(h - topmargin - bottommargin);
	return max(0, (int)val + topmargin);
}


void cCalibrationChartScene::redrawScene() {
	QGraphicsSimpleTextItem* simpletext;
	QGraphicsTextItem* htmltext;

	QGraphicsLineItem* line;

	int xstep, ystep;
	int w = origwidth;
	int h = origheight;
	int rulergranularity = 10;

	QFont myFont("Arial", 8);
	QFontMetrics fm(myFont);

	QList<QGraphicsItem *> usedtextitems;
		
	double maxyvalue = getMaximumYValue();

	scene->removeItem(zoomgroup);
	scene->removeItem(cursorsimpletextitem);

	scene->clear();

	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);

	cursorsimpletextitem->setVisible(false);
	scene->addItem(cursorsimpletextitem);

	// bug fix - mouseMoveEvent is not fired if any QGraphicsTextItem is not in the scene
	scene->addText("");

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

	simpletext = scene->addSimpleText(QString::number(minxvalue), myFont);
	simpletext->setPos(QPointF(leftmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));
	simpletext->setZValue(1);

	if (maxxvalue - minxvalue > 0.01) {
		xstep = (w - leftmargin - rightmargin) / rulergranularity;
		for (int i = 1; i < rulergranularity; i++) {
			simpletext = scene->addSimpleText(QString::number(minxvalue + (maxxvalue - minxvalue) / (double)rulergranularity * (double)i), myFont);
			simpletext->setPos(QPointF(leftmargin + xstep * i - simpletext->boundingRect().width() / 2, h - bottommargin + 12));
			simpletext->setZValue(1);
		}
	}

	simpletext = scene->addSimpleText(QString::number(maxxvalue), myFont);
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

	simpletext = scene->addSimpleText(QString::number(0), myFont);
	simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2));
	simpletext->setZValue(1);

	ystep = (h - topmargin - bottommargin) / rulergranularity;
	if (maxyvalue > 0) {
		for (int i = 1; i < rulergranularity; i++) {
			simpletext = scene->addSimpleText(QString::number(maxyvalue / (double)rulergranularity * (double)i), myFont);
			simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2 - ystep * i));
			simpletext->setZValue(1);
		}
	}

	if (maxyvalue > 0) {
		simpletext = scene->addSimpleText(QString::number(maxyvalue), myFont);
		simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - std::max(h - topmargin - bottommargin, 0) - simpletext->boundingRect().height() / 2));
		simpletext->setZValue(1);
	}

	string graphlabel;

	// calibration curve
	if (graphtype == 0) {

		QPainterPath rpath;
		const int step = 1;

		int xmin = getXPositionFromValue(minxvalue, w);
		int xmax = getXPositionFromValue(maxxvalue, w);
		int xval, yval, tmpyval1, tmpyval2;

		for (int i = xmin; i < xmax; i += step) {
			yval = getYPositionFromValue(a * getValueFromXPosition(i, w) + b, h);

			if (rpath.elementCount() == 0) {
				if (yval <= h - bottommargin) {
					rpath.moveTo(i, yval);
				}
			}
			else {
				if (yval >= topmargin) {
					rpath.lineTo(i, yval);
				}
			}
		}

		scene->addPath(rpath, QPen(Qt::red, 2, Qt::SolidLine));

		for (int i = 0; i < (int)xvalues.size(); i++) {
			xval = getXPositionFromValue(xvalues[i], w);
			yval = getYPositionFromValue(yvalues[i], h);

			if ((xval >= xmin) && (xval <= xmax) && (yval >= topmargin) && (yval <= h - bottommargin)) {
				tmpyval1 = getYPositionFromValue(min(getMaximumYValue(), yvalues[i] + sd[i]), h);
				tmpyval2 = max(topmargin, tmpyval1);
				scene->addLine(xval, yval, xval, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));

				if (tmpyval1 > topmargin) {
					scene->addLine(xval - 2, tmpyval2, xval + 2, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));
				}

				tmpyval1 = getYPositionFromValue(min(getMaximumYValue(), yvalues[i] - sd[i]), h);
				tmpyval2 = min(h - bottommargin, tmpyval1);
				scene->addLine(xval, yval, xval, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));

				if (tmpyval1 < h - bottommargin) {
					scene->addLine(xval - 2, tmpyval2, xval + 2, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));
				}

				scene->addEllipse(QRectF(xval - 4, yval - 4, 8, 8), QPen(Qt::blue, 1, Qt::SolidLine), QBrush(Qt::blue));
			}
		}

		for (int i = 0; i < (int)datax.size(); i++) {
			xval = getXPositionFromValue(datax[i], w);
			yval = getYPositionFromValue(datay[i], h);

			if ((xval >= xmin) && (xval <= xmax) && (yval >= topmargin) && (yval <= h - bottommargin)) {
				tmpyval1 = getYPositionFromValue(min(getMaximumYValue(), datay[i] + datasd[i]), h);
				tmpyval2 = max(topmargin, tmpyval1);
				scene->addLine(xval, yval, xval, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));

				if (tmpyval1 > topmargin) {
					scene->addLine(xval - 2, tmpyval2, xval + 2, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));
				}

				tmpyval1 = getYPositionFromValue(min(getMaximumYValue(), datay[i] - datasd[i]), h);
				tmpyval2 = min(h - bottommargin, tmpyval1);
				scene->addLine(xval, yval, xval, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));

				if (tmpyval1 < h - bottommargin) {
					scene->addLine(xval - 2, tmpyval2, xval + 2, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));
				}

				scene->addEllipse(QRectF(xval - 4, yval - 4, 8, 8), QPen(Qt::green, 1, Qt::SolidLine), QBrush(Qt::green));
			}
		}

		if (!hidelabels) {
			graphlabel = "y = " + to_string(a) + "x";
			if (b != 0) {
				if (b > 0) {
					graphlabel += " + " + to_string(b);
				}
				else {
					graphlabel += " - " + to_string(fabs(b));
				}
			}
		}

		htmltext = scene->addText("", myFont);
		htmltext->setHtml(graphlabel.c_str());
		htmltext->setPos(QPointF(leftmargin + 10, topmargin));
		htmltext->setZValue(1);

		int linespace = 16;

		if (!xvalues.empty() && (equationtype < 2)) {
			if (!hidelabels) {
				graphlabel = "R<sup>2</sup> = " + to_string(calculateR2Value(a, b, xvalues, yvalues));

				htmltext = scene->addText("", myFont);
				htmltext->setHtml(graphlabel.c_str());
				htmltext->setPos(QPointF(leftmargin + 10, topmargin + linespace));
				htmltext->setZValue(1);

				graphlabel = "LOD = " + to_string(calculateLOD(a, xvalues, yvalues));

				htmltext = scene->addText("", myFont);
				htmltext->setHtml(graphlabel.c_str());
				htmltext->setPos(QPointF(leftmargin + 10, topmargin + 3 * linespace));
				htmltext->setZValue(1);

				graphlabel = "LOQ = " + to_string(calculateLOQ(a, xvalues, yvalues));

				htmltext = scene->addText("", myFont);
				htmltext->setHtml(graphlabel.c_str());
				htmltext->setPos(QPointF(leftmargin + 10, topmargin + 4 * linespace));
				htmltext->setZValue(1);
			}
		}

		if (!hidelabels) {

			usedtextitems.clear();

			for (int i = (int)xvalues.size() - 1; i >= 0; i--) {
				xval = getXPositionFromValue(xvalues[i], w);
				yval = getYPositionFromValue(yvalues[i], h);

				if ((xval >= xmin) && (xval <= xmax) && (yval >= topmargin) && (yval <= h - bottommargin)) {
					graphlabel = "Calib. " + to_string(xvalues[i]);
					simpletext = scene->addSimpleText(graphlabel.c_str(), myFont);
					simpletext->setPos(QPointF(xval + 10, yval - 5));
					simpletext->setZValue(1);

					for (auto& it : usedtextitems) {
						if (simpletext->collidesWithItem(it, Qt::IntersectsItemBoundingRect)) {
							scene->removeItem(simpletext);
							simpletext = 0;
							break;
						}
					}

					if (simpletext) {
						usedtextitems.append(simpletext);
					}
				}
			}

			for (int i = (int)datax.size() - 1; i >= 0; i--) {
				xval = getXPositionFromValue(datax[i], w);
				yval = getYPositionFromValue(datay[i], h);

				if ((xval >= xmin) && (xval <= xmax) && (yval >= topmargin) && (yval <= h - bottommargin)) {
					graphlabel = datagroups[i];
					simpletext = scene->addSimpleText(graphlabel.c_str(), myFont);
					simpletext->setPos(QPointF(xval + 10, yval - 5));
					simpletext->setZValue(1);

					for (auto& it : usedtextitems) {
						if (simpletext->collidesWithItem(it, Qt::IntersectsItemBoundingRect)) {
							scene->removeItem(simpletext);
							simpletext = 0;
							break;
						}
					}

					if (simpletext) {
						usedtextitems.append(simpletext);
					}
				}
			}

		}

	}
	// collection time and concentration
	else {

		int xmin = getXPositionFromValue(minxvalue, w);
		int xmax = getXPositionFromValue(maxxvalue, w);
		int xval, yval, tmpyval1, tmpyval2;
		double xsd;

		for (int i = 0; i < (int)datax.size(); i++) {
			xval = getXPositionFromValue(datatimevalues[i], w);
			yval = getYPositionFromValue(datax[i], h);

			if ((xval >= xmin) && (xval <= xmax) && (yval >= topmargin) && (yval <= h - bottommargin)) {
				xsd = 0;
				if (a != 0) {
					xsd = (datasd[i] - b) / a;
				}

				tmpyval1 = getYPositionFromValue(min(getMaximumYValue(), datax[i] + xsd), h);
				tmpyval2 = max(topmargin, tmpyval1);
				scene->addLine(xval, yval, xval, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));

				if (tmpyval1 > topmargin) {
					scene->addLine(xval - 2, tmpyval2, xval + 2, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));
				}

				tmpyval1 = getYPositionFromValue(min(getMaximumYValue(), datax[i] - xsd), h);
				tmpyval2 = min(h - bottommargin, tmpyval1);
				scene->addLine(xval, yval, xval, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));

				if (tmpyval1 < h - bottommargin) {
					scene->addLine(xval - 2, tmpyval2, xval + 2, tmpyval2, QPen(Qt::gray, 2, Qt::SolidLine));
				}

				scene->addEllipse(QRectF(xval - 4, yval - 4, 8, 8), QPen(Qt::green, 1, Qt::SolidLine), QBrush(Qt::green));
			}
		}

		if (!hidelabels) {

			usedtextitems.clear();

			for (int i = (int)datax.size() - 1; i >= 0; i--) {
				xval = getXPositionFromValue(datatimevalues[i], w);
				yval = getYPositionFromValue(datax[i], h);

				if ((xval >= xmin) && (xval <= xmax) && (yval >= topmargin) && (yval <= h - bottommargin)) {
					graphlabel = datagroups[i];
					simpletext = scene->addSimpleText(graphlabel.c_str(), myFont);
					simpletext->setPos(QPointF(xval + 10, yval - 5));
					simpletext->setZValue(1);

					for (auto& it : usedtextitems) {
						if (simpletext->collidesWithItem(it, Qt::IntersectsItemBoundingRect)) {
							scene->removeItem(simpletext);
							simpletext = 0;
							break;
						}
					}

					if (simpletext) {
						usedtextitems.append(simpletext);
					}
				}
			}

		}

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


void cCalibrationChartScene::updateZoomGroup() {
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
		
	QString qstr = "x: ";
	qstr += QString::number(getValueFromXPosition((pressedx < currentx)?pressedx:currentx, origwidth));
	qstr += "-";
	qstr += QString::number(getValueFromXPosition((pressedx < currentx)?currentx:pressedx, origwidth));
	qstr += "\ndiff: ";
	qstr += QString::number(getValueFromXPosition((pressedx < currentx)?currentx:pressedx, origwidth) - getValueFromXPosition((pressedx < currentx)?pressedx:currentx, origwidth));
		
	zoomsimpletextitem->setFont(myFont);
	zoomsimpletextitem->setText(qstr);
	zoomsimpletextitem->setPos(QPointF(pressedx, pressedy - 2));

	zoomgroup->setVisible(true);
}


void cCalibrationChartScene::calculateMinMaxXValue() {
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

	double tmpminvalue = getValueFromXPosition((pressedx < currentx) ? pressedx : currentx, origwidth);
	double tmpmaxvalue = getValueFromXPosition((pressedx < currentx) ? currentx : pressedx, origwidth);

	if (enablemousexvalueselection) {
		minxvalue = tmpminvalue;
		maxxvalue = tmpmaxvalue;
	}
	else {
		if (pressedx > currentx) {
			minxvalue = min(minxvalue + tmpmaxvalue - tmpminvalue, 1000000.0);
			maxxvalue = min(maxxvalue + tmpmaxvalue - tmpminvalue, 1000000.0);
		}
		else {
			minxvalue = max(0.0, minxvalue - tmpmaxvalue + tmpminvalue);
			maxxvalue = max(0.0, maxxvalue - tmpmaxvalue + tmpminvalue);
		}
	}
}


double cCalibrationChartScene::getMaximumYValue() {
	double maxyvalue;
	double xsd;

	if (graphtype == 0) {
		return a * maxxvalue + b;
	}
	else {
		maxyvalue = 0;
		for (size_t i = 0; i < datax.size(); i++) {
			xsd = 0;
			if (a != 0) {
				xsd = (datasd[i] - b) / a;
			}

			if ((datax[i] + xsd > maxyvalue) && (datatimevalues[i] >= minxvalue) && (datatimevalues[i] <= maxxvalue)) {
				maxyvalue = datax[i] + xsd + 5;
			}
		}
	}

	return maxyvalue;
}


void cCalibrationChartScene::zoomIn() {
	if (currentscale < 32) {
		currentscale += factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cCalibrationChartScene::zoomOut() {
	if (currentscale > 1) {
		currentscale -= factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cCalibrationChartScene::normalSize() {
	currentscale = 1;
	setMatrix(originalmatrix);
	verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
}


void cCalibrationChartScene::setXValueInterval(double minxvalue, double maxxvalue) {
	if (maxxvalue < minxvalue) {
		double tmp = maxxvalue;
		maxxvalue = minxvalue;
		minxvalue = tmp;
	}

	this->minxvalue = std::max(0.0, minxvalue);
	this->maxxvalue = std::min(maxxvalue, 1000000.0);
	emit updateXValueInterval(this->minxvalue, this->maxxvalue);

	redrawScene();
	viewport()->update();
}


void cCalibrationChartScene::resetXValueInterval() {
	minxvalue = 0;
	if (graphtype == 0) {
		maxxvalue = getMaximumConcentration();
		if (maxxvalue == 0) {
			maxxvalue = 1000;
		}
	}
	else {
		maxxvalue = (double)getMaximumCollectionTime();
		if (maxxvalue == 0) {
			maxxvalue = 240;
		}
	}

	emit updateXValueInterval(minxvalue, maxxvalue);

	redrawScene();
}


//void cCalibrationChartScene::enableMouseXValueSelectionTool(bool enable) {
//	enablemousexvalueselection = enable;
//	pressedx = -1;
//	pressedy = -1;
//
//	redrawScene();
//}

