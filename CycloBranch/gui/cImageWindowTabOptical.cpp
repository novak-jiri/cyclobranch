#include "gui/cImageWindowTabOptical.h"
#include "core/cMzML.h"


void readLifMetadata(string filename, vector<cLifMetadata>& parsedmetadata) {
	parsedmetadata.clear();

	ifstream is;
	is.open(filename, ifstream::binary);

	if (!is.good()) {
		return;
	}

	char c;

	is.get(c);

	// LIF magic byte
	if (c != 0x70) {
		return;
	}

	is.seekg(7, ios::cur);

	if (!is.good()) {
		return;
	}

	is.get(c);

	// LIF memory byte
	if (c != 0x2a) {
		return;
	}

	if (!is.good()) {
		return;
	}

	// length of xml metadata
	unsigned unicodelenght;
	is.read((char *)&unicodelenght, sizeof(unsigned));

	// xml metadata
	string xmlmetadata;
	xmlmetadata.resize(unicodelenght);
	for (unsigned i = 0; i < 2 * unicodelenght; i++) {
		if (!is.good()) {
			return;
		}

		is.get(c);

		if (i % 2 == 0) {
			if ((c >= 0x20) && (c <= 0x7e)) {
				xmlmetadata[i / 2] = c;
			}
			else {
				xmlmetadata[i / 2] = ' ';
			}
		}
	}

	// print xml metadata
	bool printxml = false;
	if (printxml) {
		for (auto& it : xmlmetadata) {
			cout << it;
			if (it == '>') {
				cout << endl;
			}
		}
		cout << endl;
	}

	parseLifMetadata(xmlmetadata, parsedmetadata);

	unsigned long long datasize;
	unsigned unicodeheadersize;
	string headername;

	while (is.good()) {
		datasize = 0;
		unicodeheadersize = 0;
		headername.clear();

		is.get(c);

		if (c != 0x70) {
			break;
		}

		if (!is.good()) {
			break;
		}

		is.seekg(7, ios::cur);

		if (!is.good()) {
			break;
		}

		is.get(c);

		if (c != 0x2a) {
			break;
		}

		if (!is.good()) {
			break;
		}

		is.read((char*)&datasize, sizeof(unsigned long long));

		is.get(c);

		if (c != 0x2a) {
			break;
		}

		if (!is.good()) {
			break;
		}

		is.read((char*)&unicodeheadersize, sizeof(unsigned));

		if (!is.good()) {
			break;
		}

		headername.resize(unicodeheadersize);
		for (unsigned i = 0; i < 2 * unicodeheadersize; i++) {
			if (!is.good()) {
				return;
			}

			is.get(c);

			if (i % 2 == 0) {
				if ((c >= 0x20) && (c <= 0x7e)) {
					headername[i / 2] = c;
				}
				else {
					headername[i / 2] = ' ';
				}
			}
		}

		for (auto& it : parsedmetadata) {
			if ((it.memoryblockid.compare(headername) == 0) && (it.memoryblocksize == datasize)) {
				it.memoryblockoffsets.push_back((unsigned long long)is.tellg());
				break;
			}
		}

		is.seekg((streampos)datasize, ios::cur);
	}

	// delete 3D images
	bool delete3dimages = false;
	if (delete3dimages) {
		size_t pos = 0;
		while (pos < parsedmetadata.size()) {
			if ((parsedmetadata[pos].microscopytype == 1) && (parsedmetadata[pos].dimz > 1)) {
				parsedmetadata.erase(parsedmetadata.begin() + pos);
			}
			else {
				pos++;
			}
		}
	}

	// delete empty images
	bool deleteemptyimages = true;
	if (deleteemptyimages) {
		size_t pos = 0;
		while (pos < parsedmetadata.size()) {
			if (parsedmetadata[pos].memoryblocksize == 0) {
				parsedmetadata.erase(parsedmetadata.begin() + pos);
			}
			else {
				pos++;
			}
		}
	}

	// delete tiles
	bool deletetiles = true;
	if (deletetiles) {
		size_t pos = 0;
		while (pos < parsedmetadata.size()) {
			if (parsedmetadata[pos].tiles.size() > 1) {
				cout << "deleting: " << endl;
				parsedmetadata[pos].print();
				parsedmetadata.erase(parsedmetadata.begin() + pos);
			}
			else {
				pos++;
			}
		}
	}

	// expand z-layers (fluorescence)
	bool expandzlayers = false;
	if (expandzlayers) {
		if (parsedmetadata.size() > 0) {
			if (parsedmetadata[0].microscopytype == 1) {

				vector<cLifMetadata> tmpmetadata;

				for (int i = 0; i < (int)parsedmetadata.size(); i++) {

					if (parsedmetadata[i].dimz > 1) {

						for (int j = 0; j < parsedmetadata[i].dimz; j++) {
							tmpmetadata.push_back(parsedmetadata[i]);

							tmpmetadata.back().memoryblocksize = parsedmetadata[i].memoryblocksize / (unsigned long long)parsedmetadata[i].dimz;

							if (tmpmetadata.back().memoryblockoffsets.size() == 0) {
								tmpmetadata.back().memoryblockoffsets.push_back(0);
							}

							tmpmetadata.back().memoryblockoffsets.back() += (unsigned long long)j * tmpmetadata.back().memoryblocksize;
						}

					}
					else {

						tmpmetadata.push_back(parsedmetadata[i]);

					}

				}

				parsedmetadata = tmpmetadata;
			}
		}
	}

	// expand color channels (fluorescence)
	bool expandcolorchannels = false;
	if (expandcolorchannels) {
		if (parsedmetadata.size() > 0) {
			if (parsedmetadata[0].microscopytype == 1) {

				vector<cLifMetadata> tmpmetadata;

				for (int i = 0; i < (int)parsedmetadata.size(); i++) {
					if (parsedmetadata[i].channels.size() == parsedmetadata[i].luts.size()) {

						for (int j = 0; j < (int)parsedmetadata[i].channels.size(); j++) {

							if (parsedmetadata[i].channels[j] == gray) {
								tmpmetadata.push_back(parsedmetadata[i]);

								tmpmetadata.back().channels.clear();
								tmpmetadata.back().luts.clear();

								tmpmetadata.back().channels.push_back(parsedmetadata[i].channels[j]);
								tmpmetadata.back().luts.push_back(parsedmetadata[i].luts[j]);

								tmpmetadata.back().memoryblocksize = parsedmetadata[i].memoryblocksize / (unsigned long long)parsedmetadata[i].channels.size();

								if (tmpmetadata.back().memoryblockoffsets.size() == 0) {
									tmpmetadata.back().memoryblockoffsets.push_back(0);
								}

								tmpmetadata.back().memoryblockoffsets.back() += (unsigned long long)j * tmpmetadata.back().memoryblocksize;
							}

						}

					}
				}

				parsedmetadata = tmpmetadata;
			}
		}
	}

	// expand z-layers, color channels and group images with the same color channel (fluorescence)
	bool expandfluorescence = true;
	if (expandfluorescence) {
		if (parsedmetadata.size() > 0) {
			if (parsedmetadata[0].microscopytype == 1) {

				vector<cLifMetadata> tmpmetadata;
				unsigned long long memoryoffset;
				int dimz;

				for (int i = 0; i < (int)parsedmetadata.size(); i++) {

					if (parsedmetadata[i].channels.size() == parsedmetadata[i].luts.size()) {

						for (int j = 0; j < (int)parsedmetadata[i].channels.size(); j++) {

							if (parsedmetadata[i].channels[j] == gray) {

								dimz = max(parsedmetadata[i].dimz, 1);

								tmpmetadata.push_back(parsedmetadata[i]);

								tmpmetadata.back().channels.clear();
								tmpmetadata.back().luts.clear();

								tmpmetadata.back().channels.push_back(parsedmetadata[i].channels[j]);
								tmpmetadata.back().luts.push_back(parsedmetadata[i].luts[j]);

								// fix image name for fluorescence images
								if ((tmpmetadata.back().microscopytype == 1) && (tmpmetadata.back().luts.size() == 1) && (tmpmetadata.back().luts[0].size() > 0)) {
									tmpmetadata.back().imagename += ", ";
									tmpmetadata.back().imagename += tmpmetadata.back().luts[0].c_str();
								}

								//tmpmetadata.back().dimz = 1;

								tmpmetadata.back().memoryblocksize = parsedmetadata[i].memoryblocksize / ((unsigned long long)parsedmetadata[i].channels.size() * (unsigned long long)dimz);

								if (tmpmetadata.back().memoryblockoffsets.size() == 0) {
									tmpmetadata.back().memoryblockoffsets.push_back(0);
								}

								memoryoffset = tmpmetadata.back().memoryblockoffsets[0];
								tmpmetadata.back().memoryblockoffsets.clear();

								for (int k = 0; k < dimz; k++) {
									tmpmetadata.back().memoryblockoffsets.push_back(memoryoffset + (unsigned long long)k * parsedmetadata[i].memoryblocksize / (unsigned long long)dimz + (unsigned long long)j * tmpmetadata.back().memoryblocksize);
								}

							}

						}

					}

				}

				parsedmetadata = tmpmetadata;
			}
		}
	}

	// print parsed metadata
	bool printparsedmetadata = false;
	if (printparsedmetadata) {
		for (auto& it : parsedmetadata) {
			it.print();
		}
	}
}


