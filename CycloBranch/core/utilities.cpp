#include "core/utilities.h"

#include "core/cBrick.h"


QString appname = "CycloBranch";
QString appversion = "v. 2.0.8 (64-bit)";


#if OS_TYPE == UNX
	QString installdir = "/usr/share/cyclobranch/";
#endif

#if OS_TYPE == OSX
	QString installdir = "";
#endif


void storeIntVector(vector<int>& v, ofstream& os) {
	int size = (int)v.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)v.size(); i++) {
		os.write((char *)&(v[i]), sizeof(int));
	}
}


void loadIntVector(vector<int>& v, ifstream& is) {
	int size;
	is.read((char *)&size, sizeof(int));
	v.resize(size);
	for (int i = 0; i < (int)v.size(); i++) {
		is.read((char *)&(v[i]), sizeof(int));
	}
}


void storeDoubleVector(vector<double>& v, ofstream& os) {
	int size = (int)v.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)v.size(); i++) {
		os.write((char *)&(v[i]), sizeof(double));
	}
}


void loadDoubleVector(vector<double>& v, ifstream& is) {
	int size;
	is.read((char *)&size, sizeof(int));
	v.resize(size);
	for (int i = 0; i < (int)v.size(); i++) {
		is.read((char *)&(v[i]), sizeof(double));
	}
}


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


void storeStringIntMap(map<string, int>& map, ofstream& os) {
	int size;
	string s;

	size = (int)map.size();
	os.write((char *)&size, sizeof(int));
	for (auto it = map.begin(); it != map.end(); ++it) {
		s = it->first;
		storeString(s, os);
		os.write((char *)&it->second, sizeof(int));
	}
}


void loadStringIntMap(map<string, int>& map, ifstream& is) {
	int size, value;
	string s;

	is.read((char *)&size, sizeof(int));
	map.clear();
	for (int i = 0; i < size; i++) {
		loadString(s, is);
		is.read((char *)&value, sizeof(int));
		map[s] = value;
	}
}


void storeStringIntMapVector(vector< map<string, int> >& vector, ofstream& os) {
	int size = (int)vector.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)vector.size(); i++) {
		storeStringIntMap(vector[i], os);
	}
}


void loadStringIntMapVector(vector< map<string, int> >& vector, ifstream& is) {
	int size;
	is.read((char *)&size, sizeof(int));
	vector.resize(size);
	for (int i = 0; i < (int)vector.size(); i++) {
		loadStringIntMap(vector[i], is);
	}
}


void storeIntStringMap(map<int, string>& map, ofstream& os) {
	int size = (int)map.size();
	os.write((char *)&size, sizeof(int));
	for (auto it = map.begin(); it != map.end(); ++it) {
		os.write((char *)&it->first, sizeof(int));
		storeString(it->second, os);
	}
}


void loadIntStringMap(map<int, string>& map, ifstream& is) {
	int size, value;
	string s;

	is.read((char *)&size, sizeof(int));
	map.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&value, sizeof(int));
		loadString(s, is);
		map[value] = s;
	}
}


void convertStringIntUnorderedMapToStringVector(unordered_map<string, int>& map, vector<string>& vector) {
	vector.resize(map.size());
	unordered_map<string, int>::iterator it;
	while (map.begin() != map.end()) {
		it = map.begin();
		vector[it->second] = it->first;
		map.erase(it);
	}
}


