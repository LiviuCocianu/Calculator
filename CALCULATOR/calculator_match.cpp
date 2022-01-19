#include <iostream>
#include <regex>
#include "calculator_match.h"

using namespace std;

CalculatorMatch::CalculatorMatch() {
	ecuatie = "";
	nrRez = 0;
	rezultate = nullptr;
	pozitii = nullptr;
}

CalculatorMatch::CalculatorMatch(string ecuatie, char op) : CalculatorMatch() {
	this->ecuatie = ecuatie;

	regex expr;

	switch (op) {
		case '[':
			expr = regex("(\\[[^\\[\\]]+\\])");
			break;
		case '(':
			expr = regex("(\\([^\\(\\)]+\\))");
			break;
		default:
			expr = regex(CalculatorMatch::getRegexOperatie(op));
			break;
	}

	int nrPotriviri = CalculatorMatch::getNumarRezultate(ecuatie, expr);

	rezultate = new string[nrPotriviri];
	pozitii = new int[nrPotriviri];

	CalculatorMatch::getPotriviri(ecuatie, expr, rezultate, pozitii, nrRez);
}

CalculatorMatch::~CalculatorMatch() {
	nrRez = 0;
	delete[] rezultate;
	rezultate = nullptr;
	delete[] pozitii;
	pozitii = nullptr;
}

CalculatorMatch::CalculatorMatch(const CalculatorMatch& cm) {
	ecuatie = cm.ecuatie;
	nrRez = cm.nrRez;

	if (cm.nrRez > 0) {
		rezultate = new string[cm.nrRez];
		pozitii = new int[cm.nrRez];

		if (cm.rezultate != nullptr) {
			for (int i = 0; i < cm.nrRez; i++) {
				rezultate[i] = cm.rezultate[i];
			}
		}

		if (cm.pozitii != nullptr) {
			for (int i = 0; i < cm.nrRez; i++) {
				pozitii[i] = cm.pozitii[i];
			}
		}
	} else {
		rezultate = nullptr;
		pozitii = nullptr;
	}
}

string CalculatorMatch::getEcuatie() {
	string cpy = ecuatie;
	return cpy;
}

string* CalculatorMatch::getRezultate() {
	string* copie = new string[nrRez];

	for (int i = 0; i < nrRez; i++) {
		copie[i] = rezultate[i];
	}

	return copie;
}

int CalculatorMatch::getNumarRezultate() {
	return nrRez;
}

int* CalculatorMatch::getPozitii() {
	int* copie = new int[nrRez];

	for (int i = 0; i < nrRez; i++) {
		copie[i] = pozitii[i];
	}

	return copie;
}

string CalculatorMatch::getRegexOperatie(char oper) {
	string expr = "(-?\\d+\\.?(\\d+)?|[a-zA-Z]{1})\\";
	expr.push_back(oper);
	expr.append("(-?\\d+\\.?(\\d+)?|[a-zA-Z]{1})");

	return expr;
}

void CalculatorMatch::setEcuatie(string ecuatie) {
	if (ecuatie.empty()) return;
	this->ecuatie = ecuatie;
}

void CalculatorMatch::setRezultate(string* rez, int nrRez) {
	if (rez == nullptr || nrRez <= 0) {
		rezultate = nullptr;
		return;
	}

	delete[] rezultate;
	rezultate = new string[nrRez];

	for (int i = 0; i < nrRez; i++) {
		rezultate[i] = rez[i];
	}
}

void CalculatorMatch::setNumarRezultate(int nrRez) {
	if (nrRez <= 0) return;
	if (nrRez == this->nrRez) return;

	string* rez = new string[nrRez];
	int* poz = new int[nrRez];

	for (int i = 0; i < nrRez; i++) {
		rez[i] = rezultate[i];
		poz[i] = pozitii[i];
	}

	delete[] rezultate;
	rezultate = rez;
	delete[] pozitii;
	pozitii = poz;
	this->nrRez = nrRez;
}

void CalculatorMatch::setPozitii(int* poz, int nrRez) {
	if (poz == nullptr || nrRez <= 0) {
		pozitii = nullptr;
		return;
	}

	delete[] pozitii;
	pozitii = new int[nrRez];

	for (int i = 0; i < nrRez; i++) {
		pozitii[i] = poz[i];
	}
}

void CalculatorMatch::substituie(string& dest, string rez, int indexRez) {
	dest.replace(getPozitii()[indexRez], getRezultate()[indexRez].size(), rez);
}

void CalculatorMatch::getPotriviri(string s, regex r, string* matches, int* pozitii, int& nrRez) {
	nrRez = 0;

	smatch res;
	string::const_iterator searchStart(s.cbegin());

	string pseudoRezultate[400];
	int pseudoPozitii[400];

	while (regex_search(searchStart, s.cend(), res, r)) {
		pseudoRezultate[nrRez] = res[0];

		// Ecuatia va fi taiata de la sfarsitul rezultatului anterior incolo,
		// prin urmare calculam care ar fi "pozitia globala" in intreaga ecuatie
		if (nrRez > 0) {
			pseudoPozitii[nrRez] = pseudoPozitii[nrRez - 1] + pseudoRezultate[nrRez - 1].size() + res.position();
		} else {
			pseudoPozitii[nrRez] = res.position();
		}

		nrRez++;

		searchStart = res.suffix().first;
	}

	for (int i = 0; i < nrRez; i++) {
		matches[i] = pseudoRezultate[i];
		pozitii[i] = pseudoPozitii[i];
	}
}

int CalculatorMatch::getNumarRezultate(string s, regex r) {
	int nrRez = 0;

	smatch res;
	string::const_iterator searchStart(s.cbegin());

	while (regex_search(searchStart, s.cend(), res, r)) {
		nrRez++;
		searchStart = res.suffix().first;
	}

	return nrRez;
}

CalculatorMatch& CalculatorMatch::operator=(const CalculatorMatch& cm) {
	if (this == &cm) return *this;

	ecuatie = cm.ecuatie;
	nrRez = cm.nrRez;

	if (cm.nrRez > 0) {
		rezultate = new string[cm.nrRez];
		pozitii = new int[cm.nrRez];

		if (cm.rezultate != nullptr) {
			for (int i = 0; i < cm.nrRez; i++) {
				rezultate[i] = cm.rezultate[i];
			}
		}

		if (cm.pozitii != nullptr) {
			for (int i = 0; i < cm.nrRez; i++) {
				pozitii[i] = cm.pozitii[i];
			}
		}
	} else {
		rezultate = nullptr;
		pozitii = nullptr;
	}

	return *this;
}

string& CalculatorMatch::operator[](int i) {
	if (i > nrRez) return rezultate[nrRez - 1];
	if (i < 0) return rezultate[0];
	return rezultate[i];
}

CalculatorMatch& CalculatorMatch::operator++() {
	CalculatorMatch cpy = *this;
	cpy.setNumarRezultate(cpy.getNumarRezultate() + 1);
	return cpy;
}

istream& operator>>(istream& in, CalculatorMatch& cm) {
	return in;
}

ostream& operator<<(ostream& out, const CalculatorMatch& cm) {
	return out;
}