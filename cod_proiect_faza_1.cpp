#pragma once
#include <iostream>
#include "validator.h"
using namespace std;

enum class CalculatorError {
	empty, exit, unknown,
	no_arithmetics, forbidden_chars, excessive_symbols,
	illegal_division, improper_brackets, lacks_brackets,
	bracket_order, illegal_brackets, illegal_square_brackets,
	operator_position, negative_root
};

class Calculator {
private:
	string ecuatie;
	double rezultat;
	Validator validator;
	short errorFlag;
	static const char operatori[6];
public:
	Calculator();
	Calculator(string ecuatie);
	~Calculator();
	Calculator(const Calculator& c);

	string getEcuatie();
	double getRezultat();
	Validator& getValidator();
	/*
		Acest getter va fi folosit la citirea continua de la tastatura a
		ecuatiei pana cand nu vor mai exista erori de sintaxa

		Flag-ul se va reseta pentru a permite buclei while sa continue corect
	*/
	short getErrorFlag();
	static char* getOperatori();

	void setEcuatie(string ecuatie);
	void setRezultat(double rez);
	void setValidator(Validator& v); //
	void setErrorFlag(short flag);

	bool proceseaza();
	double calculeaza();

	Calculator& operator=(const Calculator& c);
	Calculator& operator+(const Calculator& c);
	Calculator& operator-(const Calculator& c);
	/*
	Citeste ecuatia de la tastatura.
	Poate genera exceptia enum CalculatorError
	*/
	friend istream& operator>>(istream& in, Calculator& c) throw(CalculatorError);
	friend ostream& operator<<(ostream& out, const Calculator& c);
};

#include <iostream>
#include <string>
#include <regex>
#include <iomanip>

#include "calculator.h"
#include "calculator_match.h"
#include "subcalcul.h"
#include "functii.h"

using namespace std;

const char Calculator::operatori[6] = { '#', '^', '/', '*', '-', '+' };

Calculator::Calculator() {
	validator = *(new Validator(&(this->ecuatie)));
	errorFlag = -1;
	rezultat = DBL_MAX;
}

Calculator::Calculator(string ecuatie) {
	this->ecuatie = ecuatie;
	this->validator = *(new Validator(&(this->ecuatie)));
	errorFlag = -1;
	rezultat = DBL_MAX;
}

Calculator::~Calculator() {
	ecuatie.clear();
	errorFlag = 0;
	rezultat = 0;
}

Calculator::Calculator(const Calculator& c) {
	ecuatie = c.ecuatie;
	validator = c.validator;
	errorFlag = c.errorFlag;
	rezultat = c.rezultat;
}

string Calculator::getEcuatie() {
	string cpy = "";
	cpy = ecuatie;
	return cpy;
}

double Calculator::getRezultat() {
	return rezultat;
}

Validator& Calculator::getValidator() {
	return validator;
}

short Calculator::getErrorFlag() {
	short copie = errorFlag;
	setErrorFlag(-1);
	return copie;
}

char* Calculator::getOperatori() {
	char operatori[6];
	for (int i = 0; i < 6; i++) operatori[i] = Calculator::operatori[i];
	return operatori;
}

void Calculator::setEcuatie(string ecuatie) {
	if (ecuatie.length() == 0) return;
	this->ecuatie = ecuatie;
}

void Calculator::setRezultat(double rez) {
	rezultat = rez;
}

void Calculator::setValidator(Validator& v) {
	if (v.getEcuatie().empty()) return;
	validator.setAdresaEcuatie(v.getAdresaEcuatie());
}

void Calculator::setErrorFlag(short flag) {
	errorFlag = flag < -1 ? -1 : flag;
}

bool Calculator::proceseaza() {
	short exitStatus = getValidator().valideaza();

	if (exitStatus != 0) {
		setErrorFlag(2);

		if (exitStatus == 1) throw CalculatorError::no_arithmetics;
		else if (exitStatus == 2) throw CalculatorError::excessive_symbols;
		else if (exitStatus == 3) throw CalculatorError::forbidden_chars;
		else if (exitStatus == 4) throw CalculatorError::illegal_division;
		else if (exitStatus == 5) throw CalculatorError::operator_position;
		else if (exitStatus == 6) throw CalculatorError::improper_brackets;
		else if (exitStatus == 7) throw CalculatorError::lacks_brackets;
		else if (exitStatus == 8) throw CalculatorError::bracket_order;
		else if (exitStatus == 9) throw CalculatorError::illegal_brackets;
		else if (exitStatus == 10) throw CalculatorError::illegal_square_brackets;
		else throw CalculatorError::unknown;

		return false;
	}

	return true;
}