void parseLifMetadataSubTree(DOMElement* subtree, vector<cLifMetadata>& parsedmetadata) {
	DOMNode* currentNode3 = subtree->getFirstChild();
	while (currentNode3) {

		if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {

			DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
			if (compareElementTagName(currentElement3, "Element")) {


				cLifMetadata metadata;
				metadata.imagename = getAttribute(currentElement3, "Name");
				parsedmetadata.push_back(metadata);

				vector<cLifMetadata> subtreemetadata;


				// childrens of Element
				DOMNode* currentNode4 = currentElement3->getFirstChild();
				while (currentNode4) {

					if (currentNode4->getNodeType() == DOMNode::ELEMENT_NODE) {

						DOMElement* currentElement4 = dynamic_cast<xercesc::DOMElement*>(currentNode4);
						if (compareElementTagName(currentElement4, "Memory")) {

							parsedmetadata.back().memoryblocksize = stoull(getAttribute(currentElement4, "Size"));
							parsedmetadata.back().memoryblockid = getAttribute(currentElement4, "MemoryBlockID");

						}

						if (compareElementTagName(currentElement4, "Data")) {


							// childrens of Data
							DOMNode* currentNode5 = currentElement4->getFirstChild();
							while (currentNode5) {

								if (currentNode5->getNodeType() == DOMNode::ELEMENT_NODE) {

									DOMElement* currentElement5 = dynamic_cast<xercesc::DOMElement*>(currentNode5);
									if (compareElementTagName(currentElement5, "Image")) {


										// childrens of Image
										DOMNode* currentNode6 = currentElement5->getFirstChild();
										while (currentNode6) {

											if (currentNode6->getNodeType() == DOMNode::ELEMENT_NODE) {

												DOMElement* currentElement6 = dynamic_cast<xercesc::DOMElement*>(currentNode6);
												if (compareElementTagName(currentElement6, "ImageDescription")) {


													// childrens of ImageDescription
													DOMNode* currentNode7 = currentElement6->getFirstChild();
													while (currentNode7) {

														if (currentNode7->getNodeType() == DOMNode::ELEMENT_NODE) {

															DOMElement* currentElement7 = dynamic_cast<xercesc::DOMElement*>(currentNode7);
															if (compareElementTagName(currentElement7, "Dimensions")) {


																// childrens of Dimensions
																DOMNode* currentNode8 = currentElement7->getFirstChild();
																while (currentNode8) {

																	if (currentNode8->getNodeType() == DOMNode::ELEMENT_NODE) {

																		DOMElement* currentElement8 = dynamic_cast<xercesc::DOMElement*>(currentNode8);
																		if (compareElementTagName(currentElement8, "DimensionDescription")) {

																			string dimid = getAttribute(currentElement8, "DimID");

																			if (dimid.compare("1") == 0) {
																				parsedmetadata.back().dimx = QVariant(getAttribute(currentElement8, "NumberOfElements").c_str()).toInt();
																				parsedmetadata.back().width = QVariant(getAttribute(currentElement8, "Length").c_str()).toDouble();
																			}

																			if (dimid.compare("2") == 0) {
																				parsedmetadata.back().dimy = QVariant(getAttribute(currentElement8, "NumberOfElements").c_str()).toInt();
																				parsedmetadata.back().height = QVariant(getAttribute(currentElement8, "Length").c_str()).toDouble();
																			}

																			if (dimid.compare("3") == 0) {
																				parsedmetadata.back().dimz = QVariant(getAttribute(currentElement8, "NumberOfElements").c_str()).toInt();
																			}

																			if (dimid.compare("10") == 0) {
																				parsedmetadata.back().dim10 = QVariant(getAttribute(currentElement8, "NumberOfElements").c_str()).toInt();
																			}

																		}

																	}

																	currentNode8 = currentNode8->getNextSibling();

																}


															}

															if (compareElementTagName(currentElement7, "Channels")) {


																// childrens of Channels
																DOMNode* currentNode8 = currentElement7->getFirstChild();
																while (currentNode8) {

																	if (currentNode8->getNodeType() == DOMNode::ELEMENT_NODE) {

																		DOMElement* currentElement8 = dynamic_cast<xercesc::DOMElement*>(currentNode8);
																		if (compareElementTagName(currentElement8, "ChannelDescription")) {

																			eChannelType channeltype = (eChannelType)QVariant(getAttribute(currentElement8, "ChannelTag").c_str()).toInt();
																			parsedmetadata.back().channels.push_back(channeltype);

																			string lut = getAttribute(currentElement8, "LUTName");
																			parsedmetadata.back().luts.push_back(lut);

																		}

																	}

																	currentNode8 = currentNode8->getNextSibling();

																}


															}

														}

														currentNode7 = currentNode7->getNextSibling();

													}


												}


												if (compareElementTagName(currentElement6, "Attachment")) {


													if (getAttribute(currentElement6, "Name").compare("TileScanInfo") == 0) {
														parsedmetadata.back().tilescaninfoflipx = QVariant(getAttribute(currentElement6, "FlipX").c_str()).toInt();
														parsedmetadata.back().tilescaninfoflipy = QVariant(getAttribute(currentElement6, "FlipY").c_str()).toInt();
													}

													// childrens of Attachment
													DOMNode* currentNode7 = currentElement6->getFirstChild();
													while (currentNode7) {

														if (currentNode7->getNodeType() == DOMNode::ELEMENT_NODE) {

															DOMElement* currentElement7 = dynamic_cast<xercesc::DOMElement*>(currentNode7);
															if (compareElementTagName(currentElement7, "ATLCameraSettingDefinition")) {

																parsedmetadata.back().flipx = QVariant(getAttribute(currentElement7, "FlipX").c_str()).toInt();
																parsedmetadata.back().flipy = QVariant(getAttribute(currentElement7, "FlipY").c_str()).toInt();
																parsedmetadata.back().magnification = QVariant(getAttribute(currentElement7, "Magnification").c_str()).toInt();
																parsedmetadata.back().stageposx = QVariant(getAttribute(currentElement7, "StagePosX").c_str()).toDouble();
																parsedmetadata.back().stageposy = QVariant(getAttribute(currentElement7, "StagePosY").c_str()).toDouble();
																parsedmetadata.back().microscopemodel = getAttribute(currentElement7, "MicroscopeModel");
																parsedmetadata.back().inversemicroscopemodel = QVariant(getAttribute(currentElement7, "IsInverseMicroscopeModel").c_str()).toInt();
																parsedmetadata.back().microscopytype = 0;

															}

															if (compareElementTagName(currentElement7, "ATLConfocalSettingDefinition")) {

																parsedmetadata.back().flipx = QVariant(getAttribute(currentElement7, "FlipX").c_str()).toInt();
																parsedmetadata.back().flipy = QVariant(getAttribute(currentElement7, "FlipY").c_str()).toInt();
																parsedmetadata.back().magnification = QVariant(getAttribute(currentElement7, "Magnification").c_str()).toInt();
																parsedmetadata.back().stageposx = QVariant(getAttribute(currentElement7, "StagePosX").c_str()).toDouble();
																parsedmetadata.back().stageposy = QVariant(getAttribute(currentElement7, "StagePosY").c_str()).toDouble();
																parsedmetadata.back().microscopemodel = getAttribute(currentElement7, "MicroscopeModel");
																parsedmetadata.back().inversemicroscopemodel = QVariant(getAttribute(currentElement7, "IsInverseMicroscopeModel").c_str()).toInt();
																parsedmetadata.back().microscopytype = 1;

															}

															if (compareElementTagName(currentElement7, "Tile")) {

																cTileInfo tileinfo;
																tileinfo.fieldx = QVariant(getAttribute(currentElement7, "FieldX").c_str()).toInt();
																tileinfo.fieldy = QVariant(getAttribute(currentElement7, "FieldY").c_str()).toInt();
																tileinfo.posx = QVariant(getAttribute(currentElement7, "PosX").c_str()).toDouble();
																tileinfo.posy = QVariant(getAttribute(currentElement7, "PosY").c_str()).toDouble();
																parsedmetadata.back().tiles.push_back(tileinfo);

															}

															if (compareElementTagName(currentElement7, "HardwareSetting")) {


																// childrens of HardwareSetting
																DOMNode* currentNode8 = currentElement7->getFirstChild();
																while (currentNode8) {

																	if (currentNode8->getNodeType() == DOMNode::ELEMENT_NODE) {

																		DOMElement* currentElement8 = dynamic_cast<xercesc::DOMElement*>(currentNode8);
																		if (compareElementTagName(currentElement8, "LDM_Block_Sequential")) {


																			// childrens of LDM_Block_Sequential
																			DOMNode* currentNode9 = currentElement8->getFirstChild();
																			while (currentNode9) {

																				if (currentNode9->getNodeType() == DOMNode::ELEMENT_NODE) {

																					DOMElement* currentElement9 = dynamic_cast<xercesc::DOMElement*>(currentNode9);
																					if (compareElementTagName(currentElement9, "LDM_Block_Sequential_Master")) {


																						// childrens of LDM_Block_Sequential_Master
																						DOMNode* currentNode10 = currentElement9->getFirstChild();
																						while (currentNode10) {

																							if (currentNode10->getNodeType() == DOMNode::ELEMENT_NODE) {

																								DOMElement* currentElement10 = dynamic_cast<xercesc::DOMElement*>(currentNode10);
																								if (compareElementTagName(currentElement10, "ATLConfocalSettingDefinition")) {

																									if (parsedmetadata.back().microscopytype == -1) {
																										parsedmetadata.back().magnification = QVariant(getAttribute(currentElement10, "Magnification").c_str()).toInt();
																										parsedmetadata.back().stageposx = QVariant(getAttribute(currentElement10, "StagePosX").c_str()).toDouble();
																										parsedmetadata.back().stageposy = QVariant(getAttribute(currentElement10, "StagePosY").c_str()).toDouble();
																										parsedmetadata.back().microscopemodel = getAttribute(currentElement10, "MicroscopeModel");
																										parsedmetadata.back().inversemicroscopemodel = QVariant(getAttribute(currentElement10, "IsInverseMicroscopeModel").c_str()).toInt();
																										parsedmetadata.back().microscopytype = 1;
																									}

																								}

																							}

																							currentNode10 = currentNode10->getNextSibling();

																						}


																					}

																				}

																				currentNode9 = currentNode9->getNextSibling();

																			}


																		}

																	}

																	currentNode8 = currentNode8->getNextSibling();

																}


															}

														}

														currentNode7 = currentNode7->getNextSibling();

													}


												}

											}

											currentNode6 = currentNode6->getNextSibling();

										}


									}

								}

								currentNode5 = currentNode5->getNextSibling();

							}


						}

						if (compareElementTagName(currentElement4, "Children")) {


							parseLifMetadataSubTree(currentElement4, subtreemetadata);

						}

					}

					currentNode4 = currentNode4->getNextSibling();

				}


				for (auto& it : subtreemetadata) {
					parsedmetadata.push_back(it);
				}


			}

		}

		currentNode3 = currentNode3->getNextSibling();

	}

}


