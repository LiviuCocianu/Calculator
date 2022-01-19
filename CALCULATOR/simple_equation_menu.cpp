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