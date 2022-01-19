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