void parseLifMetadata(string& metadata, vector<cLifMetadata>& parsedmetadata) {
	MemBufInputSource lifxml((const XMLByte*)metadata.c_str(), metadata.size(), "lif metadata");

	XercesDOMParser* parser = new XercesDOMParser();
	parser->parse(lifxml);

	DOMDocument* document = parser->getDocument();
	DOMElement* root = document->getDocumentElement();

	if (!root) {
		delete parser;
		return;
	}

	if (compareElementTagName(root, "LMSDataContainerHeader")) {
		
		// childrens of LMSDataContainerHeader
		DOMNode* currentNode1 = root->getFirstChild();
		while (currentNode1) {

			if (currentNode1->getNodeType() == DOMNode::ELEMENT_NODE) {

				DOMElement* currentElement1 = dynamic_cast<xercesc::DOMElement*>(currentNode1);
				if (compareElementTagName(currentElement1, "Element")) {
					

					// childrens of Element
					DOMNode* currentNode2 = currentElement1->getFirstChild();
					while (currentNode2) {

						if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {

							DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
							if (compareElementTagName(currentElement2, "Children")) {
								
								
								// childrens of Children
								parseLifMetadataSubTree(currentElement2, parsedmetadata);


							}

						}

						currentNode2 = currentNode2->getNextSibling();

					}


				}

			}

			currentNode1 = currentNode1->getNextSibling();

		}

	}

	delete parser;
}


