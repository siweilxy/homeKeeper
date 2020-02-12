/*
 * MysqlDataBase.hpp
 *
 *  Created on: 12 Feb 2020
 *      Author: siwei
 */

#ifndef INCLUDE_MYSQLDATABASE_HPP_
#define INCLUDE_MYSQLDATABASE_HPP_

#include <string>
#include <mysql/mysql.h>

#include "json.hpp"
#include "common.h"
#include "log.h"

using json = nlohmann::json;

class MysqlDataBase
{
private:
    std::string configFile;
    std::string ip;
    std::string port;
    std::string dateBase;
    std::string user;
    std::string passwd;
    std::string sql;
    virtual int getResFromDb() = 0;
    int initDb ()
    {
        ENTER
        int ret = 0;
        conn = mysql_init (NULL);

        if (mysql_real_connect (conn, ip.c_str (), user.c_str (),
                                passwd.c_str (), dateBase.c_str (), atoi(port.c_str()), NULL,
                                0) == nullptr)
        {
            LOG(ERROR) << "mysql_real_connect ERROR:" << mysql_error (conn);
            return FAILED;
        }

        stmt = mysql_stmt_init (conn);

        ret = mysql_stmt_prepare (stmt, sql.c_str (), strlen (sql.c_str ()));
        if (ret != SUCCESS)
        {
            LOG(ERROR) << "mysql_stmt_prepare error  " << mysql_error (conn)<<" sql is "<<sql;
        }

        if(sqlBind() != SUCCESS)
        {
            LOG(ERROR) << "sqlBInd error";
            return FAILED;
        }
        EXIT
        return SUCCESS;
    }
public:
    MYSQL *conn = nullptr;
    MYSQL_STMT *stmt = nullptr; //创建MYSQL_STMT句柄
    virtual int sqlBind () = 0;
    virtual ~MysqlDataBase ()
    {
        if(stmt != nullptr)
        {
            mysql_stmt_close(stmt);
        }

        if(conn != nullptr)
        {
            mysql_close(conn);
        }
    };

    void setSql(std::string sql)
    {
        this->sql = sql;
    }

    int init ()
    {
        ENTER
        char *configPath = getenv ("CONFIG_FILE");
        int ret = 0;
        if (configPath == nullptr)
        {
            LOG(ERROR) << "getenv failed";
            return FAILED;
        }

        file file (configPath);
        configFile = file.getRes ();
        LOG(INFO)<<"configFIle is "<<configFile;
        auto js = json::parse (configFile);
        ip = js["ip"];
        port = js["port"];
        dateBase = js["dateBase"];
        user = js["user"];
        passwd = js["passwd"];
        LOG(INFO) << "ip is " << ip << " port is " << port
                << " dateBase is " << dateBase << " user is " << user
                << " passwd is " << passwd<< " sql is "<<sql;

        ret = initDb ();
        if (ret != SUCCESS)
        {
            LOG(ERROR) << "initDb failed";
            return FAILED;
        }

        EXIT
        return SUCCESS;
    }
};

#endif /* INCLUDE_MYSQLDATABASE_HPP_ */
