#include "core/cSequenceDatabase.h"


#include "gui/cMainThread.h"


void cSequenceDatabase::parseRT(bool keeprt) {
	double rtmin, rtmax, rttmp;
	string s;
	string tmp;
	size_t pos;

	for (auto& it : sequences) {
		s = it.getName();
		pos = s.find("@RT");
		if (pos != string::npos) {
			s = s.substr(pos + 3);

			tmp = s;
			s.clear();
			for (auto& it2 : tmp) {
				if (!isspace(it2)) {
					s.push_back(it2);
				}
			}
			
			sscanf_s(s.c_str(), "%lf-%lf", &rtmin, &rtmax);

			if (rtmin > rtmax) {
				rttmp = rtmin;
				rtmin = rtmax;
				rtmax = rttmp;
			}

			s = it.getName();
			s = s.substr(0, pos);

			if (keeprt) {
				std::ostringstream out;
				out.precision(2);
				out << std::fixed << s;
				out << "@RT";
				out << rtmin;
				out << "-";
				out << rtmax;

				s = out.str();
			}

			it.setName(s);

			it.setRetentionTime(rtmin, rtmax);
		}
	}
}


cSequenceDatabase::cSequenceDatabase() {
	clear();
}


void cSequenceDatabase::loadFromPlainTextStream(ifstream &stream, bool keeprt) {
	string s, type;
	cSequence sequence;
	size_t pos;

	sequences.clear();
	while (stream.good()) {
		getline(stream,s);

		// skip a comment
		if ((s.length() > 0) && (s[0] == '#')) {
			continue;
		}

		sequence.clear();

		// load the peptide type
		pos = s.find('\t');
		if (pos != string::npos) {
			type = s.substr(0, pos);
			sequence.setPeptideType(getPeptideTypeFromString(type));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load name
		pos = s.find('\t');
		if (pos != string::npos) {
			sequence.setName(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load summary formula
		pos = s.find('\t');
		if (pos != string::npos) {
			sequence.setSummaryFormula(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load mass
		pos = s.find('\t');
		if (pos != string::npos) {
			// the value is ignored
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load sequence
		pos = s.find('\t');
		if (pos != string::npos) {
			sequence.setSequence(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load N-terminal modification
		pos = s.find('\t');
		if (pos != string::npos) {
			sequence.setNTterminalModification(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load C-terminal modification
		pos = s.find('\t');
		if (pos != string::npos) {
			sequence.setCTterminalModification(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load branch modification
		pos = s.find('\t');
		if (pos != string::npos) {
			sequence.setBranchModification(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load reference
		#if OS_TYPE != WIN
			if ((s.size() > 0) && (s.back() == '\r')) {
				s = s.substr(0, s.size() - 1);
			}
		#endif
		sequence.setReference(s);

		sequences.push_back(sequence);
	}

	parseRT(keeprt);
}


void cSequenceDatabase::storeToPlainTextStream(ofstream &stream, bool keeprt) {
	parseRT(keeprt);

	cSummaryFormula formula;
	for (int i = 0; i < (int)sequences.size(); i++) {
		stream << getStringFromPeptideType(sequences[i].getPeptideType()) << "\t";
		stream << sequences[i].getName() << "\t";
		stream << sequences[i].getSummaryFormula() << "\t";
		formula.setFormula(sequences[i].getSummaryFormula());
		stream << std::fixed << std::setprecision(10) << formula.getMass() << "\t";
		stream << sequences[i].getSequence() << "\t";
		stream << sequences[i].getNTterminalModification() << "\t";
		stream << sequences[i].getCTterminalModification() << "\t";
		stream << sequences[i].getBranchModification() << "\t";
		stream << sequences[i].getReference() << endl;
	}
}


int cSequenceDatabase::size() {
	return (int)sequences.size();
}


cSequence& cSequenceDatabase::operator[](int position) {
	return sequences[position];
}


void cSequenceDatabase::clear() {
	sequences.clear();
}


void cSequenceDatabase::push_back(cSequence& sequence) {
	sequences.push_back(sequence);
}


void cSequenceDatabase::erase(int index) {
	if ((index >= 0) && (index < (int)sequences.size())) {
		sequences.erase(sequences.begin() + index);
	}
}


void cSequenceDatabase::store(ofstream& os) {
	int size;

	size = (int)sequences.size();
	os.write((char *)&size, sizeof(int));

	for (int i = 0; i < (int)sequences.size(); i++) {
		sequences[i].store(os);
	}
}


void cSequenceDatabase::load(ifstream& is, int fileversionpart1, int fileversionpart2, int fileversionpart3) {
	int size;

	is.read((char *)&size, sizeof(int));
	sequences.resize(size);

	for (int i = 0; i < (int)sequences.size(); i++) {
		sequences[i].load(is, fileversionpart1, fileversionpart2, fileversionpart3);
	}
}


void cSequenceDatabase::attachDecoys(bool& terminatecomputation) {
	cSequence sequence;
	string summary;

	int seqdbsize = (int)sequences.size();
	sequence.setPeptideType(other);
	sequence.setDecoy(true);

	for (int i = 0; i < seqdbsize; i++) {
		if (terminatecomputation) {
			sequences.clear();
			return;
		}

		summary = sequences[i].getSummaryFormula() + "H";
		sequence.setSummaryFormula(summary);
		sequences.push_back(sequence);
	}
}

