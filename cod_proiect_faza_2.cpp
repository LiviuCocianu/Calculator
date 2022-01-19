#pragma once
#include <iostream>
#include <map>
#include "validator.h"
using namespace std;

enum class CalculatorError {
	empty, exit, unknown,
	no_arithmetics, forbidden_chars, excessive_symbols,
	division_by_zero, improper_brackets, lacks_brackets,
	bracket_order, illegal_brackets, illegal_square_brackets,
	operator_position, negative_root, unknown_variable,
	invalid_cmd_digit, out_of_range, invalid_variable_name,
	file_error, empty_file, improper_floats
};

struct Variabila {
	char nume;
	double valoare;
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

		void executa();
		bool proceseaza();
		double calculeaza();
		/*
		Salveaza rezultatul calculului intr-o variabila localizata 
		intr-un fisier binar. Daca o variabila cu acest nume
		exista deja, valoarea va fi inlocuita cu cea noua.

		Returneaza statusul executiei
		*/
		bool salveazaVariabila(char nume);
		/*
		Returneaza valoarea variabilei cu numele specificat
		din fisierul binar "vars". Daca nu exista, un obiect
		cu simbolul '!' si valoarea -1 va fi returnat
		*/
		static Variabila getVariabila(char nume);
		static map<char, double> getVariabile();
		/*
		Preia un fisier text si rezolva fiecare ecuatie
		din el, rand pe rand, si afiseaza rezultatele
		*/
		map<string, double> calculeazaEcuatii(string fisierText);


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
#include <fstream>

#include "calculator.h"
#include "calculator_match.h"
#include "subcalcul.h"
#include "functii.h"

#include "meniu.h"
#include "simple_equation_menu.h"
#include "save_variable_menu.h"
#include "multi_equation_menu.h"

using namespace std;

const char Calculator::operatori[6] = { '#', '^', '/', '*', '-', '+' };

Calculator::Calculator() {
	ecuatie = "NULL";
	validator = *(new Validator(&ecuatie));
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
	validator.setEcuatie(ecuatie);
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

void Calculator::executa() {
	if (equalsIgnoreCase(ecuatie, "exit")) {
		setErrorFlag(0);
		throw CalculatorError::exit;
	}

	if (regex_match(ecuatie, regex("^\\s+$")) || ecuatie.length() == 0) {
		setErrorFlag(1);
		setEcuatie("NULL");
		throw CalculatorError::empty;
	}

	bool succes = proceseaza();
	if (succes) {
		double rezultat = calculeaza();
		setRezultat(rezultat);
		cout << fixed << setprecision(4) << endl << "> Rezultat: " << rezultat << endl << endl;
		setEcuatie("NULL");

		// Seteaza flag-ul cu o valoare peste 0 pentru a permite buclei while din main
		// sa continue sa primeasca ecuatii dupa acest calcul
		setErrorFlag(50);
	}
}

bool Calculator::proceseaza() {
	short exitStatus = getValidator().valideaza();

	if (exitStatus != 0) {
		setErrorFlag(2);
		setEcuatie("NULL");

		if (exitStatus == 1) throw CalculatorError::no_arithmetics;
		else if (exitStatus == 2) throw CalculatorError::excessive_symbols;
		else if (exitStatus == 3) throw CalculatorError::forbidden_chars;
		else if (exitStatus == 4) throw CalculatorError::improper_floats;
		else if (exitStatus == 5) throw CalculatorError::division_by_zero;
		else if (exitStatus == 6) throw CalculatorError::operator_position;
		else if (exitStatus == 7) throw CalculatorError::improper_brackets;
		else if (exitStatus == 8) throw CalculatorError::lacks_brackets;
		else if (exitStatus == 9) throw CalculatorError::bracket_order;
		else if (exitStatus == 10) throw CalculatorError::illegal_brackets;
		else if (exitStatus == 11) throw CalculatorError::illegal_square_brackets;
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
			parPatrate->substituie(rez, to_string(grup->calculeaza()), 0);
		}
	}

	if (contains(rez, "(")) {
		while (contains(rez, "(")) {
			CalculatorMatch* parRotunde = new CalculatorMatch(rez, '(');
			Subcalcul* grup = new Subcalcul(parRotunde->getRezultate()[0]);
			parRotunde->substituie(rez, to_string(grup->calculeaza()), 0);
		}
	}

	return (new Subcalcul(rez))->calculeaza();
}

bool Calculator::salveazaVariabila(char nume) {
	fstream vars;
	bool p = false;
	vars.open("vars.bin", ios::out | ios::app | ios::binary);
	vars.close();
	vars.open("vars.bin", ios::in | ios::out | ios::binary);

	if (vars.is_open()) {
		Variabila var, cpy;
		var.nume = nume;
		var.valoare = rezultat;

		int size = ifileSize(vars);

		if (size > 0) {
			while (!vars.eof()) {
				vars.read((char*)&cpy, sizeof(Variabila));

				// Suprascrie variabila existenta cu cea noua
				if (cpy.nume == var.nume) {
					long pos = vars.tellp();
					vars.seekp(pos - sizeof(Variabila));

					vars.write((char*)&var, sizeof(Variabila));
					p = true;
					break;
				}
			}
		}

		/*
		Daca nu s-a gasit o variabila duplicata,
		scrie variabila noua la sfarsit
		*/
		if (!p) {
			vars.clear();
			vars.seekp(0, ios_base::end);

			vars.write((char*)&var, sizeof(Variabila));
		}

		vars.close();
	} else {
		setErrorFlag(2);
		throw CalculatorError::file_error;
	}

	return true;
}

Variabila Calculator::getVariabila(char nume) {
	fstream vars;
	Variabila var;
	bool p = false;

	vars.open("vars.bin", ios::in | ios::binary);

	if (vars.is_open()) {
		if (ifileSize(vars) > 0) {
			while (!vars.eof()) {
				vars.read((char*) &var, sizeof(Variabila));

				if (nume == var.nume) {
					p = true;
					break;
				}
			}
		}

		vars.close();
	} else {
		throw CalculatorError::file_error;
	}

	if (!p) {
		var.nume = '!';
		var.valoare = -1;
	}

	return var;
}

map<char, double> Calculator::getVariabile() {
	fstream vars;
	Variabila var;
	map<char, double> output;

	vars.open("vars.bin", ios::in | ios::binary);

	if (vars.is_open()) {
		if (ifileSize(vars) > 0) {
			while (!vars.eof()) {
				vars.read((char*)&var, sizeof(Variabila));
				output.insert(pair<char, double>(var.nume, var.valoare));
			}
		}

		vars.close();
	} else {
		throw CalculatorError::file_error;
	}

	return output;
}

map<string, double> Calculator::calculeazaEcuatii(string fisierText) {
	map<string, double> output;
	ifstream f;
	f.open(fisierText, ios::in);

	if (f.is_open()) {
		string ecuatie;

		if (ifileSize(f) > 0) {
			cout << endl;
			printLine("  Rezultate:", 1);

			while (!f.eof()) {
				getline(f, ecuatie);
				setEcuatie(ecuatie);

				bool succes = proceseaza();
				if (succes) {
					double rezultat = calculeaza();

					output.insert(pair<string, double>(ecuatie, rezultat));

					cout << fixed << setprecision(4) << "  > " << ecuatie << " = " << rezultat << endl;
					setEcuatie("NULL");
				}
			}
		} else {
			f.close();
			setErrorFlag(2);
			throw CalculatorError::empty_file;
		}

		f.close();
	} else {
		setErrorFlag(2);
		throw CalculatorError::file_error;
	}

	return output;
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

	if (equalsIgnoreCase(c.getEcuatie(), "NULL")) {
		Meniu menu(c);

		menu.adaugaOptiune(new SimpleEquationMenu(1));
		menu.adaugaOptiune(new SaveVariableMenu(2));
		menu.adaugaOptiune(new MultiEquationMenu(3));

		menu.afiseaza();
	} else {
		c.executa();
		c.setErrorFlag(0);
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
		void substituie(string& dest, string rez, int indexRez);
		// Returneaza potrivirile, numarul lor si pozitiile in string pentru regex-ul dat
		static void getPotriviri(string s, regex r, string* matches, int* pozitii, int& nrRez);
		static int getNumarRezultate(string s, regex r);

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

#pragma once

#include <map>
#include "calculator.h"
#include "menu_option.h"

class Meniu {
	private:
		int nrOptiuni;
		Calculator& calculator;
		map<int, MenuOption*> optiuni;
		int optiuneSelectata;

		int citesteOptiune();
	public:
		Meniu();
		Meniu(Calculator& c);
		Meniu(const Meniu& m);
		~Meniu();

		int getNumarOptiuni();
		Calculator& getCalculator();
		map<int, MenuOption*> getOptiuni();
		int getOptiuneSelectata();

		void setCalculator(Calculator& c);
		void setOptiuni(map<int, MenuOption*> op);
		void setOptiuneSelectata(int s);

		void afiseaza();
		void adaugaOptiune(MenuOption* op);
		int incrementOptiuni();
		int decrementOptiuni();

		Meniu& operator=(const Meniu& m);
		friend ostream& operator<<(ostream& out, const Meniu& m);
};

#include "meniu.h"
#include "functii.h"

#include "simple_equation_menu.h"
#include "save_variable_menu.h"
#include "multi_equation_menu.h"

using namespace std;

// Nu apela constructorul implicit
Meniu::Meniu() : calculator(*(new Calculator())) {
	nrOptiuni = 0;
	optiuneSelectata = 0;
}

Meniu::Meniu(Calculator& c) : calculator(c) {
	nrOptiuni = 0;
	optiuneSelectata = 0;
}

Meniu::Meniu(const Meniu& m) : calculator(m.calculator) {
	nrOptiuni = m.nrOptiuni;
	optiuni = m.optiuni;
	optiuneSelectata = m.optiuneSelectata;
}

Meniu::~Meniu() {
	nrOptiuni = 0;
	optiuneSelectata = 0;
}

int Meniu::getNumarOptiuni() {
	return nrOptiuni;
}

Calculator& Meniu::getCalculator() {
	return calculator;
}

map<int, MenuOption*> Meniu::getOptiuni() {
	map<int, MenuOption*> cpy(optiuni);
	return cpy;
}

int Meniu::getOptiuneSelectata() {
	return optiuneSelectata;
}

void Meniu::setCalculator(Calculator& c) {
	calculator = c;
}

void Meniu::setOptiuni(map<int, MenuOption*> op) {
	if (op.empty()) {
		optiuni.clear();
		return;
	}

	optiuni = op;
}

void Meniu::setOptiuneSelectata(int s) {
	optiuneSelectata = s;
}

void Meniu::afiseaza() {
	printLine("", 10);
	printLine("  Ce doresti sa accesezi?", 0);
	printLine("  (( Introdu cifra corespunzatoare comenzii ))", 0);
	printLine("  (( Introdu 'exit' pentru a iesi din aplicatie ))", 1);

	map<int, MenuOption*>::iterator it;
	for (it = optiuni.begin(); it != optiuni.end(); it++) {
		cout << "  " << it->first << " | " << it->second->getTitlu() << endl;
	}

	cout << endl;

	int optiune = citesteOptiune();
	MenuOption* opt = optiuni[optiune];
	setOptiuneSelectata(optiune);

	opt->executa(calculator);
}

int Meniu::citesteOptiune() {
	string cifraS;
	int cifra;
	bool p = false;

	do {
		bool p1 = false;

		while (!p1) {
			try {
				getline(cin, cifraS);
				cout << endl;
				cifra = stoi(cifraS);
				p1 = true;
			}
			catch (invalid_argument ia) {
				if (equalsIgnoreCase(cifraS, "exit")) {
					getCalculator().setErrorFlag(0);
					throw CalculatorError::exit;
				}

				getCalculator().setErrorFlag(2);
				throw CalculatorError::invalid_cmd_digit;
			}
			catch (out_of_range oor) {
				getCalculator().setErrorFlag(2);
				throw CalculatorError::out_of_range;
			}
		}

		if (cifra > getNumarOptiuni() || cifra < 1) {
			cout << "[ERROR] Numarul nu se afla in intervalul [1, " << getNumarOptiuni() << "]" << endl << endl;
			continue;
		}

		p = true;
	} while (!p);

	return cifra;
}

void Meniu::adaugaOptiune(MenuOption* op) {
	optiuni.insert(pair<int, MenuOption*>(incrementOptiuni(), op));
}

int Meniu::incrementOptiuni() {
	nrOptiuni += 1;
	return nrOptiuni;
}

int Meniu::decrementOptiuni() {
	nrOptiuni -= 1;
	return nrOptiuni;
}

Meniu& Meniu::operator=(const Meniu& m) {
	if (this == &m) return *this;

	nrOptiuni = m.nrOptiuni;
	calculator = m.calculator;
	optiuni = m.optiuni;
	optiuneSelectata = m.optiuneSelectata;

	return *this;
}

istream& operator>>(istream& in, Meniu& m) {
	return in;
}

ostream& operator<<(ostream& out, const Meniu& m) {
	out << "Numar optiuni: " << m.nrOptiuni << endl;
	out << "Optiune selectata: " << m.optiuneSelectata << endl;
	return out;
}

#pragma once

#include "calculator.h"

class MenuOption {
	private:
		int numar = 0;
	public:
		MenuOption();
		MenuOption(int numar);
		~MenuOption();
		MenuOption(const MenuOption& sem);

		virtual string getTitlu();
		int getNumar();

		void setNumar(int numar);

		virtual void executa(Calculator& c);

		MenuOption& operator=(const MenuOption& sem);
};

#pragma once

#include "menu_option.h"
#include "meniu.h";

using namespace std;

MenuOption::MenuOption() {
	numar = 0;
}

MenuOption::MenuOption(int numar) {
	this->numar = numar;
}

MenuOption::~MenuOption() {
	numar = 0;
}

MenuOption::MenuOption(const MenuOption& sem) {
	numar = sem.numar;
}

string MenuOption::getTitlu() {
	return "";
}

int MenuOption::getNumar() {
	return numar;
}

void MenuOption::setNumar(int numar) {
	this->numar = numar;
}

void MenuOption::executa(Calculator& c) {

}

MenuOption& MenuOption::operator=(const MenuOption& sem) {
	if (this == &sem) return *this;

	numar = sem.numar;

	return *this;
}

istream& operator>>(istream& in, MenuOption& mo) {
	return in;
}

ostream& operator<<(ostream& out, MenuOption& mo) {
	out << "Titlu: " << mo.getTitlu() << endl;
	out << "Index: " << mo.getNumar() << endl;
	return out;
}

#pragma once

#include "menu_option.h"

using namespace std;

class SimpleEquationMenu : public MenuOption {
	private:

	public:
		SimpleEquationMenu();
		SimpleEquationMenu(int numar);
		~SimpleEquationMenu();
		SimpleEquationMenu(const SimpleEquationMenu& sem);

		string getTitlu() override;
		void executa(Calculator& c) override;
		
		SimpleEquationMenu& operator=(const SimpleEquationMenu& sem);
};

#include "simple_equation_menu.h"
#include "meniu.h";

using namespace std;

SimpleEquationMenu::SimpleEquationMenu() {
	setNumar(0);
}

SimpleEquationMenu::SimpleEquationMenu(int numar) {
	setNumar(numar);
}

SimpleEquationMenu::~SimpleEquationMenu() {

}

SimpleEquationMenu::SimpleEquationMenu(const SimpleEquationMenu& sem) {

}

string SimpleEquationMenu::getTitlu() {
	return "Calculeaza o ecuatie de la tastatura";
}

void SimpleEquationMenu::executa(Calculator& c) {
	string ecuatie;

	cout << "Introdu ecuatia: ";
	getline(cin, ecuatie);

	c.setEcuatie(ecuatie);
	c.executa();
}

SimpleEquationMenu& SimpleEquationMenu::operator=(const SimpleEquationMenu& sem) {
	return *this;
}

#pragma once

#include "menu_option.h"

using namespace std;

class SaveVariableMenu : public MenuOption {
	private:

	public:
		SaveVariableMenu();
		SaveVariableMenu(int numar);
		~SaveVariableMenu();
		SaveVariableMenu(const SaveVariableMenu& sem);

		string getTitlu() override;
		void executa(Calculator& c) override;

		SaveVariableMenu& operator=(const SaveVariableMenu& sem);
};

#include "save_variable_menu.h"
#include "meniu.h";
#include "functii.h"

using namespace std;

SaveVariableMenu::SaveVariableMenu() {
	setNumar(0);
}

SaveVariableMenu::SaveVariableMenu(int numar) {
	setNumar(numar);
}

SaveVariableMenu::~SaveVariableMenu() {

}

SaveVariableMenu::SaveVariableMenu(const SaveVariableMenu& sem) {

}

string SaveVariableMenu::getTitlu() {
	return "Calculeaza o ecuatie si salveaza rezultatul intr-o variabila";
}

void SaveVariableMenu::executa(Calculator& c) {
	string numeVar;

	printLine(" Conventii: ", 0);
	printLine(" - Numele variabilei trebuie sa fie o litera!", 0);
	printLine(" - Variabilele sunt case-sensitive!", 1);

	bool p3 = false;

	do {
		cout << "Introdu numele variabilei: ";
		getline(cin, numeVar);

		if (!regex_search(numeVar, regex("^[a-zA-Z]{1}$"))) {
			c.setErrorFlag(2);
			throw CalculatorError::invalid_variable_name;
		}

		p3 = true;
	} while (!p3);

	string ecuatie;

	cout << "Introdu ecuatia: ";
	getline(cin, ecuatie);

	c.setEcuatie(ecuatie);
	c.executa();

	/*
	Preia rezultatul calculului anterior si
	transpune-l intr-o variabila
	*/
	bool succes = c.salveazaVariabila(numeVar.c_str()[0]);

	if (succes)
		cout << "Variabila cu numele '" << numeVar << "' a fost salvata!" << endl << endl;
	else
		cout << "Variabila nu a putut fi salvata.." << endl << endl;
}

SaveVariableMenu& SaveVariableMenu::operator=(const SaveVariableMenu& sem) {
	return *this;
}

#pragma once

#include "menu_option.h"

using namespace std;

class MultiEquationMenu : public MenuOption {
	private:

	public:
		MultiEquationMenu();
		MultiEquationMenu(int numar);
		~MultiEquationMenu();
		MultiEquationMenu(const MultiEquationMenu& sem);

		string getTitlu() override;
		void executa(Calculator& c) override;

		MultiEquationMenu& operator=(const MultiEquationMenu& sem);
};

#include <fstream>
#include <iomanip>

#include "multi_equation_menu.h"
#include "meniu.h";
#include "functii.h"

using namespace std;

MultiEquationMenu::MultiEquationMenu() {
	setNumar(0);
}

MultiEquationMenu::MultiEquationMenu(int numar) {
	setNumar(numar);
}

MultiEquationMenu::~MultiEquationMenu() {

}

MultiEquationMenu::MultiEquationMenu(const MultiEquationMenu& sem) {

}

string MultiEquationMenu::getTitlu() {
	return "Calculeaza mai multe ecuatii dintr-un fisier dat";
}

void MultiEquationMenu::executa(Calculator& c) {
	string cale;

	bool p4 = false;

	do {
		cout << "Introdu calea catre fisierul text: ";
		getline(cin, cale);

		if (!regex_search(cale, regex(".*\\.txt$"))) {
			printLine("[ERROR] Calea trebuie sa duca la un fisier .txt", 1);
			continue;
		}

		p4 = true;
	} while (!p4);

	map<string, double> rezultate = c.calculeazaEcuatii(cale);
	string rasp;

	p4 = false;

	do {
		cout << endl;
		printLine("  Doresti sa salvezi rezultatele? Introdu Y sau N", 1);

		getline(cin, rasp);
		cout << endl;

		if (!regex_search(rasp, regex("(Y|N)"))) {
			printLine("[ERROR] Raspuns invalid, incearca Y sau N", 1);
			continue;
		}

		p4 = true;
	} while (!p4);

	if (rasp == "Y") {
		p4 = false;
		cale = "";

		do {
			cout << "Introdu numele fisierului text: ";
			getline(cin, cale);

			if (!regex_search(cale, regex(".*\\.txt$"))) {
				printLine("[ERROR] Fisierul trebuie sa se termine in .txt", 1);
				continue;
			}

			p4 = true;
		} while (!p4);

		ofstream frez;
		frez.open(cale, ios::out | ios::trunc);

		if (frez.is_open()) {
			map<string, double>::iterator it;

			for (it = rezultate.begin(); it != rezultate.end(); it++) {
				frez << fixed << setprecision(4) << it->first << " = " << it->second << "\n";
			}

			frez.close();

			printLine("Rezultatele au fost salvate la calea specificata!", 0);
		}
		else {
			c.setErrorFlag(2);
			throw CalculatorError::file_error;
		}
	}

	c.setErrorFlag(2);
}

MultiEquationMenu& MultiEquationMenu::operator=(const MultiEquationMenu& sem) {
	return *this;
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
	fragment = new char[1] {'\0'};
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

	if (regex_search(ecuatieCpy, regex("\\+-"))) {
		ecuatieCpy = regex_replace(ecuatieCpy, regex("\\+-"), "-");
	} else if (regex_search(ecuatieCpy, regex("-\\+"))) {
		ecuatieCpy = regex_replace(ecuatieCpy, regex("-\\+"), "+");
	} else if (regex_search(ecuatieCpy, regex("--"))) {
		ecuatieCpy = regex_replace(ecuatieCpy, regex("--"), "+");
	}

	while (regex_search(ecuatieCpy, regex(CalculatorMatch::getRegexOperatie(oper)))) {
		CalculatorMatch* operatii = new CalculatorMatch(ecuatieCpy, oper);
		int n = 0;
		string* sp = split(operatii->getRezultate()[0], oper, n);

		double n1, n2;

		try {
			n1 = stod(sp[0]);
			n2 = stod(sp[1]);
		} catch (invalid_argument ia) {
			for (int i = 0; i < 2; i++) {
				if (regex_search(sp[i], regex("^[a-zA-Z]{1}$"))) {
					Variabila v = Calculator::getVariabila(sp[i].c_str()[0]);

					if (v.nume == '!') {
						throw CalculatorError::unknown_variable;
						break;
					} else {
						cout << "-- " << v.nume << " = " << v.valoare << endl;

						if (i == 0) n1 = v.valoare;
						else if (i == 1) n2 = v.valoare;
					}
				} else {
					if (i == 0) n1 = stod(sp[0]);
					else if (i == 1) n2 = stod(sp[1]);
				}
			}
		}

		double rez = 0;

		switch (oper) {
			case '^': rez = powf(n1, n2); break;
			case '#':
				if (n1 < 0 && (((int) n2) % 2 == 0)) {
					throw CalculatorError::negative_root;
					break;
				}

				if (n1 < 0) {
					rez = nthRoot(n1, n2); 
				} else {
					rez = powf(n1, 1 / n2);
				}
				
				break;
			case '/':
				if (n1 == 0 || n2 == 0) {
					throw CalculatorError::division_by_zero;
					break;
				}

				rez = n1 / n2;
				break;
			case '*': rez = n1 * n2; break;
			case '+': rez = n1 + n2; break;
			case '-': rez = n1 - n2; break;
		}

		if (rez >= 0 && n1 < 0) {
			string cpy("+");
			cpy += to_string(rez);
			operatii->substituie(ecuatieCpy, cpy, 0);
		} else {
			operatii->substituie(ecuatieCpy, to_string(rez), 0);
		}
	}

	return ecuatieCpy;
}

double Subcalcul::calculeaza() {
	string fragment = getFragment();

	if (startsWith(fragment, "[")) {
		// Desfa paranteza
		fragment = unwrap(fragment, 1);

		while (contains(fragment, "(")) {
			CalculatorMatch* parRotunde = new CalculatorMatch(fragment, '(');
			Subcalcul* grup = new Subcalcul(parRotunde->getRezultate()[0]);
			parRotunde->substituie(fragment, to_string(grup->calculeaza()), 0);
		}

		return (new Subcalcul(fragment))->calculeaza();
	} else {
		if (startsWith(fragment, "(")) 
			fragment = unwrap(fragment, 1);

		// Evalueaza ecuatia data pentru toti operatorii
		for (int i = 0; i < 6; i++) {
			// # ^ / * - +
			char oper = Calculator::getOperatori()[i];

			if (contains(fragment, oper)) {
				fragment = Subcalcul::evalueaza(fragment, oper);
			}
		}
	}

	if (startsWith(fragment, "-0")) fragment = "0";
	
	return (double) stod(fragment);
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
			4 - Ecuatie cu numere de forma 1.234.5; 4.527.3.623, etc
			5 - Ecuatia contine impartiri la 0
			6 - Ecuatia incepe cu un operator altul decat minus
			7 - Ecuatia are paranteze scrise incorect
			8 - Ecuatia are paranteze patrate, dar nu si rotunde
			9 - Ecuatia are paranteze ordonate incorect
			10 - Ecuatia are paranteze de forma ((n+m))
			11 - Ecuatia are paranteze de forma [n+m]
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

short Validator::valideaza() {
	// Elimina toate spatiile din ecuatie
	ajusteaza();
	string ecuatie = getEcuatie();

	// Verifica daca exista semne ce se repeta unele dupa altele 
	regex c1 = regex("((\\+)|(\\-)|(\\*)|(\\/)|(\\^)|(\\#)|(\\.)){2,}");
	// Verifica daca alte caractere decat cele permise se gasesc in ecuatie
	regex c2 = regex("[^\\+\\-\\*\\/\\^\\#\\.\\(\\)\\[\\]\\da-zA-Z]");
	// Verifica numere de forma 1.234.5; 4.527.3.623, etc
	regex c3 = regex("\\d+(\\.\\d+){2,}");

	// Verifica daca ecuatia are cel putin o operatie. Ex: 2+3; 6/2; 5.2+0.8, a+2, 52+C
	bool p = false;
	for (int i = 0; i < 6; i++) {
		if (regex_search(ecuatie, regex(CalculatorMatch::getRegexOperatie(Calculator::getOperatori()[i])))) {
			p = true;
			break;
		}
	}

	if (!p) return 1;

	if (regex_search(ecuatie, c1)) return 2;
	if (regex_search(ecuatie, c2)) return 3;
	if (regex_search(ecuatie, c3)) return 4;
	// Verificare superficiala pentru impartiri la 0
	// Nota: O verificare mai serioasa se va face la calcul, nu la validare
	if (regex_search(ecuatie, regex("\\/0"))) return 5;

	// Verifica daca ecuatia incepe cu un operator nepermis
	for (int i = 0; i < 6; i++) {
		char oper = Calculator::getOperatori()[i];
		if (oper == '-') continue;

		if (contains(ecuatie, oper)) {
			string expr = "^\\";
			expr += oper;

			if (regex_search(ecuatie, regex(expr))) return 6;
		}
	}

	const int errParanteze = 7;
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
		if (contains(ecuatie, "[") && !contains(ecuatie, "(") && !contains(ecuatie, ")")) return 8;

		// Din punctul asta stim ca parantezele sunt inchise corect,
		// deci putem verifica si doar pe unul din capete
		if (contains(ecuatie, "(")) {
			CalculatorMatch* parRotunde = new CalculatorMatch(ecuatie, '(');

			// Verificam daca ordinea parantezelor este corecta
			for (int i = 0; i < parRotunde->getNumarRezultate(); i++) {
				if (contains(parRotunde->getRezultate()[i], "[")) return 9;
				// Verifica daca exista un grup de paranteze rotunde care 
				// sa fie de forma ((n+m)), forma ilegala
				if (contains(unwrap(parRotunde->getRezultate()[i], 1), "(")) return 10;
			}
		}

		if (contains(ecuatie, "[")) {
			CalculatorMatch* parPatrate = new CalculatorMatch(ecuatie, '[');

			// Verifica daca exista un grup de paranteze patrate care 
			// sa fie de forma [n+m], forma ilegala
			for (int i = 0; i < parPatrate->getNumarRezultate(); i++) {
				if (!contains(parPatrate->getRezultate()[i], "(")) return 11;
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
/*
	Gaseste potriviri pentru regex "r" si inlocuieste
	a n-a potrivire in string "src" cu string "with"
*/
void replaceAt(string& src, string with, regex r, int n);
// Inlocuieste toate potrivirile cu string-ul dat
void replaceAll(string& src, string with, regex r);
int ifileSize(istream& f);
int ofileSize(ostream& f);

double nthRoot(float num, float gr);
double mod(double n);

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

#include <iostream>
#include <string>
#include <regex>

#include "calculator.h"
#include "functii.h"

using namespace std;

void main(int argc, char **argv) {
	Calculator c;
	bool first = true;

	printLine("#############################################", 0);
	printLine("#                                           #", 0);
	printLine("#    Aplicatie 'calculator'                 #", 0);
	printLine("#    Proiect POO Anul II                    #", 0);
	printLine("#                                           #", 0);
	printLine("#    Dezvoltator: Cocianu Liviu-Ionut       #", 0);
	printLine("#                 grupa 1049, seria C       #", 0);
	printLine("#                                           #", 0);
	printLine("#############################################", 1);

	do {
		try {
			/*
			Citirea prin cin va fi ignorata daca utilizatorul a
			introdus o ecuatie prin linia de comanda, si va
			trece automat la calculare si afisarea rezultatului
			*/
			if (argc > 1 && first) {
				printLine("! Pentru ridicari la puteri, includeti ecuatia intre ghilimele", 0);
				printLine("  deoarece linia de comanda nu poate citi caracterul '^' !", 1);

				printLine("#############################################", 1);

				/*
				Abreviaza ecuatia in cazul in care este
				imprastiata intre mai multi parametri
				*/
				string ec;
				for (int i = 1; i < argc; i++) ec += argv[i];

				c.setEcuatie(ec);
				first = false;
			}
			
			cin >> c;
		} catch (CalculatorError cod) {
			// Toate mesajele date de erori sunt gestionate aici

			if(cod != CalculatorError::exit)
				cout << "[ERROR] ";

			switch (cod) {
				case CalculatorError::empty:
					printLine("Linia introdusa a fost lasata goala", 1);
					break;
				case CalculatorError::no_arithmetics:
					printLine("Nu s-a gasit nicio operatie", 1);
					break;
				case CalculatorError::negative_root:
					printLine("Operatia introdusa contine un radical necalculabil", 1);
					break;
				case CalculatorError::forbidden_chars:
					printLine("Caractere nepermise in ecuatie", 1);
					break;
				case CalculatorError::excessive_symbols:
					printLine("Exces de simboluri", 1);
					break;
				case CalculatorError::improper_floats:
					printLine("Ecuatia contine un numar ce nu respecta forma unui numar rational", 1);
					break;
				case CalculatorError::division_by_zero:
					printLine("Impartire la 0", 1);
					break;
				case CalculatorError::improper_brackets:
					printLine("Paranteze inchise necorespunzator", 1);
					break;
				case CalculatorError::lacks_brackets:
					printLine("Ecuatia are paranteze patrate, dar nu si rotunde", 1);
					break;
				case CalculatorError::bracket_order:
					printLine("Ordinea parantezelor este incorecta", 1);
					break;
				case CalculatorError::illegal_brackets:
					printLine("Paranteze de forma ..((n+m)).. nu sunt permise", 0);
					printLine("Exemplu permis: (n+m)", 1);
					break;
				case CalculatorError::illegal_square_brackets:
					printLine("Paranteze de forma ..[n+m].. nu sunt permise", 0);
					printLine("Exemplu permis: [(n+m)]", 1);
					break;
				case CalculatorError::operator_position:
					printLine("Ecuatia nu poate incepe cu un operator inafara de minus", 1);
					break;
				case CalculatorError::unknown_variable:
					printLine("Ecuatia contine o variabila nedefinita", 1);
					break;
				case CalculatorError::invalid_cmd_digit:
					printLine("Linia introdusa nu este o cifra valida", 1);
					break;
				case CalculatorError::out_of_range:
					printLine("Numarul introdus este prea mare", 1);
					break;
				case CalculatorError::invalid_variable_name:
					printLine("Numele variabilei nu respecta conventiile", 1);
					break;
				case CalculatorError::file_error:
					printLine("Fisierul nu a putut fi deschis", 1);
					break;
				case CalculatorError::empty_file:
					printLine("Fisierul nu are continut", 1);
					break;
				case CalculatorError::unknown:
					printLine("Linia nu a putut fi validata din motive necunoscute", 1);
					break;
				case CalculatorError::exit:
					printLine("Calculatorul a fost inchis!", 0);
					break;
			}

			c.setEcuatie("NULL");
		}
	} while (c.getErrorFlag() > 0 && first);
}