#include "core/cMzML.h"

#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include "core/cPeaksList.h"
#include "gui/cMainThread.h"


bool compareElementTagName(DOMElement* element, const char* name) {
	XMLCh* transcoded = XMLString::transcode(name);
	if (XMLString::equals(element->getTagName(), transcoded)) {
		XMLString::release(&transcoded);
		return true;
	}
	XMLString::release(&transcoded);
	return false;
}


string getAttribute(DOMElement* element, const char* name) {
	XMLCh* transcodedname = XMLString::transcode(name);
	char* transcodedvalue = XMLString::transcode(element->getAttribute(transcodedname));
	string value = transcodedvalue;
	XMLString::release(&transcodedname);
	XMLString::release(&transcodedvalue);
	return value;
}


void setAttribute(DOMElement* element, const char* name, const char* value) {
	XMLCh* transcodedname = XMLString::transcode(name);
	XMLCh* transcodedvalue = XMLString::transcode(value);
	element->setAttribute(transcodedname, transcodedvalue);
	XMLString::release(&transcodedname);
	XMLString::release(&transcodedvalue);
}


cMzML::cMzML() {
	profilespectra = false;
	parser = new XercesDOMParser();
	document = 0;
}


cMzML::~cMzML() {
	delete parser;
}


int cMzML::parse(string& filename, vector<cPeaksList>& peaklists, eModeType mode, cMainThread* os, bool& terminatecomputation) {

	parser->parse(filename.c_str());
	document = parser->getDocument();

	DOMElement* root = document->getDocumentElement();

	if (!root) {
		return 0;
	}

	if (!compareElementTagName(root, "mzML")) {

		DOMNode* currentNode1 = root->getFirstChild();
		while (currentNode1) {

			
			if (currentNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
			
				DOMElement* currentElement1 = dynamic_cast<xercesc::DOMElement*>(currentNode1);
				if (compareElementTagName(currentElement1, "mzML")) {

					root = currentElement1;
					break; // root found, it is not necessary to traverse the rest of tree

				}

			}


			currentNode1 = currentNode1->getNextSibling();

		}

		if (!root) {
			return 0;
		}

	}

	profilespectra = false;
	int count = 0;
	ofstream mgfofstream;

	char tempstring[1024];
	string peaksstring;
	peaksstring.reserve(25000000);

	int strip;

	string mgfname = filename.substr(0, (int)filename.size() - 4);
	stringstream ss;
	
	// childrens of mzML
	DOMNode* currentNode1 = root->getFirstChild();
	while (currentNode1) {
		
		
		if (currentNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
			
			DOMElement* currentElement1 = dynamic_cast<xercesc::DOMElement*>(currentNode1);
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


										string title = getAttribute(currentElement3, "id");


										// childrens of spectrum
										DOMNode* currentNode4 = currentNode3->getFirstChild();
										while (currentNode4) {

											
											if (currentNode4->getNodeType() == DOMNode::ELEMENT_NODE) {
			
												DOMElement* currentElement4 = dynamic_cast<xercesc::DOMElement*>(currentNode4);
												if (compareElementTagName(currentElement4, "cvParam")) {


														string accession = getAttribute(currentElement4, "accession");

														if (accession.compare("MS:1000128") == 0) {			
															profilespectra = true;
														}

														if (accession.compare("MS:1000511") == 0) {

															int level = atoi(getAttribute(currentElement4, "value").c_str());

															if ((level == 1) && (mode != dereplication)) {
																continue;
															}
															
															if ((level > 1) && (mode == dereplication)) {
																continue;
															}

														}


												}

												if (compareElementTagName(currentElement4, "binaryDataArrayList")) {


													cPeaksList peaklist;
													bool peaklistdefined = false;
														

													// childrens of binaryDataArrayList
													DOMNode* currentNode5 = currentNode4->getFirstChild();
													while (currentNode5) {

														
														if (currentNode5->getNodeType() == DOMNode::ELEMENT_NODE) {
			
															DOMElement* currentElement5 = dynamic_cast<xercesc::DOMElement*>(currentNode5);
															if (compareElementTagName(currentElement5, "binaryDataArray")) {


																// childrens of binaryDataArray
																bool precision64 = false;
																bool mzratio = false;
																bool intensity = false;

																DOMNode* currentNode6 = currentNode5->getFirstChild();
																while (currentNode6) {

																	
																	if (currentNode6->getNodeType() == DOMNode::ELEMENT_NODE) {
			

																		DOMElement* currentElement6 = dynamic_cast<xercesc::DOMElement*>(currentNode6);
																		if (compareElementTagName(currentElement6, "cvParam")) {


																			string accession = getAttribute(currentElement6, "accession");

																			if (accession.compare("MS:1000523") == 0) {			
																				precision64 = true;
																			}

																			if (accession.compare("MS:1000514") == 0) {
																				mzratio = true;
																			}

																			if (accession.compare("MS:1000515") == 0) {
																				intensity = true;
																			}

																			if (accession.compare("MS:1000574") == 0) {
																				// zlib compression detected
																				if (mgfofstream.is_open()) {
																					mgfofstream.close();
																				}
																				return -3;
																			}

																			
																		}


																		if (compareElementTagName(currentElement6, "binary")) {

																			if (currentElement6->getChildNodes()->getLength() > 0) {
																				
																				XMLSize_t binarysize = 0;
																				XMLByte* decoded = Base64::decodeToXMLByte(currentElement6->getChildNodes()->item(0)->getNodeValue(), &binarysize, XMLPlatformUtils::fgMemoryManager);
																				double dblval;
																				float fltval;
																		
																				int datasize = 4;
																				if (precision64) {
																					datasize = 8;
																				}

																				if (mzratio) {																					
																					for (int ii = 0; ii < binarysize/datasize; ii++) {
																						if (precision64) {
																							memcpy(&dblval, &decoded[ii*datasize], datasize);
																						}
																						else {
																							memcpy(&fltval, &decoded[ii*datasize], datasize);
																						}

																						if (peaklistdefined) {
																							peaklist[ii].mzratio = precision64?dblval:fltval;
																						}
																						else {
																							cPeak peak;
																							peak.mzratio = precision64?dblval:fltval;
																							peaklist.add(peak);
																						}
																					}
																				}

																				if (intensity) {
																					for (int ii = 0; ii < binarysize/datasize; ii++) {
																						if (precision64) {
																							memcpy(&dblval, &decoded[ii*datasize], datasize);
																						}
																						else {
																							memcpy(&fltval, &decoded[ii*datasize], datasize);
																						}

																						if (peaklistdefined) {
																							peaklist[ii].absoluteintensity = precision64?dblval:fltval;
																						}
																						else {
																							cPeak peak;
																							peak.absoluteintensity = precision64?dblval:fltval;
																							peaklist.add(peak);
																						}
																					}																					
																				}

																				XMLPlatformUtils::fgMemoryManager->deallocate(decoded);
																				peaklistdefined = true;
																			
																			}
																			else {
																			
																				// empty peaklist - nothing to do
																			
																			}

																		}


																	}


																	currentNode6 = currentNode6->getNextSibling();

																}


															}

														}


														currentNode5 = currentNode5->getNextSibling();

													}


													if (profilespectra) {

														if (count == 0) {
															ss << mgfname << setw(10) << setfill('0') << 0 << ".mgf";
															mgfofstream.open(ss.str());
														}
														
														if ((count + 1) % 100 == 0) {
															*os << count + 1 << " ";
														}
														if ((count + 1) % 2500 == 0) {
															*os << endl;
														}

														mgfofstream << "BEGIN IONS" << endl;
														mgfofstream << "TITLE=" << title << endl;
														mgfofstream << "SCAN=" << to_string(count + 1) << endl;
														mgfofstream << "PEPMASS=1" << endl;
														mgfofstream << "RTINSECONDS=1" << endl;
														mgfofstream << "CHARGE=1+" << endl << endl;

														peaksstring.clear();
														for (int ii = 0; ii < peaklist.size(); ii++) {
															sprintf_s(tempstring, "%f %f\n\0", peaklist[ii].mzratio, peaklist[ii].absoluteintensity);
															peaksstring.append(tempstring);
														}
														mgfofstream << peaksstring;
														mgfofstream << "END IONS" << endl << endl;

														if (((count + 1) % 100 == 0) && (count > 0)) {
															mgfofstream.close();
															stringstream ss;
															strip = (count + 1) / 100;
															ss << mgfname << setw(10) << setfill('0') << strip << ".mgf";
															mgfofstream.open(ss.str());
														}

														cPeaksList emptypeaklist;
														emptypeaklist.setTitle(title);
														peaklists.push_back(emptypeaklist);
														count++;

													}
													else {

														peaklist.setTitle(title);
														peaklists.push_back(peaklist);

													}

													if (terminatecomputation) {
														peaklists.clear();
														if (mgfofstream.is_open()) {
															mgfofstream.close();
														}
														return -1;
													}
													

												}

											}


											currentNode4 = currentNode4->getNextSibling();

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

		}


		currentNode1 = currentNode1->getNextSibling();

	}

	if (mgfofstream.is_open()) {
		mgfofstream.close();
	}

	return 0;
}


bool cMzML::hasProfileSpectra() {
	return profilespectra;
}

