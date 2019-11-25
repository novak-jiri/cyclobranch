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

	visibleionseries = "";
	visibleneutralloss = "all";
	visibletrotationid = -1;

	reportisomers = false;
	hidelabels = false;
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


void cBranchedWidget::setReportIsomers(bool reportisomers) {
	this->reportisomers = reportisomers;
	repaint();
}


void cBranchedWidget::hidePeakLabels(bool state) {
	this->hidelabels = state;
	repaint();
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

	if (!reportisomers) {
		stripIsomersFromStringVector(backboneacronyms);
		stripIsomersFromStringVector(branchacronyms);
	}

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
			if (theoreticalspectrum->getTheoreticalPeaks()->size() > 0) {
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
		if (theoreticalspectrum->getTheoreticalPeaks()->size() > 0) {
			painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
			painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);	
			painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart - 20, topmargin + verticalstep*i + (verticalstep + 10)/2 + verticalstep/8);
			painter.drawLine(leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2 - verticalstep/8, leftmargin + horizontalstep/2 + horizontalstep*tpermutations[0].middlebranchstart + 20, topmargin + verticalstep*i + (verticalstep + 10)/2);
		}
	}

	if (parameters && !hidelabels && theoreticalspectrum->getTheoreticalPeaks()) {
	
		unordered_set<cIonLabel, hash_cIonLabel> labels;
		labels.clear();

		cPeaksList* thpeaks = theoreticalspectrum->getTheoreticalPeaks();
		string description;
		size_t pos;
		int rotid, serid;
		for (int i = 0; i < thpeaks->size(); i++) {

			if ((*thpeaks)[i].matched > 0) {

				description = (*thpeaks)[i].description;
				description = description.substr(0, description.find(':'));

				if (description.find('[') == string::npos) {
					if (parameters->precursorcharge > 0) {
						pos = description.find("+ ");
						if (pos != string::npos) {
							description = description.substr(pos + 2);
						}
						else {
							continue;
						}
					}
					else {
						pos = description.find("- ");
						if (pos != string::npos) {
							description = description.substr(pos + 2);
						}
						else {
							continue;
						}
					}

					if (visibleionseries.empty() || (!visibleionseries.empty() && (parameters->iondefinitions[(*thpeaks)[i].iontype].name.compare(visibleionseries) == 0))) {

						if ((visibleneutralloss.compare("all") == 0)
							|| ((visibleneutralloss.compare("none") == 0) && (description.find(" -") == string::npos))
							|| ((visibleneutralloss.compare("all") != 0) && (visibleneutralloss.compare("none") != 0) && (description.substr(description.find(" -") + 2).compare(visibleneutralloss) == 0))) {

							rotid = (*thpeaks)[i].rotationid;
							if ((rotid >= 0) && ((visibletrotationid == -1) || (visibletrotationid == rotid))) {

								serid = (*thpeaks)[i].seriesid;
								if (serid >= 0) {

									if (parameters->iondefinitions[(*thpeaks)[i].iontype].nterminal) {
										switch (rotid) {
											case 0:
												if (serid < tpermutations[0].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep / 4 + horizontalstep / 8 + horizontalstep * serid + horizontalstep / 2, topmargin + verticalstep * branchsize - 35, description, false);
												}												
												if (serid >= tpermutations[0].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep / 4 + horizontalstep / 8 + horizontalstep * (serid - branchsize) + horizontalstep / 2, topmargin + verticalstep * branchsize - 35, description, false);
												}
												break;
											case 1:
												if (serid < tpermutations[1].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep / 2 + horizontalstep * tpermutations[0].middlebranchstart + 25, topmargin + verticalstep * serid + verticalstep / 2 - verticalstep / 8, description, false);
												}
												if (serid >= tpermutations[1].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep / 4 + horizontalstep / 8 + horizontalstep * (serid - branchsize) + horizontalstep / 2, topmargin + verticalstep * branchsize - 35, description, false);
												}
												break;
											case 2:
												if (serid < tpermutations[2].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep * (backbonesize - serid - 1), topmargin + verticalstep * branchsize + 35, description, false);
												}
												if (serid >= tpermutations[2].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep * (backbonesize - serid + branchsize - 1), topmargin + verticalstep * branchsize + 35, description, false);
												}
												break;
											case 3:
												if (serid < tpermutations[3].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep / 4 + horizontalstep / 8 + horizontalstep * serid + horizontalstep / 2, topmargin + verticalstep * branchsize - 35, description, false);
												}
												if (serid >= tpermutations[3].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep / 2 + horizontalstep * tpermutations[0].middlebranchstart - 25, topmargin + verticalstep * (branchsize - serid - 2 + backbonesize) + verticalstep / 2, description, true);
												}
												break;
											case 4:
												if (serid < tpermutations[4].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep / 2 + horizontalstep * tpermutations[0].middlebranchstart + 25, topmargin + verticalstep * serid + verticalstep / 2 - verticalstep / 8, description, false);
												}
												if (serid >= tpermutations[4].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep * (backbonesize - serid + branchsize - 1), topmargin + verticalstep * branchsize + 35, description, false);
												}
												break;
											case 5:
												if (serid < tpermutations[5].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep * (backbonesize - serid - 1), topmargin + verticalstep * branchsize + 35, description, false);
												}
												if (serid >= tpermutations[5].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep / 2 + horizontalstep * tpermutations[0].middlebranchstart - 25, topmargin + verticalstep * (branchsize - serid - 2 + backbonesize) + verticalstep / 2, description, true);
												}
												break;
											default:
												break;
										}
									}

									if (parameters->iondefinitions[(*thpeaks)[i].iontype].cterminal) {
										switch (rotid) {
											case 0:
												if (serid < tpermutations[2].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep * (backbonesize - serid - 1), topmargin + verticalstep * branchsize + 35, description, false);
												}
												if (serid >= tpermutations[2].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep * (backbonesize - serid + branchsize - 1), topmargin + verticalstep * branchsize + 35, description, false);
												}
												break;
											case 1:
												if (serid < tpermutations[5].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep * (backbonesize - serid - 1), topmargin + verticalstep * branchsize + 35, description, false);
												}
												if (serid >= tpermutations[5].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep / 2 + horizontalstep * tpermutations[0].middlebranchstart - 25, topmargin + verticalstep * (branchsize - serid - 2 + backbonesize) + verticalstep / 2, description, true);
												}
												break;
											case 2:
												if (serid < tpermutations[0].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep * serid + horizontalstep / 2 + horizontalstep / 4 + horizontalstep / 8, topmargin + verticalstep * branchsize - 35, description, false);
												}
												if (serid >= tpermutations[0].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep * (serid - branchsize) + horizontalstep / 2 + horizontalstep / 4 + horizontalstep / 8, topmargin + verticalstep * branchsize - 35, description, false);
												}
												break;
											case 3:
												if (serid < tpermutations[4].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep / 2 + horizontalstep * tpermutations[0].middlebranchstart + 25, topmargin + verticalstep * serid + verticalstep / 2 - verticalstep / 8, description, false);
												}
												if (serid >= tpermutations[4].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep * (backbonesize - serid + branchsize - 1), topmargin + verticalstep * branchsize + 35, description, false);
												}
												break;
											case 4:
												if (serid < tpermutations[3].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep / 4 + horizontalstep / 8 + horizontalstep * serid + horizontalstep / 2, topmargin + verticalstep * branchsize - 35, description, false);
												}
												if (serid >= tpermutations[3].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep / 2 + horizontalstep * tpermutations[0].middlebranchstart - 25, topmargin + verticalstep * (branchsize - serid - 2 + backbonesize) + verticalstep / 2, description, true);
												}
												break;
											case 5:
												if (serid < tpermutations[1].middlebranchstart) {
													insertLabel(labels, leftmargin + horizontalstep / 2 + horizontalstep * tpermutations[0].middlebranchstart + 25, topmargin + verticalstep * serid + verticalstep / 2 - verticalstep / 8, description, false);
												}
												if (serid >= tpermutations[1].middlebranchend) {
													insertLabel(labels, leftmargin + horizontalstep / 4 + horizontalstep / 8 + horizontalstep * (serid - branchsize) + horizontalstep / 2, topmargin + verticalstep * branchsize - 35, description, false);
												}
												break;
											default:
												break;
										}
									}

								}

							}

						}

					}

				}

			}

		}

		drawLabels(painter, labels, width());

	}
	
}


void cBranchedWidget::ionSeriesChanged(QString text) {
	if (!parameters) {
		visibleionseries = "";
		return;
	}

	if (text.compare("all") == 0) {
		visibleionseries = "";
	}
	else {
		visibleionseries = text.toStdString();
	}

	repaint();
}


void cBranchedWidget::neutralLossChanged(QString text) {
	if (!parameters) {
		visibleneutralloss = "all";
		return;
	}

	visibleneutralloss = text.toStdString();

	repaint();
}


void cBranchedWidget::trotationChanged(int index) {
	visibletrotationid = index - 1;
	repaint();
}

