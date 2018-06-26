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
	rawdatapeaklist = 0;
	origwidth = 0;
	origheight = 0;
	currentscale = 1;
	absoluteintensity = false;
	rawdatastate = false;
	hideunmatched = false;
	hidematched = false;
	hidescrambled = false;
	hidelabels = false;
	factor = 0.2;
	visibleionseriespart1 = "";
	visibleionseriespart2 = "";
	visibleneutralloss = "all";
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

	maxmzoverhead = 5.0;

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


void cSpectrumSceneWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, cPeakListSeries* rawdata, int rowid) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;

	if (parameters && parameters->useprofiledata) {
		if ((parameters->peaklistfileformat == baf) || ((parameters->mode == dereplication) && (parameters->peaklistfileformat == imzML))) {
			if ((rowid > 0) && (rowid <= rawdata->size())) {
				this->rawdatapeaklist = &((*rawdata)[rowid - 1]);
			}
		}
	}

	minmzratio = 0;
	maxmzratio = theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio() + maxmzoverhead;
	emit updateMZInterval(minmzratio, maxmzratio);

	setScene(scene);

	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

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
	hideunmatched = state;
	redrawScene();
}


void cSpectrumSceneWidget::hideMatchedPeaks(bool state) {
	hidematched = state;
	redrawScene();
}


void cSpectrumSceneWidget::hideScrambledPeaks(bool state) {
	hidescrambled = state;
	redrawScene();
}


void cSpectrumSceneWidget::hidePeakLabels(bool state) {
	hidelabels = state;
	redrawScene();
}