double Calculator::calculeaza() {
	string rez = removeSpaces(ecuatie);

	if (contains(rez, "[")) {
		while (contains(rez, "[")) {
			CalculatorMatch* parPatrate = new CalculatorMatch(rez, '[');
			Subcalcul* grup = new Subcalcul(parPatrate->getRezultate()[0]);
			parPatrate->substituie(rez, grup->calculeaza(), 0);
		}
	}

	if (contains(rez, "(")) {
		while (contains(rez, "(")) {
			CalculatorMatch* parRotunde = new CalculatorMatch(rez, '(');
			Subcalcul* grup = new Subcalcul(parRotunde->getRezultate()[0]);
			parRotunde->substituie(rez, grup->calculeaza(), 0);
		}
	}

	return (new Subcalcul(rez))->calculeaza();
}

Calculator& Calculator::operator=(const Calculator& c) {
	if (this == &c) return *this;
	ecuatie = c.ecuatie;
	validator = c.validator;
	errorFlag = c.errorFlag;
	rezultat = c.rezultat;
	return *this;
}

Calculator& Calculator::operator+(const Calculator& c) {
	Calculator cpy = *this;

	if (getRezultat() != DBL_MAX && c.rezultat != DBL_MAX) {
		cpy.setRezultat(cpy.getRezultat() + c.rezultat);
	}

	return cpy;
}

Calculator& Calculator::operator-(const Calculator& c) {
	Calculator cpy = *this;

	if (getRezultat() != DBL_MAX && c.rezultat != DBL_MAX) {
		cpy.setRezultat(cpy.getRezultat() - c.rezultat);
	}

	return cpy;
}

istream& operator>>(istream& in, Calculator& c) throw(CalculatorError) {
	string ecuatie;

	cout << "Introdu ecuatia: ";

	getline(in, ecuatie);

	if (equalsIgnoreCase(ecuatie, "exit")) {
		c.setErrorFlag(0);
		throw CalculatorError::exit;
	}

	if (regex_match(ecuatie, regex("^\\s+$")) || ecuatie.length() == 0) {
		c.setErrorFlag(1);
		throw CalculatorError::empty;
	}

	c.setEcuatie(ecuatie);
	bool succes = c.proceseaza();
	if (succes) {
		// Seteaza flag-ul cu o valoare peste 0 pentru a permite buclei while din main
		// sa continue sa primeasca ecuatii dupa acest calcul
		c.setErrorFlag(50);
		double rezultat = c.calculeaza();
		c.setRezultat(rezultat);
		cout << fixed << setprecision(4) << "Rezultat: " << rezultat << endl << endl;
	}

	return in;
}

ostream& operator<<(ostream& out, const Calculator& c) {
	return out;
}

#pragma once

#include <string>
#include "subcalcul.h"

using namespace std;

class CalculatorMatch {
private:
	string ecuatie;
	int nrRez;
	string* rezultate;
	int* pozitii;
public:
	CalculatorMatch();
	CalculatorMatch(string ecuatie, char op);
	~CalculatorMatch();
	CalculatorMatch(const CalculatorMatch& cm);

	string getEcuatie(); //
	string* getRezultate();
	int getNumarRezultate();
	int* getPozitii();

	void setEcuatie(string ecuatie); //
	void setRezultate(string* rez, int nrRez); //
	void setNumarRezultate(int nrRez); //
	void setPozitii(int* poz, int nrRez); //

	// Aceasta metoda nu este un getter!!
	static string getRegexOperatie(char oper);
	// Substituie rezultatul de la index-ul specificat cu rezultatul subcalculului
	void substituie(string& dest, double rez, int indexRez);

