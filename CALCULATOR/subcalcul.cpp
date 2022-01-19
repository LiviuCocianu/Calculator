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