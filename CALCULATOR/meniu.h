#pragma once

#include <map>
#include "calculator.h"
#include "menu_option.h"

class Meniu {
	private:
		int nrOptiuni;
		Calculator& calculator;
		map<int, MenuOption*> optiuni;
		int optiuneSelectata;

		int citesteOptiune();
	public:
		Meniu();
		Meniu(Calculator& c);
		Meniu(const Meniu& m);
		~Meniu();

		int getNumarOptiuni();
		Calculator& getCalculator();
		map<int, MenuOption*> getOptiuni();
		int getOptiuneSelectata();

		void setCalculator(Calculator& c);
		void setOptiuni(map<int, MenuOption*> op);
		void setOptiuneSelectata(int s);

		void afiseaza();
		void adaugaOptiune(MenuOption* op);
		int incrementOptiuni();
		int decrementOptiuni();

		Meniu& operator=(const Meniu& m);
		friend ostream& operator<<(ostream& out, const Meniu& m);
};