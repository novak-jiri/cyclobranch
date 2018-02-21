#include "gui/cCyclicWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>


cCyclicWidget::cCyclicWidget() {
	parameters = 0;
	theoreticalspectrum = 0;
	visiblerotationid = -1;
}


void cCyclicWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;
}


/*
void cCyclicWidget::paintEvent(QPaintEvent *event) {
	string s;

	// count number of lines to draw
	bool skip;
	int linenumber = 0;
	for (int i = 0; i < (int)theoreticalspectrum->getVisualCoverage().size(); i++) {
		
		skip = true;
		for (int j = 0; j < (int)theoreticalspectrum->getVisualCoverage()[i].series.size(); j++) {
			if (theoreticalspectrum->getVisualCoverage()[i].series[j] > 0) {
				skip = false;
				break;
			}
		}

		if (skip) {
			continue;
		}

		linenumber++;
	}
	
	if ((theoreticalspectrum->getAcronyms().size() == 0) || (linenumber == 0)) {
		return;
	}

	QPainter painter(this);
	const int topmargin = 80;
	const int leftmargin = 60;
	const int bottommargin = 50;
	const int rightmargin = 50;
	const int maskmargin = 30;
	const int verticalstep = (height() - topmargin - bottommargin)/linenumber;
	const int horizontalstep = (width() - leftmargin - rightmargin)/(int)theoreticalspectrum->getAcronyms().size();
 
    QFont myFont("Courier", 9);

    painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
	painter.setFont(myFont);

	// draw candidate
	int r = (int)theoreticalspectrum->getAcronyms().size();
	int hint = (int)theoreticalspectrum->getVisualCoverage().size()/(2*r);
	for (int i = 0; i < r; i++) {
		painter.drawLine(leftmargin + horizontalstep*i, topmargin - 30, leftmargin + horizontalstep*i, topmargin - 10);
		painter.drawText(leftmargin + horizontalstep*i, topmargin - 30, horizontalstep, 20, Qt::AlignCenter, theoreticalspectrum->getAcronyms()[i].c_str());
		
		s = theoreticalspectrum->getVisualCoverage()[i*hint].name.substr(0, theoreticalspectrum->getVisualCoverage()[i*hint].name.rfind('_'));
		painter.drawText(leftmargin + horizontalstep*i - horizontalstep/2, topmargin - 80, horizontalstep, 30, Qt::AlignCenter, s.c_str());
		if (i == 0) {
			painter.drawText(leftmargin + horizontalstep*r - horizontalstep/2, topmargin - 80, horizontalstep, 30, Qt::AlignCenter, s.c_str());
		}

		if (i == 0) {
			s = theoreticalspectrum->getVisualCoverage()[r*hint].name.substr(0, theoreticalspectrum->getVisualCoverage()[r*hint].name.rfind('_'));
		}
		else {
			s = theoreticalspectrum->getVisualCoverage()[(2*r - i)*hint].name.substr(0, theoreticalspectrum->getVisualCoverage()[(2*r - i)*hint].name.rfind('_'));
		}
		painter.drawText(leftmargin + horizontalstep*i - horizontalstep/2, topmargin - 60, horizontalstep, 30, Qt::AlignCenter, s.c_str());
		if (i == 0) {
			painter.drawText(leftmargin + horizontalstep*r - horizontalstep/2, topmargin - 60, horizontalstep, 30, Qt::AlignCenter, s.c_str());
		}
	}
	painter.drawLine(leftmargin + horizontalstep*r, topmargin - 30, leftmargin + horizontalstep*r, topmargin - 10);
	
	painter.drawText(0, topmargin - 80, 20, 30, Qt::AlignCenter, ">>>");
	painter.drawText(0, topmargin - 60, 20, 30, Qt::AlignCenter, "<<<");

	// draw lines
	linenumber = 0;
	for (int i = 0; i < (int)theoreticalspectrum->getVisualCoverage().size(); i++) {

		skip = true;
		for (int j = 0; j < (int)theoreticalspectrum->getVisualCoverage()[i].series.size(); j++) {
			if (theoreticalspectrum->getVisualCoverage()[i].series[j] > 0) {
				skip = false;
				break;
			}
		}

		if (skip) {
			continue;
		}
		
		painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine));
		for (int j = 0; j < (int)theoreticalspectrum->getVisualCoverage()[i].series.size(); j++) {
			if (theoreticalspectrum->getVisualCoverage()[i].series[j] > 0) {
				painter.setBrush(QBrush(Qt::blue, Qt::SolidPattern));					
			}
			else {
				painter.setBrush(QBrush(Qt::red, Qt::NoBrush));
			}
			painter.drawRect(leftmargin + horizontalstep*j, topmargin + verticalstep*linenumber, horizontalstep, verticalstep);
		}

		painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
		painter.setBrush(QBrush(Qt::red, Qt::NoBrush));

		painter.drawText(0, topmargin + verticalstep*linenumber + verticalstep/2 - 9, 80, 20, Qt::AlignLeft, theoreticalspectrum->getVisualCoverage()[i].name.c_str());

		linenumber++;

	}

}
*/


