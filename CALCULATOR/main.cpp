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