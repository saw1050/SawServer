#include<request.h>

ExpertHttpRequest::ExpertHttpRequest(ExpertHttpRequest&& other)
{
    accept_                 =  std::move(other.accept_             );
    acceptCharset_          =  std::move(other.acceptCharset_      );
    acceptEncoding_         =  std::move(other.acceptEncoding_     );
    acceptLanguage_         =  std::move(other.acceptLanguage_     );
    acceptRanges_           =  std::move(other.acceptRanges_       );
    allow_                  =  std::move(other.allow_              );
    authorization_          =  std::move(other.authorization_      );
    cacheControl_           =  std::move(other.cacheControl_       );
    connection_             =  std::move(other.connection_         );
    contentEncoding_        =  std::move(other.contentEncoding_    );
    contentLanguage_        =  std::move(other.contentLanguage_    );
    contentLength_          =  std::move(other.contentLength_      );
    cookie_                 =  std::move(other.cookie_             );
    date_                   =  std::move(other.date_               );
    eTag_                   =  std::move(other.eTag_               );
    expires_                =  std::move(other.expires_            );
    host_                   =  std::move(other.host_               );
    range_                  =  std::move(other.range_              );
    referer_                =  std::move(other.referer_            );
    userAgent_              =  std::move(other.userAgent_          );
    userAgentForFirefox_    =  std::move(other.userAgentForFirefox_);
    contentType_            =  std::move(other.contentType_        );
    method_                 =  std::move(other.method_             );
    version_                =  std::move(other.version_            );
    url_                    =  std::move(other.url_                );
    query_                  =  std::move(other.query_              );
    body_                   =  std::move(other.body_               );
}