void cCyclicWidget::paintEvent(QPaintEvent *event) {

	if (theoreticalspectrum->getVisualCoverage().size() == 0) {
		return;
	}

	QPainter painter(this);
	const int topmargin = 20;//max(20,(height() - 80)/2);
	const int leftmargin = 20;
	const int bottommargin = 80;
	const int rightmargin = 20;

	int size = (int)theoreticalspectrum->getAcronyms().size();
	const double pi = 3.141592653589793;
	double angle = 2*pi/(double)size;
	int centerx = width()/2;
	int centery = height()/2;
	int radius = min(width() - leftmargin - rightmargin, height() - topmargin - bottommargin)/2;
	double cumulativeangle;

	const int horizontalstep = (width() - leftmargin - rightmargin)/std::max(size, 1);

	QFont myFont("Courier", 9);
	painter.setFont(myFont);

	painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	painter.drawEllipse(QPointF(centerx,centery), radius, radius);

	int xsize = horizontalstep*2/3;
	for (int i = 0; i < size; i++) {
		cumulativeangle = angle*(double)i;
		if (cumulativeangle < pi/2) {
			painter.setBrush(QBrush(palette().color(QPalette::Background), Qt::SolidPattern));
			painter.drawRect(centerx + sin(cumulativeangle)*radius - xsize/2, centery - sin(pi/2 - cumulativeangle)*radius - 10, xsize, 20);
			painter.setBrush(QBrush(palette().color(QPalette::Background), Qt::NoBrush));
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			painter.drawText(centerx + sin(cumulativeangle)*radius - xsize/2, centery - sin(pi/2 - cumulativeangle)*radius - 10, xsize, 20, Qt::AlignCenter, theoreticalspectrum->getAcronyms()[i].c_str());
			painter.drawText(centerx + sin(cumulativeangle)*radius - xsize/2, centery - sin(pi/2 - cumulativeangle)*radius - 30, xsize, 20, Qt::AlignLeft, to_string(i + 1).c_str());
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawRect(centerx + sin(cumulativeangle)*radius - xsize/2, centery - sin(pi/2 - cumulativeangle)*radius - 10, xsize, 20);
		}
		else if ((cumulativeangle >= pi/2) && (cumulativeangle <= pi)) {
			painter.setBrush(QBrush(palette().color(QPalette::Background), Qt::SolidPattern));
			painter.drawRect(centerx + sin(pi - cumulativeangle)*radius - xsize/2, centery + sin(cumulativeangle - pi/2)*radius - 10, xsize, 20);
			painter.setBrush(QBrush(palette().color(QPalette::Background), Qt::NoBrush));
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			painter.drawText(centerx + sin(pi - cumulativeangle)*radius - xsize/2, centery + sin(cumulativeangle - pi/2)*radius - 10, xsize, 20, Qt::AlignCenter, theoreticalspectrum->getAcronyms()[i].c_str());
			painter.drawText(centerx + sin(pi - cumulativeangle)*radius - xsize/2, centery + sin(cumulativeangle - pi/2)*radius - 30, xsize, 20, Qt::AlignLeft, to_string(i + 1).c_str());
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawRect(centerx + sin(pi - cumulativeangle)*radius - xsize/2, centery + sin(cumulativeangle - pi/2)*radius - 10, xsize, 20);
		}
		else if ((cumulativeangle >= pi) && (cumulativeangle <= 3*pi/2)) {
			painter.setBrush(QBrush(palette().color(QPalette::Background), Qt::SolidPattern));
			painter.drawRect(centerx - sin(cumulativeangle - pi)*radius - xsize/2, centery + sin(3*pi/2 - cumulativeangle)*radius - 10, xsize, 20);
			painter.setBrush(QBrush(palette().color(QPalette::Background), Qt::NoBrush));
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			painter.drawText(centerx - sin(cumulativeangle - pi)*radius - xsize/2, centery + sin(3*pi/2 - cumulativeangle)*radius - 10, xsize, 20, Qt::AlignCenter, theoreticalspectrum->getAcronyms()[i].c_str());
			painter.drawText(centerx - sin(cumulativeangle - pi)*radius - xsize/2, centery + sin(3*pi/2 - cumulativeangle)*radius - 30, xsize, 20, Qt::AlignLeft, to_string(i + 1).c_str());
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawRect(centerx - sin(cumulativeangle - pi)*radius - xsize/2, centery + sin(3*pi/2 - cumulativeangle)*radius - 10, xsize, 20);
		}
		else {
			painter.setBrush(QBrush(palette().color(QPalette::Background), Qt::SolidPattern));
			painter.drawRect(centerx - sin(2*pi - cumulativeangle)*radius - xsize/2, centery - sin(cumulativeangle - 3*pi/2)*radius - 10, xsize, 20);
			painter.setBrush(QBrush(palette().color(QPalette::Background), Qt::NoBrush));
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			painter.drawText(centerx - sin(2*pi - cumulativeangle)*radius - xsize/2, centery - sin(cumulativeangle - 3*pi/2)*radius - 10, xsize, 20, Qt::AlignCenter, theoreticalspectrum->getAcronyms()[i].c_str());
			painter.drawText(centerx - sin(2*pi - cumulativeangle)*radius - xsize/2, centery - sin(cumulativeangle - 3*pi/2)*radius - 30, xsize, 20, Qt::AlignLeft, to_string(i + 1).c_str());
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawRect(centerx - sin(2*pi - cumulativeangle)*radius - xsize/2, centery - sin(cumulativeangle - 3*pi/2)*radius - 10, xsize, 20);
		}
	}

	painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
	int linesize = 20;
	int cornerlinesize = horizontalstep/8;
	for (int i = 0; i < size; i++) {
		cumulativeangle = angle*(double)i + angle/(double)2;
		if (cumulativeangle < pi/2) {
			QPoint p1(centerx + sin(cumulativeangle)*(radius - linesize), centery - sin(pi/2 - cumulativeangle)*(radius - linesize));
			QPoint p2(centerx + sin(cumulativeangle)*(radius + linesize), centery - sin(pi/2 - cumulativeangle)*(radius + linesize));
			QPoint p3(centerx + sin(cumulativeangle)*(radius + linesize) - sin(pi/2 - cumulativeangle)*cornerlinesize, centery - sin(pi/2 - cumulativeangle)*(radius + linesize) - sin(cumulativeangle)*cornerlinesize);
			QPoint p4(centerx + sin(cumulativeangle)*(radius - linesize) + sin(pi/2 - cumulativeangle)*cornerlinesize, centery - sin(pi/2 - cumulativeangle)*(radius - linesize) + sin(cumulativeangle)*cornerlinesize);
			painter.drawLine(p1, p2); 
			painter.drawLine(p3, p2); 
			painter.drawLine(p1, p4); 
		}
		else if ((cumulativeangle >= pi/2) && (cumulativeangle <= pi)) {
			QPoint p1(centerx + sin(pi - cumulativeangle)*(radius - linesize), centery + sin(cumulativeangle - pi/2)*(radius - linesize));
			QPoint p2(centerx + sin(pi - cumulativeangle)*(radius + linesize), centery + sin(cumulativeangle - pi/2)*(radius + linesize));
			QPoint p3(centerx + sin(pi - cumulativeangle)*(radius + linesize) + sin(cumulativeangle - pi/2)*cornerlinesize, centery + sin(cumulativeangle - pi/2)*(radius + linesize) - sin(pi - cumulativeangle)*cornerlinesize);
			QPoint p4(centerx + sin(pi - cumulativeangle)*(radius - linesize) - sin(cumulativeangle - pi/2)*cornerlinesize, centery + sin(cumulativeangle - pi/2)*(radius - linesize) + sin(pi - cumulativeangle)*cornerlinesize);
			painter.drawLine(p1, p2); 
			painter.drawLine(p3, p2); 
			painter.drawLine(p1, p4); 
		}
		else if ((cumulativeangle >= pi) && (cumulativeangle <= 3*pi/2)) {
			QPoint p1(centerx - sin(cumulativeangle - pi)*(radius - linesize), centery + sin(3*pi/2 - cumulativeangle)*(radius - linesize));
			QPoint p2(centerx - sin(cumulativeangle - pi)*(radius + linesize), centery + sin(3*pi/2 - cumulativeangle)*(radius + linesize));
			QPoint p3(centerx - sin(cumulativeangle - pi)*(radius + linesize) + sin(3*pi/2 - cumulativeangle)*cornerlinesize, centery + sin(3*pi/2 - cumulativeangle)*(radius + linesize) + sin(cumulativeangle - pi)*cornerlinesize);
			QPoint p4(centerx - sin(cumulativeangle - pi)*(radius - linesize) - sin(3*pi/2 - cumulativeangle)*cornerlinesize, centery + sin(3*pi/2 - cumulativeangle)*(radius - linesize) - sin(cumulativeangle - pi)*cornerlinesize);
			painter.drawLine(p1, p2); 
			painter.drawLine(p3, p2); 
			painter.drawLine(p1, p4); 
		}
		else {
			QPoint p1(centerx - sin(2*pi - cumulativeangle)*(radius - linesize), centery - sin(cumulativeangle - 3*pi/2)*(radius - linesize));
			QPoint p2(centerx - sin(2*pi - cumulativeangle)*(radius + linesize), centery - sin(cumulativeangle - 3*pi/2)*(radius + linesize));
			QPoint p3(centerx - sin(2*pi - cumulativeangle)*(radius + linesize) - sin(cumulativeangle - 3*pi/2)*cornerlinesize, centery - sin(cumulativeangle - 3*pi/2)*(radius + linesize) + sin(2*pi - cumulativeangle)*cornerlinesize);
			QPoint p4(centerx - sin(2*pi - cumulativeangle)*(radius - linesize) + sin(cumulativeangle - 3*pi/2)*cornerlinesize, centery - sin(cumulativeangle - 3*pi/2)*(radius - linesize) - sin(2*pi - cumulativeangle)*cornerlinesize);
			painter.drawLine(p1, p2); 
			painter.drawLine(p3, p2); 
			painter.drawLine(p1, p4); 
		}		
	}
		
	unordered_set<cIonLabel, hash_cIonLabel> labels;
	labels.clear();

	string name;
	int shiftedposition = 0;
	int len = (int)theoreticalspectrum->getVisualCoverage()[0].series.size();
	for (int i = 0; i < 2*size; i++) {
		if ((visiblerotationid == -1) || (visiblerotationid == i)) {
			for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
				for (int k = 0; k < len; k++) {
					if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
						name = theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].name.substr(0, theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].name.find('_') + 1);
						name += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
						if (i < size) {
							cumulativeangle = angle*(double)((i + k) % size) + angle/(double)2;
							if (cumulativeangle < pi/2) {
								QPoint p3(centerx + sin(cumulativeangle)*(radius + linesize) - sin(pi/2 - cumulativeangle)*cornerlinesize, centery - sin(pi/2 - cumulativeangle)*(radius + linesize) - sin(cumulativeangle)*cornerlinesize);
								insertLabel(labels, p3.x() + 10, p3.y() - 10, name, false);
							}
							else if ((cumulativeangle >= pi/2) && (cumulativeangle <= pi)) {
								QPoint p3(centerx + sin(pi - cumulativeangle)*(radius + linesize) + sin(cumulativeangle - pi/2)*cornerlinesize, centery + sin(cumulativeangle - pi/2)*(radius + linesize) - sin(pi - cumulativeangle)*cornerlinesize);
								insertLabel(labels, p3.x() + 10, p3.y() - 10, name, false);
							}
							else if ((cumulativeangle >= pi) && (cumulativeangle <= 3*pi/2)) {
								QPoint p3(centerx - sin(cumulativeangle - pi)*(radius + linesize) + sin(3*pi/2 - cumulativeangle)*cornerlinesize, centery + sin(3*pi/2 - cumulativeangle)*(radius + linesize) + sin(cumulativeangle - pi)*cornerlinesize);
								insertLabel(labels, p3.x() - 10, p3.y() - 10, name, true);
							}
							else {
								QPoint p3(centerx - sin(2*pi - cumulativeangle)*(radius + linesize) - sin(cumulativeangle - 3*pi/2)*cornerlinesize, centery - sin(cumulativeangle - 3*pi/2)*(radius + linesize) + sin(2*pi - cumulativeangle)*cornerlinesize);
								insertLabel(labels, p3.x() - 10, p3.y() - 10, name, true);
							}
						}
						else {
							cumulativeangle = angle*(double)((2*size - i - 1 + size - k - 1) % size) + angle/(double)2;
							if (cumulativeangle < pi/2) {
								QPoint p4(centerx + sin(cumulativeangle)*(radius - linesize) + sin(pi/2 - cumulativeangle)*cornerlinesize, centery - sin(pi/2 - cumulativeangle)*(radius - linesize) + sin(cumulativeangle)*cornerlinesize);
								insertLabel(labels, p4.x() - 5, p4.y() - 5, name, true);
							}
							else if ((cumulativeangle >= pi/2) && (cumulativeangle <= pi)) {
								QPoint p4(centerx + sin(pi - cumulativeangle)*(radius - linesize) - sin(cumulativeangle - pi/2)*cornerlinesize, centery + sin(cumulativeangle - pi/2)*(radius - linesize) + sin(pi - cumulativeangle)*cornerlinesize);
								insertLabel(labels, p4.x() - 5, p4.y() - 5, name, true);
							}
							else if ((cumulativeangle >= pi) && (cumulativeangle <= 3*pi/2)) {
								QPoint p4(centerx - sin(cumulativeangle - pi)*(radius - linesize) - sin(3*pi/2 - cumulativeangle)*cornerlinesize, centery + sin(3*pi/2 - cumulativeangle)*(radius - linesize) - sin(cumulativeangle - pi)*cornerlinesize);
								insertLabel(labels, p4.x() + 10, p4.y() - 10, name, false);
							}
							else {
								QPoint p4(centerx - sin(2*pi - cumulativeangle)*(radius - linesize) + sin(cumulativeangle - 3*pi/2)*cornerlinesize, centery - sin(cumulativeangle - 3*pi/2)*(radius - linesize) - sin(2*pi - cumulativeangle)*cornerlinesize);
								insertLabel(labels, p4.x() + 10, p4.y() - 10, name, false);
							}
						}		
					}
				}
			}
		}
	}

	painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
	for (auto it = labels.begin(); it != labels.end(); ++it) {
		if (it->alignright) {
			painter.drawText(it->x - (int)it->label.size()*7, it->y, (int)it->label.size()*7, 20, Qt::AlignLeft, it->label.c_str());
		}
		else {
			painter.drawText(it->x, it->y, width(), 20, Qt::AlignLeft, it->label.c_str());
		}
	}
	
}


void cCyclicWidget::rotationChanged(int index) {
	visiblerotationid = index - 1;
	repaint();
}

