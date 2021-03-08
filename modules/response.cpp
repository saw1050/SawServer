#include<response.h>

ExpertHttpResponse::ExpertHttpResponse(ExpertHttpResponse&& other)
{
    accept_             =   std::move(other.accept_           );
    acceptCharset_      =   std::move(other.acceptCharset_    );
    acceptEncoding_     =   std::move(other.acceptEncoding_   );
    acceptLanguage_     =   std::move(other.acceptLanguage_   );
    acceptRanges_       =   std::move(other.acceptRanges_     );
    allow_              =   std::move(other.allow_            );
    authorization_      =   std::move(other.authorization_    );
    cacheControl_       =   std::move(other.cacheControl_     );
    connection_         =   std::move(other.connection_       );
    contentEncoding_    =   std::move(other.contentEncoding_  );
    contentLanguage_    =   std::move(other.contentLanguage_  );
    contentLength_      =   std::move(other.contentLength_    );
    contentRange_       =   std::move(other.contentRange_     );
    contentType_        =   std::move(other.contentType_      );
    date_               =   std::move(other.date_             );
    eTag_               =   std::move(other.eTag_             );
    expires_            =   std::move(other.expires_          );
    referer_            =   std::move(other.referer_          );
    server_             =   std::move(other.server_           );
    setCookie_          =   std::move(other.setCookie_        );
    version_            =   std::move(other.version_          );
    status_             =   std::move(other.status_           );
    body_               =   std::move(other.body_             );
}

