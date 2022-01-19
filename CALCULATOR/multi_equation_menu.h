#pragma once

#include "menu_option.h"

using namespace std;

class MultiEquationMenu : public MenuOption {
	private:

	public:
		MultiEquationMenu();
		MultiEquationMenu(int numar);
		~MultiEquationMenu();
		MultiEquationMenu(const MultiEquationMenu& sem);

		string getTitlu() override;
		void executa(Calculator& c) override;

		MultiEquationMenu& operator=(const MultiEquationMenu& sem);
};