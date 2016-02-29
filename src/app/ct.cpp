/*
 * ct.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: joxmox
 */

#include <iostream>

#include "Curse.hpp"


int main() {
	Curse* c = new Curse{};
	Win* w1 = c->creWin(5, 10, 2, 2);
	Win* w2 = c->creWin(7, 20, 10, 20);
	w1->pos(0, 0);
	w1->print('1');
	w2->pos(0, 0);
	w2->print('2');
	c->refresh();
	string line;
	getline(cin, line);
}
