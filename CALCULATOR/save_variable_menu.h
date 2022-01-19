#pragma once

#include "menu_option.h"

using namespace std;

class SaveVariableMenu : public MenuOption {
	private:

	public:
		SaveVariableMenu();
		SaveVariableMenu(int numar);
		~SaveVariableMenu();
		SaveVariableMenu(const SaveVariableMenu& sem);

		string getTitlu() override;
		void executa(Calculator& c) override;

		SaveVariableMenu& operator=(const SaveVariableMenu& sem);
};