ExpertHttpRequest ExpertHttpRequest::parse(String request)
{
    ExpertHttpRequest instance;
    std::regex linecutter("\n");
    std::sregex_token_iterator taker(request.begin(), request.end(), linecutter, -1);
    std::sregex_token_iterator end;
    
    // 解析请求行
    {
        int stage = 0;
        String method;
        String url;
        String version;
        String query;
        for(auto word : taker->str())
        {
            switch (stage)
            {
            case 0:
                if(isalpha(word))
                {
                    method += word;
                    stage = 1;
                }
                else if(word != ' ')
                {
                    return ExpertHttpRequest();
                }
                break;
            case 1:
                if(word == ' ')
                {
                    stage = 2;
                }
                else if(!isalpha(word))
                {
                    return ExpertHttpRequest();
                }
                else
                {
                    method += word;
                }
                break;
            case 2:
                if(word == '/')
                {
                    url += word;
                    stage = 3;
                }
                else
                {
                    return ExpertHttpRequest();
                }
                break;
            case 3:
                if(word == ' ')
                {
                    stage = 5;
                }
                else if(word == '?')
                {
                    stage = 4;
                }
                else if(word == '\r')
                {
                    return ExpertHttpRequest();
                }
                else
                {
                    url += word;
                }
                break;
            case 4:
                if(word == ' ')
                {
                    stage = 5;
                }
                else if(word == '?' || word == '\r')
                {
                    return ExpertHttpRequest();
                }
                else
                {
                    query += word;
                }
                break;
            case 5:
                if(word == '\r')
                {
                    stage = 6;
                }
                else if(word == '?')
                {
                    return ExpertHttpRequest();
                }
                else
                {
                    version += word;
                }
                break;
            case 6:
                return ExpertHttpRequest();
                break;
            default:
                break;
            }
        }
        if(stage != 6)
            return ExpertHttpRequest();
        transform(method.begin(), method.end(), method.begin(), ::toupper);
        transform(version.begin(), version.end(), version.begin(), ::toupper);
        if(method == "GET")
            instance.setMethod(GET);
        else if(method == "POST")
            instance.setMethod(POST);
        else if(method == "PUT")
            instance.setMethod(PUT);
        else if(method == "DELETE")
            instance.setMethod(DEL);
        else if(method == "TRACE")
            instance.setMethod(TRACE);
        else if(method == "OPTIONS")
            instance.setMethod(OPTIONS);
        else if(method == "HEAD")
            instance.setMethod(HEAD);
        else if(method == "PATCH")
            instance.setMethod(PATCH);
        else if(method == "CONNECT")
            instance.setMethod(CONNECT);
        else
            return ExpertHttpRequest();

        instance.setUrl(url);
        if(!query.empty())
            instance.setQuery(stringToSSMap(query, '&'));
        std::cout<<"string to map:"<<instance.getQuery().size()<<std::endl;

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
            return ExpertHttpRequest();
            
    }

    // 解析头部
    SSMap headpairs;
    {
        bool finish = false;
        while(++taker != end && !finish)
        {
            String headline = std::move(taker->str());
            String key;
            String value;
            if(headline.front() == '\r')
                continue;
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
                            return ExpertHttpRequest();
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

        // Content-Type
        iterator = headpairs.find("content-type");
        if(iterator != headpairs.end())
        {
            instance.setContentType(stringToContentType(iterator->second));
        }

        // Cookie
        iterator = headpairs.find("cookie");
        if(iterator != headpairs.end())
        {
            instance.setCookie(stringToSSMap(iterator->second, ';'));
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

        // Host
        iterator = headpairs.find("host");
        if(iterator != headpairs.end())
        {
            instance.setHost(stringToSPair(iterator->second, ':'));
        }

        // Range
        iterator = headpairs.find("range");
        if(iterator != headpairs.end())
        {
            instance.setRange(stringToURange(iterator->second));
        }

        // Referer
        iterator = headpairs.find("referer");
        if(iterator != headpairs.end())
        {
            instance.setReferer(iterator->second);
        }

        // User-Agent
        iterator = headpairs.find("user-agent");
        if(iterator != headpairs.end())
        {
            instance.setUserAgent(iterator->second);
        }

        // User-Agent:Firefox
        iterator = headpairs.find("user-agent:firefox");
        if(iterator != headpairs.end())
        {
            instance.setUserAgentForFirefox(iterator->second);
        }
    }
    return instance;
}

ExpertHttpRequest::String ExpertHttpRequest::getHead() const
{
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss.precision(1);
    switch (method_)
    {
    case GET:
        oss << "GET ";
        break;
    case POST:
        oss << "POST ";
        break;
    case PUT:
        oss << "PUT ";
        break;
    case DEL:
        oss << "DELETE ";
        break;
    case TRACE:
        oss << "TRACE ";
        break;
    case OPTIONS:
        oss << "OPTIONS ";
        break;
    case HEAD:
        oss << "HEAD ";
        break;
    default:
        return String();
        break;
    }
    if(!url_.empty())
    {
        oss << url_;
    }
    else
    {
        return String();
    }
    if(!query_.empty())
    {
        oss << "?";
        for(auto iter = query_.begin(); iter != query_.end(); iter++)
        {
            if(iter != query_.begin())
                oss << "&";
            oss << iter->first << "=" << iter->second;
        }
    }
    oss << " ";
    if(version_ == HTTP_1_1)
    {
        oss << "HTTP/1.1\r\n";
    }
    else if(version_ == HTTP_2_0)
    {
        oss << "HTTP/2\r\n";
    }
    else if(version_ == HTTP_1_0)
    {
        oss << "HTTP/1.0\r\n";
    }
    else
    {
        oss << "HTTP/0.9\r\n";
    }
    if(!accept_.empty())
    {
        oss << "Accept: ";
        bool isFirst = true;
        for(auto weightitem : accept_)
        {
            if(!isFirst || (isFirst = !isFirst))        // I was intentional
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
    if(!contentType_.type.empty())
    {
        oss << "Content-Type: "<< contentType_.type;
        if(!contentType_.property.first.empty() && !contentType_.property.second.empty())
        {
            oss << "; " << contentType_.property.first << "=" << contentType_.property.second;
        }
        oss << "\r\n";
    }
    if(!cookie_.empty())
    {
        oss << "Cookie: ";
        bool isFirst = true;
        for(auto cookie : cookie_)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << "; ";
            }
            oss << cookie.first << "=" << cookie.second;
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
    if(!host_.first.empty())
    {
        oss << "Host: " << host_.first;
        if(!host_.second.empty())
        {
            oss << ":" << host_.second;
        }
        oss << "\r\n";
    }
    if(!range_.unit.empty() && !range_.ranges.empty())
    {
        oss << "Range: "<< range_.unit << "=";
        bool isFirst = true;
        for(auto range : range_.ranges)
        {
            if(!isFirst || (isFirst = !isFirst))
            {
                oss << ", ";
            }
            oss << range.start << "-";
            if(range.end >= 0)
            {
                oss << range.end;
            }
        }
        oss << "\r\n";
    }
    if(!referer_.empty())
    {
        oss << "Referer: " << referer_ << "\r\n";
    }
    if(!userAgent_.empty())
    {
        oss << "User-Agent: " << userAgent_ << "\r\n";
    }
    if(!userAgentForFirefox_.empty())
    {
        oss << "User-Agent:Firefox: " << userAgentForFirefox_ << "\r\n";
    }


    return oss.str();
}

void ExpertHttpRequest::setHead(ExpertHttpRequest::String head)
{
    *this = std::move(parse(head));
}

bool ExpertHttpRequest::operator==(const ExpertHttpRequest* request)
{
    ExpertHttpRequest* erequest = (ExpertHttpRequest*)request;
    return  accept_                 ==  erequest->accept_               &&
            acceptCharset_          ==  erequest->acceptCharset_        &&
            acceptEncoding_         ==  erequest->acceptEncoding_       &&
            acceptLanguage_         ==  erequest->acceptLanguage_       &&
            acceptRanges_           ==  erequest->acceptRanges_         &&
            allow_                  ==  erequest->allow_                &&
            authorization_          ==  erequest->authorization_        &&
            cacheControl_           ==  erequest->cacheControl_         &&
            connection_             ==  erequest->connection_           &&
            contentEncoding_        ==  erequest->contentEncoding_      &&
            contentLanguage_        ==  erequest->contentLanguage_      &&
            contentLength_          ==  erequest->contentLength_        &&
            cookie_                 ==  erequest->cookie_               &&
            // date_                   ==  erequest->date_                 &&
            eTag_                   ==  erequest->eTag_                 &&
            // expires_                ==  erequest->expires_              &&
            host_                   ==  erequest->host_                 &&
            range_                  ==  erequest->range_                &&
            referer_                ==  erequest->referer_              &&
            userAgent_              ==  erequest->userAgent_            &&
            userAgentForFirefox_    ==  erequest->userAgentForFirefox_  &&
            contentType_            ==  erequest->contentType_          &&
            method_                 ==  erequest->method_               &&
            version_                ==  erequest->version_              &&
            url_                    ==  erequest->url_                  &&
            body_                   ==  erequest->body_;
}