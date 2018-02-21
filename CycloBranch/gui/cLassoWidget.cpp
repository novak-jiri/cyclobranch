#include "gui/cLassoWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>


void generateBranchLabelsDown(bool nterminal, int rotationid, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int topmargin, int horizontalstep, int verticalstep, int visiblerotationid, int branchstart) {
	string name;
	int j;
	if ((visiblerotationid == -1) || (visiblerotationid == rotationid/6)) {
		for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
			if ((nterminal && parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) || (!nterminal && parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal)) {
				j = 0;
				while (j < branchstart) {
					if (theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].series[j] > 0) {
						name = theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].name.substr(0, theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].name.rfind('_') + 1);
						name += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name[0] + to_string(j + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name.substr(1);
						insertLabel(labels, centerx + 25, topmargin + verticalstep*j + verticalstep/2 - verticalstep/8, name, false);
					}
					j++;
				}
			}
		}
	}
}


void generateBranchLabelsUp(bool nterminal, int rotationid, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int topmargin, int horizontalstep, int verticalstep, int visiblerotationid, int branchend) {
	string name;
	int j;
	int count;
	if ((visiblerotationid == -1) || (visiblerotationid == rotationid/6)) {
		for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
			if ((nterminal && parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) || (!nterminal && parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal)) {
				j = 0;
				count = (int)theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].series.size() - branchend;
				while (j < count) {
					if (theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].series[j + branchend] > 0) {
						name = theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].name.substr(0, theoreticalspectrum->getVisualCoverage()[rotationid*parameters->fragmentionsfortheoreticalspectra.size() + i].name.rfind('_') + 1);
						name += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name[0] + to_string(j + branchend + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name.substr(1);
						insertLabel(labels, centerx - 25, topmargin + verticalstep*(count - j - 1) + verticalstep/2, name, true);
					}
					j++;
				}
			}
		}
	}
}


cLassoWidget::cLassoWidget() {
	parameters = 0;
	theoreticalspectrum = 0;
	visiblerotationid = -1;
	visibletrotationid = -1;
}


void cLassoWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;
}


