#pragma once

#include <iostream>
#include <string>

using namespace std;

class Subcalcul {
	private:
		char* fragment;
		int n;
	public:
		Subcalcul();
		Subcalcul(string fragment);
		~Subcalcul();
		Subcalcul(const Subcalcul& sc);

		string getFragment();
		int getMarime();
		void setFragment(string fragment);
		void setMarime(int n);

		static string evalueaza(string ecuatie, char oper);
		double calculeaza();

		Subcalcul& operator=(const Subcalcul& sc);
		bool operator>(const Subcalcul& c);
		bool operator<(const Subcalcul& c);
		friend istream& operator>>(istream& in, Subcalcul& sc);
		friend ostream& operator<<(ostream& out, const Subcalcul& sc);
};