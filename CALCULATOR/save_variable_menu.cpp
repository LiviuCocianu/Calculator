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