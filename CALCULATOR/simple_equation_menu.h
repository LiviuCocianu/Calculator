#pragma once

#include "menu_option.h"

using namespace std;

class SimpleEquationMenu : public MenuOption {
	private:

	public:
		SimpleEquationMenu();
		SimpleEquationMenu(int numar);
		~SimpleEquationMenu();
		SimpleEquationMenu(const SimpleEquationMenu& sem);

		string getTitlu() override;
		void executa(Calculator& c) override;
		
		SimpleEquationMenu& operator=(const SimpleEquationMenu& sem);
};