cImageWindowTabOptical::cImageWindowTabOptical(QWidget *parent) : QWidget(parent) {
	gridlayout = new QGridLayout();

	
	labelstartfrom = new QLabel("Begin import at : ");

	comboboxstartfrom = new QComboBox();

	gridlayout->addWidget(labelstartfrom, 0, 0, 1, 1);
	gridlayout->addWidget(comboboxstartfrom, 0, 1, 1, 9);


	labelusevaluesfrom = new QLabel("Navigation : ");

	comboboxusevaluesfrom = new QComboBox();

	gridlayout->addWidget(labelusevaluesfrom, 1, 0, 1, 1);
	gridlayout->addWidget(comboboxusevaluesfrom, 1, 1, 1, 9);


	labellif = new QLabel("LIF File : ");
	lineeditlif = new QLineEdit();
	buttonlif = new QPushButton("Browse");

	gridlayout->addWidget(labellif, 2, 0, 1, 1);
	gridlayout->addWidget(lineeditlif, 2, 1, 1, 8);
	gridlayout->addWidget(buttonlif, 2, 9, 1, 1);


	labelfilenames = new QLabel("TIF Images (optional) : ");

	listwidgetfilenames = new QListWidget();
	listwidgetfilenames->setSelectionMode(QAbstractItemView::NoSelection);

	buttonfilenames = new QPushButton("Browse");

	gridlayout->addWidget(labelfilenames, 3, 0, 1, 1);
	gridlayout->addWidget(listwidgetfilenames, 3, 1, 1, 9);
	gridlayout->addWidget(buttonfilenames, 4, 9, 1, 1);


	setLayout(gridlayout);

	connect(buttonfilenames, SIGNAL(released()), this, SLOT(browseButtonReleased()));
	connect(buttonlif, SIGNAL(released()), this, SLOT(lifButtonReleased()));
}


