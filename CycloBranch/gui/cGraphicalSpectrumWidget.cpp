#include "gui/cGraphicalSpectrumWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QScrollArea>


cGraphicalSpectrumWidget::cGraphicalSpectrumWidget(QWidget* parent) {
	this->parent = parent;
	parameters = 0;
	theoreticalspectrum = 0;
	origwidth = 0;
	origheight = 0;
	scale = 1;
	hideunmatched = false;
	hidematched = false;
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

	setDefaultSize();
}


cGraphicalSpectrumWidget::~cGraphicalSpectrumWidget() {
}


void cGraphicalSpectrumWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;
	minmzratio = 0;
	maxmzratio = theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio();
	emit updateMZInterval(minmzratio, maxmzratio);
}


void cGraphicalSpectrumWidget::paintEvent(QPaintEvent *event) {
	QPainter painter(this);

	setDefaultSize();

	painter.setFont(QFont("Arial", 8));
	painter.translate(0, 0);
	painter.scale(scale, scale);
	painter.setPen(QPen(Qt::black, 1, Qt::DashLine));

	// zooming window
	if ((pressedx != -1) && (pressedy != -1)) {
		painter.drawRect(pressedx, pressedy, currentx - pressedx, currenty - pressedy);
		QString qstr = "m/z: ";
		qstr += QString::number(getMZRatioFromXPosition((pressedx < currentx)?pressedx:currentx, origwidth));
		qstr += "-";
		qstr += QString::number(getMZRatioFromXPosition((pressedx < currentx)?currentx:pressedx, origwidth));
		painter.drawText(QPoint(pressedx, pressedy - 2), qstr);
	}
	
    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));

	int x;
	double y;

	char tmpbuf[30];
	string s;

	int w = origwidth;
	int h = origheight;

	// maximum intensity in the interval <minmzratio, maxmzratio>
	double maxintensity = theoreticalspectrum->getExperimentalSpectrum().getMaximumIntensityFromMZInterval(minmzratio, maxmzratio);

	// x axis
	painter.drawLine(QPoint(leftmargin, h - bottommargin), QPoint(w - rightmargin, h - bottommargin));

	// x axis ruler
	painter.drawLine(QPoint(leftmargin, h - bottommargin), QPoint(leftmargin, h - bottommargin + 10));
	painter.drawLine(QPoint(w - rightmargin, h - bottommargin), QPoint(w - rightmargin, h - bottommargin + 10));
	painter.drawText(QPoint(leftmargin - 2, h - bottommargin + 22), QString::number(minmzratio));
	painter.drawText(QPoint(w - rightmargin - 2, h - bottommargin + 22), QString::number(maxmzratio));

	// y axis
	painter.drawLine(QPoint(leftmargin, h - bottommargin), QPoint(leftmargin, topmargin));
	
	// y axis ruler
	painter.drawLine(QPoint(leftmargin - 10, topmargin), QPoint(leftmargin, topmargin));
	painter.drawLine(QPoint(leftmargin - 10, h - bottommargin), QPoint(leftmargin, h - bottommargin));
	drawVerticalText(&painter, QString::number(0), leftmargin - 15, h - bottommargin);
	drawVerticalText(&painter, QString::number(maxintensity), leftmargin - 15, topmargin);

	// peaks
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

		x = getXPositionFromMZRatio(theoreticalspectrum->getExperimentalSpectrum()[i].mzratio, origwidth);

		y = theoreticalspectrum->getExperimentalSpectrum()[i].intensity/maxintensity * (h - topmargin - bottommargin);

		string coloredtrotationstring;
		if (coloredtrotationid != -1) {
			if (parameters->peptidetype == branched) {
				coloredtrotationstring = " " + to_string(coloredtrotationid + 1) + "_";
			}
			if (parameters->peptidetype == lasso) {
				coloredtrotationstring = "_" + to_string(coloredtrotationid + 1) + "_";
			}
		}

		vector<string> hits;
		hits.clear();
		s = theoreticalspectrum->getExperimentalSpectrum()[i].description;
		int position = (int)s.find(',');
		while (position != string::npos) {
			hits.push_back(s.substr(0, position));
			s = s.substr(position + 1);

			if ((parameters->peptidetype == cyclic) && (coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) {
				hits.pop_back();
			}

			if ((parameters->peptidetype == branched) && (coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)) {
				hits.pop_back();
			}

			if ((parameters->peptidetype == lasso) && (((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) || ((coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)))) {
				hits.pop_back();
			}

			position = (int)s.find(',');
		}

		if (s.size() > 0) {
			hits.push_back(s);

			if ((parameters->peptidetype == cyclic) && (coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) {
				hits.pop_back();
			}

			if ((parameters->peptidetype == branched) && (coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)) {
				hits.pop_back();
			}

			if ((parameters->peptidetype == lasso) && (((coloredrotationid != -1) && (hits.back().find(coloredrotationstring) == string::npos)) || ((coloredtrotationid != -1) && (hits.back().find(coloredtrotationstring) == string::npos)))) {
				hits.pop_back();
			}
		}

		sprintf_s(tmpbuf,"%.3f\0",theoreticalspectrum->getExperimentalSpectrum()[i].mzratio);
		s = tmpbuf;
		hits.push_back(s);

		if (hits.size() > 1) {
			painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
		}
		else {
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		}

		painter.drawLine(QPoint(x, h - bottommargin - 2), QPoint(x, h - bottommargin - std::max((int)y, 3)));

		for (int i = 0; i < (int)hits.size(); i++) {
			painter.drawText(x - 2, h - bottommargin - std::max((int)y, 3) - 15*((int)hits.size() - i), width(), 20, Qt::AlignLeft, hits[i].c_str());
		}

	}	

}


void cGraphicalSpectrumWidget::wheelEvent(QWheelEvent *event) {
	if (event->delta() > 0) {
		zoomIn();
	}
	else {
		zoomOut();
	}

	event->accept();
}


void cGraphicalSpectrumWidget::mouseMoveEvent(QMouseEvent *event) {
	currentx  = event->x()/scale;
	currenty = event->y()/scale;
 
	repaint();

    event->accept();
}


void cGraphicalSpectrumWidget::mouseReleaseEvent(QMouseEvent *event) {
	if (pressedx == currentx) {
		pressedx = -1;
		currentx = -1;

		repaint();
	}
    
	if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {

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

		double tmpminmzratio = getMZRatioFromXPosition((pressedx < currentx)?pressedx:currentx, origwidth);
		double tmpmaxmzratio = getMZRatioFromXPosition((pressedx < currentx)?currentx:pressedx, origwidth);

		minmzratio = tmpminmzratio;
		maxmzratio = tmpmaxmzratio;

		emit updateMZInterval(minmzratio, maxmzratio);

		pressedx = -1;
		pressedy = -1;

		repaint();
	
	}

	event->accept();
}


void cGraphicalSpectrumWidget::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		pressedx = event->x()/scale;
		pressedy = event->y()/scale;

		currentx = pressedx;
		currenty = pressedy;

		repaint();
	}

	if (event->button() == Qt::RightButton) {
		pressedx = -1;
		pressedy = -1;

		repaint();
	}

	if (event->button() == Qt::MiddleButton) {
		pressedx = -1;
		pressedy = -1;

		resetMZInterval();

		repaint();
	}

	event->accept();
}


