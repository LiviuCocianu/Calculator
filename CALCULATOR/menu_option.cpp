#pragma once

#include "menu_option.h"
#include "meniu.h";

using namespace std;

MenuOption::MenuOption() {
	numar = 0;
}

MenuOption::MenuOption(int numar) {
	this->numar = numar;
}

MenuOption::~MenuOption() {
	numar = 0;
}

MenuOption::MenuOption(const MenuOption& sem) {
	numar = sem.numar;
}

string MenuOption::getTitlu() {
	return "";
}

int MenuOption::getNumar() {
	return numar;
}

void MenuOption::setNumar(int numar) {
	this->numar = numar;
}

void MenuOption::executa(Calculator& c) {

}

MenuOption& MenuOption::operator=(const MenuOption& sem) {
	if (this == &sem) return *this;

	numar = sem.numar;

	return *this;
}

istream& operator>>(istream& in, MenuOption& mo) {
	return in;
}

ostream& operator<<(ostream& out, MenuOption& mo) {
	out << "Titlu: " << mo.getTitlu() << endl;
	out << "Index: " << mo.getNumar() << endl;
	return out;
}