/*
 * EmailInfo.hpp
 *
 *  Created on: 11 Feb 2020
 *      Author: siwei
 */

#ifndef INCLUDE_TBLEMAILINFO_HPP_
#define INCLUDE_TBLEMAILINFO_HPP_

#include "MysqlDataBase.hpp"
#include <vector>

class tblEmailInfo : public MysqlDataBase
{
private:
    std::string sqlForSelect =
            "select username,passwd,smtpServer,recipient,mailfrom from emailInfo";
    std::vector<emailInfo_t> emailInfos;
    MYSQL_BIND params[5];
    char userName[100];
    char passwd[100];
    char smtpServer[100];
    char recipient[100];
    char mailFrom[100];

    int getResFromDb ()
    {
        ENTER
        int ret = 0;
        while (1)
        {
            ret = mysql_stmt_fetch (stmt);
            if (ret == 0)
            {
                emailInfo_t emailInfo;
                emailInfo.mailFrom = mailFrom;
                emailInfo.passwd = passwd;
                emailInfo.recipient = recipient;
                emailInfo.smtpServer = smtpServer;
                emailInfo.userName = userName;
                emailInfos.push_back (emailInfo);
            }
            else if (ret == 100)
            {
                LOG(INFO) << "ret is 100,ended";
                return SUCCESS;
            }
            else
            {
                LOG(ERROR) << "FETCH ERROR " << mysql_error (conn);
                return FAILED;
            }

        }
        EXIT
        return SUCCESS;
    }

public:
    int sqlBind () override
    {
        ENTER
        int ret = 0;
        memset(params, 0, sizeof(params));
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = userName;
        params[0].buffer_length = sizeof(userName);

        params[1].buffer_type = MYSQL_TYPE_STRING;
        params[1].buffer = passwd;
        params[1].buffer_length = sizeof(passwd);

        params[2].buffer_type = MYSQL_TYPE_STRING;
        params[2].buffer = smtpServer;
        params[2].buffer_length = sizeof(smtpServer);

        params[3].buffer_type = MYSQL_TYPE_STRING;
        params[3].buffer = recipient;
        params[3].buffer_length = sizeof(recipient);

        params[4].buffer_type = MYSQL_TYPE_STRING;
        params[4].buffer = mailFrom;
        params[4].buffer_length = sizeof(mailFrom);

        ret = mysql_stmt_bind_result (stmt, params); //用于将结果集中的列与数据缓冲和长度缓冲关联（绑定）起来
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_bind_result error: "<<ret;
        }

        ret = mysql_stmt_execute (stmt);           //执行与语句句柄相关的预处理
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_execute error: "<<ret;
        }

        ret = mysql_stmt_store_result (stmt);      //以便后续的mysql_stmt_fetch()调用能返回缓冲数据
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_store_result error: "<<ret;
        }

        EXIT
        return SUCCESS;
    }

    std::vector<emailInfo_t> getRes ()
    {
        ENTER
        int ret = getResFromDb();
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"getResFromDb ERROR: "<<ret;
        }
        EXIT
        return emailInfos;
    }

    tblEmailInfo ()
    {
        setSql (sqlForSelect);
    }
};

#endif /* INCLUDE_TBLEMAILINFO_HPP_ */
