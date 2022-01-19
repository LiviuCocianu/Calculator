#pragma once
#include <iostream>
#include <string>
#include "calculator.h"

using namespace std;

class Validator {
	private:
		string* ecuatie;
	public:
		Validator();
		Validator(string* ecuatie);
		~Validator();
		Validator(const Validator& v);

		string getEcuatie();
		string* getAdresaEcuatie();
		void setEcuatie(string ecuatie); //
		void setAdresaEcuatie(string* ecuatie);

		/*
			Valideaza ecuatia primita si returneaza o valoare mai
			mare decat 0 daca exista erori de validare

			1 - Ecuatie fara operatii
			2 - Ecuatie cu operatori scrisi incorect
			3 - Ecuatie cu caractere nepermise
			4 - Ecuatie cu numere de forma 1.234.5; 4.527.3.623, etc
			5 - Ecuatia contine impartiri la 0
			6 - Ecuatia incepe cu un operator altul decat minus
			7 - Ecuatia are paranteze scrise incorect
			8 - Ecuatia are paranteze patrate, dar nu si rotunde
			9 - Ecuatia are paranteze ordonate incorect
			10 - Ecuatia are paranteze de forma ((n+m))
			11 - Ecuatia are paranteze de forma [n+m]
		*/
		short valideaza();
		// Face ajustari in spatierea ecuatiei
		void ajusteaza();

		Validator& operator=(const Validator& v);
		// Verifica daca validarea este pozitiva sau nu
		bool operator!();
		bool operator<=(Validator& v);
		bool operator>=(Validator& v);
		friend istream& operator>>(istream& in, Validator& v);
		friend ostream& operator<<(ostream& out, const Validator& v);
};