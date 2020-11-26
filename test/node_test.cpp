/*
 * node_test.cpp
 *
 *  Created on: 2020年11月26日
 *      Author: siwei
 */



#include <hmLog.hpp>
#include "node.hpp"
int main()
{
	hmLog::getInstance().init(0);
	node node;
	INFO("node [%s]",node.getNodeName().c_str());
	return 0;
}
