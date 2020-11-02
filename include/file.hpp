/*
 * File.hpp
 *
 *  Created on: 11 Feb 2020
 *      Author: siwei
 */

#ifndef INCLUDE_FILE_HPP_
#define INCLUDE_FILE_HPP_

#include <string>
#include <stdio.h>
#include "json.hpp"
#include "common.h"

using json = nlohmann::json;

class file
{
private:
    std::string path;
    std::string res;
    std::string type;
    int reTryTime =5;
    FILE* fp;
    char buf[1024]={0};
public:
    /*
     *  "r" = "rt" 打开一个文本文件，文件必须存在，只允许读
        "r+" = "rt+"打开一个文本文件，文件必须存在，允许读写
        "rb" 打开一个二进制文件，文件必须存在，只允许读
        “rb+” 打开一个二进制文件，文件必须存在，允许读写
        "w" = “wt”新建一个文本文件，已存在的文件将内容清空，只允许写
        "w+" = "wt+"新建一个文本文件，已存在的文件将内容清空，允许读写
        “wb”新建一个二进制文件，已存在的文件将内容清空，只允许写
        “wb+”新建一个二进制文件，已存在的文件将内容清空，允许读写
        "a" = "at"打开或新建一个文本文件，只允许在文件末尾追写
        "a+" = "at+"打开或新建一个文本文件，可以读，但只允许在文件末尾追写
        “ab”打开或新建一个二进制文件，只允许在文件末尾追写
        “ab+”打开或新建一个二进制文件，可以读，但只允许在文件末尾追写
     * */
    file (std::string path,std::string type="at+"):path(path),type(type)
    {
        fp = fopen(path.c_str(),type.c_str());
        if(fp == nullptr)
        {
        	printf("fp is nullptr\n");
        }
    }

    int write(const std::string& fileInfo)
    {
        int ret = 0;
        //std::cout<<"fileInfo is "<<fileInfo<<std::endl;
        ret = fwrite(fileInfo.c_str(),fileInfo.length(),1,fp);
        if(ret < 0)
        {
        	printf("error[%d][%s]\n",errno,strerror(errno));
        	ret = reOpen();
        	if(ret == SUCCESS)
        	{
        		fwrite(fileInfo.c_str(),fileInfo.length(),1,fp);
        	}else
        	{
            	printf("error[%d][%s]\n",errno,strerror(errno));
        		return ret;
        	}
        }

        ret = fflush(fp);
        if(ret  < 0)
        {
        	printf("error[%d][%s]\n",errno,strerror(errno));
        }
        return ret;
    }

    int write(const char* fileInfo,int length)
        {
            int ret = 0;
            //std::cout<<"fileInfo is "<<fileInfo<<std::endl;
            ret = fwrite(fileInfo,length,1,fp);
            if(ret < 0)
            {
            	printf("error[%d][%s]\n",errno,strerror(errno));
            	ret = reOpen();
            	if(ret == SUCCESS)
            	{
            		fwrite(fileInfo,length,1,fp);
            	}else
            	{
                	printf("error[%d][%s]\n",errno,strerror(errno));
            		return ret;
            	}
            }

            ret = fflush(fp);
            if(ret  < 0)
            {
            	printf("error[%d][%s]\n",errno,strerror(errno));
            }
            return ret;
        }

    std::string getRes()
    {
    	std::string resBuf="";
    	int ret = 0;
    	memset(buf,0,sizeof(buf));
        ret = fread(buf,1024,1024,fp);
        if(ret < 0)
        {
        	printf("error[%d][%s]\n",errno,strerror(errno));
        	ret = reOpen();
        	if(ret == SUCCESS)
        	{
                ret = fread(buf,1024,1024,fp);
        	}else
        	{
            	printf("error[%d][%s]\n",errno,strerror(errno));
        		return "";
        	}
        }
        resBuf = buf;
        return resBuf;
    }

    std::string getJsonString(const std::string& key)
    {
    	std::string jsonStr = getRes();
    	if(jsonStr.empty())
    	{
    		printf("json is empty\n");
    		return "";
    	}

        auto js = json::parse (jsonStr);
        return js[key];
    }

    ~file()
    {
        fflush(fp);
        fclose(fp);
    }

    int reOpen()
    {
    	int openCount = 0;
    	fclose(fp);
    	fp = nullptr;
        while(openCount<reTryTime)
        {
        	fp = fopen(path.c_str(),type.c_str());
        	if(fp == nullptr)
        	{
        		printf("error [%d][%s]\n",errno,strerror(errno));
        		openCount++;
        	}else
        	{
        		break;
        	}
        }
       if(openCount >= reTryTime)
       {
    	   return FAILED;
       }
       return SUCCESS;
    }
};

#endif /* INCLUDE_FILE_HPP_ */