void cGraphicalSpectrumWidget::setDefaultSize() {
	if ((origwidth != parent->width() - 2) || (origheight != parent->height() - 2)) {
		origwidth = parent->width() - 2;
		origheight = parent->height() - 2;
		setFixedSize(origwidth*scale, origheight*scale);
	}
}


double cGraphicalSpectrumWidget::getMZRatioFromXPosition(int x, int w) {
	return (double)(x - leftmargin)/(double)(w - leftmargin - rightmargin)*(maxmzratio - minmzratio) + minmzratio;
}


int cGraphicalSpectrumWidget::getXPositionFromMZRatio(double mzratio, int w) {
	double val = mzratio - minmzratio;
	val /= maxmzratio - minmzratio;
	val *= double(w - leftmargin - rightmargin);
	return (int)val + leftmargin;
}


void cGraphicalSpectrumWidget::drawVerticalText(QPainter* painter, QString text, int x, int y) {
	painter->save();
	painter->translate(x, y);
	painter->rotate(-90);
	painter->drawText(QPoint(0, 0), text);
	painter->restore();
}


void cGraphicalSpectrumWidget::hideUnmatchedPeaks(int state) {
	if (state == Qt::Unchecked) {
		hideunmatched = false;
	}
	else {
		hideunmatched = true;
	}

	repaint();
}


void cGraphicalSpectrumWidget::hideMatchedPeaks(int state) {
	if (state == Qt::Unchecked) {
		hidematched = false;
	}
	else {
		hidematched = true;
	}

	repaint();
}


void cGraphicalSpectrumWidget::zoomIn() {
	if (scale < 32) {
		scale += factor;
		setFixedSize(origwidth*scale, origheight*scale);
		repaint();
		((QScrollArea *)parent)->verticalScrollBar()->setSliderPosition(((QScrollArea *)parent)->verticalScrollBar()->maximum());
	}
}


void cGraphicalSpectrumWidget::zoomOut() {
	if (scale > 1) {
		scale -= factor;
		setFixedSize(origwidth*scale, origheight*scale);
		repaint();
		((QScrollArea *)parent)->verticalScrollBar()->setSliderPosition(((QScrollArea *)parent)->verticalScrollBar()->maximum());
	}
}


void cGraphicalSpectrumWidget::normalSize() {
	scale = 1;
	setFixedSize(origwidth, origheight);
	repaint();
}


void cGraphicalSpectrumWidget::setMZInterval(double minmz, double maxmz) {
	if (maxmz < minmz) {
		double tmp = maxmz;
		maxmz = minmz;
		minmz = tmp;
	}

	minmzratio = std::max(0.0, minmz);
	maxmzratio = std::min(maxmz, theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio());
	emit updateMZInterval(minmzratio, maxmzratio);
	repaint();
}


void cGraphicalSpectrumWidget::resetMZInterval() {
	minmzratio = 0;
	maxmzratio = theoreticalspectrum->getExperimentalSpectrum().getMaximumMZRatio();
	emit updateMZInterval(minmzratio, maxmzratio);
	repaint();
}


void cGraphicalSpectrumWidget::rotationChanged(int index) {
	coloredrotationid = index - 1;
	repaint();
}


void cGraphicalSpectrumWidget::trotationChanged(int index) {
	coloredtrotationid = index - 1;
	repaint();
}


void cGraphicalSpectrumWidget::rotationChanged(QString text) {
	coloredrotationstring = " " + text.toStdString() + "_";
	repaint();
}

