/*
 * test1.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: joxmox
 */

#include <vector>
#include <string>
#include <iostream>

using namespace std;

void f1() {
	cout << "f1" << endl;
}

void f2() {
	cout << "f2" << endl;
}

void f3() {
	cout << "f3" << endl;
}

int main() {
	vector<void (*)()> v;
	v.push_back(f1);
	v.push_back(f2);
	v.push_back(f3);
	v[0]();
	v[1]();
}

