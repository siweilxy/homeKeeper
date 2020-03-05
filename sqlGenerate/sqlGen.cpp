/*
 * sqlGen.cpp
 *
 *  Created on: 3 Mar 2020
 *      Author: siwei
 */

#include <stdio.h>
#include <string>
#include <vector>
#include <mysql/mysql.h>
#include <iostream>
#include <string.h>
typedef struct field_s
{
    std::string name;
    std::string type;
    std::string length;
}field_t;

int getTypeAndLength(std::string res,field_t&field)
{
    auto iter = res.find("(");
    if(iter == res.npos)
    {
        field.type = res;
        return 0;
    }
    field.type = res.substr(0,iter);
    if(field.type == "varchar"||field.type == "char")
    {
        std::cout<<"iter is "<<iter<<"len is "<<res.length() - iter - 1<<std::endl;
        field.length = res.substr(iter +1,res.length() - iter - 2);
    }


    std::cout<<"res is "<<res<<std::endl;
    return 0;
}

int main (int argc, char **argv)
{
    std::string ip, port, dbUser, passwd, dbName, tableName, sql;
    int ret = 0;
    std::vector<field_t>fieldInfo;

    MYSQL *conn = nullptr;
    MYSQL_RES *res_ptr; /*指向查询结果的指针*/
    MYSQL_FIELD *field; /*字段结构指针*/
    MYSQL_ROW result_row; /*按行返回的查询信息*/
    int row, column; /*查询返回的行数和列数*/
    if (argc < 7)
    {
        printf ("argv error sqlGen ip port dbUser passwd dbName\n");
        return 0;
    }
    ip = argv[1];
    port = argv[2];
    dbUser = argv[3];
    passwd = argv[4];
    dbName = argv[5];
    tableName = argv[6];
    sql = "desc " + tableName;
    printf ("ip:%s, port:%s,dbUser:%s,passwd:%s,dbName:%s\n", ip.c_str (),
            port.c_str (), dbUser.c_str (), passwd.c_str (), dbName.c_str ());

    conn = mysql_init (NULL);

    if (mysql_real_connect (conn, ip.c_str (), dbUser.c_str (), passwd.c_str (),
                            dbName.c_str (), atoi (port.c_str ()), NULL, 0)
            == nullptr)
    {
        printf ("mysql_real_connect ERROR:%s\n", mysql_error (conn));
        return 0;
    }

    ret = mysql_query (conn, sql.c_str ());
    if (ret != 0)
    {
        printf ("mysql_query ERROR:%s\n", mysql_error (conn));
        return 0;
    }
    res_ptr = mysql_store_result (conn);
    /*取得結果的行数和*/
    column = mysql_num_fields (res_ptr);
    row = mysql_num_rows (res_ptr);
    printf ("查询到 %d 行 \n", row);
    /*输出結果的字段名*/
//    for (int i = 0; field = mysql_fetch_field (res_ptr); i++)
//        printf ("%10s ", field->name);
//    printf ("\n");
    /*按行输出結果*/
    for (int i = 1; i < row + 1; i++)
    {
        result_row = mysql_fetch_row (res_ptr);
        for (int j = 0; j < column; j=j+6)
        {
            field_t info;
            info.name=result_row[j];
            getTypeAndLength(result_row[j+1],info);
            fieldInfo.push_back(info);
            //printf ("%10s ", result_row[j]);
        }
    }
    for(auto info :fieldInfo)
    {
        std::cout<<"name:"<<info.name<<std::endl;
        std::cout<<"type:"<<info.type<<std::endl;
        std::cout<<"length:"<<info.length<<std::endl;
    }


    return 0;
}
