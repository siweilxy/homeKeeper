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
#include "hmLog.hpp"
#include "file.hpp"
#include <iconv.h>

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
        if(conn == nullptr)
        {
            ERROR("mysql_init ERROR:" <<" errno is "<<mysql_errno( conn )<< " sql error: "<<mysql_error (conn));
            return FAILED;
        }

        if (mysql_real_connect (conn, ip.c_str (), user.c_str (),
                                passwd.c_str (), dateBase.c_str (), atoi(port.c_str()), NULL,
                                0) == nullptr)
        {
            ERROR("mysql_real_connect ERROR:[%s]", mysql_error (conn));
            return FAILED;
        }

        if(setAllSql() != SUCCESS)
        {
            ERROR("setAllSql error");
            return FAILED;
        }

        if(sqlBind() != SUCCESS)
        {
            ERROR("sqlBInd error");
            return FAILED;
        }
        EXIT
        return SUCCESS;
    }
public:

    int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
    {
        iconv_t cd;
        int rc;
        char **pin = &inbuf;
        char **pout = &outbuf;

        cd = iconv_open(to_charset,from_charset);
        if (cd==0) return -1;
        memset(outbuf,0,outlen);
        if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
        iconv_close(cd);
        return 0;
    }

    /*UNICODE码转为GB2312码*/
    int u2g(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
    {
        char utf8[]="utf-8";
        char gb[]="gb2312";
        return code_convert(utf8,gb,inbuf,inlen,outbuf,outlen);
    }
    /*GB2312码转为UNICODE码*/
    int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
    {
        char utf8[]="utf-8";
        char gb[]="gb2312";
        return code_convert(gb,utf8,inbuf,inlen,outbuf,outlen);
    }

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
        if (configPath == nullptr||strlen(configPath) == 0)
        {
            ERROR("getenv failed");
            return FAILED;
        }

        file file (configPath);
        configFile = file.getRes ();
        INFO("configFIle is [%s]",configFile.c_str());
        auto js = json::parse (configFile);
        ip = js["ip"];
        port = js["port"];
        dateBase = js["dateBase"];
        user = js["user"];
        passwd = js["passwd"];
        INFO("ip is [%s],port is [%s] dateBase is [%s] user is [%s] passwd id [%s]" , ip.c_str(),port.c_str(),
        		dateBase.c_str(),user.c_str(),passwd.c_str());

        ret = initDb ();
        if (ret != SUCCESS)
        {
            ERROR("initDb failed");
            return FAILED;
        }

        EXIT
        return SUCCESS;
    }
};

#endif /* INCLUDE_MYSQLDATABASE_HPP_ */