void cLassoWidget::paintEvent(QPaintEvent *event) {

	if (theoreticalspectrum->getVisualCoverage().size() == 0) {
		return;
	}

	vector<string> backboneacronyms;
	vector<string> branchacronyms;
	backboneacronyms = theoreticalspectrum->getBackboneAcronyms();
	branchacronyms = theoreticalspectrum->getBranchAcronyms();

	QPainter painter(this);
	const int topmargin = 20;
	const int leftmargin = 20;
	const int bottommargin = 80;
	const int rightmargin = 20;

	int backbonesize = (int)backboneacronyms.size();
	int branchsize = (int)branchacronyms.size();
	double angle = 2*pi/(double)backbonesize;
	int centerx = width()/2;
	int centery = height()*3/4;
	int radius = (height()/2 - bottommargin)/2;

	const int horizontalstep = (width() - leftmargin - rightmargin)/std::max(backbonesize, 1);
	const int verticalstep = (centery - radius - topmargin - 10)/std::max(branchsize, 1);

	QFont myFont("Courier", 9);
	painter.setFont(myFont);

	int linesize = 20;
	int cornerlinesize = horizontalstep/8;
	paintCircle(painter, (QColor &)(palette().color(QPalette::Background)), backboneacronyms, centerx, centery, radius, angle, horizontalstep, linesize, cornerlinesize);


	// paint the branch
	for (int i = 0; i < branchsize; i++) {
		painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		painter.drawText(centerx - horizontalstep/3, topmargin + verticalstep*i, horizontalstep*2/3, 20, Qt::AlignCenter, branchacronyms[branchsize - i - 1].c_str());
		painter.drawText(centerx - horizontalstep/3, topmargin + verticalstep*i - 20, horizontalstep*2/3, 20, Qt::AlignLeft, to_string(backbonesize + branchsize - i).c_str());

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawRect(centerx - horizontalstep/3, topmargin + verticalstep*i, horizontalstep*2/3, 20);

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawLine(centerx, topmargin + verticalstep*i + 20, centerx, topmargin + verticalstep*i + verticalstep);
		painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
		painter.drawLine(centerx - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, centerx + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);	
		painter.drawLine(centerx - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, centerx - 20, topmargin + verticalstep*i + (verticalstep + 10)/2 + verticalstep/8);
		painter.drawLine(centerx + 20, topmargin + verticalstep*i + (verticalstep + 10)/2 - verticalstep/8, centerx + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);

		if (i == branchsize - 1) {
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawLine(centerx, topmargin + verticalstep*i + verticalstep, centerx, centery - radius - 10);
		}
	}


	// get lasso rotations
	vector<cCandidate> lassorotations;
	theoreticalspectrum->getCandidate().getLassoRotations(lassorotations, true);

	// get T-permutations of lasso rotations
	vector<vector<TRotationInfo> > trotationsoflassorotations;
	trotationsoflassorotations.resize(lassorotations.size());
	for (int i = 0; i < (int)lassorotations.size(); i++) {
		lassorotations[i].getPermutationsOfBranches(trotationsoflassorotations[i]);
	}

	unordered_set<cIonLabel, hash_cIonLabel> labels;
	labels.clear();

	int rotationid;
	int half = (int)theoreticalspectrum->getVisualCoverage().size()/(int)parameters->fragmentionsfortheoreticalspectra.size()/2;
	for (int i = 0; i < half; i++) {
		
		if ((visibletrotationid != -1) && (visibletrotationid != i % 6)) {
			continue;
		}
		
		if (i/6 == 0) {
			rotationid = half/6;
		}
		else {
			rotationid = 2*half/6 - i/6;
		}
		
		switch (i % 6)
		{
		case 0:
			generateCyclicLabelsToRight(true, i, i/6, 0, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][0].middlebranchstart, trotationsoflassorotations[i/6][0].middlebranchend);
			break;
		case 1:
			generateBranchLabelsDown(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][1].middlebranchstart);
			generateCyclicLabelsToRight(true, i, -1, trotationsoflassorotations[i/6][1].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][1].middlebranchstart, trotationsoflassorotations[i/6][1].middlebranchend);
			break;
		case 2:
			generateCyclicLabelsToRight(true, rotationid*6 + 2, i/6, 0, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[rotationid][2].middlebranchstart, trotationsoflassorotations[rotationid][2].middlebranchend);
			break;
		case 3:
			generateCyclicLabelsToRight(true, i, i/6, 0, trotationsoflassorotations[i/6][3].middlebranchstart, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][3].middlebranchstart, trotationsoflassorotations[i/6][3].middlebranchend);
			generateBranchLabelsUp(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][3].middlebranchend);
			generateBranchLabelsDown(false, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][4].middlebranchstart);
			generateCyclicLabelsToLeft(false, i, 0, trotationsoflassorotations[i/6][4].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][4].middlebranchstart, trotationsoflassorotations[i/6][4].middlebranchend);
			break;
		case 4:
			generateBranchLabelsDown(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][4].middlebranchstart);
			generateCyclicLabelsToLeft(true, i, 0, trotationsoflassorotations[i/6][4].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][4].middlebranchstart, trotationsoflassorotations[i/6][4].middlebranchend);
			break;
		case 5:
			generateCyclicLabelsToRight(true, rotationid*6 + 5, i/6, 0, trotationsoflassorotations[rotationid][5].middlebranchstart, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[rotationid][5].middlebranchstart, trotationsoflassorotations[rotationid][5].middlebranchend);
			generateBranchLabelsUp(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][5].middlebranchend);
			generateBranchLabelsDown(false, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][1].middlebranchstart);
			generateCyclicLabelsToRight(false, i, -1, trotationsoflassorotations[i/6][1].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][1].middlebranchstart, trotationsoflassorotations[i/6][1].middlebranchend);
			break;
		default:
			break;
		}

	}

	for (int i = half; i < 2*half; i++) {

		if ((visibletrotationid != -1) && (visibletrotationid != i % 6)) {
			continue;
		}

		if (i/6 == half/6) {
			rotationid = 0;
		}
		else {
			rotationid = 2*half/6 - i/6;
		}

		switch (i % 6)
		{
		case 0:
			generateCyclicLabelsToLeft(true, i, i/6, 0, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][0].middlebranchstart, trotationsoflassorotations[i/6][0].middlebranchend);
			break;
		case 1:
			generateBranchLabelsDown(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][1].middlebranchstart);
			generateCyclicLabelsToLeft(true, i, 0, trotationsoflassorotations[i/6][1].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][1].middlebranchstart, trotationsoflassorotations[i/6][1].middlebranchend);
			break;
		case 2:
			generateCyclicLabelsToLeft(true, rotationid*6 + 2, i/6, 0, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[rotationid][2].middlebranchstart, trotationsoflassorotations[rotationid][2].middlebranchend);
			break;
		case 3:
			generateCyclicLabelsToLeft(true, i, i/6, 0, trotationsoflassorotations[i/6][3].middlebranchstart, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][3].middlebranchstart, trotationsoflassorotations[i/6][3].middlebranchend);
			generateBranchLabelsUp(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][3].middlebranchend);
			generateBranchLabelsDown(false, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][4].middlebranchstart);
			generateCyclicLabelsToRight(false, i, -1, trotationsoflassorotations[i/6][4].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][4].middlebranchstart, trotationsoflassorotations[i/6][4].middlebranchend);
			break;
		case 4:
			generateBranchLabelsDown(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][4].middlebranchstart);
			generateCyclicLabelsToRight(true, i, -1, trotationsoflassorotations[i/6][4].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][4].middlebranchstart, trotationsoflassorotations[i/6][4].middlebranchend);
			break;
		case 5:
			generateCyclicLabelsToLeft(true, rotationid*6 + 5, i/6, 0, trotationsoflassorotations[rotationid][5].middlebranchstart, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[rotationid][5].middlebranchstart, trotationsoflassorotations[rotationid][5].middlebranchend);
			generateBranchLabelsUp(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][5].middlebranchend);
			generateBranchLabelsDown(false, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsoflassorotations[i/6][1].middlebranchstart);
			generateCyclicLabelsToLeft(false, i, 0, trotationsoflassorotations[i/6][1].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsoflassorotations[i/6][1].middlebranchstart, trotationsoflassorotations[i/6][1].middlebranchend);
			break;
		default:
			break;
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


void cLassoWidget::rotationChanged(int index) {
	visiblerotationid = index - 1;
	repaint();
}


void cLassoWidget::trotationChanged(int index) {
	visibletrotationid = index - 1;
	repaint();
}