bool isWhiteSpace(char c) {
	return isspace(c);
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
	switch (peptidetype) {
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


bool checkFile(string filename) {
	ifstream f(filename.c_str());
	return f.good();
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
	switch (peptidetype) {
		case linear:
			return "linear";
		case cyclic:
			return "cyclic";
		case branched:
			return "branched";
		case branchcyclic:
			return "branch-cyclic";
		case linearpolyketide:
			return "linear-polyketide";
		case cyclicpolyketide:
			return "cyclic-polyketide";
		case other:
			return "other";
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


string stripHTML(string& htmlstring) {
	string s;
	bool add = true;

	for (int i = 0; i < (int)htmlstring.size(); i++) {
		if (htmlstring[i] == '<') {
			add = false;
			continue;
		}
		if (htmlstring[i] == '>') {
			add = true;
			continue;
		}
		if (add) {
			s += htmlstring[i];
		}
	}
	return s;
}


void stripIsomers(string& peptidesequence) {
	string tmpsequence;
	bool copy, insideblock, insidehtml;

	tmpsequence = peptidesequence;
	peptidesequence = "";
	copy = true;
	insideblock = false;
	insidehtml = false;
	for (int j = 0; j < (int)tmpsequence.size(); j++) {
		if (tmpsequence[j] == '[') {
			insideblock = true;
		}
		if (tmpsequence[j] == '<') {
			insidehtml = true;
		}
		if (tmpsequence[j] == '>') {
			insidehtml = false;
		}
		if ((tmpsequence[j] == '/') && insideblock && !insidehtml) {
			copy = false;
		}
		if (tmpsequence[j] == ']') {
			insideblock = false;
			copy = true;
		}
		if (copy) {
			peptidesequence += tmpsequence[j];
		}
	}
}


void stripIsomersFromStringVector(vector<string>& acronyms) {
	int size = (int)acronyms.size();
	size_t pos;

	for (int i = 0; i < size; i++) {
		pos = acronyms[i].find('/');
		if (pos != string::npos) {
			acronyms[i] = acronyms[i].substr(0, pos);
		}
	}
}


bool proxyModelCheckInt(QAbstractItemModel* model, int index, int row, int column, QString str, const QModelIndex& parent) {
	switch (index) {
		case 0:
			if (model->data(model->index(row, column, parent)).toInt() == str.toInt()) {
				return true;
			}
			break;
		case 1:
			if (model->data(model->index(row, column, parent)).toInt() < str.toInt()) {
				return true;
			}
			break;
		case 2:
			if (model->data(model->index(row, column, parent)).toInt() <= str.toInt()) {
				return true;
			}
			break;
		case 3:
			if (model->data(model->index(row, column, parent)).toInt() > str.toInt()) {
				return true;
			}
			break;
		case 4:
			if (model->data(model->index(row, column, parent)).toInt() >= str.toInt()) {
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}


bool proxyModelCheckDouble(QAbstractItemModel* model, int index, int row, int column, QString str, const QModelIndex& parent) {
	switch (index) {
		case 0:
			if (model->data(model->index(row, column, parent)).toDouble() == str.toDouble()) {
				return true;
			}
			break;
		case 1:
			if (model->data(model->index(row, column, parent)).toDouble() < str.toDouble()) {
				return true;
			}
			break;
		case 2:
			if (model->data(model->index(row, column, parent)).toDouble() <= str.toDouble()) {
				return true;
			}
			break;
		case 3:
			if (model->data(model->index(row, column, parent)).toDouble() > str.toDouble()) {
				return true;
			}
			break;
		case 4:
			if (model->data(model->index(row, column, parent)).toDouble() >= str.toDouble()) {
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}


bool proxyModelCheckString(QAbstractItemModel* model, int index, int row, int column, QString& itemstr, QString str, const QModelIndex& parent, bool wholeword, Qt::CaseSensitivity casesensitive) {
	string tmpstr = itemstr.toStdString();
	QString qstr = stripHTML(tmpstr).c_str();
	switch (index) {
		case 0:
			if (wholeword) {
				if (qstr.compare(str, casesensitive) == 0) {
					return true;
				}
			}
			else {
				return qstr.contains(str, casesensitive);
			}
			break;
		case 1:
			if (qstr.compare(str, casesensitive) < 0) {
				return true;
			}
			break;
		case 2:
			if (qstr.compare(str, casesensitive) <= 0) {
				return true;
			}
			break;
		case 3:
			if (qstr.compare(str, casesensitive) > 0) {
				return true;
			}
			break;
		case 4:
			if (qstr.compare(str, casesensitive) >= 0) {
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

