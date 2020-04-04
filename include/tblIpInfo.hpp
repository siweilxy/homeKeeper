/*
 * tblIpInfo.hpp
 *
 *  Created on: 30 Mar 2020
 *      Author: siwei
 */

#ifndef INCLUDE_TBLIPINFO_HPP_
#define INCLUDE_TBLIPINFO_HPP_


#include "MysqlDataBase.hpp"
#include <vector>

class tblIpInfo : public MysqlDataBase
{
private:
    std::string select="select";
    std::string insert="insert";
    std::string update="update";

    std::vector<ipInfo_t> ipInfos;

    std::string sqlForSelect =
            "select rec_id,ip，send_flag from ipInfo where rec_id=(select max(rec_id) from ipInfo) for update";
    std::string sqlForInsert =
            "insert into ipInfo (ip,crt_ts,upd_ts,send_flag) values (?,now(),now(),0)";
    std::string sqlForUpdate=
            "update ipInfo set send_flag=1 where ip=?";

    MYSQL_BIND params_select[3];
    MYSQL_BIND params_insert[1];
    MYSQL_BIND params_update[1];

    int rec_id = 0;
    char ip[100];
    char send_flag[1];
    char ip_insert[100];
    char ip_update[100];

    int insertToDb(std::string ip)
    {
        ip_insert = ip.c_str();
        auto ret = mysql_stmt_execute(getStmt()[insert].stmt);
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_execute error: "<<ret;
            return FAILED;
        }
        return SUCCESS;
    }

    int updateToDb(std::string ip)
    {
        ip_update = ip.c_str();
        auto ret = mysql_stmt_execute(getStmt()[update].stmt);
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_execute error: "<<ret;
            return FAILED;
        }
        return SUCCESS;
    }

    int getResFromDb ()
    {
        ENTER
        int ret = 0;
        auto stmt= getStmt()[select].stmt;
        ret = mysql_stmt_execute (stmt);           //执行与语句句柄相关的预处理
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_execute error: "<<ret;
            return FAILED;
        }

        ret = mysql_stmt_store_result (stmt);      //以便后续的mysql_stmt_fetch()调用能返回缓冲数据
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_store_result error: "<<ret;
            return FAILED;
        }

        while (1)
        {
            ret = mysql_stmt_fetch (stmt);
            if (ret == 0)
            {
                ipInfo_t ipInfo;
                ipInfo.ip = ip;
                ipInfo.rec_id = rec_id;
                ipInfo.send_flag = send_flag;
                ipInfos.push_back(ipInfo);
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
        auto selectStmt= getStmt()[select].stmt;
        auto insertStmt= getStmt()[insert].stmt;
        auto updateStmt= getStmt()[update].stmt;

        memset(params_select, 0, sizeof(params_select));
        memset(params_insert, 0, sizeof(params_insert));
        memset(params_update, 0, sizeof(params_update));

        params_select[0].buffer_type = MYSQL_TYPE_LONG;
        params_select[0].buffer = &rec_id;
        params_select[0].buffer_length = sizeof(rec_id);

        params_select[1].buffer_type = MYSQL_TYPE_STRING;
        params_select[1].buffer = ip;
        params_select[1].buffer_length = sizeof(ip);

        params_select[2].buffer_type = MYSQL_TYPE_STRING;
        params_select[2].buffer = send_flag;
        params_select[2].buffer_length = sizeof(send_flag);

        params_insert[0].buffer_type = MYSQL_TYPE_STRING;
        params_insert[0].buffer = ip_insert;
        params_insert[0].buffer_length = sizeof(ip_insert);

        params_update[0].buffer_type = MYSQL_TYPE_STRING;
        params_update[0].buffer = ip_update;
        params_update[0].buffer_length = sizeof(ip_update);

        ret = mysql_stmt_bind_result (selectStmt, params_select); //用于将结果集中的列与数据缓冲和长度缓冲关联（绑定）起来
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_bind_result error: "<<ret;
            return FAILED;
        }

        ret = mysql_stmt_bind_param(insertStmt, params_insert);
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_bind_param error: "<<ret;
            return FAILED;
        }

        ret = mysql_stmt_bind_param(updateStmt, params_update);
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_bind_param error: "<<ret;
            return FAILED;
        }

        EXIT
        return SUCCESS;
    }

    std::vector<ipInfos> getRes ()
    {
        ENTER
        int ret = getResFromDb();
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"getResFromDb ERROR: "<<ret;
        }
        EXIT
        return ipInfos;
    }


    int setAllSql() override
    {
        setSql (select,sqlForSelect);
        setSql (update,sqlForUpdate);
        setSql (insert,sqlForInsert);
        return SUCCESS;
    }

    tblIpInfo ()
    {
    }
};


#endif /* INCLUDE_TBLIPINFO_HPP_ */
