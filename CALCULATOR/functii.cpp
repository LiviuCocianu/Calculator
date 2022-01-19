#include <iostream>
#include <regex>
#include <sstream>
#include <stdlib.h>

#include "functii.h"
#include "calculator_match.h"

using namespace std;

void printLine(const char* linie, int n) {
	cout << linie << endl;
	for (int i = 0; i < n; i++) cout << endl;
}

bool contains(string s, string match) {
	return s.find(match) != string::npos;
}

bool contains(string s, char match) {
	string wrap;
	wrap += match;
	return contains(s, wrap);
}

bool equalsIgnoreCase(string s1, string s2) {
	return regex_match(s1, regex(s2, regex_constants::icase));
}

string removeSpaces(string s) {
	s.erase(remove(s.begin(), s.end(), ' '), s.end());
	return s;
}

bool startsWith(string s, string seq) {
	return s.rfind(seq, 0) == 0;
}

string unwrap(string s, int n) {
	if (n < 1) n = 1;
	if (n > s.size()) n = s.size();

	s.replace(0, n, "");
	s.replace(s.size() - n, n, "");
	return s;
}

string* split(string s, char delim, int &n) {
	stringstream ss(s);
	string segment;
	string* ret;

	n = 0;
	while (getline(ss, segment, delim)) {
		n++;
	}

	stringstream ss2(s);
	ret = new string[n];

	int m = n;
	while (getline(ss2, segment, delim)) {
		ret[n - m] = segment;
		m--;
	}

	return ret;
}

void replaceAt(string& src, string with, regex r, int n) {
	string* potriviri;
	int* pozitii;
	int nrRez;

	int nrPotriviri = CalculatorMatch::getNumarRezultate(src, r);

	potriviri = new string[nrPotriviri];
	pozitii = new int[nrPotriviri];

	CalculatorMatch::getPotriviri(src, r, potriviri, pozitii, nrRez);

	CalculatorMatch cm;

	cm.setRezultate(potriviri, nrRez);
	cm.setPozitii(pozitii, nrRez);
	cm.setNumarRezultate(nrRez);

	cm.substituie(src, with, n);
}

int ifileSize(istream& f) {
	int pre = f.tellg();
	f.seekg(0, f.end);
	int length = f.tellg();
	f.seekg(0, pre);

	return length;
}

int ofileSize(ostream& f) {
	int pre = f.tellp();
	f.seekp(0, f.end);
	int length = f.tellp();
	f.seekp(0, pre);

	return length;
}

double mod(double n) {
	return n >= 0 ? n : -n;
}

double nthRoot(float num, float gr) {
	double xPre = rand() % 10;
	double eps = 1e-3;
	double delX = INT_MAX;
	double xK;

	while (delX > eps) {
		xK = ((gr - 1.0) * xPre +
			(double) num / powf(xPre, gr - 1)) / (double) gr;
		delX = mod(xK - xPre);
		xPre = xK;
	}

	return xK;
}