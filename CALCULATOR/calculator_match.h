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