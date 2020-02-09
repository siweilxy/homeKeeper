/*
 * curl.hpp
 *
 *  Created on: 10 Feb 2020
 *      Author: siwei
 */

#include <string>
#include <vector>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

std::string url;
std::string result;
char *res_buf = NULL;
int shift = 0;

size_t
copy_data (void *ptr, size_t size, size_t nmemb, void *stream)
{
    int res_size;

    res_size = size * nmemb;
    res_buf = (char*) malloc (res_size);
    memcpy (res_buf, ptr, res_size);
    shift += res_size;
    result = res_buf;
    free(res_buf);
    return size * nmemb;
}

std::string
curlUtil (std::string url)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init ();    // 初始化
    if (curl)
    {
        //curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");// 代理
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str ());
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, copy_data);
        res = curl_easy_perform (curl);   // 执行
        if (res != 0)
        {
            curl_easy_cleanup (curl);
        }
    }
    return result;
}

