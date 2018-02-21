#include "gui/cBranchedWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>


cBranchedWidget::cBranchedWidget() {
	parameters = 0;
	theoreticalspectrum = 0;
}


void cBranchedWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;
}


void cBranchedWidget::paintEvent(QPaintEvent *event) {

	if (theoreticalspectrum->getVisualCoverage().size() == 0) {
		return;
	}

	vector<string> backboneacronyms;
	vector<string> branchacronyms;
	theoreticalspectrum->getCandidate().getBackboneAcronyms(parameters->bricksdatabase, backboneacronyms);
	theoreticalspectrum->getCandidate().getBranchAcronyms(parameters->bricksdatabase, branchacronyms);

	QPainter painter(this);
	const int topmargin = 20;
	const int leftmargin = 20;
	const int bottommargin = 80;
	const int rightmargin = 20;

	int branchsize = (int)branchacronyms.size();
	int backbonesize = (int)theoreticalspectrum->getAcronyms().size() - branchsize;

	vector<TRotationInfo> tpermutations;
	theoreticalspectrum->getCandidate().getPermutationsOfBranches(tpermutations);
	
	const int horizontalstep = (width() - leftmargin - rightmargin)/std::max(backbonesize, 1);
	const int verticalstep = (height() - topmargin - bottommargin)/std::max(branchsize, 1);

	QFont myFont("Courier", 9);
	painter.setFont(myFont);

	for (int i = 0; i < backbonesize; i++) {
		painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		painter.drawText(leftmargin + horizontalstep/4 + horizontalstep*i, topmargin + verticalstep*branchsize, horizontalstep/2, 20, Qt::AlignCenter, backboneacronyms[i].c_str());

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawRect(leftmargin + horizontalstep/4 + horizontalstep*i, topmargin + verticalstep*branchsize, horizontalstep/2, 20);

		if (i < backbonesize - 1) {
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawLine(leftmargin + horizontalstep/4 + horizontalstep*i + horizontalstep/2, topmargin + verticalstep*branchsize + 11, leftmargin + horizontalstep/4 + horizontalstep*(i + 1), topmargin + verticalstep*branchsize + 11);
			painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
			painter.drawLine(leftmargin + horizontalstep*(i + 1), topmargin + verticalstep*branchsize - 10, leftmargin + horizontalstep*(i + 1), topmargin + verticalstep*branchsize + 30);	
			painter.drawLine(leftmargin + horizontalstep/4 + horizontalstep/8 + horizontalstep*i + horizontalstep/2, topmargin + verticalstep*branchsize - 10, leftmargin + horizontalstep*(i + 1), topmargin + verticalstep*branchsize - 10);
			painter.drawLine(leftmargin + horizontalstep*(i + 1), topmargin + verticalstep*branchsize + 30, leftmargin + horizontalstep*(i + 1) + horizontalstep/8, topmargin + verticalstep*branchsize + 30);
		}
	}
	
	for (int i = 0; i < branchsize; i++) {
		painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		painter.drawText(leftmargin + horizontalstep/4 + horizontalstep*tpermutations[0].middlebranchstart, topmargin + verticalstep*i, horizontalstep/2, 20, Qt::AlignCenter, branchacronyms[branchsize - i - 1].c_str());

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawRect(leftmargin + horizontalstep/4 + horizontalstep*tpermutations[0].middlebranchstart, topmargin + verticalstep*i, horizontalstep/2, 20);

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart, topmargin + verticalstep*i + 20, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart, topmargin + verticalstep*i + verticalstep);
		painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
		painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);	
		painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2 + verticalstep/8);
		painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2 - verticalstep/8, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);
	}

	unordered_set<cIonLabel, hash_cIonLabel> labels;
	labels.clear();

	int position;
	string name;
	int len = (int)theoreticalspectrum->getVisualCoverage()[0].series.size();
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
			
			position = 0;
			if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].nterminal) {
				for (int k = 0; k < len; k++) {
					switch (i)
					{
					case 0:
						if ((k < tpermutations[i].middlebranchstart) || (k >= tpermutations[i].middlebranchend)) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/4 + horizontalstep/8 + horizontalstep*position + horizontalstep/2, topmargin + verticalstep*branchsize - 35, name, false);
							}
							position++;
						}
						break;
					case 1:
						if (k < tpermutations[i].middlebranchstart) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 25, topmargin + verticalstep*k + verticalstep/2 - verticalstep/8, name, false);
							}
						}
						if (k >= tpermutations[i].middlebranchend) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/4 + horizontalstep/8 + horizontalstep*(position + tpermutations[i].middlebranchend - tpermutations[i].middlebranchstart) + horizontalstep/2, topmargin + verticalstep*branchsize - 35, name, false);
							}
							position++;
						}
						break;
					case 2:
						if ((k < tpermutations[i].middlebranchstart) || (k >= tpermutations[i].middlebranchend)) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*(backbonesize - position - 2 + 1), topmargin + verticalstep*branchsize + 35, name, false);
							}
							position++;
						}
						break;
					case 3:
						if (k < tpermutations[i].middlebranchstart) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/4 + horizontalstep/8 + horizontalstep*position + horizontalstep/2, topmargin + verticalstep*branchsize - 35, name, false);
							}
							position++;
						}
						if (k >= tpermutations[i].middlebranchend) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 25, topmargin + verticalstep*(branchsize - k - 2 + backbonesize) + verticalstep/2, name, true);
							}
						}
						break;
					case 4:
						if (k < tpermutations[i].middlebranchstart) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 25, topmargin + verticalstep*k + verticalstep/2 - verticalstep/8, name, false);
							}
						}
						if (k >= tpermutations[i].middlebranchend) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*(backbonesize - tpermutations[i].middlebranchend + tpermutations[i].middlebranchstart - position - 2 + 1), topmargin + verticalstep*branchsize + 35, name, false);
							}
							position++;
						}
						break;
					case 5:
						if (k < tpermutations[i].middlebranchstart) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*(backbonesize - position - 2 + 1), topmargin + verticalstep*branchsize + 35, name, false);
							}
							position++;
						}
						if (k >= tpermutations[i].middlebranchend) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[k] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(k + 1) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 25, topmargin + verticalstep*(branchsize - k - 2 + backbonesize) + verticalstep/2, name, true);
							}
						}
						break;
					default:
						break;
					}
				}
			}
			
			position = 0;
			if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].cterminal) {
				for (int k = len - 1; k >= 0; k--) {
					switch (i)
					{
					case 0:
						if ((k < tpermutations[i].middlebranchstart) || (k >= tpermutations[i].middlebranchend)) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*(backbonesize - position - 1), topmargin + verticalstep*branchsize + 35, name, false);
							}
							position++;
						}
						break;
					case 1:
						if (k < tpermutations[i].middlebranchstart) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 25, topmargin + verticalstep*k + verticalstep/2, name, true);
							}
						}
						if (k >= tpermutations[i].middlebranchend) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*(backbonesize - position - 1), topmargin + verticalstep*branchsize + 35, name, false);
							}
							position++;
						}
						break;
					case 2:
						if ((k < tpermutations[i].middlebranchstart) || (k >= tpermutations[i].middlebranchend)) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*position + horizontalstep/2 + horizontalstep/4 + horizontalstep/8, topmargin + verticalstep*branchsize - 35, name, false);
							}
							position++;
						}
						break;
					case 3:
						if (k < tpermutations[i].middlebranchstart) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*(backbonesize - position - tpermutations[i].middlebranchend + tpermutations[i].middlebranchstart - 1), topmargin + verticalstep*branchsize + 35, name, false);
							}
							position++;
						}
						if (k >= tpermutations[i].middlebranchend) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 25, topmargin + verticalstep*(branchsize - k - 2 + backbonesize) + verticalstep/2 - verticalstep/8, name, false);
							}
						}
						break;
					case 4:
						if (k < tpermutations[i].middlebranchstart) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 25, topmargin + verticalstep*k + verticalstep/2, name, true);
							}
						}
						if (k >= tpermutations[i].middlebranchend) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*position + horizontalstep/2 + horizontalstep/4 + horizontalstep/8, topmargin + verticalstep*branchsize - 35, name, false);
							}
							position++;
						}
						break;
					case 5:
						if (k < tpermutations[i].middlebranchstart) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep*(position + tpermutations[i].middlebranchend - tpermutations[i].middlebranchstart) + horizontalstep/2 + horizontalstep/4 + horizontalstep/8, topmargin + verticalstep*branchsize - 35, name, false);
							}
							position++;
						}
						if (k >= tpermutations[i].middlebranchend) {
							if (theoreticalspectrum->getVisualCoverage()[i*parameters->fragmentionsfortheoreticalspectra.size() + j].series[len - k - 1] > 0) {
								name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name[0] + to_string(len - k) + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(1);
								insertLabel(labels, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 25, topmargin + verticalstep*(branchsize - k - 2 + backbonesize) + verticalstep/2 - verticalstep/8, name, false);
							}
						}
						break;
					default:
						break;
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

