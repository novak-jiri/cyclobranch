#include "core/cImzML.h"

#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include "core/cPeaksList.h"


cImzML::cImzML() {
	profilespectra = false;
	use_64bit_float_mz_precision = false;
	use_64bit_float_intensity_precision = false;

	imzmlitems.clear();

	parser = new XercesDOMParser();
	document = 0;
}


cImzML::~cImzML() {
	delete parser;
}


int cImzML::parse(string& filename, int& defaultmaxx, int& defaultmaxy, int& pixelsize, eVendorType& vendor) {

	parser->parse(filename.c_str());
	document = parser->getDocument();

	DOMElement* root = document->getDocumentElement();
	if (!root) {
		return 0;
	}

	profilespectra = false;
	use_64bit_float_mz_precision = false;
	use_64bit_float_intensity_precision = false;
	defaultmaxx = 1;
	defaultmaxy = 1;
	pixelsize = 1;
	vendor = unknownvendor;

	// childrens of mzML
	DOMNode* currentNode1 = root->getFirstChild();
	while (currentNode1) {
		
		
		if (currentNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
			
			DOMElement* currentElement1 = dynamic_cast<xercesc::DOMElement*>(currentNode1);

			if (compareElementTagName(currentElement1, "fileDescription")) {


				// childrens of fileDescription
				DOMNode* currentNode2 = currentNode1->getFirstChild();
				while (currentNode2) {

					
					if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
			
						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (compareElementTagName(currentElement2, "fileContent")) {
						
								
							// childrens of fileContent
							DOMNode* currentNode3 = currentNode2->getFirstChild();
							while (currentNode3) {

								
								if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
			
									DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
									if (compareElementTagName(currentElement3, "cvParam")) {
											
										string accession = getAttribute(currentElement3, "accession");

										// profile spectrum
										if (accession.compare("MS:1000128") == 0) {											
											profilespectra = true;
										}

									}

								}


								currentNode3 = currentNode3->getNextSibling();

							}

				
						}

					}


					currentNode2 = currentNode2->getNextSibling();

				}

			}


			if (compareElementTagName(currentElement1, "referenceableParamGroupList")) {


				// childrens of referenceableParamGroupList
				DOMNode* currentNode2 = currentNode1->getFirstChild();
				while (currentNode2) {

					
					if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
			
						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (compareElementTagName(currentElement2, "referenceableParamGroup")) {

							string id = getAttribute(currentElement2, "id");
							
							if (id.compare("mzArray") == 0) {			
									
								
								// childrens of referenceableParamGroup
								DOMNode* currentNode3 = currentNode2->getFirstChild();
								while (currentNode3) {

									
									if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
			
										DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
										if (compareElementTagName(currentElement3, "cvParam")) {
											
											string accession = getAttribute(currentElement3, "accession");

											// 64-bit float
											if (accession.compare("MS:1000523") == 0) {											
												use_64bit_float_mz_precision = true;
											}

											// zlib compression detected
											if (accession.compare("MS:1000574") == 0) {
												return 1;
											}

										}

									}


									currentNode3 = currentNode3->getNextSibling();

								}


							}

							if ((id.compare("intensities") == 0) || (id.compare("intensityArray") == 0)) {


								// childrens of referenceableParamGroup
								DOMNode* currentNode3 = currentNode2->getFirstChild();
								while (currentNode3) {


									if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {

										DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
										if (compareElementTagName(currentElement3, "cvParam")) {

											string accession = getAttribute(currentElement3, "accession");

											// 64-bit float
											if (accession.compare("MS:1000523") == 0) {
												use_64bit_float_intensity_precision = true;
											}

											// zlib compression detected
											if (accession.compare("MS:1000574") == 0) {
												return 1;
											}

										}

									}


									currentNode3 = currentNode3->getNextSibling();

								}


							}

							if (id.compare("spectrum") == 0) {			
									
								
								// childrens of referenceableParamGroup
								DOMNode* currentNode3 = currentNode2->getFirstChild();
								while (currentNode3) {

									
									if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
			
										DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
										if (compareElementTagName(currentElement3, "cvParam")) {
											
											string accession = getAttribute(currentElement3, "accession");

											// profile spectrum
											if (accession.compare("MS:1000128") == 0) {
												profilespectra = true;
											}											

										}

									}


									currentNode3 = currentNode3->getNextSibling();

								}


							}

						}

					}


					currentNode2 = currentNode2->getNextSibling();

				}


			}


			if (compareElementTagName(currentElement1, "softwareList")) {


				// childrens of scanSettingsList
				DOMNode* currentNode2 = currentNode1->getFirstChild();
				while (currentNode2) {


					if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {

						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (compareElementTagName(currentElement2, "software")) {


							// childrens of scanSettings
							DOMNode* currentNode3 = currentNode2->getFirstChild();
							while (currentNode3) {


								if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {

									DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
									if (compareElementTagName(currentElement3, "cvParam")) {

										string accession = getAttribute(currentElement3, "accession");

										// Bruker software
										if (accession.compare("MS:1000692") == 0) {
											vendor = bruker;
										}

										// MassLynx
										if (accession.compare("MS:1000534") == 0) {
											vendor = waters;
										}

									}

								}


								currentNode3 = currentNode3->getNextSibling();

							}


						}
						

					}


					currentNode2 = currentNode2->getNextSibling();

				}


			}


			if (compareElementTagName(currentElement1, "scanSettingsList")) {


				// childrens of scanSettingsList
				DOMNode* currentNode2 = currentNode1->getFirstChild();
				while (currentNode2) {


					if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {

						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (compareElementTagName(currentElement2, "scanSettings")) {


							// childrens of scanSettings
							DOMNode* currentNode3 = currentNode2->getFirstChild();
							while (currentNode3) {


								if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {

									DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
									if (compareElementTagName(currentElement3, "cvParam")) {

										string accession = getAttribute(currentElement3, "accession");
										string value = getAttribute(currentElement3, "value");

										// max count of pixel x
										if (accession.compare("IMS:1000042") == 0) {
											defaultmaxx = stoi(value);
										}

										// max count of pixel y
										if (accession.compare("IMS:1000043") == 0) {
											defaultmaxy = stoi(value);
										}

										// pixel size
										if (accession.compare("IMS:1000046") == 0) {
											pixelsize = stoi(value);
										}

									}

								}


								currentNode3 = currentNode3->getNextSibling();

							}


						}


					}


					currentNode2 = currentNode2->getNextSibling();

				}


			}


			if (compareElementTagName(currentElement1, "run")) {


				// childrens of run
				DOMNode* currentNode2 = currentNode1->getFirstChild();
				while (currentNode2) {

					
					if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
			
						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (compareElementTagName(currentElement2, "spectrumList")) {


							// childrens of spectrumList
							DOMNode* currentNode3 = currentNode2->getFirstChild();
							while (currentNode3) {


								if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
			
									DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
									if (compareElementTagName(currentElement3, "spectrum")) {


										cImzMLItem imzmlitem;
										imzmlitem.title = getAttribute(currentElement3, "id");


										// childrens of spectrum
										DOMNode* currentNode4 = currentNode3->getFirstChild();
										while (currentNode4) {

											
											if (currentNode4->getNodeType() == DOMNode::ELEMENT_NODE) {
			
												DOMElement* currentElement4 = dynamic_cast<xercesc::DOMElement*>(currentNode4);
												if (compareElementTagName(currentElement4, "scanList")) {


													// childrens of scanList
													DOMNode* currentNode5 = currentNode4->getFirstChild();
													while (currentNode5) {

														
														if (currentNode5->getNodeType() == DOMNode::ELEMENT_NODE) {
			
															DOMElement* currentElement5 = dynamic_cast<xercesc::DOMElement*>(currentNode5);
															if (compareElementTagName(currentElement5, "scan")) {


																// childrens of scan
																DOMNode* currentNode6 = currentNode5->getFirstChild();
																while (currentNode6) {

																	
																	if (currentNode6->getNodeType() == DOMNode::ELEMENT_NODE) {
			
																		DOMElement* currentElement6 = dynamic_cast<xercesc::DOMElement*>(currentNode6);
																		if (compareElementTagName(currentElement6, "cvParam")) {

																			string accession = getAttribute(currentElement6, "accession");

																			// position x
																			if (accession.compare("IMS:1000050") == 0) {			
																				imzmlitem.x = atoi(getAttribute(currentElement6, "value").c_str());
																			}
																			
																			// position y
																			if (accession.compare("IMS:1000051") == 0) {
																				imzmlitem.y = atoi(getAttribute(currentElement6, "value").c_str());
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

												if (compareElementTagName(currentElement4, "binaryDataArrayList")) {


													// childrens of binaryDataArrayList
													DOMNode* currentNode5 = currentNode4->getFirstChild();
													while (currentNode5) {

														
														if (currentNode5->getNodeType() == DOMNode::ELEMENT_NODE) {
			
															DOMElement* currentElement5 = dynamic_cast<xercesc::DOMElement*>(currentNode5);
															if (compareElementTagName(currentElement5, "binaryDataArray")) {


																// childrens of binaryDataArray
																bool mzarray = false;
																bool intensityarray = false;
																unsigned long long offset = 0;
																unsigned long long length = 0;

																DOMNode* currentNode6 = currentNode5->getFirstChild();
																while (currentNode6) {

																	
																	if (currentNode6->getNodeType() == DOMNode::ELEMENT_NODE) {
			
																		DOMElement* currentElement6 = dynamic_cast<xercesc::DOMElement*>(currentNode6);
																		if (compareElementTagName(currentElement6, "referenceableParamGroupRef")) {

																			string ref = getAttribute(currentElement6, "ref");

																			if (ref.compare("mzArray") == 0) {
																				mzarray = true;
																			}

																			if ((ref.compare("intensities") == 0) || (ref.compare("intensityArray") == 0)) {
																				intensityarray = true;
																			}

																		}

																		if (compareElementTagName(currentElement6, "cvParam")) {

																			string accession = getAttribute(currentElement6, "accession");

																			// external offset
																			if (accession.compare("IMS:1000102") == 0) {			
																				offset = stoull(getAttribute(currentElement6, "value"));
																			}

																			// external encoded length
																			if (accession.compare("IMS:1000104") == 0) {
																				length = stoull(getAttribute(currentElement6, "value"));
																			}
																			
																		}

																	}


																	currentNode6 = currentNode6->getNextSibling();

																}

																if (mzarray) {
																	imzmlitem.mzstart = offset;
																	imzmlitem.mzlength = length;
																}

																if (intensityarray) {
																	imzmlitem.intensitystart = offset;
																	imzmlitem.intensitylength = length;
																}

															}

														}


														currentNode5 = currentNode5->getNextSibling();

													}


												}

											}


											currentNode4 = currentNode4->getNextSibling();

										}


										imzmlitems.push_back(imzmlitem);


									}

								}


								currentNode3 = currentNode3->getNextSibling();

							}

							
						}

					}


					currentNode2 = currentNode2->getNextSibling();

				}


			}

		}


		currentNode1 = currentNode1->getNextSibling();

	}

	return 0;

}


void cImzML::updateRawDataToPeakList(vector<cPeaksList>& peaklists, string& convertedibdfilename) {

	if (!document) {
		return;
	}

	DOMElement* root = document->getDocumentElement();
	if (!root) {
		return;
	}

	unsigned long long offset = 16;
	int currentid = 0;

	int mz_size = 4;
	if (use_64bit_float_mz_precision) {
		mz_size = 8;
	}

	int intensity_size = 4;
	if (use_64bit_float_intensity_precision) {
		intensity_size = 8;
	}

	// childrens of mzML
	DOMNode* currentNode1 = root->getFirstChild();
	while (currentNode1) {
		
		
		if (currentNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
			
			DOMElement* currentElement1 = dynamic_cast<xercesc::DOMElement*>(currentNode1);


			if (compareElementTagName(currentElement1, "fileDescription")) {


				// childrens of fileDescription
				DOMNode* currentNode2 = currentNode1->getFirstChild();
				while (currentNode2) {

					
					if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
			
						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (compareElementTagName(currentElement2, "fileContent")) {
						
								
							// childrens of fileContent
							DOMNode* currentNode3 = currentNode2->getFirstChild();
							while (currentNode3) {

								
								if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
			
									DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
									if (compareElementTagName(currentElement3, "cvParam")) {
											
										string accession = getAttribute(currentElement3, "accession");

										// continuous
										if (accession.compare("IMS:1000030") == 0) {
											setAttribute(currentElement3, "accession", "IMS:1000031");
											setAttribute(currentElement3, "name", "processed");
										}

										// profile spectrum
										if (accession.compare("MS:1000128") == 0) {											
											setAttribute(currentElement3, "accession", "MS:1000127");
											setAttribute(currentElement3, "name", "centroid spectrum");
										}

										// MD5 checksum
										if (accession.compare("IMS:1000090") == 0) {
											QFile ibd(convertedibdfilename.c_str());
											if (ibd.open(QFile::ReadOnly)) {
												QCryptographicHash hash(QCryptographicHash::Md5);
												if (hash.addData(&ibd)) {
													setAttribute(currentElement3, "value", hash.result().toHex().toUpper());
												}
											}
										}

									}

								}


								currentNode3 = currentNode3->getNextSibling();

							}

				
						}

					}


					currentNode2 = currentNode2->getNextSibling();

				}

			}


			if (compareElementTagName(currentElement1, "referenceableParamGroupList")) {


				// childrens of referenceableParamGroupList
				DOMNode* currentNode2 = currentNode1->getFirstChild();
				while (currentNode2) {

					
					if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
			
						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (compareElementTagName(currentElement2, "referenceableParamGroup")) {

							string id = getAttribute(currentElement2, "id");
							
							if (id.compare("spectrum") == 0) {			
									
								
								// childrens of referenceableParamGroup
								DOMNode* currentNode3 = currentNode2->getFirstChild();
								while (currentNode3) {

									
									if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
			
										DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
										if (compareElementTagName(currentElement3, "cvParam")) {
											
											string accession = getAttribute(currentElement3, "accession");

											// profile spectrum
											if (accession.compare("MS:1000128") == 0) {
												setAttribute(currentElement3, "accession", "MS:1000127");
												setAttribute(currentElement3, "name", "centroid spectrum");
											}											

										}

									}


									currentNode3 = currentNode3->getNextSibling();

								}


							}

						}

					}


					currentNode2 = currentNode2->getNextSibling();

				}


			}


			if (compareElementTagName(currentElement1, "run")) {


				// childrens of run
				DOMNode* currentNode2 = currentNode1->getFirstChild();
				while (currentNode2) {

					
					if (currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
			
						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (compareElementTagName(currentElement2, "spectrumList")) {


							// childrens of spectrumList
							DOMNode* currentNode3 = currentNode2->getFirstChild();
							while (currentNode3) {

								
								if (currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
			
									DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
									if (compareElementTagName(currentElement3, "spectrum")) {


										// childrens of spectrum
										DOMNode* currentNode4 = currentNode3->getFirstChild();
										while (currentNode4) {

											
											if (currentNode4->getNodeType() == DOMNode::ELEMENT_NODE) {
			
												DOMElement* currentElement4 = dynamic_cast<xercesc::DOMElement*>(currentNode4);
												if (compareElementTagName(currentElement4, "cvParam")) {

													string accession = getAttribute(currentElement4, "accession");

													// profile spectrum
													if (accession.compare("MS:1000128") == 0) {											
														setAttribute(currentElement4, "accession", "MS:1000127");
														setAttribute(currentElement4, "name", "centroid spectrum");
													}

												}

												if (compareElementTagName(currentElement4, "binaryDataArrayList")) {


													// childrens of binaryDataArrayList
													DOMNode* currentNode5 = currentNode4->getFirstChild();
													while (currentNode5) {

														
														if (currentNode5->getNodeType() == DOMNode::ELEMENT_NODE) {
			
															DOMElement* currentElement5 = dynamic_cast<xercesc::DOMElement*>(currentNode5);
															if (compareElementTagName(currentElement5, "binaryDataArray")) {


																// childrens of binaryDataArray
																bool mzarray = false;
																bool intensityarray = false;
																unsigned long long length = 0;

																DOMNode* currentNode6 = currentNode5->getFirstChild();
																while (currentNode6) {

																	
																	if (currentNode6->getNodeType() == DOMNode::ELEMENT_NODE) {

			
																		DOMElement* currentElement6 = dynamic_cast<xercesc::DOMElement*>(currentNode6);
																		if (compareElementTagName(currentElement6, "referenceableParamGroupRef")) {

																			string ref = getAttribute(currentElement6, "ref");

																			if (ref.compare("mzArray") == 0) {
																				mzarray = true;
																			}

																			if ((ref.compare("intensities") == 0) || (ref.compare("intensityArray") == 0)) {
																				intensityarray = true;
																			}

																		}

																		if (compareElementTagName(currentElement6, "cvParam")) {

																			string accession = getAttribute(currentElement6, "accession");

																			if (mzarray) {
																				// external offset
																				if (accession.compare("IMS:1000102") == 0) {
																					stringstream ss;
																					ss << offset;
																					setAttribute(currentElement6, "value", ss.str().c_str());
																				}

																				// external encoded length
																				if (accession.compare("IMS:1000104") == 0) {
																					stringstream ss;
																					ss << peaklists[currentid].size() * mz_size;
																					setAttribute(currentElement6, "value", ss.str().c_str());
																				}

																				// external array length
																				if (accession.compare("IMS:1000103") == 0) {
																					stringstream ss;
																					ss << peaklists[currentid].size();
																					setAttribute(currentElement6, "value", ss.str().c_str());
																				}
																			}

																			if (intensityarray) {
																				// external offset
																				if (accession.compare("IMS:1000102") == 0) {			
																					stringstream ss;
																					ss << offset + peaklists[currentid].size() * mz_size;
																					setAttribute(currentElement6, "value", ss.str().c_str());
																				}

																				// external encoded length
																				if (accession.compare("IMS:1000104") == 0) {
																					stringstream ss;
																					ss << peaklists[currentid].size() * intensity_size;
																					setAttribute(currentElement6, "value", ss.str().c_str());
																				}

																				// external array length
																				if (accession.compare("IMS:1000103") == 0) {
																					stringstream ss;
																					ss << peaklists[currentid].size();
																					setAttribute(currentElement6, "value", ss.str().c_str());
																				}
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

											}


											currentNode4 = currentNode4->getNextSibling();

										}


										offset += peaklists[currentid].size() * (mz_size + intensity_size);
										currentid++;


									}

								}


								currentNode3 = currentNode3->getNextSibling();

							}

							
						}

					}


					currentNode2 = currentNode2->getNextSibling();

				}


			}

		}


		currentNode1 = currentNode1->getNextSibling();

	}


}


vector<cImzMLItem>& cImzML::getItems() {
	return imzmlitems;
}


bool cImzML::hasProfileSpectra() {
	return profilespectra;
}


bool cImzML::use64BitMzPrecision() {
	return use_64bit_float_mz_precision;
}


bool cImzML::use64BitIntensityPrecision() {
	return use_64bit_float_intensity_precision;
}


void cImzML::write(string& filename) {
	if (document == 0) {
		return;
	}

#if OS_TYPE == WIN
	DOMImplementation *implementation = DOMImplementationRegistry::getDOMImplementation(L"LS");
#else
	XMLCh* features = XMLString::transcode("LS");
	DOMImplementation *implementation = DOMImplementationRegistry::getDOMImplementation(features);
	XMLString::release(&features);
#endif
	DOMLSSerializer *serializer = ((DOMImplementationLS*)implementation)->createLSSerializer(); 

	if (serializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true)) {
		serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true); 
	}
	XMLCh* newline = XMLString::transcode("\r\n");
	serializer->setNewLine(newline);
	XMLString::release(&newline);

	XMLCh* outputpath = XMLString::transcode(filename.c_str()); 
	XMLFormatTarget *formatTarget = new LocalFileFormatTarget(outputpath);
	XMLString::release(&outputpath);

	DOMLSOutput *output = ((DOMImplementationLS*)implementation)->createLSOutput(); 
	output->setByteStream(formatTarget); 
	serializer->write(document, output); 
	serializer->release();  
	delete formatTarget; 
	output->release(); 
}