void cImageWindowTabOptical::setFormValues(QString& lastdirmicroscopyimage, int numberoflayers) {
	this->lastdirmicroscopyimage = &lastdirmicroscopyimage;

	QString layername = "Microscopy Image ";
	QString tmpname;
	comboboxstartfrom->clear();
	comboboxusevaluesfrom->clear();

	tmpname = layername + "(nav.)";
	comboboxusevaluesfrom->insertItem(0, tmpname);

	for (int i = 0; i < numberoflayers - 4; i++) {
		tmpname = layername + QVariant(i + 1).toString();
		comboboxstartfrom->insertItem(i, tmpname);
		comboboxusevaluesfrom->insertItem(i + 1, tmpname);
	}

	lineeditlif->clear();
	listwidgetfilenames->clear();
	imagenames.clear();
}


void cImageWindowTabOptical::getFormValues(int& startfromindex, QStringList& tiffilenames, int& navigationindex, QString& liffilename) {
	startfromindex = comboboxstartfrom->currentIndex();
	tiffilenames = imagenames;
	navigationindex = comboboxusevaluesfrom->currentIndex();
	liffilename = lineeditlif->text();
}


cImageWindowTabOptical::~cImageWindowTabOptical() {
	delete labelstartfrom;
	delete comboboxstartfrom;

	delete labelusevaluesfrom;
	delete comboboxusevaluesfrom;

	delete labellif;
	delete lineeditlif;
	delete buttonlif;

	delete labelfilenames;
	delete listwidgetfilenames;
	delete buttonfilenames;

	delete gridlayout;
}


void cImageWindowTabOptical::browseButtonReleased() {
	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Select Images..."), *lastdirmicroscopyimage, tr("Images (*.tif *.tiff)"));
	if (!filenames.isEmpty()) {
		listwidgetfilenames->clear();
		for (auto& it : filenames) {
			listwidgetfilenames->addItem(it);
		}
		imagenames = filenames;
	}
}


void cImageWindowTabOptical::lifButtonReleased() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Select LIF file ..."), *lastdirmicroscopyimage, tr("LIF Files (*.lif)"));
	if (!filename.isEmpty()) {
		lineeditlif->setText(filename);
	}
}

