#include "core/utilities.h"


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

