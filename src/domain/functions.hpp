/*
 * functions.hpp
 *
 *  Created on: Feb 28, 2016
 *      Author: joxmox
 */

#ifndef DOMAIN_FUNCTIONS_HPP_
#define DOMAIN_FUNCTIONS_HPP_

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Editor.hpp"

namespace fun {

using namespace std;
using namespace log4cxx;

void illegalChar(Editor* ed) {
	LoggerPtr logger{Logger::getLogger("fun::charIllegal")};
	LOG4CXX_DEBUG(logger, "received unexpected character: " << ed->getKey());
}

void character(Editor* ed) {
	LoggerPtr logger{Logger::getLogger("fun::character")};
	LOG4CXX_DEBUG(logger, "received normal typing character: " << ed->getKey() << " - " << char(ed->getKey()));
	ed->insertChar();
}

void exit(Editor* ed) {
	ed->quit();
}

void debug(Editor* ed) {
	ed->debug();
}

}

#endif /* DOMAIN_FUNCTIONS_HPP_ */
