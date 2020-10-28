/*
 * hmLog.hpp
 *
 *  Created on: 2020年10月28日
 *      Author: siwei
 */

#ifndef INCLUDE_HMLOG_HPP_
#define INCLUDE_HMLOG_HPP_

#include <stdio.h>
class hmLog
{
public:
    static hmLog& getInstance()
    {
        static hmLog instance;
        return instance;
    }

	~hmLog()
	{
		printf("hmLog 结束\n");
	}

private:
	hmLog()
	{
		printf("hmLog 启动\n");
	}
};


#endif /* INCLUDE_HMLOG_HPP_ */
