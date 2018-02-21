#include "gui/cBranchedWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QPrinter>
#include <QSvgGenerator>


cBranchedWidget::cBranchedWidget() {
	parameters = 0;
	theoreticalspectrum = 0;
	visibletrotationid = -1;
}


void cBranchedWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;
}


void cBranchedWidget::exportToPDF(QString filename, bool postscript) {
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


void cBranchedWidget::exportToPNG(QString filename) {
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


void cBranchedWidget::exportToSVG(QString filename) {
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


void cBranchedWidget::paintEvent(QPaintEvent *event) {
	QPainter painter;
	painter.begin(this);
	paint(painter);
	painter.end();
	event->accept();
}


void cBranchedWidget::paint(QPainter& painter) {

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

	vector<TRotationInfo> tpermutations;
	theoreticalspectrum->getCandidate().getPermutationsOfBranches(tpermutations);
	
	const int horizontalstep = (width() - leftmargin - rightmargin)/std::max(backbonesize, 1);
	const int verticalstep = (height() - topmargin - bottommargin)/std::max(branchsize, 1);

	QFont myFont("Courier", 9);
	QFontMetrics fm(myFont);
	painter.setFont(myFont);

	for (int i = 0; i < backbonesize; i++) {
		painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		if (fm.width(backboneacronyms[i].c_str()) > horizontalstep/2) {
			painter.drawText(leftmargin + horizontalstep/4 + horizontalstep*i + 2, topmargin + verticalstep*branchsize, horizontalstep/2 - 2, 20, Qt::AlignLeft|Qt::AlignVCenter, backboneacronyms[i].c_str());
		}
		else {
			painter.drawText(leftmargin + horizontalstep/4 + horizontalstep*i, topmargin + verticalstep*branchsize, horizontalstep/2, 20, Qt::AlignCenter, backboneacronyms[i].c_str());
		}

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawRect(leftmargin + horizontalstep/4 + horizontalstep*i, topmargin + verticalstep*branchsize, horizontalstep/2, 20);

		if (i < backbonesize - 1) {
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawLine(leftmargin + horizontalstep/4 + horizontalstep*i + horizontalstep/2, topmargin + verticalstep*branchsize + 11, leftmargin + horizontalstep/4 + horizontalstep*(i + 1), topmargin + verticalstep*branchsize + 11);
			if (theoreticalspectrum->getVisualCoverage().size() > 0) {
				painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
				painter.drawLine(leftmargin + horizontalstep*(i + 1), topmargin + verticalstep*branchsize - 10, leftmargin + horizontalstep*(i + 1), topmargin + verticalstep*branchsize + 30);	
				painter.drawLine(leftmargin + horizontalstep/4 + horizontalstep/8 + horizontalstep*i + horizontalstep/2, topmargin + verticalstep*branchsize - 10, leftmargin + horizontalstep*(i + 1), topmargin + verticalstep*branchsize - 10);
				painter.drawLine(leftmargin + horizontalstep*(i + 1), topmargin + verticalstep*branchsize + 30, leftmargin + horizontalstep*(i + 1) + horizontalstep/8, topmargin + verticalstep*branchsize + 30);
			}
		}
	}
	
	for (int i = 0; i < branchsize; i++) {
		painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		if (fm.width(branchacronyms[branchsize - i - 1].c_str()) > horizontalstep/2) {
			painter.drawText(leftmargin + horizontalstep/4 + horizontalstep*tpermutations[0].middlebranchstart + 2, topmargin + verticalstep*i, horizontalstep/2 - 2, 20, Qt::AlignLeft|Qt::AlignVCenter, branchacronyms[branchsize - i - 1].c_str());
		}
		else {
			painter.drawText(leftmargin + horizontalstep/4 + horizontalstep*tpermutations[0].middlebranchstart, topmargin + verticalstep*i, horizontalstep/2, 20, Qt::AlignCenter, branchacronyms[branchsize - i - 1].c_str());
		}	

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawRect(leftmargin + horizontalstep/4 + horizontalstep*tpermutations[0].middlebranchstart, topmargin + verticalstep*i, horizontalstep/2, 20);

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart, topmargin + verticalstep*i + 20, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart, topmargin + verticalstep*i + verticalstep);
		if (theoreticalspectrum->getVisualCoverage().size() > 0) {
			painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
			painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);	
			painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2 + verticalstep/8);
			painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2 - verticalstep/8, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);
		}
	}

	if (parameters && (theoreticalspectrum->getVisualCoverage().size() > 0)) {
	
		unordered_set<cIonLabel, hash_cIonLabel> labels;
		labels.clear();

		int position;
		string name;
		int len = (int)theoreticalspectrum->getVisualCoverage()[0].series.size();
		for (int i = 0; i < 6; i++) {

			if ((visibletrotationid != -1) && (visibletrotationid != i)) {
				continue;
			}

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
				painter.drawText(it->x - fm.width(it->label.c_str()), it->y, fm.width(it->label.c_str()), 20, Qt::AlignLeft, it->label.c_str());
			}
			else {
				painter.drawText(it->x, it->y, width(), 20, Qt::AlignLeft, it->label.c_str());
			}
		}

	}
	
}


void cBranchedWidget::trotationChanged(int index) {
	visibletrotationid = index - 1;
	repaint();
}

