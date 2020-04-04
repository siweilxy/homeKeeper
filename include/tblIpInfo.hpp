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
            "select rec_id,ip,send_flag from ipInfo where rec_id=(select max(rec_id) from ipInfo) for update";
    std::string sqlForInsert =
            "insert into ipInfo (ip,crt_ts,upd_ts,send_flag) values (?,now(),now(),0)";
    std::string sqlForUpdate=
            "update ipInfo set send_flag=1,upd_ts=now() where ip=?";

    MYSQL_BIND params_select[3];
    MYSQL_BIND params_insert[1];
    MYSQL_BIND params_update[1];

    int rec_id = 0;
    char ip[100] = {0};
    char send_flag[1];
    char ip_insert[100]={0};
    char ip_update[100]={0};

    int setAllSql() override
    {
        setSql (select,sqlForSelect);
        setSql (update,sqlForUpdate);
        setSql (insert,sqlForInsert);
        return SUCCESS;
    }

    int sqlBind () override
    {
        ENTER
        int ret = 0;
        memset(params_select, 0, sizeof(params_select));
        memset(params_insert, 0, sizeof(params_insert));
        memset(params_update, 0, sizeof(params_update));

        for(auto& iter:getStmt())
        {
            iter.second.stmt = mysql_stmt_init (getConn());
            ret = mysql_stmt_prepare (iter.second.stmt, iter.second.sql.c_str (), strlen (iter.second.sql.c_str ()));
            if (ret != SUCCESS)
            {
                LOG(ERROR) << "mysql_stmt_prepare error  " << mysql_stmt_error (iter.second.stmt);
                iter.second.flag = 1;
                return FAILED;
            }
        }

        auto selectStmt= getStmt()[select].stmt;
        auto insertStmt= getStmt()[insert].stmt;
        auto updateStmt= getStmt()[update].stmt;


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
            LOG(ERROR)<<"mysql_stmt_bind_result error: "<<mysql_stmt_error(selectStmt);
            return FAILED;
        }

        ret = mysql_stmt_bind_param(insertStmt, params_insert);
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_bind_param error: "<<mysql_stmt_error(insertStmt);
            return FAILED;
        }

        ret = mysql_stmt_bind_param(updateStmt, params_update);
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_bind_param error: "<<mysql_stmt_error(updateStmt);
            return FAILED;
        }

        EXIT
        return SUCCESS;
    }

public:
    int updateToDb(std::string ip)
    {
        snprintf(ip_update,sizeof(ip_update),"%s",ip.c_str());
        auto ret = mysql_stmt_execute(getStmt()[update].stmt);
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_execute error: "<<mysql_stmt_error(getStmt()[update].stmt);
            return FAILED;
        }
        ret = mysql_commit(getConn());
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_commit error: "<<mysql_stmt_error(getStmt()[update].stmt);
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
                LOG(ERROR) << "FETCH ERROR " << mysql_stmt_error (stmt);
                return FAILED;
            }

        }
        EXIT
        return SUCCESS;
    }

    int insertToDb(std::string ip)
    {
        snprintf(ip_insert,sizeof(ip_insert),"%s",ip.c_str());
        auto ret = mysql_stmt_execute(getStmt()[insert].stmt);
        if(ret != SUCCESS)
        {
            LOG(ERROR)<<"mysql_stmt_execute error: "<<mysql_stmt_error(getStmt()[insert].stmt);
            LOG(ERROR)<<"ip is "<<ip.c_str();
            return FAILED;
        }
        return SUCCESS;
    }

    std::vector<ipInfo_t> getRes ()
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

    tblIpInfo ()
    {
    }
};


#endif /* INCLUDE_TBLIPINFO_HPP_ */
