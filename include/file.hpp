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

class file
{
private:
    std::string path;
    std::string res;
    std::string type;
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
    }

    int write(std::string fileInfo)
    {
        int ret = 0;
        std::cout<<"fileInfo is "<<fileInfo<<std::endl;
        ret = fwrite(fileInfo.c_str(),fileInfo.length(),1,fp);
        return ret;
    }

    std::string getRes()
    {
        fread(buf,1024,1024,fp);
        res = buf;
        return res;
    }

    ~file()
    {
        fflush(fp);
        fclose(fp);
    }
};

#endif /* INCLUDE_FILE_HPP_ */
