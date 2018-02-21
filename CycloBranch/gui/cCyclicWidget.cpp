#include "gui/cCyclicWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>


void paintCircle(QPainter& painter, QColor& brushcolor, vector<string>& acronymsofblocks, int centerx, int centery, int radius, double angle, int horizontalstep, int linesize, int cornerlinesize) {
	double cumulativeangle;

	painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	painter.drawEllipse(QPointF(centerx,centery), radius, radius);

	int xsize = horizontalstep*2/3;
	string blockid;
	for (int i = 0; i < (int)acronymsofblocks.size(); i++) {
		cumulativeangle = angle*(double)(i + 1);
		blockid = to_string(i + 1);
		if (cumulativeangle < pi/2) {
			painter.setBrush(QBrush(brushcolor, Qt::SolidPattern));
			painter.drawRect(centerx + sin(cumulativeangle)*radius - xsize/2, centery - sin(pi/2 - cumulativeangle)*radius - 10, xsize, 20);
			painter.setBrush(QBrush(brushcolor, Qt::NoBrush));
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			painter.drawText(centerx + sin(cumulativeangle)*radius - xsize/2, centery - sin(pi/2 - cumulativeangle)*radius - 10, xsize, 20, Qt::AlignCenter, acronymsofblocks[i].c_str());
			painter.drawText(centerx + sin(cumulativeangle)*radius - xsize/2, centery - sin(pi/2 - cumulativeangle)*radius - 30, xsize, 20, Qt::AlignLeft, blockid.c_str());
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawRect(centerx + sin(cumulativeangle)*radius - xsize/2, centery - sin(pi/2 - cumulativeangle)*radius - 10, xsize, 20);
		}
		else if ((cumulativeangle >= pi/2) && (cumulativeangle <= pi)) {
			painter.setBrush(QBrush(brushcolor, Qt::SolidPattern));
			painter.drawRect(centerx + sin(pi - cumulativeangle)*radius - xsize/2, centery + sin(cumulativeangle - pi/2)*radius - 10, xsize, 20);
			painter.setBrush(QBrush(brushcolor, Qt::NoBrush));
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			painter.drawText(centerx + sin(pi - cumulativeangle)*radius - xsize/2, centery + sin(cumulativeangle - pi/2)*radius - 10, xsize, 20, Qt::AlignCenter, acronymsofblocks[i].c_str());
			painter.drawText(centerx + sin(pi - cumulativeangle)*radius - xsize/2, centery + sin(cumulativeangle - pi/2)*radius - 30, xsize, 20, Qt::AlignLeft, blockid.c_str());
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawRect(centerx + sin(pi - cumulativeangle)*radius - xsize/2, centery + sin(cumulativeangle - pi/2)*radius - 10, xsize, 20);
		}
		else if ((cumulativeangle >= pi) && (cumulativeangle <= 3*pi/2)) {
			painter.setBrush(QBrush(brushcolor, Qt::SolidPattern));
			painter.drawRect(centerx - sin(cumulativeangle - pi)*radius - xsize/2, centery + sin(3*pi/2 - cumulativeangle)*radius - 10, xsize, 20);
			painter.setBrush(QBrush(brushcolor, Qt::NoBrush));
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			painter.drawText(centerx - sin(cumulativeangle - pi)*radius - xsize/2, centery + sin(3*pi/2 - cumulativeangle)*radius - 10, xsize, 20, Qt::AlignCenter, acronymsofblocks[i].c_str());
			painter.drawText(centerx - sin(cumulativeangle - pi)*radius - xsize/2, centery + sin(3*pi/2 - cumulativeangle)*radius - 30, xsize, 20, Qt::AlignLeft, blockid.c_str());
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawRect(centerx - sin(cumulativeangle - pi)*radius - xsize/2, centery + sin(3*pi/2 - cumulativeangle)*radius - 10, xsize, 20);
		}
		else {
			painter.setBrush(QBrush(brushcolor, Qt::SolidPattern));
			painter.drawRect(centerx - sin(2*pi - cumulativeangle)*radius - xsize/2, centery - sin(cumulativeangle - 3*pi/2)*radius - 10, xsize, 20);
			painter.setBrush(QBrush(brushcolor, Qt::NoBrush));
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			painter.drawText(centerx - sin(2*pi - cumulativeangle)*radius - xsize/2, centery - sin(cumulativeangle - 3*pi/2)*radius - 10, xsize, 20, Qt::AlignCenter, acronymsofblocks[i].c_str());
			painter.drawText(centerx - sin(2*pi - cumulativeangle)*radius - xsize/2, centery - sin(cumulativeangle - 3*pi/2)*radius - 30, xsize, 20, Qt::AlignLeft, blockid.c_str());
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawRect(centerx - sin(2*pi - cumulativeangle)*radius - xsize/2, centery - sin(cumulativeangle - 3*pi/2)*radius - 10, xsize, 20);
		}
	}

	painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
	for (int i = 0; i < (int)acronymsofblocks.size(); i++) {
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
}


void generateCyclicLabelsToRight(bool nterminal, int rotationid, int rotationstart, int fragmentstart, int fragmentend, int numberofringblocks, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int centery, int radius, double angle, int linesize, int cornerlinesize, int visiblerotationid, int branchstart, int branchend) {
	double cumulativeangle;
	string name;
	int m;
	if ((visiblerotationid == -1) || ((parameters->peptidetype == cyclic) && (visiblerotationid == rotationid)) || ((parameters->peptidetype == lasso) && (visiblerotationid == rotationid/6))) {
		for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
			if ((nterminal && parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) || (!nterminal && parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal)) {
				m = 0;
				for (int j = fragmentstart; j < fragmentend; j++) {	
					if ((branchstart == -1) || (branchend == -1) || ((branchstart >= 0) && (j < branchstart)) || ((branchend >= 0) && (j >= branchend))) {
						if (theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].series[j] > 0) {
							name = theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].name.substr(0, theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].name.rfind('_') + 1);
							name += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name[0] + to_string(j + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name.substr(1);
							cumulativeangle = angle*(double)((rotationstart + m + 1) % numberofringblocks) + angle/(double)2;
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
						m++;
					}
				}
			}
		}
	}
}