ExpertHttpResponse ExpertHttpResponse::parse(String response)
{
    ExpertHttpResponse instance;
    std::regex linecutter("\n");
    std::sregex_token_iterator taker(response.begin(), response.end(), linecutter, -1);
    std::sregex_token_iterator end;
    
    // 解析响应行
    {
        int stage = 1;
        String version;
        String status;
        for(auto word : taker->str())
        {
            if(word == ' ' || word == '\r')
            {
                if((stage++) > 1)
                {
                    transform(version.begin(), version.end(), version.begin(), ::toupper);
                    if(version == "HTTP/0.9")
                        instance.setVersion(HTTP_0_9);
                    else if(version == "HTTP/1.0")
                        instance.setVersion(HTTP_1_0);
                    else if(version == "HTTP/1.1")
                        instance.setVersion(HTTP_1_1);
                    else if(version == "HTTP/2")
                        instance.setVersion(HTTP_2_0);
                    else if(version == "HTTP/2.0")
                        instance.setVersion(HTTP_2_0);
                    else
                        return ExpertHttpResponse();
                    int s = atoi(status.c_str());
                    if(s >= 100)
                        instance.setStatus(s);
                    else
                        return ExpertHttpResponse();
                    break;
                }
                continue;
            }
            switch (stage)
            {
            case 1:
                version += word;
                if(version.size() > 9)
                    return ExpertHttpResponse();
                break;
            case 2:
                status += word;
                if(status.size() > 3)
                    return ExpertHttpResponse();
                break;
            case 3:
                version += word;
                if(version.size() > 9)
                    return ExpertHttpResponse();
                break;
            
            default:
                break;
            }
        }
        if(stage < 2)
            return ExpertHttpResponse();
    }

    // 解析头部
    SSMMap headpairs;
    {
        bool finish = false;
        while(++taker != end && !finish)
        {
            String headline = std::move(taker->str());
            String key;
            String value;
            if(headline.front() == '\r')
            {
                finish = true;
                continue;
            }
            int stage = 1;
            bool firstwordforvalue = true;
            for(auto word : headline)
            {
                if(stage == 1)  // 解析 Key
                {
                    if(word != ':')
                    {
                        key += word;
                        if(key.size() > 40)
                            return ExpertHttpResponse();
                    }
                    else if(word == '\r')
                    {
                        finish = true;
                        break;
                    }
                    else
                    {
                        stage++;
                    }
                }
                else if(stage == 2)     // 解析 Value
                {
                    if(word == '\r')
                        break;
                    if(firstwordforvalue && word == ' ')
                    {
                        continue;
                    }
                    else
                    {
                        firstwordforvalue = false;
                    }
                    value  += word;
                }
            }
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            headpairs.emplace(key, value);
        }
    }

    // 转化
    {
        // Accept
        auto iterator = headpairs.find("accept");
        if(iterator != headpairs.end())
        {
            instance.setAccept(stringToWSet(iterator->second));
        }

        // Accept-Charset
        iterator = headpairs.find("accept-charset");
        if(iterator != headpairs.end())
        {
            instance.setAcceptCharset(stringToWSet(iterator->second));
        }

        // Accept-Encoding
        iterator = headpairs.find("accept-encoding");
        if(iterator != headpairs.end())
        {
            instance.setAcceptCharset(stringToWSet(iterator->second));
        }

        // Accept-Language
        iterator = headpairs.find("accept-language");
        if(iterator != headpairs.end())
        {
            instance.setAcceptCharset(stringToWSet(iterator->second));
        }

        // Accept-Ranges
        iterator = headpairs.find("accept-ranges");
        if(iterator != headpairs.end())
        {
            if(iterator->second == "none")
                instance.setAcceptRanges(false);
            else
                instance.setAcceptRanges(true);
        }

        // Allow
        iterator = headpairs.find("allow");
        if(iterator != headpairs.end())
        {
            instance.setAllow(stringToSSet(iterator->second));
        }

        // Authorization
        iterator = headpairs.find("authorization");
        if(iterator != headpairs.end())
        {
            instance.setAuthorization(stringToSPair(iterator->second, ' '));
        }

        // Cache-Control
        iterator = headpairs.find("cache-control");
        if(iterator != headpairs.end())
        {
            instance.setCacheControl(stringToSPair(iterator->second, '='));
        }

        // Connection
        iterator = headpairs.find("connection");
        if(iterator != headpairs.end())
        {
            instance.setConnection(std::move(iterator->second));
        }

        // Content-Encoding
        iterator = headpairs.find("content-encoding");
        if(iterator != headpairs.end())
        {
            instance.setContentEncoding(stringToSSet(iterator->second));
        }

        // Content-Language
        iterator = headpairs.find("content-language");
        if(iterator != headpairs.end())
        {
            instance.setContentLanguage(stringToSSet(iterator->second));
        }

        // Content-Length
        iterator = headpairs.find("content-length");
        if(iterator != headpairs.end())
        {
            instance.setContentLength(atoi(iterator->second.c_str()));
        }

        // Content-Range
        iterator = headpairs.find("content-range");
        if(iterator != headpairs.end())
        {
            instance.setContentRange(stringToCRange(iterator->second));
        }

        // Content-Type
        iterator = headpairs.find("content-type");
        if(iterator != headpairs.end())
        {
            instance.setContentType(stringToContentType(iterator->second));
        }

        // Date
        iterator = headpairs.find("date");
        if(iterator != headpairs.end())
        {
            instance.setDate(httpStringToTime(iterator->second));
        }

        // ETag
        iterator = headpairs.find("etag");
        if(iterator != headpairs.end())
        {
            instance.setETag(iterator->second);
        }

        // Expires
        iterator = headpairs.find("expires");
        if(iterator != headpairs.end())
        {
            instance.setExpires(httpStringToTime(iterator->second));
        }

        // Referer
        iterator = headpairs.find("referer");
        if(iterator != headpairs.end())
        {
            instance.setReferer(iterator->second);
        }

        // Server
        iterator = headpairs.find("server");
        if(iterator != headpairs.end())
        {
            instance.setServer(iterator->second);
        }

        // Set-Cookie
        iterator = headpairs.find("set-cookie");
        while(iterator != headpairs.end())
        {
            if(iterator->first == "set-cookie")
                instance.setSetCookie(stringToSSMap(iterator->second, ';'));
            iterator++;
        }
    }
    return instance;
}

