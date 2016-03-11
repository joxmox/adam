/*
 * ct.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: joxmox
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Curse.hpp"

using namespace std;
using namespace log4cxx;

LoggerPtr logger{Logger::getLogger("curtest")};

void f(Win* w) {
	random_device r;
	uniform_int_distribution<> rect(5, 200);
	string s = "ABC1234567890";
	for (auto j = 0; j < 9; j++) {
		for (auto i = 0; i < s.size(); i++) {
			w->pos(j, i);
			w->printStr(s[i]);
			w->refresh();
			this_thread::sleep_for(chrono::milliseconds(rect(r)));
		}
		int key = w->readKey();
		w->pos(9, 0);
		w->printStr(string(1, key));
		w->refresh();
	}
}

int main() {
    PropertyConfigurator::configure("conf/log4cxx.conf");
	Curse c;
	Win* w1 = c.creWin(10,20,0,0);
	Win* w2 = c.creWin(10,20,12,20);
	Win* w3 = c.creWin(10,20,6, 40);
	w3->pos(0, 0);
	w3->printStr("Hello!");
	w3->refresh();
	thread t1 {f, w1};
	thread t2 {f, w2};
	t1.join();
	t2.join();
	w3->pos(1, 0);
	w3->printStr("Done!");
	w3->refresh();
	this_thread::sleep_for(chrono::milliseconds(2000));
	delete w1;
	delete w2;
}