void generateCyclicLabelsToLeft(bool nterminal, int rotationid, int rotationstart, int fragmentstart, int fragmentend, int numberofringblocks, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int centery, int radius, double angle, int linesize, int cornerlinesize, int visiblerotationid, int branchstart, int branchend) {
	double cumulativeangle;
	string name;
	int m;
	if ((visiblerotationid == -1) || ((parameters->peptidetype == cyclic) && (visiblerotationid == rotationid)) || ((parameters->peptidetype == lasso) && (visiblerotationid == rotationid/6))) {
		for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
			if ((nterminal && parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) || (!nterminal && parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal)) {
				m = 0;
				for (int j = fragmentstart; j < fragmentend; j++) {		
					if ((branchstart == -1) || (branchend == -1) || ((branchstart >= 0) && (j < branchstart)) || ((branchend >= 0) && (j >= branchend))) {
						if (theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].series[j] > 0) {
							name = theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].name.substr(0, theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].name.rfind('_') + 1);
							name += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name[0] + to_string(j + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name.substr(1);
							cumulativeangle = angle*(double)((2*numberofringblocks - rotationstart + numberofringblocks - m - 1) % numberofringblocks) + angle/(double)2;
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
						m++;
					}
				}
			}
		}
	}
}


cCyclicWidget::cCyclicWidget() {
	parameters = 0;
	theoreticalspectrum = 0;
	visiblerotationid = -1;
}


void cCyclicWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;
}


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
	double angle = 2*pi/(double)size;
	int centerx = width()/2;
	int centery = height()/2;
	int radius = min(width() - leftmargin - rightmargin, height() - topmargin - bottommargin)/2;

	const int horizontalstep = (width() - leftmargin - rightmargin)/std::max(size, 1);

	QFont myFont("Courier", 9);
	painter.setFont(myFont);

	int linesize = 20;
	int cornerlinesize = horizontalstep/8;
	paintCircle(painter, (QColor &)(palette().color(QPalette::Background)), theoreticalspectrum->getAcronyms(), centerx, centery, radius, angle, horizontalstep, linesize, cornerlinesize);
		
	unordered_set<cIonLabel, hash_cIonLabel> labels;
	labels.clear();
	
	int half = (int)theoreticalspectrum->getVisualCoverage().size()/(int)parameters->fragmentionsfortheoreticalspectra.size()/2;
	for (int i = 0; i < half; i++) {
		generateCyclicLabelsToRight(true, i, i, 0, size - 1, size, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, -1, -1);
	}
	for (int i = half; i < 2*half; i++) {
		generateCyclicLabelsToLeft(true, i, i, 0, size - 1, size, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, -1, -1);
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

