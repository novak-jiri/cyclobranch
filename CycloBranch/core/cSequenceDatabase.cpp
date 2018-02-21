#include "core/cSequenceDatabase.h"


#include "gui/cMainThread.h"


cSequenceDatabase::cSequenceDatabase() {
	clear();
}


void cSequenceDatabase::loadFromPlainTextStream(ifstream &stream) {
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
			if (type.compare("linear") == 0) {
				sequence.setPeptideType(linear);
			}
			if (type.compare("cyclic") == 0) {
				sequence.setPeptideType(cyclic);
			}
			if (type.compare("branched") == 0) {
				sequence.setPeptideType(branched);
			}
			if (type.compare("branch-cyclic") == 0) {
				sequence.setPeptideType(lasso);
			}
			if (type.compare("linearpolysaccharide") == 0) {
				sequence.setPeptideType(linearpolysaccharide);
			}
			if (type.compare("other") == 0) {
				sequence.setPeptideType(other);
			}
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
		#if OS_TYPE == UNX
			if ((s.size() > 0) && (s.back() == '\r')) {
				s = s.substr(0, s.size() - 1);
			}
		#endif
		sequence.setReference(s);

		sequences.push_back(sequence);
	}

}


void cSequenceDatabase::storeToPlainTextStream(ofstream &stream) {
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


void cSequenceDatabase::store(ofstream& os) {
	int size;

	size = (int)sequences.size();
	os.write((char *)&size, sizeof(int));

	for (int i = 0; i < (int)sequences.size(); i++) {
		sequences[i].store(os);
	}
}


void cSequenceDatabase::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	sequences.resize(size);

	for (int i = 0; i < (int)sequences.size(); i++) {
		sequences[i].load(is);
	}
}

