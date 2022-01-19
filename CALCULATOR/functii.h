#pragma once

#include <string>
#include <regex>
#include <list>

using namespace std;

// Afiseaza o linie cu inca n linii goale posterior
void printLine(const char* linie, int n);
// Verifica daca un string contine secventa data
bool contains(string s, string match);
// Verifica daca un string contine caracterul dat
bool contains(string s, char match);
// Verifica daca doua string-uri sunt egale, indiferent de majuscule
bool equalsIgnoreCase(string s1, string s2);
// Elimina toate spatiile dintr-un string
string removeSpaces(string s);
// Verifica daca un string incepe cu secventa data
bool startsWith(string s, string seq);
// Elimina n caractere de la inceputul si sfarsitul unui string
string unwrap(string s, int n);
string* split(string s, char delim, int& n);
/*
	Gaseste potriviri pentru regex "r" si inlocuieste
	a n-a potrivire in string "src" cu string "with"
*/
void replaceAt(string& src, string with, regex r, int n);
// Inlocuieste toate potrivirile cu string-ul dat
void replaceAll(string& src, string with, regex r);
int ifileSize(istream& f);
int ofileSize(ostream& f);

double nthRoot(float num, float gr);
double mod(double n);