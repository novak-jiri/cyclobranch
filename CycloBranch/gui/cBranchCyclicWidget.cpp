#include "gui/cBranchCyclicWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QPrinter>
#include <QSvgGenerator>


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


cBranchCyclicWidget::cBranchCyclicWidget() {
	parameters = 0;
	theoreticalspectrum = 0;
	visiblerotationid = -1;
	visibletrotationid = -1;
}


void cBranchCyclicWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;
}


void cBranchCyclicWidget::exportToPDF(QString filename, bool postscript) {
	QPrinter printer;
	if (postscript) {
		printer.setPaperSize(QSizeF(width() + 100, height() + 100), QPrinter::DevicePixel);
		printer.setPageMargins (50, 50, 50, 50, QPrinter::DevicePixel);
	}
	else {
		printer.setPaperSize(QSizeF(width(), height()), QPrinter::DevicePixel);
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
	paint(painter);
	painter.end();
}


void cBranchCyclicWidget::exportToPNG(QString filename) {
	QImage image(width(), height(), QImage::Format_ARGB32);
	image.fill(Qt::white);

	QPainter painter;
	if (!painter.begin(&image)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
	paint(painter);
	painter.end();
	image.save(filename);
}


void cBranchCyclicWidget::exportToSVG(QString filename) {
	QSvgGenerator generator;
	generator.setFileName(filename);
	generator.setSize(QSize(width(), height()));
	generator.setViewBox(QRect(0, 0, width(), height()));

	QPainter painter;
	if (!painter.begin(&generator)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	paint(painter);
	painter.end();
}


void cBranchCyclicWidget::paintEvent(QPaintEvent *event) {
	QPainter painter;
	painter.begin(this);
	paint(painter);
	painter.end();
}


void cBranchCyclicWidget::paint(QPainter& painter) {

	if (!theoreticalspectrum) {
		return;
	}

	vector<string> backboneacronyms;
	vector<string> branchacronyms;
	backboneacronyms = theoreticalspectrum->getCandidate().getBackboneAcronyms();
	branchacronyms = theoreticalspectrum->getCandidate().getBranchAcronyms();

	const int topmargin = 20;
	const int leftmargin = 20;
	const int bottommargin = 80;
	const int rightmargin = 20;

	int backbonesize = (int)backboneacronyms.size();
	int branchsize = (int)branchacronyms.size();

	if (branchsize < 1) {
		return;
	}

	double angle = 2*pi/(double)backbonesize;
	int centerx = width()/2;
	int centery = height()*3/4;
	int radius = (height()/2 - bottommargin)/2;

	const int horizontalstep = (width() - leftmargin - rightmargin)/std::max(backbonesize, 1);
	const int verticalstep = (centery - radius - topmargin - 10)/std::max(branchsize, 1);

	QFont myFont("Courier", 9);
	QFontMetrics fm(myFont);
	painter.setFont(myFont);

	unordered_set<cIonLabel, hash_cIonLabel> labels;
	labels.clear();

	int linesize = 20;
	int cornerlinesize = horizontalstep/8;

	paintCircle(painter, backboneacronyms, centerx, centery, radius, angle, horizontalstep, linesize, cornerlinesize, theoreticalspectrum->getVisualCoverage().size() > 0, visiblerotationid, labels);


	// paint the branch
	for (int i = 0; i < branchsize; i++) {
		painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		if (fm.width(branchacronyms[branchsize - i - 1].c_str()) > horizontalstep*2/3) {
			painter.drawText(centerx - horizontalstep/3 + 2, topmargin + verticalstep*i, horizontalstep*2/3 - 2, 20, Qt::AlignLeft|Qt::AlignVCenter, branchacronyms[branchsize - i - 1].c_str());
		}
		else {
			painter.drawText(centerx - horizontalstep/3, topmargin + verticalstep*i, horizontalstep*2/3, 20, Qt::AlignCenter, branchacronyms[branchsize - i - 1].c_str());
		}	

		if (theoreticalspectrum->getVisualCoverage().size() > 0) {
			painter.drawText(centerx - horizontalstep/3, topmargin + verticalstep*i - 20, horizontalstep*2/3, 20, Qt::AlignLeft, to_string(backbonesize + branchsize - i).c_str());
		}

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawRect(centerx - horizontalstep/3, topmargin + verticalstep*i, horizontalstep*2/3, 20);

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawLine(centerx, topmargin + verticalstep*i + 20, centerx, topmargin + verticalstep*i + verticalstep);

		if (theoreticalspectrum->getVisualCoverage().size() > 0) {
			painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
			painter.drawLine(centerx - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, centerx + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);	
			painter.drawLine(centerx - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, centerx - 20, topmargin + verticalstep*i + (verticalstep + 10)/2 + verticalstep/8);
			painter.drawLine(centerx + 20, topmargin + verticalstep*i + (verticalstep + 10)/2 - verticalstep/8, centerx + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);
		}

		if (i == branchsize - 1) {
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawLine(centerx, topmargin + verticalstep*i + verticalstep, centerx, centery - radius - 10);
		}
	}


	if (parameters && (theoreticalspectrum->getVisualCoverage().size() > 0)) {

		// get branch-cyclic rotations
		vector<cCandidate> branchcyclicrotations;
		theoreticalspectrum->getCandidate().getBranchCyclicRotations(branchcyclicrotations, true);

		// get T-permutations of branch-cyclic rotations
		vector<vector<TRotationInfo> > trotationsofbranchcyclicrotations;
		trotationsofbranchcyclicrotations.resize(branchcyclicrotations.size());
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			branchcyclicrotations[i].getPermutationsOfBranches(trotationsofbranchcyclicrotations[i]);
		}

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
				generateCyclicLabelsToRight(true, i, i/6, 0, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][0].middlebranchstart, trotationsofbranchcyclicrotations[i/6][0].middlebranchend);
				break;
			case 1:
				generateBranchLabelsDown(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][1].middlebranchstart);
				generateCyclicLabelsToRight(true, i, -1, trotationsofbranchcyclicrotations[i/6][1].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][1].middlebranchstart, trotationsofbranchcyclicrotations[i/6][1].middlebranchend);
				break;
			case 2:
				generateCyclicLabelsToRight(true, rotationid*6 + 2, i/6, 0, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[rotationid][2].middlebranchstart, trotationsofbranchcyclicrotations[rotationid][2].middlebranchend);
				break;
			case 3:
				generateCyclicLabelsToRight(true, i, i/6, 0, trotationsofbranchcyclicrotations[i/6][3].middlebranchstart, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][3].middlebranchstart, trotationsofbranchcyclicrotations[i/6][3].middlebranchend);
				generateBranchLabelsUp(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][3].middlebranchend);
				generateBranchLabelsDown(false, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][4].middlebranchstart);
				generateCyclicLabelsToLeft(false, i, 0, trotationsofbranchcyclicrotations[i/6][4].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][4].middlebranchstart, trotationsofbranchcyclicrotations[i/6][4].middlebranchend);
				break;
			case 4:
				generateBranchLabelsDown(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][4].middlebranchstart);
				generateCyclicLabelsToLeft(true, i, 0, trotationsofbranchcyclicrotations[i/6][4].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][4].middlebranchstart, trotationsofbranchcyclicrotations[i/6][4].middlebranchend);
				break;
			case 5:
				generateCyclicLabelsToRight(true, rotationid*6 + 5, i/6, 0, trotationsofbranchcyclicrotations[rotationid][5].middlebranchstart, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[rotationid][5].middlebranchstart, trotationsofbranchcyclicrotations[rotationid][5].middlebranchend);
				generateBranchLabelsUp(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][5].middlebranchend);
				generateBranchLabelsDown(false, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][1].middlebranchstart);
				generateCyclicLabelsToRight(false, i, -1, trotationsofbranchcyclicrotations[i/6][1].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][1].middlebranchstart, trotationsofbranchcyclicrotations[i/6][1].middlebranchend);
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
				generateCyclicLabelsToLeft(true, i, i/6, 0, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][0].middlebranchstart, trotationsofbranchcyclicrotations[i/6][0].middlebranchend);
				break;
			case 1:
				generateBranchLabelsDown(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][1].middlebranchstart);
				generateCyclicLabelsToLeft(true, i, 0, trotationsofbranchcyclicrotations[i/6][1].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][1].middlebranchstart, trotationsofbranchcyclicrotations[i/6][1].middlebranchend);
				break;
			case 2:
				generateCyclicLabelsToLeft(true, rotationid*6 + 2, i/6, 0, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[rotationid][2].middlebranchstart, trotationsofbranchcyclicrotations[rotationid][2].middlebranchend);
				break;
			case 3:
				generateCyclicLabelsToLeft(true, i, i/6, 0, trotationsofbranchcyclicrotations[i/6][3].middlebranchstart, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][3].middlebranchstart, trotationsofbranchcyclicrotations[i/6][3].middlebranchend);
				generateBranchLabelsUp(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][3].middlebranchend);
				generateBranchLabelsDown(false, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][4].middlebranchstart);
				generateCyclicLabelsToRight(false, i, -1, trotationsofbranchcyclicrotations[i/6][4].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][4].middlebranchstart, trotationsofbranchcyclicrotations[i/6][4].middlebranchend);
				break;
			case 4:
				generateBranchLabelsDown(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][4].middlebranchstart);
				generateCyclicLabelsToRight(true, i, -1, trotationsofbranchcyclicrotations[i/6][4].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][4].middlebranchstart, trotationsofbranchcyclicrotations[i/6][4].middlebranchend);
				break;
			case 5:
				generateCyclicLabelsToLeft(true, rotationid*6 + 5, i/6, 0, trotationsofbranchcyclicrotations[rotationid][5].middlebranchstart, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[rotationid][5].middlebranchstart, trotationsofbranchcyclicrotations[rotationid][5].middlebranchend);
				generateBranchLabelsUp(true, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][5].middlebranchend);
				generateBranchLabelsDown(false, i, labels, parameters, theoreticalspectrum, centerx, topmargin, horizontalstep, verticalstep, visiblerotationid, trotationsofbranchcyclicrotations[i/6][1].middlebranchstart);
				generateCyclicLabelsToLeft(false, i, 0, trotationsofbranchcyclicrotations[i/6][1].middlebranchend, backbonesize + branchsize - 1, backbonesize, labels, parameters, theoreticalspectrum, centerx, centery, radius, angle, linesize, cornerlinesize, visiblerotationid, trotationsofbranchcyclicrotations[i/6][1].middlebranchstart, trotationsofbranchcyclicrotations[i/6][1].middlebranchend);
				break;
			default:
				break;
			}

		}

		painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
		for (auto it = labels.begin(); it != labels.end(); ++it) {
			if (it->alignright) {
				painter.drawText(it->x - fm.width(it->label.c_str()), it->y, fm.width(it->label.c_str()), 20, Qt::AlignLeft, it->label.c_str());
			}
			else {
				painter.drawText(it->x, it->y, width(), 20, Qt::AlignLeft, it->label.c_str());
			}
		}

	}

}


void cBranchCyclicWidget::rotationChanged(int index) {
	visiblerotationid = index - 1;
	repaint();
}


void cBranchCyclicWidget::trotationChanged(int index) {
	visibletrotationid = index - 1;
	repaint();
}