ExpertHttpResponse::String ExpertHttpResponse::getHead() const
{
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss.precision(1);
    if(version_ == HTTP_1_1)
    {
        oss << "HTTP/1.1 ";
    }
    else if(version_ == HTTP_2_0)
    {
        oss << "HTTP/2 ";
    }
    else if(version_ == HTTP_1_0)
    {
        oss << "HTTP/1.0 ";
    }
    else
    {
        oss << "HTTP/0.9 ";
    }
    // oss << status_ << " ";
    switch (status_)
    {
    case 100:
        oss << "100 Continue\r\n";
        break;
    case 101:
        oss << "101 Switching Protocols\r\n";
        break;
    case 200:
        oss << "200 OK\r\n";
        break;
    case 201:
        oss << "201 Created\r\n";
        break;
    case 202:
        oss << "202 Accepted\r\n";
        break;
    case 203:
        oss << "203 Non Authoritative Information\r\n";
        break;
    case 204:
        oss << "204 No Content\r\n";
        break;
    case 205:
        oss << "205 Reset Content\r\n";
        break;
    case 206:
        oss << "206 Partial Content\r\n";
        break;
    case 300:
        oss << "300 Multiple Choices\r\n";
        break;
    case 301:
        oss << "301 Moved Permanently\r\n";
        break;
    case 302:
        oss << "302 Found\r\n";
        break;
    case 303:
        oss << "303 See Other\r\n";
        break;
    case 304:
        oss << "304 Not Modified\r\n";
        break;
    case 305:
        oss << "305 Use Proxy\r\n";
        break;
    case 306:
        oss << "306 Unused\r\n";
        break;
    case 307:
        oss << "307 Temporary Redirect\r\n";
        break;
    case 400:
        oss << "400 Bad Request\r\n";
        break;
    case 401:
        oss << "401 Unauthorized\r\n";
        break;
    case 402:
        oss << "402 Payment Required\r\n";
        break;
    case 403:
        oss << "403 Forbidden\r\n";
        break;
    case 404:
        oss << "404 Not Found\r\n";
        break;
    case 405:
        oss << "405 Method Not Allowed\r\n";
        break;
    case 406:
        oss << "406 Not Acceptable\r\n";
        break;
    case 407:
        oss << "407 Proxy Authentication Required\r\n";
        break;
    case 408:
        oss << "408 Request Time-out\r\n";
        break;
    case 409:
        oss << "409 Conflict\r\n";
        break;
    case 410:
        oss << "410 Gone\r\n";
        break;
    case 411:
        oss << "411 Length Required\r\n";
        break;
    case 412:
        oss << "412 Precondition Failed\r\n";
        break;
    case 413:
        oss << "413 Request Entity Too Large\r\n";
        break;
    case 414:
        oss << "414 Request-URI Too Large\r\n";
        break;
    case 415:
        oss << "415 Unsupported Media Type\r\n";
        break;
    case 416:
        oss << "416 Requested range not satisfiable\r\n";
        break;
    case 417:
        oss << "417 Expectation Failed\r\n";
        break;
    case 500:
        oss << "500 Internal Server Error\r\n";
        break;
    case 501:
        oss << "501 Not Implemented\r\n";
        break;
    case 502:
        oss << "502 Bad Gateway\r\n";
        break;
    case 503:
        oss << "503 Service Unavailable\r\n";
        break;
    case 504:
        oss << "504 Gateway Time-out\r\n";
        break;
    case 505:
        oss << "505 HTTP Version not supported\r\n";
        break;
    default:
        return String();
        break;
    }

    if(!accept_.empty())
    {
        oss << "Accept: ";
        bool isFirst = true;
        for(auto weightitem : accept_)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << ", ";
            }
            oss << weightitem.value;
            if(weightitem.weight - 0.95 < 0)
            {
                oss << ";q=" << weightitem.weight;
            }
        }
        oss << "\r\n";
    }
    if(!acceptCharset_.empty())
    {
        oss << "Accept-Charset: ";
        bool isFirst = true;
        for(auto weightitem : acceptCharset_)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << ", ";
            }
            oss << weightitem.value;
            if(weightitem.weight - 0.95 < 0)
            {
                oss << ";q=" << weightitem.weight;
            }
        }
        oss << "\r\n";
    }
    if(!acceptEncoding_.empty())
    {
        oss << "Accept-Encoding: ";
        bool isFirst = true;
        for(auto weightitem : acceptEncoding_)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << ", ";
            }
            oss << weightitem.value;
            if(weightitem.weight - 0.95 < 0)
            {
                oss << ";q=" << weightitem.weight;
            }
        }
        oss << "\r\n";
    }
    if(!acceptLanguage_.empty())
    {
        oss << "Accept-Language: ";
        bool isFirst = true;
        for(auto weightitem : acceptLanguage_)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << ", ";
            }
            oss << weightitem.value;
            if(weightitem.weight - 0.95 < 0)
            {
                oss << ";q=" << weightitem.weight;
            }
        }
        oss << "\r\n";
    }
    if(acceptRanges_)
    {
        oss << "Accept-Ranges: bytes" << "\r\n";
    }
    if(!allow_.empty())
    {
        oss << "Allow: ";
        bool isFirst = true;
        for(auto value : allow_)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << ", ";
            }
            oss << value;
        }
        oss << "\r\n";
    }
    if(!authorization_.first.empty() && !authorization_.second.empty())
    {
        oss << "Authorization: " << authorization_.first << " " << authorization_.second << "\r\n";
    }
    if(!cacheControl_.first.empty())
    {
        oss << "Cache-Control: " << cacheControl_.first;
        if(!cacheControl_.second.empty())
        {
            oss << "=" << cacheControl_.second;
        }
        oss << "\r\n";
    }
    if(!connection_.empty())
    {
        oss << "Connection: " << connection_ << "\r\n";
    }
    if(!contentEncoding_.empty())
    {
        oss << "Content-Encoding: ";
        bool isFirst = true;
        for(auto value : contentEncoding_)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << ", ";
            }
            oss << value;
        }
        oss << "\r\n";
    }
    if(!contentLanguage_.empty())
    {
        oss << "Content-Language: ";
        bool isFirst = true;
        for(auto value : contentLanguage_)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << ", ";
            }
            oss << value;
        }
        oss << "\r\n";
    }
    if(contentLength_ >= 0)
    {
        oss << "Content-Length: " << contentLength_ << "\r\n";
    }
    if(!contentRange_.unit.empty())
    {
        oss << "Content-Range: "<< contentRange_.unit << " ";
        if(contentRange_.range.start >= 0 && contentRange_.range.end >= 0)
        {
            oss << contentRange_.range.start << "-" << contentRange_.range.end << "/";
        }
        else
        {
            oss << "*/";
        }
        if(contentRange_.length >= 0)
        {
            oss << contentRange_.length;
        }
        else
        {
            oss << "*";
        }
        oss << "\r\n";
    }
    if(!contentType_.type.empty())
    {
        oss << "Content-Type: "<< contentType_.type;
        if(!contentType_.property.first.empty() && !contentType_.property.second.empty())
        {
            oss << "; " << contentType_.property.first << "=" << contentType_.property.second;
        }
        oss << "\r\n";
    }

    if( date_.tm_wday != -1  && date_.tm_mday != -1   &&
        date_.tm_mon != -1   && date_.tm_year != -1   &&
        date_.tm_hour != -1  && date_.tm_min != -1    &&
        date_.tm_sec != -1)
    {
        oss << "Date: " << timeToHttpString(date_) << "\r\n";
    }
    if(!eTag_.empty())
    {
        oss << "ETag: " << eTag_ << "\r\n";
    }
    if( expires_.tm_wday != -1  && expires_.tm_mday != -1   &&
        expires_.tm_mon != -1   && expires_.tm_year != -1   &&
        expires_.tm_hour != -1  && expires_.tm_min != -1    &&
        expires_.tm_sec != -1)
    {
        oss << "Expires: " << timeToHttpString(expires_) << "\r\n";
    }
    if(!referer_.empty())
    {
        oss << "Referer: " << referer_ << "\r\n";
    }
    if(!server_.empty())
    {
        oss << "Server: " << server_ << "\r\n";
    }
    for(auto cookies : setCookie_)
    {
        oss << "Set-Cookie: ";
        bool isFirst = true;
        for(auto cookie : cookies)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << "; ";
            }
            oss << cookie.first << "=" << cookie.second;
        }
        oss << "\r\n";
    }
    for(auto pair : otherItems_)
    {
        oss << pair.first << ": " << pair.second << "\r\n";
    }
    oss << "\r\n";
    return oss.str();
}

