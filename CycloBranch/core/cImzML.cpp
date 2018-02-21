#include "core/cImzML.h"


cImzML::cImzML() {
	XMLPlatformUtils::Initialize();
	parser = new XercesDOMParser();
}


cImzML::~cImzML() {
	delete parser;
	XMLPlatformUtils::Terminate();
}


void cImzML::parse(string filename) {

	parser->parse(filename.c_str());
	DOMDocument* document = parser->getDocument();

	DOMElement* root = document->getDocumentElement();
	if (!root) {
		return;
	}


	// childrens of mzML
	for (XMLSize_t i = 0; i < root->getChildNodes()->getLength(); i++) {
		
		DOMNode* currentNode1 = root->getChildNodes()->item(i);
		if (currentNode1->getNodeType() && currentNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
			
			DOMElement* currentElement1 = dynamic_cast<xercesc::DOMElement*>(currentNode1);
			if (XMLString::equals(currentElement1->getTagName(), XMLString::transcode("run"))) {
				

				// childrens of run
				for (XMLSize_t j = 0; j < currentNode1->getChildNodes()->getLength(); j++) {

					DOMNode* currentNode2 = currentNode1->getChildNodes()->item(j);
					if (currentNode2->getNodeType() && currentNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
			
						DOMElement* currentElement2 = dynamic_cast<xercesc::DOMElement*>(currentNode2);
						if (XMLString::equals(currentElement2->getTagName(), XMLString::transcode("spectrumList"))) {


							// childrens of spectrumList
							for (XMLSize_t k = 0; k < currentNode2->getChildNodes()->getLength(); k++) {

								DOMNode* currentNode3 = currentNode2->getChildNodes()->item(k);
								if (currentNode3->getNodeType() && currentNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
			
									DOMElement* currentElement3 = dynamic_cast<xercesc::DOMElement*>(currentNode3);
									if (XMLString::equals(currentElement3->getTagName(), XMLString::transcode("spectrum"))) {


										cImzMLItem imzmlitem;


										// childrens of spectrum
										for (XMLSize_t l = 0; l < currentNode3->getChildNodes()->getLength(); l++) {

											DOMNode* currentNode4 = currentNode3->getChildNodes()->item(l);
											if (currentNode4->getNodeType() && currentNode4->getNodeType() == DOMNode::ELEMENT_NODE) {
			
												DOMElement* currentElement4 = dynamic_cast<xercesc::DOMElement*>(currentNode4);
												if (XMLString::equals(currentElement4->getTagName(), XMLString::transcode("scanList"))) {


													// childrens of scanList
													for (XMLSize_t m = 0; m < currentNode4->getChildNodes()->getLength(); m++) {

														DOMNode* currentNode5 = currentNode4->getChildNodes()->item(m);
														if (currentNode5->getNodeType() && currentNode5->getNodeType() == DOMNode::ELEMENT_NODE) {
			
															DOMElement* currentElement5 = dynamic_cast<xercesc::DOMElement*>(currentNode5);
															if (XMLString::equals(currentElement5->getTagName(), XMLString::transcode("scan"))) {


																// childrens of scan
																for (XMLSize_t n = 0; n < currentNode5->getChildNodes()->getLength(); n++) {

																	DOMNode* currentNode6 = currentNode5->getChildNodes()->item(n);
																	if (currentNode6->getNodeType() && currentNode6->getNodeType() == DOMNode::ELEMENT_NODE) {
			
																		DOMElement* currentElement6 = dynamic_cast<xercesc::DOMElement*>(currentNode6);
																		if (XMLString::equals(currentElement6->getTagName(), XMLString::transcode("cvParam"))) {


																			const XMLCh* xmlch_name = currentElement6->getAttribute(XMLString::transcode("name"));
																			string name = XMLString::transcode(xmlch_name);

																			const XMLCh* xmlch_value = currentElement6->getAttribute(XMLString::transcode("value"));
																			if (name.compare("position x") == 0) {			
																				imzmlitem.x = atoi(XMLString::transcode(xmlch_value));
																			}

																			if (name.compare("position y") == 0) {
																				imzmlitem.y = atoi(XMLString::transcode(xmlch_value));
																			}

																			
																		}

																	}

																}


															}

														}

													}


												}

												if (XMLString::equals(currentElement4->getTagName(), XMLString::transcode("binaryDataArrayList"))) {


													// childrens of binaryDataArrayList
													for (XMLSize_t m = 0; m < currentNode4->getChildNodes()->getLength(); m++) {

														DOMNode* currentNode5 = currentNode4->getChildNodes()->item(m);
														if (currentNode5->getNodeType() && currentNode5->getNodeType() == DOMNode::ELEMENT_NODE) {
			
															DOMElement* currentElement5 = dynamic_cast<xercesc::DOMElement*>(currentNode5);
															if (XMLString::equals(currentElement5->getTagName(), XMLString::transcode("binaryDataArray"))) {


																// childrens of binaryDataArray
																bool mzarray = false;
																unsigned long long offset = 0;
																unsigned long long length = 0;
																for (XMLSize_t n = 0; n < currentNode5->getChildNodes()->getLength(); n++) {

																	DOMNode* currentNode6 = currentNode5->getChildNodes()->item(n);
																	if (currentNode6->getNodeType() && currentNode6->getNodeType() == DOMNode::ELEMENT_NODE) {
			
																		DOMElement* currentElement6 = dynamic_cast<xercesc::DOMElement*>(currentNode6);
																		if (XMLString::equals(currentElement6->getTagName(), XMLString::transcode("cvParam"))) {


																			const XMLCh* xmlch_name = currentElement6->getAttribute(XMLString::transcode("name"));
																			string name = XMLString::transcode(xmlch_name);

																			const XMLCh* xmlch_value = currentElement6->getAttribute(XMLString::transcode("value"));
																			if (name.compare("external offset") == 0) {			
																				offset = stoull(XMLString::transcode(xmlch_value));
																			}

																			if (name.compare("external encoded length") == 0) {
																				length = stoull(XMLString::transcode(xmlch_value));
																			}

																			
																		}

																		if (XMLString::equals(currentElement6->getTagName(), XMLString::transcode("referenceableParamGroupRef"))) {

																			const XMLCh* xmlch_ref = currentElement6->getAttribute(XMLString::transcode("ref"));
																			string ref = XMLString::transcode(xmlch_ref);

																			if (ref.compare("mzArray") == 0) {
																				mzarray = true;
																			}

																		}

																	}

																}

																if (mzarray) {
																	imzmlitem.mzstart = offset;
																	imzmlitem.mzlength = length;
																}
																else {
																	imzmlitem.intensitystart = offset;
																	imzmlitem.intensitylength = length;
																}

															}

														}

													}


												}

											}

										}


										imzmlitems.push_back(imzmlitem);


									}

								}

							}

							
						}

					}

				}


			}

		}

	}


}


vector<cImzMLItem>& cImzML::getItems() {
	return imzmlitems;
}

