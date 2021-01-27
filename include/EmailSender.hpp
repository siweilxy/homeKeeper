/*
 * EmailSender.hpp
 *
 *  Created on: Jan 18, 2020
 *      Author: siwei
 */

#ifndef INCLUDE_EMAILSENDER_HPP_
#define INCLUDE_EMAILSENDER_HPP_

#include <string>
#include <vector>
#include <curl/curl.h>
#include <string.h>
#include <sstream>
#include "base64.hpp"
#include <mimetic/mimetic.h>
#include "hmLog.hpp"
class EmailSender
{
public:
    EmailSender (const std::string &charset = "gb2312")
    {
        m_strCharset = charset;
        m_vRecvMail.clear ();
    }    // 也可以传入utf

    //设置stmp服务器、用户名、密码、端口（端口其实不用指定，libcurl默认25，但如果是smtps则默认是465）
    void SetSmtpServer (const std::string &username,
                        const std::string &password,
                        const std::string &servername, const std::string &port =
                                "25")
    {
        m_strUserName = username;
        m_strPassword = password;
        m_strServerName = servername;
        m_strPort = port;
    }
    //发送者姓名，可以不用

    void SetSendName (const std::string &sendname)
    {
        std::string strTemp = "";
        strTemp += "=?";
        strTemp += m_strCharset;
        strTemp += "?B?";
        strTemp += base64_encode ((unsigned char*) sendname.c_str (),
                                  sendname.size ());
        strTemp += "?=";
        m_strSendName = strTemp;
    }

    //发送者邮箱
    void SetSendMail (const std::string &sendmail)
    {
        m_strSendMail = sendmail;
    }

    //添加收件人
    void AddRecvMail (const std::string &recvmail)
    {
        m_vRecvMail.push_back (recvmail);
    }

    //设置主题
    void SetSubject (const std::string &subject)
    {
        m_strSubject = subject;
    }

    //设置正文内容
    void SetBodyContent (const std::string &content)
    {
        std::stringstream temp;
        mimetic::Base64::Encoder b64;
        std::ostreambuf_iterator<char> out(temp);
        mimetic::code(content.begin(),  content.end(), b64, out);
        std::string str = temp.str();
        m_strContent = str;
    }

    //添加附件
    void AddAttachment (const std::string &filename)
    {
        m_vAttachMent.push_back (filename);
    }

    //发送邮件
    bool SendMail ()
    {
        CreatMessage ();
        bool ret = true;
        CURL *curl;
        CURLcode res = CURLE_OK;
        struct curl_slist *recipients = NULL;

        curl = curl_easy_init ();
        if (curl)
        {
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUserName.c_str ());
            curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str ());
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            std::string tmp = "smtp://";
            tmp += m_strServerName;
            // 注意不能直接传入tmp，应该带上.c_str()，否则会导致下面的
            // curl_easy_perform调用返回CURLE_COULDNT_RESOLVE_HOST错误
            // 码
            curl_easy_setopt(curl, CURLOPT_URL, tmp.c_str ());
            /* If you want to connect to a site who isn't using a certificate that is
             * signed by one of the certs in the CA bundle you have, you can skip the
             * verification of the server's certificate. This makes the connection
             * A LOT LESS SECURE.
             *
             * If you have a CA cert for the server stored someplace else than in the
             * default bundle, then the CURLOPT_CAPATH option might come handy for
             * you. */
#ifdef SKIP_PEER_VERIFICATION
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

            /* If the site you're connecting to uses a different host name that what
             * they have mentioned in their server certificate's commonName (or
             * subjectAltName) fields, libcurl will refuse to connect. You can skip
             * this check, but this will make the connection less secure. */
#ifdef SKIP_HOSTNAME_VERIFICATION
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

