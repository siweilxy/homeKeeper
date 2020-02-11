/*
 * EmailInfo.hpp
 *
 *  Created on: 11 Feb 2020
 *      Author: siwei
 */

#ifndef INCLUDE_TBLEMAILINFO_HPP_
#define INCLUDE_TBLEMAILINFO_HPP_

#include <string>
#include "json.hpp"
#include "common.h"
#include "log.h"

using json = nlohmann::json;
class tblEmailInfo
{
private:
    std::string configFile;
    std::string ip;
    std::string port;
    std::string dateBase;
    std::string user;
    std::string passwd;

public:
    tblEmailInfo ()
    {
    }
    int
    init ()
    {
        char *configPath = getenv ("CONFIG_FILE");
        if (configPath == nullptr)
        {
            LOG(ERROR) << "getenv failed";
            return FAILED;
        }

        file file (configPath);
        configFile = file.getRes ();

        auto js = json::parse (configFile);
        ip = js["ip"];
        port = js["port"];
        dateBase = js["dateBase"];
        user = js["user"];
        passwd = js["passwd"];
        LOG(INFO) << "ip is " << ip << " port is " << port
                << " dateBase is " << dateBase << " user is " << user
                << " passwd is " << passwd;
        return SUCCESS;
    }
};

#endif /* INCLUDE_TBLEMAILINFO_HPP_ */
