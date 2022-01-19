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