            /* Note that this option isn't strictly required, omitting it will result
             * in libcurl sending the MAIL FROM command with empty sender data. All
             * autoresponses should have an empty reverse-path, and should be directed
             * to the address in the reverse-path which triggered them. Otherwise,
             * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
             * details.
             */
            //curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
            curl_easy_setopt(curl, CURLOPT_MAIL_FROM, m_strSendMail.c_str ());
            /* Add two recipients, in this particular case they correspond to the
             * To: and Cc: addressees in the header, but they could be any kind of
             * recipient. */
            for (size_t i = 0; i < m_vRecvMail.size (); i++)
            {

                recipients = curl_slist_append (recipients,
                                                m_vRecvMail[i].c_str ());
            }
            curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

            std::stringstream stream;

//            stream.str (m_strMessage.c_str ());
//            stream.flush ();

            /* We're using a callback function to specify the payload (the headers and
             * body of the message). You could just use the CURLOPT_READDATA option to
             * specify a FILE pointer to read from. */

            // 注意回调函数必须设置为static
            curl_easy_setopt(curl, CURLOPT_READFUNCTION,
                             &EmailSender::payload_source);
            curl_easy_setopt(curl, CURLOPT_READDATA, (void* )&m_strMessage);
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            /* Since the traffic will be encrypted, it is very useful to turn on debug
             * information within libcurl to see what is happening during the
             * transfer */
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            int nTimes = 0;
            /* Send the message */
            res = curl_easy_perform (curl);
            CURLINFO info = CURLINFO_NONE;
            //curl_easy_getinfo(curl, info);
            /* Check for errors */

            while (res != CURLE_OK)
            {

                nTimes++;
                if (nTimes > 5)
                {
                    break;
                }
                fprintf (stderr, "curl_easy_perform() failed: %s\n\n",
                         curl_easy_strerror (res));

                char achTip[512] = { 0 };
                sprintf (achTip, "curl_easy_perform() failed: %s\n\n",
                         curl_easy_strerror (res));
                ret = false;

                sleep( 100 );
                res = curl_easy_perform(curl);
            }

            /* Free the list of recipients */
            curl_slist_free_all (recipients);

            /* Always cleanup */
            curl_easy_cleanup (curl);

        }
        return ret;
    }
