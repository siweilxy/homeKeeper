/*
 * downloadfile.hpp
 *
 *  Created on: 12 Jul 2020
 *      Author: siwei
 */

#ifndef HOMEKEEPER_DOWNLOADFILE_HPP_
#define HOMEKEEPER_DOWNLOADFILE_HPP_

#include "MysqlDataBase.hpp"
#include <vector>
class downloadFile : public MysqlDataBase
{
private:
    std::string select="select";
    std::string update="update";

    char fileName[512];
    char fileNameUpdate[512];
    char path[512];
    std::vector<downloadFile_t> dlFiles;

    std::string sqlForSelect =
            "select fileName,path from downloadfile where send_flag=0 and fileSize<3000000000 limit 1";

    std::string sqlForUpdate=
            "update downloadfile set send_flag=1,upd_ts=now(),send_ts=now() where fileName=?";

    MYSQL_BIND params_select[2];
    MYSQL_BIND params_update[1];

    int setAllSql() override
    {
        setSql (select,sqlForSelect);
        setSql (update,sqlForUpdate);
        return SUCCESS;
    }

    int sqlBind () override
    {
        ENTER
        int ret = 0;
        memset(params_select, 0, sizeof(params_select));
        memset(params_update, 0, sizeof(params_update));

        for(auto& iter:getStmt())
        {
            iter.second.stmt = mysql_stmt_init (getConn());
            ret = mysql_stmt_prepare (iter.second.stmt, iter.second.sql.c_str (), strlen (iter.second.sql.c_str ()));
            if (ret != SUCCESS)
            {
                ERROR("mysql_stmt_prepare error:%s" ,mysql_stmt_error (iter.second.stmt));
                iter.second.flag = 1;
                return FAILED;
            }
        }

        auto selectStmt= getStmt()[select].stmt;
        auto updateStmt= getStmt()[update].stmt;

        params_select[0].buffer_type = MYSQL_TYPE_STRING;
        params_select[0].buffer = fileName;
        params_select[0].buffer_length = sizeof(fileName);

        params_select[1].buffer_type = MYSQL_TYPE_STRING;
        params_select[1].buffer = path;
        params_select[1].buffer_length = sizeof(path);

        params_update[0].buffer_type = MYSQL_TYPE_STRING;
        params_update[0].buffer = fileNameUpdate;
        params_update[0].buffer_length = sizeof(fileNameUpdate);

        ret = mysql_stmt_bind_result (selectStmt, params_select); //用于将结果集中的列与数据缓冲和长度缓冲关联（绑定）起来
        if(ret != SUCCESS)
        {
            ERROR("mysql_stmt_bind_result error:%s ",mysql_stmt_error(selectStmt));
            return FAILED;
        }

        ret = mysql_stmt_bind_param(updateStmt, params_update);
        if(ret != SUCCESS)
        {
            ERROR("mysql_stmt_bind_param error: %s",mysql_stmt_error(updateStmt));
            return FAILED;
        }

        EXIT
        return SUCCESS;
    }

public:
    int updateToDb(std::string fileName_update)
    {
        snprintf(fileNameUpdate,sizeof(fileNameUpdate),"%s",fileName_update.c_str());
        auto ret = mysql_stmt_execute(getStmt()[update].stmt);
        if(ret != SUCCESS)
        {
            ERROR("mysql_stmt_execute error: %s",mysql_stmt_error(getStmt()[update].stmt));
            return FAILED;
        }
        ret = mysql_commit(getConn());
        if(ret != SUCCESS)
        {
            ERROR("mysql_commit error: %s",mysql_stmt_error(getStmt()[update].stmt));
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
            ERROR("mysql_stmt_execute error: %d",ret);
            return FAILED;
        }

        ret = mysql_stmt_store_result (stmt);      //以便后续的mysql_stmt_fetch()调用能返回缓冲数据
        if(ret != SUCCESS)
        {
            ERROR("mysql_stmt_store_result error: %d",ret);
            return FAILED;
        }

        while (1)
        {
            ret = mysql_stmt_fetch (stmt);
            if (ret == 0)
            {
                downloadFile_t downloadFile;
                downloadFile.fileName = fileName;
                downloadFile.path = path;
                dlFiles.push_back(downloadFile);
            }
            else if (ret == 100)
            {
                INFO("ret is 100,ended");
                return SUCCESS;
            }
            else
            {
                ERROR("FETCH ERROR %s",mysql_stmt_error (stmt));
                return FAILED;
            }

        }
        EXIT
        return SUCCESS;
    }

    int getRes (std::vector<downloadFile_t>& res)
    {
        ENTER
        int ret = getResFromDb();
        if(ret != SUCCESS)
        {
            ERROR("getResFromDb ERROR: %d",ret);
            return FAILED;
        }
        EXIT
        res = std::move(dlFiles);
        return SUCCESS;
    }

    downloadFile ()
    {
    }
};



#endif /* HOMEKEEPER_DOWNLOADFILE_HPP_ */
