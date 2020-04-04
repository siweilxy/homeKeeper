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
#include <map>

#include "json.hpp"
#include "common.h"
#include "log.h"
#include "file.hpp"

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
    MYSQL *conn = nullptr;
    std::map<std::string,sql_stmt_t> sqlStmts;

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

        if(setAllSql() != SUCCESS)
        {
            LOG(ERROR) << "setAllSql error";
            return FAILED;
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
    MYSQL* getConn()
    {
        return conn;
    }
    std::map<std::string,sql_stmt_t>& getStmt()
    {
        return sqlStmts;
    }

    virtual int sqlBind () = 0;
    virtual int setAllSql() = 0;
    virtual ~MysqlDataBase ()
    {
        for(auto iter:sqlStmts)
        {
            LOG(ERROR) << "iter: "<<iter.first;
            if(iter.second.flag == 0)
            {
                mysql_stmt_close(iter.second.stmt);
            }
        }

        if(conn != nullptr)
        {
            mysql_close(conn);
        }
    };

    void setSql(std::string id,std::string sql)
    {
        sql_stmt_t stmt;
        stmt.sql = sql;
        stmt.stmt = nullptr;
        sqlStmts[id] = stmt;
        stmt.flag = 0;
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
                << " passwd is " << passwd;

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
