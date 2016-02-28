/*
 * Buffer.cpp
 *
 *  Created on: Oct 21, 2015
 *      Author: sebis
 */



#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Buffer.hpp"

using namespace std;
using namespace log4cxx;

Buffer::Buffer(const string& buffName) {
	LoggerPtr logger{Logger::getLogger("Buffer")};
	LOG4CXX_DEBUG(logger, "new buffer created: " << buffName);
	name = buffName;
	maxLine = -1;
	topLine = 0;
	curLine = 0;
}