	CalculatorMatch& operator=(const CalculatorMatch& cm);
	string& operator[](int i);
	CalculatorMatch& operator++();
	friend istream& operator>>(istream& in, CalculatorMatch& cm);
	friend ostream& operator<<(ostream& out, const CalculatorMatch& cm);
};

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

	smatch res;
	string::const_iterator searchStart(ecuatie.cbegin());

	while (regex_search(searchStart, ecuatie.cend(), res, expr)) {
		if (rezultate == nullptr)
			rezultate = new string[res.size()];
		if (pozitii == nullptr)
			pozitii = new int[res.size()];

		rezultate[nrRez] = res[0];
		pozitii[nrRez] = res.position(nrRez);

		nrRez++;

		searchStart = res.suffix().first;
	}
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
	string expr = "-?\\d+\\.?(\\d+)?\\";
	expr.push_back(oper);
	expr.append("-?\\d+\\.?(\\d+)?");
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

void CalculatorMatch::substituie(string& dest, double rez, int indexRez) {
	dest.replace(getPozitii()[indexRez], getRezultate()[indexRez].size(), to_string(rez));
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

#pragma once

#include <string>
#include <regex>
#include <list>

using namespace std;

// Afiseaza o linie cu inca n linii goale posterior
void printLine(const char* linie, int n);
// Verifica daca un string contine secventa data
bool contains(string s, string match);
// Verifica daca un string contine caracterul dat
bool contains(string s, char match);
// Verifica daca doua string-uri sunt egale, indiferent de majuscule
bool equalsIgnoreCase(string s1, string s2);
// Elimina toate spatiile dintr-un string
string removeSpaces(string s);
// Verifica daca un string incepe cu secventa data
bool startsWith(string s, string seq);
// Elimina n caractere de la inceputul si sfarsitul unui string
string unwrap(string s, int n);
string* split(string s, char delim, int& n);

double nthRoot(float num, float gr);
double mod(double n);

#include <iostream>
#include <regex>
#include <sstream>
#include <stdlib.h>
#include "functii.h"

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

#pragma once

#include <iostream>
#include <string>

using namespace std;

class Subcalcul {
private:
	char* fragment;
	int n;
public:
	Subcalcul();
	Subcalcul(string fragment);
	~Subcalcul();
	Subcalcul(const Subcalcul& sc);

	string getFragment();
	int getMarime();
	void setFragment(string fragment);
	void setMarime(int n);

	static string evalueaza(string ecuatie, char oper);
	double calculeaza();

	Subcalcul& operator=(const Subcalcul& sc);
	bool operator>(const Subcalcul& c);
	bool operator<(const Subcalcul& c);
	friend istream& operator>>(istream& in, Subcalcul& sc);
	friend ostream& operator<<(ostream& out, const Subcalcul& sc);
};

#include <string>
#include <regex>
#include <stdlib.h>

#include "subcalcul.h"
#include "functii.h"
#include "calculator_match.h"
#include "calculator.h"

using namespace std;

Subcalcul::Subcalcul() {
	fragment = new char[1]{ '\0' };
	n = 0;
}

Subcalcul::Subcalcul(string fragment) {
	setFragment(fragment);
}

Subcalcul::~Subcalcul() {
	delete[] fragment;
	fragment = nullptr;
	n = 0;
}

Subcalcul::Subcalcul(const Subcalcul& sc) {
	delete[] fragment;
	fragment = nullptr;
	n = 0;

	if (sc.n > 0) {
		n = sc.n;
		fragment = new char[n];

		for (int i = 0; i < n; i++)
			fragment[i] = sc.fragment[i];
	}
}

string Subcalcul::getFragment() {
	string ret;
	ret += fragment;
	return ret;
}

int Subcalcul::getMarime() {
	return n;
}

void Subcalcul::setFragment(string fragment) {
	if (fragment.empty()) return;

	n = fragment.size() + 1;
	char* ret = new char[n];
	strcpy_s(ret, n, fragment.c_str());

	this->fragment = ret;
}

void Subcalcul::setMarime(int n) {
	this->n = n;
}

string Subcalcul::evalueaza(string ecuatie, char oper) {
	string ecuatieCpy = ecuatie;

	while (regex_search(ecuatieCpy, regex(CalculatorMatch::getRegexOperatie(oper)))) {
		CalculatorMatch* operatii = new CalculatorMatch(ecuatieCpy, oper);
		int n = 0;
		string* sp = split(operatii->getRezultate()[0], oper, n);

		double n1 = stod(sp[0]), n2 = stod(sp[1]);
		double rez = 0;

		switch (oper) {
		case '^': rez = powf(n1, n2); break;
		case '#':
			if (n1 < 0 && (((int)n2) % 2 == 0)) {
				throw CalculatorError::negative_root;
				break;
			}

			rez = nthRoot(n1, n2);
			break;
		case '/':
			if (n1 == 0 || n2 == 0) {
				throw CalculatorError::illegal_division;
				break;
			}

			rez = n1 / n2;
			break;
		case '*': rez = n1 * n2; break;
		case '+': rez = n1 + n2; break;
		case '-': rez = n1 - n2; break;
		}

		operatii->substituie(ecuatieCpy, rez, 0);
	}

	return ecuatieCpy;
}

double Subcalcul::calculeaza() {
	string fragment = getFragment();

	if (startsWith(fragment, "[")) {
		fragment = unwrap(fragment, 1);

		while (contains(fragment, "(")) {
			CalculatorMatch* parRotunde = new CalculatorMatch(fragment, '(');
			Subcalcul* grup = new Subcalcul(parRotunde->getRezultate()[0]);
			parRotunde->substituie(fragment, grup->calculeaza(), 0);
		}

		return (new Subcalcul(fragment))->calculeaza();
	}
	else {
		if (startsWith(fragment, "("))
			fragment = unwrap(fragment, 1);

		// Evalueaza ecuatia data pentru toti operatorii
		for (int i = 0; i < 6; i++) {
			char oper = Calculator::getOperatori()[i];
			if (contains(fragment, oper)) {
				fragment = Subcalcul::evalueaza(fragment, oper);
			}
		}
	}

	return (double)stod(fragment);
}

Subcalcul& Subcalcul::operator=(const Subcalcul& sc) {
	if (this == &sc) return *this;

	delete[] fragment;
	fragment = nullptr;
	n = 0;

	if (sc.n > 0) {
		n = sc.n;
		fragment = new char[n];

		for (int i = 0; i < n; i++)
			fragment[i] = sc.fragment[i];
	}

	return *this;
}

bool Subcalcul::operator>(const Subcalcul& c) {
	return this->n > c.n;
}

bool Subcalcul::operator<(const Subcalcul& c) {
	return this->n < c.n;
}

istream& operator>>(istream& in, Subcalcul& sc) {
	return in;
}

ostream& operator<<(ostream& out, const Subcalcul& sc) {
	return out;
}

#pragma once
#include <iostream>
#include <string>
#include "calculator.h"

using namespace std;

class Validator {
private:
	string* ecuatie;
public:
	Validator();
	Validator(string* ecuatie);
	~Validator();
	Validator(const Validator& v);

	string getEcuatie();
	string* getAdresaEcuatie();
	void setEcuatie(string ecuatie); //
	void setAdresaEcuatie(string* ecuatie);

	/*
		Valideaza ecuatia primita si returneaza o valoare mai
		mare decat 0 daca exista erori de validare

		1 - Ecuatie fara operatii
		2 - Ecuatie cu operatori scrisi incorect
		3 - Ecuatie cu caractere nepermise
		4 - Ecuatia contine impartiri la 0
		5 - Ecuatia incepe cu un operator altul decat minus
		6 - Ecuatia are paranteze scrise incorect
		7 - Ecuatia are paranteze patrate, dar nu si rotunde
		8 - Ecuatia are paranteze ordonate incorect
		9 - Ecuatia are paranteze de forma ((n+m))
		10 - Ecuatia are paranteze de forma [n+m]
	*/
	short valideaza();
	// Face ajustari in spatierea ecuatiei
	void ajusteaza();

	Validator& operator=(const Validator& v);
	// Verifica daca validarea este pozitiva sau nu
	bool operator!();
	bool operator<=(Validator& v);
	bool operator>=(Validator& v);
	friend istream& operator>>(istream& in, Validator& v);
	friend ostream& operator<<(ostream& out, const Validator& v);
};

#include <string>
#include <regex>
#include <iostream>
#include <list>

#include "calculator.h"
#include "validator.h"
#include "functii.h"
#include "calculator_match.h"

using namespace std;

Validator::Validator() {
	ecuatie = new string("");
}

Validator::Validator(string* ecuatie) {
	this->ecuatie = ecuatie;
}

Validator::~Validator() {
	ecuatie = nullptr;
}

Validator::Validator(const Validator& v) {
	ecuatie = v.ecuatie;
}

string Validator::getEcuatie() {
	return *ecuatie;
}

string* Validator::getAdresaEcuatie() {
	return ecuatie;
}

void Validator::setEcuatie(string ecuatie) {
	if (*this->ecuatie == ecuatie) return;
	*this->ecuatie = ecuatie;
}

void Validator::setAdresaEcuatie(string* ecuatie) {
	if (this->ecuatie == ecuatie) return;
	this->ecuatie = ecuatie;
}

// TODO Valideaza pentru cazuri +(n+m)

short Validator::valideaza() {
	// Verifica daca exista semne ce se repeta unele dupa altele 
	regex c2 = regex("((\\+)|(\\-)|(\\*)|(\\/)|(\\^)|(\\#)|(\\.)){2,}");
	// Verifica daca alte caractere decat cele permise se gasesc in ecuatie
	regex c3 = regex("[^\\+\\-\\*\\/\\^\\#\\.\\(\\)\\[\\]\\d]");

	// Elimina toate spatiile din ecuatie
	ajusteaza();
	string ecuatie = getEcuatie();

	// Verifica daca ecuatia are cel putin o operatie. Ex: 2+3; 6/2; 5.2+0.8
	bool p = false;
	for (int i = 0; i < 6; i++) {
		string str = "\\d+\\.?(\\d+)?\\";
		str.push_back(Calculator::getOperatori()[i]);
		str.append("\\d+\\.?(\\d+)?");

		if (regex_search(ecuatie, regex(str))) {
			p = true;
			break;
		}
	}

	if (!p) return 1;

	if (regex_search(ecuatie, c2)) return 2;
	if (regex_search(ecuatie, c3)) return 3;
	// Verificare superficiala pentru impartiri la 0
	// Nota: O verificare mai serioasa se va face la calcul, nu la validare
	if (regex_search(ecuatie, regex("\\/0"))) return 4;

	// Verifica daca ecuatia incepe cu un operator nepermis
	for (int i = 0; i < 6; i++) {
		char oper = Calculator::getOperatori()[i];
		if (oper == '-') continue;

		if (contains(ecuatie, oper)) {
			string expr = "^\\";
			expr += oper;

			if (regex_search(ecuatie, regex(expr))) return 5;
		}
	}

	const int errParanteze = 6;
	if (regex_search(ecuatie, regex("[\\(\\)\\[\\]]"))) {
		// Verifica daca, existand paranteze rotunde, acestea sunt si inchise corect
		if (contains(ecuatie, "(") && contains(ecuatie, ")")) {
			smatch b11, b12;

			regex_search(ecuatie, b11, regex("[\\(]"));
			regex_search(ecuatie, b12, regex("[\\)]"));

			if (b11.size() != b12.size()) return errParanteze;
		}
		// In cazul in care conditia de mai sus nu a fost atinsa si ecuatia
		// contine un capat de paranteza, dar nu si pe celalat
		if ((contains(ecuatie, "(") && !contains(ecuatie, ")"))
			|| (!contains(ecuatie, "(") && contains(ecuatie, ")"))) return errParanteze;

		// Verifica daca, existand paranteze patrate, acestea sunt si inchise corect
		if (contains(ecuatie, "[") && contains(ecuatie, "]")) {
			smatch b21, b22;

			regex_search(ecuatie, b21, regex("[\\[]"));
			regex_search(ecuatie, b22, regex("[\\]]"));

			if (b21.size() != b22.size()) return errParanteze;
		}
		if ((contains(ecuatie, "[") && !contains(ecuatie, "]"))
			|| (!contains(ecuatie, "[") && contains(ecuatie, "]"))) return errParanteze;

		// Ecuatia trebuie sa contina paranteze rotunde pentru a contine si patrate
		if (contains(ecuatie, "[") && !contains(ecuatie, "(") && !contains(ecuatie, ")")) return 7;

		// Din punctul asta stim ca parantezele sunt inchise corect,
		// deci putem verifica si doar pe unul din capete
		if (contains(ecuatie, "(")) {
			CalculatorMatch* parRotunde = new CalculatorMatch(ecuatie, '(');

			// Verificam daca ordinea parantezelor este corecta
			for (int i = 0; i < parRotunde->getNumarRezultate(); i++) {
				if (contains(parRotunde->getRezultate()[i], "[")) return 8;
				// Verifica daca exista un grup de paranteze rotunde care 
				// sa fie de forma ((n+m)), forma ilegala
				if (contains(unwrap(parRotunde->getRezultate()[i], 1), "(")) return 9;
			}
		}

		if (contains(ecuatie, "[")) {
			CalculatorMatch* parPatrate = new CalculatorMatch(ecuatie, '[');

			// Verifica daca exista un grup de paranteze patrate care 
			// sa fie de forma [n+m], forma ilegala
			for (int i = 0; i < parPatrate->getNumarRezultate(); i++) {
				if (!contains(parPatrate->getRezultate()[i], "(")) return 10;
			}
		}
	}

	return 0;
}

void Validator::ajusteaza() {
	*ecuatie = removeSpaces(*ecuatie);
}

Validator& Validator::operator=(const Validator& v) {
	if (this == &v) return *this;
	this->ecuatie = v.ecuatie;
	return *this;
}

bool Validator::operator!() {
	return valideaza() != 0;
}

bool Validator::operator<=(Validator& v) {
	short v1 = valideaza(), v2 = v.valideaza();
	return (v1 != 0 && v2 == 0) || (v1 == 0 && v2 == 0);
}

bool Validator::operator>=(Validator& v) {
	short v1 = valideaza(), v2 = v.valideaza();
	return (v1 == 0 && v2 != 0) || (v1 == 0 && v2 == 0);
}

istream& operator>>(istream& in, Validator& v) {
	return in;
}

ostream& operator<<(ostream& out, const Validator& v) {
	return out;
}

#include <iostream>
#include <string>
#include <regex>

#include "calculator.h"
#include "functii.h"

using namespace std;

void main() {
	Calculator c;

	printLine("#############################################", 0);
	printLine("#                                           #", 0);
	printLine("#    Aplicatie 'calculator'                 #", 0);
	printLine("#    Proiect POO Anul II                    #", 0);
	printLine("#                                           #", 0);
	printLine("#    Dezvoltator: Cocianu Liviu-Ionut       #", 0);
	printLine("#                 grupa 1049, seria C       #", 0);
	printLine("#                                           #", 0);
	printLine("#############################################", 1);

	printLine("(( Introdu 'exit' pentru a iesi din aplicatie ))", 1);

	do {
		try {
			cin >> c;
		}
		catch (CalculatorError cod) {
			switch (cod) {
			case CalculatorError::empty:
				printLine("Error: Linia introdusa a fost lasata goala", 1);
				break;
			case CalculatorError::no_arithmetics:
				printLine("Error: Nu s-a gasit nicio operatie", 1);
				break;
			case CalculatorError::negative_root:
				printLine("Error: Operatia introdusa contine un radical necalculabil", 1);
				break;
			case CalculatorError::forbidden_chars:
				printLine("Error: Caractere nepermise in ecuatie", 1);
				break;
			case CalculatorError::excessive_symbols:
				printLine("Error: Exces de simboluri", 1);
				break;
			case CalculatorError::illegal_division:
				printLine("Error: Impartire la 0", 1);
				break;
			case CalculatorError::improper_brackets:
				printLine("Error: Paranteze inchise necorespunzator", 1);
				break;
			case CalculatorError::lacks_brackets:
				printLine("Error: Ecuatia are paranteze patrate, dar nu si rotunde", 1);
				break;
			case CalculatorError::bracket_order:
				printLine("Error: Ordinea parantezelor este incorecta", 1);
				break;
			case CalculatorError::illegal_brackets:
				printLine("Error: Paranteze de forma ..((n+m)).. nu sunt permise", 0);
				printLine("Exemplu permis: (n+m)", 1);
				break;
			case CalculatorError::illegal_square_brackets:
				printLine("Error: Paranteze de forma ..[n+m].. nu sunt permise", 0);
				printLine("Exemplu permis: [(n+m)]", 1);
				break;
			case CalculatorError::operator_position:
				printLine("Error: Ecuatia nu poate incepe cu un operator inafara de minus", 1);
				break;
			case CalculatorError::unknown:
				printLine("Error: Linia nu a putut fi validata din motive necunoscute", 1);
				break;
			case CalculatorError::exit:
				printLine("Calculatorul a fost inchis!", 0);
				break;
			}
		}
	} while (c.getErrorFlag() > 0);
}