void cSpectrumSceneWidget::wheelEvent(QWheelEvent *event) {
	double part, newmin, newmax;
	if (event->delta() > 0) {
		part = fabs(maxmzratio - minmzratio) / 10.0;
		newmin = minmzratio + part;
		newmax = maxmzratio - part;
	}
	else {
		part = fabs(maxmzratio - minmzratio) / 8.0;
		newmin = max(minmzratio - part, 0.0);
		newmax = min(maxmzratio + part, theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio() + maxmzoverhead);
	}

	if (newmin < newmax) {
		minmzratio = newmin;
		maxmzratio = newmax;
		emit updateMZInterval(minmzratio, maxmzratio);
		redrawScene();
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


void cSpectrumSceneWidget::showEvent(QShowEvent *event) {
	QGraphicsView::showEvent(event);

	if (firstshow) {
		normalSize();
		firstshow = false;
	}
}


void cSpectrumSceneWidget::resizeEvent(QResizeEvent *event) {
	QGraphicsView::resizeEvent(event);
	
	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	redrawScene();	
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
	QGraphicsLineItem* line;

	int x;
	double y;
	qreal tx, ty, tw, th, sumh;

	char tmpbuf[30];
	string s;

	int xstep, ystep;
	int w = origwidth;
	int h = origheight;
	int rulergranularity = 10;

	int size;

	cPeaksList visiblepeaks;
	visiblepeaks.clear();

	QList<QGraphicsItem *> hiddenitems;

	vector<string> hits;
	set<int> experimentalmatches;

	QFont myFont("Arial", 8);
	QFontMetrics fm(myFont);
	
	
	// maximum intensity in the interval <minmzratio, maxmzratio>
	double maxintensity = 0;
	double rawdatamaxintensity = 0;

	if (parameters->useprofiledata && ((parameters->peaklistfileformat == baf) || ((parameters->mode == dereplication) && (parameters->peaklistfileformat == imzML))) && rawdatastate && (rawdatapeaklist->size() > 0)) {
		if (absoluteintensity) {
			maxintensity = theoreticalspectrum->getExperimentalSpectrum().getMaximumAbsoluteIntensityFromMZInterval(minmzratio, maxmzratio, false, false, other, false);
		}
		else {
			maxintensity = theoreticalspectrum->getExperimentalSpectrum().getMaximumRelativeIntensityFromMZInterval(minmzratio, maxmzratio, false, false, other, false);
		}
				
		if (absoluteintensity) {
			rawdatamaxintensity = rawdatapeaklist->getMaximumAbsoluteIntensityFromMZInterval(minmzratio, maxmzratio, false, false, other, false);
		}
		else {
			rawdatamaxintensity = rawdatapeaklist->getMaximumRelativeIntensityFromMZInterval(minmzratio, maxmzratio, false, false, other, false);
		}
		
		if (maxintensity == 0) {
			maxintensity = rawdatamaxintensity;
		}
	}
	else {
		if (absoluteintensity) {
			maxintensity = theoreticalspectrum->getExperimentalSpectrum().getMaximumAbsoluteIntensityFromMZInterval(minmzratio, maxmzratio, hidematched, hideunmatched, parameters->peptidetype, hidescrambled);
		}
		else {
			maxintensity = theoreticalspectrum->getExperimentalSpectrum().getMaximumRelativeIntensityFromMZInterval(minmzratio, maxmzratio, hidematched, hideunmatched, parameters->peptidetype, hidescrambled);
		}
	}


	scene->removeItem(zoomgroup);
	scene->clear();
	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);


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
		if ((parameters->mode != dereplication) && (parameters->peptidetype == cyclic) && hidescrambled && theoreticalspectrum->getExperimentalSpectrum()[i].scrambled) {
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

				visiblepeaks[visiblepeakscount].description += " (";
				if (parameters->generateisotopepattern) {
					visiblepeaks[visiblepeakscount].description += thpeaks[*it].isotopeformula;
				}
				else {
					visiblepeaks[visiblepeakscount].description += thpeaks[*it].formula.getFancySummary(thpeaks[*it].charge);
				}
				visiblepeaks[visiblepeakscount].description += ")";
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
	bool popit;
	string tmpvisibleneutralloss;
	size_t tmpposition;
	for (int i = 0; i < (int)visiblepeaks.size(); i++) {

		x = getXPositionFromMZRatio(visiblepeaks[i].mzratio, w);

		if (absoluteintensity) {
			y = visiblepeaks[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
		}
		else {
			y = visiblepeaks[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
		}

		hits.clear();

		s = visiblepeaks[i].description;
		int position = (int)s.find('@');
		size_t tmppos;
		string tmpstr;
		while (position != (int)string::npos) {
			hits.push_back(s.substr(0, position));
			s = s.substr(position + 1);

			popit = false;

			if (parameters->mode != dereplication) {

				if (visibleionseriespart1.compare("") != 0) {
					tmppos = hits.back().find(visibleionseriespart1);
					if (tmppos == string::npos) {
						popit = true;
					}
					else {
						if ((tmppos > 0) && ((hits.back()[tmppos - 1] == '_') || (hits.back()[tmppos - 1] == ' '))) {
							tmpstr = hits.back().substr(tmppos + visibleionseriespart1.size());
							while ((tmpstr.size() > 0) && (tmpstr[0] >= '0') && (tmpstr[0] <= '9')) {
								tmpstr = tmpstr.substr(1);
							}
							if (visibleionseriespart2.compare("") != 0) {
								tmpstr = tmpstr.substr(0, visibleionseriespart2.size());
								if (tmpstr.compare(visibleionseriespart2) != 0) {
									popit = true;
								}
							}
							else {
								if ((tmpstr.substr(0, 3).compare("+2H") == 0) || (tmpstr.substr(0, 3).compare("-2H") == 0)) {
									popit = true;
								}
							}
						}
						else {
							popit = true;
						}
					}
				}

				if (visibleneutralloss.compare("all") != 0) {
					if (visibleneutralloss.compare("none") == 0) {
						for (int j = 0; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {
							tmpvisibleneutralloss = "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary;
							tmpposition = hits.back().find(tmpvisibleneutralloss);
							if (tmpposition != string::npos) {
								tmpstr = hits.back().substr(tmpposition + tmpvisibleneutralloss.size());
								if ((tmpstr.size() == 0) || ((tmpstr.size() > 0) && ((tmpstr[0] == '_') || (tmpstr[0] == ' ')))) {
									popit = true;
									break;
								}
							}
						}
					}
					else {
						tmpvisibleneutralloss = "-" + visibleneutralloss;
						tmpposition = hits.back().find(tmpvisibleneutralloss);
						if (tmpposition == string::npos) {
							popit = true;
						}
						else {
							tmpstr = hits.back().substr(tmpposition + tmpvisibleneutralloss.size());
							if (!((tmpstr.size() == 0) || ((tmpstr.size() > 0) && ((tmpstr[0] == '_') || (tmpstr[0] == ' '))))) {
								popit = true;
							}
						}
					}
				}

				if (parameters->peptidetype == cyclic) {
					if (visiblepeaks[i].scrambled) {
						if (hidescrambled || (coloredrotationid != -1)) {
							popit = true;
						}
					}
					else {
						if ((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) {
							popit = true;
						}
					}
				}

				if ((parameters->peptidetype == branched) && (coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)) {
					popit = true;
				}

				if ((parameters->peptidetype == branchcyclic) && (((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) || ((coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)))) {
					popit = true;
				}

			}

			if (popit) {
				hits.pop_back();
			}

			position = (int)s.find('@');
		}

		if (s.size() > 0) {
			hits.push_back(s);

			popit = false;

			if (parameters->mode != dereplication) {

				if (visibleionseriespart1.compare("") != 0) {
					tmppos = hits.back().find(visibleionseriespart1);
					if (tmppos == string::npos) {
						popit = true;
					}
					else {
						if ((tmppos > 0) && ((hits.back()[tmppos - 1] == '_') || (hits.back()[tmppos - 1] == ' '))) {
							tmpstr = hits.back().substr(tmppos + visibleionseriespart1.size());
							while ((tmpstr.size() > 0) && (tmpstr[0] >= '0') && (tmpstr[0] <= '9')) {
								tmpstr = tmpstr.substr(1);
							}
							if (visibleionseriespart2.compare("") != 0) {
								tmpstr = tmpstr.substr(0, visibleionseriespart2.size());
								if (tmpstr.compare(visibleionseriespart2) != 0) {
									popit = true;
								}
							}
							else {
								if ((tmpstr.substr(0, 3).compare("+2H") == 0) || (tmpstr.substr(0, 3).compare("-2H") == 0)) {
									popit = true;
								}
							}
						}
						else {
							popit = true;
						}
					}
				}

				if (visibleneutralloss.compare("all") != 0) {
					if (visibleneutralloss.compare("none") == 0) {
						for (int j = 0; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {
							tmpvisibleneutralloss = "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary;
							tmpposition = hits.back().find(tmpvisibleneutralloss);
							if (tmpposition != string::npos) {
								tmpstr = hits.back().substr(tmpposition + tmpvisibleneutralloss.size());
								if ((tmpstr.size() == 0) || ((tmpstr.size() > 0) && ((tmpstr[0] == '_') || (tmpstr[0] == ' ')))) {
									popit = true;
									break;
								}
							}
						}
					}
					else {
						tmpvisibleneutralloss = "-" + visibleneutralloss;
						tmpposition = hits.back().find(tmpvisibleneutralloss);
						if (tmpposition == string::npos) {
							popit = true;
						}
						else {
							tmpstr = hits.back().substr(tmpposition + tmpvisibleneutralloss.size());
							if (!((tmpstr.size() == 0) || ((tmpstr.size() > 0) && ((tmpstr[0] == '_') || (tmpstr[0] == ' '))))) {
								popit = true;
							}
						}
					}
				}

				if (parameters->peptidetype == cyclic) {
					if (visiblepeaks[i].scrambled) {
						if (hidescrambled || (coloredrotationid != -1)) {
							popit = true;
						}
					}
					else {
						if ((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) {
							popit = true;
						}
					}
				}

				if ((parameters->peptidetype == branched) && (coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)) {
					popit = true;
				}

				if ((parameters->peptidetype == branchcyclic) && (((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) || ((coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)))) {
					popit = true;
				}

			}

			if (popit) {
				hits.pop_back();
			}
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
				if (parameters->mode == dereplication) {
					text->setTextInteractionFlags(Qt::TextBrowserInteraction);
					text->setOpenExternalLinks(true);
				}
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


	// raw data (intersections with other objects are not tested)
	if ((maxintensity > 0) && parameters->useprofiledata && ((parameters->peaklistfileformat == baf) || ((parameters->mode == dereplication) && (parameters->peaklistfileformat == imzML))) && rawdatastate && (rawdatapeaklist->size() > 0)) {

		QPainterPath rpath;
		int rx, ry, lastrx, lastry;
		bool two = false;

		size = rawdatapeaklist->size();
		for (int i = 0; i < size; i++) {
			// skip peaks which are out of range
			if (((*rawdatapeaklist)[i].mzratio < minmzratio) || ((*rawdatapeaklist)[i].mzratio > maxmzratio)) {
				continue;
			}

			rx = getXPositionFromMZRatio((*rawdatapeaklist)[i].mzratio, w);

			if (absoluteintensity) {
				ry = (maxintensity - (*rawdatapeaklist)[i].absoluteintensity) / maxintensity * (h - topmargin - bottommargin);
			}
			else {
				ry = (maxintensity - (*rawdatapeaklist)[i].relativeintensity) / maxintensity * (h - topmargin - bottommargin);
			}

			if (two) {
				if ((rx != lastrx) || (ry != lastry)) {
					rpath.lineTo(rx, ry);
				}
			}
			else {
				rpath.moveTo(rx, ry);
				two = true;
			}

			lastrx = rx;
			lastry = ry;
		}

		scene->addPath(rpath, QPen(Qt::gray, 1, Qt::SolidLine));

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
			minmzratio = min(minmzratio + tmpmaxmzratio - tmpminmzratio, theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio() + maxmzoverhead);
			maxmzratio = min(maxmzratio + tmpmaxmzratio - tmpminmzratio, theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio() + maxmzoverhead);
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


void cSpectrumSceneWidget::rawDataStateChanged(bool state) {
	rawdatastate = state;
	redrawScene();
}


void cSpectrumSceneWidget::setMZInterval(double minmz, double maxmz) {
	if (maxmz < minmz) {
		double tmp = maxmz;
		maxmz = minmz;
		minmz = tmp;
	}

	minmzratio = std::max(0.0, minmz);
	maxmzratio = std::min(maxmz, theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio() + maxmzoverhead);
	emit updateMZInterval(minmzratio, maxmzratio);

	redrawScene();
}


void cSpectrumSceneWidget::resetMZInterval() {
	minmzratio = 0;
	maxmzratio = theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio() + maxmzoverhead;
	emit updateMZInterval(minmzratio, maxmzratio);

	redrawScene();
}


void cSpectrumSceneWidget::ionSeriesChanged(QString text) {
	if (!parameters) {
		visibleionseriespart1 = "";
		visibleionseriespart2 = "";
		return;
	}

	if (text.compare("all") == 0) {
		visibleionseriespart1 = "";
		visibleionseriespart2 = "";
	}
	else {
		visibleionseriespart1 = text.toStdString().substr(0, 2);
		visibleionseriespart2 = "";
		if (text.toStdString().size() > 2) {
			visibleionseriespart2 = text.toStdString().substr(2);
		}
	}

	redrawScene();
}


void cSpectrumSceneWidget::neutralLossChanged(QString text) {
	if (!parameters) {
		visibleneutralloss = "all";
		return;
	}

	visibleneutralloss = text.toStdString();

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