void ExpertHttpResponse::setHead(ExpertHttpResponse::String head)
{
    *this = std::move(parse(head));
}

bool ExpertHttpResponse::operator==(const ExpertHttpResponse* response)
{
    ExpertHttpResponse* eresponse = (ExpertHttpResponse*)response;
    return  accept_             ==  eresponse->accept_          &&
            acceptCharset_      ==  eresponse->acceptCharset_   &&
            acceptEncoding_     ==  eresponse->acceptEncoding_  &&
            acceptLanguage_     ==  eresponse->acceptLanguage_  &&
            acceptRanges_       ==  eresponse->acceptRanges_    &&
            allow_              ==  eresponse->allow_           &&
            authorization_      ==  eresponse->authorization_   &&
            cacheControl_       ==  eresponse->cacheControl_    &&
            connection_         ==  eresponse->connection_      &&
            contentEncoding_    ==  eresponse->contentEncoding_ &&
            contentLanguage_    ==  eresponse->contentLanguage_ &&
            contentLength_      ==  eresponse->contentLength_   &&
            contentRange_       ==  eresponse->contentRange_    &&
            contentType_        ==  eresponse->contentType_     &&
            eTag_               ==  eresponse->eTag_            &&
            referer_            ==  eresponse->referer_         &&
            server_             ==  eresponse->server_          &&
            setCookie_          ==  eresponse->setCookie_       &&
            version_            ==  eresponse->version_         &&
            status_             ==  eresponse->status_          &&
            body_               ==  eresponse->body_;
}
