#include "core/utilities.h"

#include "core/cBrick.h"


QString appname = "CycloBranch";
QString appversion = "v. 1.2.192 (64-bit)";


#if OS_TYPE == UNX
	QString installdir = "/usr/share/cyclobranch/";
#endif

#if OS_TYPE == OSX
	QString installdir = "";
#endif


void storeString(string& s, ofstream& os) {
	int size = (int)s.size();
	os.write((char *)&size, sizeof(int));
	os.write(s.c_str(), size);
}


void loadString(string& s, ifstream& is) {
	int size;
	is.read((char *)&size, sizeof(int));
	s.resize(size);
	is.read(&s[0], size);
}


void storeStringVector(vector<string>& v, ofstream& os) {
	int size = (int)v.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)v.size(); i++) {
		storeString(v[i], os);
	}
}


void loadStringVector(vector<string>& v, ifstream& is) {
	int size;
	is.read((char *)&size, sizeof(int));
	v.resize(size);
	for (int i = 0; i < (int)v.size(); i++) {
		loadString(v[i], is);
	}
}


bool isWhiteSpaceExceptSpace(char c) {
	return isspace(c) && (c != ' ');
}


string& removeWhiteSpacesExceptSpaces(string& s) {
	s.erase(remove_if(begin(s), end(s), isWhiteSpaceExceptSpace), end(s));
	return s;
}


bool checkRegex(ePeptideType peptidetype, string& sequence, string& errormessage) {
	errormessage = "";

	if (sequence.compare("") == 0) {
		errormessage = "The sequence is empty.";
		return false;
	}

	regex rx;
	// [^\\[\\]]+ is used instead of .+ to prevent from a too complex regex error
	switch (peptidetype)
	{
	case linear:
	case linearpolyketide:
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case cyclic:
	case cyclicpolyketide:
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+$";
		break;
	case branched:
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case branchcyclic:
		rx = "(^(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$|^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?$)";
		break;
	case other:
	default:
		rx = ".*";
		break;
	}

	try {
		if (!(regex_search(sequence, rx))) {
			errormessage = "The format of sequence '" + sequence + "' does not correspond to the sequence type '" + getStringFromPeptideType(peptidetype) + "'.";
			return false;
		}
	}
	catch (regex_error& e) {
		errormessage = "checkRegex: regex_search failed, error no. " + to_string(e.code());
		return false;
	}

	return true;
}


void parseBranch(ePeptideType peptidetype, string& composition, vector<string>& vectorcomposition, int& branchstart, int& branchend) {
	string s = composition;
	cBrick b;
	branchstart = -1;
	branchend = -1;

	if ((peptidetype == branched) || (peptidetype == branchcyclic)) {
		int i = 0;
		while (i < (int)s.size()) {
			if (s[i] == '\\') {
				s.erase(s.begin() + i);
			}
			else {
				i++;
			}
		}

		for (int i = 0; i < (int)s.size(); i++) {
			if (s[i] == '(') {
				if (i > 0) {
					b.clear();
					b.setComposition(s.substr(0, i), false);
					branchstart = getNumberOfBricks(b.getComposition());
					s[i] = '-';
				}
				else {
					s.erase(s.begin());
					branchstart = 0;
				}
				break;
			}
		}

		for (int i = 0; i < (int)s.size(); i++) {
			if (s[i] == ')') {
				b.clear();
				b.setComposition(s.substr(0, i), false);
				branchend = getNumberOfBricks(b.getComposition()) - 1;
				if (i < (int)s.size() - 1) {
					s[i] = '-';
				}
				else {
					s.erase(s.begin() + i);
				}
				break;
			}
		}

		if (branchend <= branchstart) {
			branchstart = -1;
			branchend = -1;
		}
	}

	b.clear();
	b.setComposition(s, false);
	b.explodeToStringComposition(vectorcomposition);
}


ePeptideType getPeptideTypeFromString(string& s) {
	if (s.compare("linear") == 0) {
		return linear;
	}
	if (s.compare("cyclic") == 0) {
		return cyclic;
	}
	if (s.compare("branched") == 0) {
		return branched;
	}
	if (s.compare("branch-cyclic") == 0) {
		return branchcyclic;
	}
	if (s.compare("linear-polyketide") == 0) {
		return linearpolyketide;
	}
	if (s.compare("cyclic-polyketide") == 0) {
		return cyclicpolyketide;
	}
	if (s.compare("other") == 0) {
		return other;
	}

	return other;
}


string getStringFromPeptideType(ePeptideType peptidetype) {
	switch (peptidetype)
	{
	case linear:
		return "linear";
		break;
	case cyclic:
		return "cyclic";
		break;
	case branched:
		return "branched";
		break;
	case branchcyclic:
		return "branch-cyclic";
		break;
	case linearpolyketide:
		return "linear-polyketide";
		break;
	case cyclicpolyketide:
		return "cyclic-polyketide";
		break;
	case other:
		return "other";
		break;
	default:
		break;
	}

	return "other";
}


double cropPrecisionToSixDecimals(double value) {
	char buffer[50];
	double val;
	sprintf_s(buffer, "%.6f\0", value);
	sscanf_s(buffer, "%lf", &val);
	return val;
}


QByteArray cropDecimalsByteArray(double value) {
	char buffer[50];
	sprintf_s(buffer, "%.0f\0", value);
	QByteArray bytearray = buffer;
	return bytearray;
}


QByteArray cropPrecisionToSixDecimalsByteArray(double value) {
	char buffer[50];
	sprintf_s(buffer, "%.6f\0", value);
	QByteArray bytearray = buffer;
	return bytearray;
}


bool operator == (cCoordinates const& a, cCoordinates const& b) {
	return ((a.x == b.x) && (a.y == b.y) && (a.id == b.id));
}


double fact(int value) {
	double result = 1;
	while (value > 1) {
		result *= value;
		value--;
	}
	return result;
}


double median(vector<double>& vector) {
	double median = 0;
	sort(vector.begin(), vector.end());
	if (vector.size() > 0) {
		if (vector.size() % 2 == 0) {
			median = (vector[vector.size() / 2 - 1] + vector[vector.size() / 2]) / 2.0;
		}
		else {
			median = vector[vector.size() / 2];
		}
	}
	return median;
}

