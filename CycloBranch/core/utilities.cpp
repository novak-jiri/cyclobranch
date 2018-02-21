#include "core/utilities.h"

#include "core/cBrick.h"


QString appname = "CycloBranch";
QString appversion = "v. 1.0.1512 (64-bit)";


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
	case linearpolysaccharide:
#if POLYKETIDE_SIDEROPHORES == 1
	case linearpolyketide:
#endif
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case cyclic:
#if POLYKETIDE_SIDEROPHORES == 1
	case cyclicpolyketide:
#endif
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
#if POLYKETIDE_SIDEROPHORES == 1
	if (s.compare("linear-oligoketide-siderophore") == 0) {
		return linearpolyketide;
	}
	if (s.compare("cyclic-oligoketide-siderophore") == 0) {
		return cyclicpolyketide;
	}
#endif
	if (s.compare("linear-polysaccharide") == 0) {
		return linearpolysaccharide;
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
#if POLYKETIDE_SIDEROPHORES == 1
	case linearpolyketide:
		return "linear-oligoketide-siderophore";
		break;
	case cyclicpolyketide:
		return "cyclic-oligoketide-siderophore";
		break;
#endif
	case linearpolysaccharide:
		return "linear-polysaccharide";
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

