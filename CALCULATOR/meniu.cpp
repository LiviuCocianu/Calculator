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