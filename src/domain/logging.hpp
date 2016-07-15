/*
 * macros.hpp
 *
 *  Created on: Jun 17, 2016
 *      Author: joxmox
 */

#ifndef SRC_DOMAIN_LOGGING_HPP_
#define SRC_DOMAIN_LOGGING_HPP_

#ifdef LOG4CXX

#include "log4cxx/logger.h"
using namespace log4cxx;
#define CLASS_LOGGER(...) static log4cxx::LoggerPtr logger
#define GET_LOGGER(lg) LoggerPtr logger{Logger::getLogger(lg)}
#define INIT_LOGGER(cl, lg) LoggerPtr cl::logger{Logger::getLogger(lg)}

#else

#define LOG4CXX_FATAL(...)
#define LOG4CXX_ERROR(...)
#define LOG4CXX_WARN(...)
#define LOG4CXX_DEBUG(...)
#define LOG4CXX_TRACE(...)
#define CLASS_LOGGER(...)
#define GET_LOGGER(lg)
#define INIT_LOGGER(cl, lg)

#endif

#endif /* SRC_DOMAIN_LOGGING_HPP_ */