private:

    //回调函数，将MIME协议的拼接的字符串由libcurl发出
    static size_t payload_source (void *ptr, size_t size, size_t nmemb,
                                  void *stream)
    {
        size_t num_bytes = size * nmemb;
        char *data = (char*) ptr;
        std::stringstream *strstream = (std::stringstream*) stream;

        strstream->read (data, num_bytes);

        return strstream->gcount ();
    }

    void CreatMessage ()
    {
        mimetic::MultipartMixed head;
        std::string to;
        std::string file;
        head.header().from(m_strSendMail);
        for (size_t i = 0; i < m_vRecvMail.size (); i++)
        {
            if (i > 0)
            {
                to += ",";
            }
            to += m_vRecvMail[i];
        }

        head.header().to(to);
        head.header().subject(m_strSubject);
        head.header().push_back(mimetic::Field("Mime-Version","1.0"));

        mimetic::MimeEntity* pMe2 = new mimetic::MimeEntity;
        //说明块是一段文字，文字使用GB2312字符集
        pMe2->header().push_back(mimetic::Field("Content-Type: text/plain; charset=GB2312"));
        pMe2->header().push_back(mimetic::Field("Content-Transfer-Encoding","base64"));
        pMe2->body().assign(m_strContent);
        //把这一块加入到整个邮件中
        head.body().parts().push_back(pMe2);

                //附件
        std::string filename = "";
        std::string filetype = "";
        for (size_t i = 0; i < m_vAttachMent.size (); i++)
        {
            GetFileName (m_vAttachMent[i], filename);
            GetFileType (m_vAttachMent[i], filetype);
            SetContentType (filetype);
            SetFileName (filename);
            mimetic::MimeEntity* pMe = new mimetic::MimeEntity;

            pMe->header().push_back(mimetic::Field("Content-Type: application/octet-stream; name="+m_vAttachMent[i]));
            pMe->header().push_back(mimetic::Field("Content-Disposition : attachment; filename="+m_vAttachMent[i]));

            pMe->header().push_back(mimetic::Field("Content-Transfer-Encoding","base64"));

            FILE *pt = NULL;
            if ((pt = fopen (m_vAttachMent[i].c_str (), "rb")) == NULL)
            {
                ERROR("打开文件失败: [%s]" , m_vAttachMent[i].c_str());
                continue;
            }
            fseek (pt, 0, SEEK_END);
            int len = ftell (pt);
            fseek (pt, 0, SEEK_SET);
            int rlen = 0;
            char buf[55];
            for (size_t i = 0; i < len / 54 + 1; i++)
            {
                memset (buf, 0, 55);
                rlen = fread (buf, sizeof(char), 54, pt);
                file +=   buf;
            }
            std::stringstream temp;
            mimetic::Base64::Encoder b64;
            std::ostreambuf_iterator<char> out(temp);
            mimetic::code(file.begin(),  file.end(), b64, out);
            std::string str = temp.str();
            pMe->load(str.begin(), str.end(), mimetic::imNone);
            head.body().parts().push_back(pMe);
            fclose (pt);
            pt = NULL;
        }
        m_strMessage<<head;
        //std::cout<<head<<std::endl;
    }

    //获取文件类型
    int GetFileType (std::string const &stype)
    {
        if (stype == "txt")
        {
            return 0;
        }
        else if (stype == "xml")
        {
            return 1;
        }
        else if (stype == "html")
        {
            return 2;
        }
        else if (stype == "jpeg")
        {
            return 3;
        }
        else if (stype == "png")
        {
            return 4;
        }
        else if (stype == "gif")
        {
            return 5;
        }
        else if (stype == "exe")
        {
            return 6;
        }

        return -1;
    }

    //设置文件名
    void SetFileName (const std::string &FileName)
    {
        std::string EncodedFileName = "=?";
        EncodedFileName += m_strCharset;
        EncodedFileName += "?B?";            //修改
        EncodedFileName += base64_encode ((unsigned char*) FileName.c_str (),
                                          FileName.size ());
        EncodedFileName += "?=";
        m_strFileName = EncodedFileName;
    }

    //设置文件的contenttype
    void SetContentType (std::string const &stype)
    {
        int type = GetFileType (stype);
        switch (type)
        {            //
            case 0:
                m_strContentType = "plain/text;";
                break;

            case 1:
                m_strContentType = "text/xml;";
                break;

            case 2:
                m_strContentType = "text/html;";

            case 3:
                m_strContentType = "image/jpeg;";
                break;

            case 4:
                m_strContentType = "image/png;";
                break;

            case 5:
                m_strContentType = "image/gif;";
                break;

            case 6:
                m_strContentType = "application/x-msdownload;";
                break;

            default:
                m_strContentType = "application/octet-stream;";
                break;
        }
    }
    //得到文件名
    void GetFileName (const std::string &file, std::string &filename)
    {

        std::string::size_type p = file.find_last_of ('/');
        if (p == std::string::npos)
            p = file.find_last_of ('\\');
        if (p != std::string::npos)
        {
            p += 1; // get past folder delimeter
            filename = file.substr (p, file.length () - p);
        }
    }

    //得到文件类型
    void GetFileType (const std::string &file, std::string &stype)
    {
        std::string::size_type p = file.find_last_of ('.');
        if (p != std::string::npos)
        {
            p += 1; // get past folder delimeter
            stype = file.substr (p, file.length () - p);
        }
    }

private:
    std::string m_strCharset; //邮件编码
    std::string m_strSubject; //邮件主题
    std::string m_strContent; //邮件内容
    std::string m_strFileName; //文件名
    std::stringstream m_strMessage; // 整个MIME协议字符串
    std::string m_strUserName; //用户名
    std::string m_strPassword; //密码
    std::string m_strServerName; //smtp服务器
    std::string m_strPort; //端口
    std::string m_strSendName; //发送者姓名
    std::string m_strSendMail; //发送者邮箱
    std::string m_strContentType; //附件contenttype
    std::string m_strFileContent; //附件内容
    std::vector<std::string> m_vRecvMail; //收件人容器
    std::vector<std::string> m_vAttachMent; //附件容器
};

#endif /* INCLUDE_EMAILSENDER_HPP_ */
