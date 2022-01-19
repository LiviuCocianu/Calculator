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
