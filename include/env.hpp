/*
 * env.hpp
 *
 *  Created on: 2020年11月1日
 *      Author: siwei
 */

#ifndef INCLUDE_ENV_HPP_
#define INCLUDE_ENV_HPP_

#include <stdio.h>
#include <string>

class env
{
public:
	std::string getValue(const std::string& str)
	{
		char* tmp;
		tmp = getenv(str.c_str());
		if(tmp == nullptr)
		{
			printf("env has no %s\n",str.c_str());
			return "";
		}
		return tmp;
	}
};

#endif /* INCLUDE_ENV_HPP_ */
