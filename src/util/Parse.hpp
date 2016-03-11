/*
 * parse.hpp
 *
 *  Created on: Oct 18, 2015
 *      Author: joxmox
 */

#ifndef PARSE_HPP_
#define PARSE_HPP_

#include <vector>
#include <regex>

class Parse {
    std::vector<std::pair<int, std::regex>> pTable;
    std::string param;
public:
    Parse(std::vector<std::pair<int, std::string>> iTable);
    int decode(std::string cmd);
    std::string getParam();
};


#endif /* PARSE_HPP_ */
