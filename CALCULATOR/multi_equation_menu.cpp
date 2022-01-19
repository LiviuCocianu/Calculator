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