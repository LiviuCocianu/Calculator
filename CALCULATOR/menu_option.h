#pragma once

#include "calculator.h"

class MenuOption {
	private:
		int numar = 0;
	public:
		MenuOption();
		MenuOption(int numar);
		~MenuOption();
		MenuOption(const MenuOption& sem);

		virtual string getTitlu();
		int getNumar();

		void setNumar(int numar);

		virtual void executa(Calculator& c);

		MenuOption& operator=(const MenuOption& sem);
};