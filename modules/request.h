#pragma once
#include<head_types.h>
#include<http_body.h>
#include<unordered_map>
#include<unordered_set>
#include<ctime>
#include<sstream>
#include<regex>


// for debug--------------
#include<iostream>
#define DEBUG(str) std::cout<<str<<std::endl
// -----------------------

class ExpertHttpRequest
{
private:
    using String        = std::string;
    using BodyPointer   = std::shared_ptr<HttpBody>;
    using SSMap         = std::unordered_map<std::string, std::string>;
    using SSet          = std::unordered_set<std::string>;
    using RangeSet      = std::unordered_set<Range, RangeHashFunction, RangeEqualFunction>;
    using WSet          = std::unordered_set<WeightItem, WeightItemHashFunction, WeightItemEqualFunction>;
    using SPair         = std::pair<std::string, std::string>;
    using IPair         = std::pair<int, int>;
// Head items
    WSet        accept_;                // "<type>/<subtype>;q=<weight>"
    WSet        acceptCharset_;         // "<charset>;q=<weight>"
    WSet        acceptEncoding_;        // "<encoding>;q=<weight>"
    WSet        acceptLanguage_;        // "<language>;q=<weight>"
    bool        acceptRanges_ = false;  // bytes = true; none = false
    SSet        allow_;
    SPair       authorization_;         // <type> <credentials>
    SPair       cacheControl_;
    String      connection_;
    SSet        contentEncoding_;
    SSet        contentLanguage_;
    int         contentLength_ = -1;
    ContentType contentType_;
    SSMap       cookie_;
    tm          date_ = { -1, -1, -1, -1, -1, -1, -1, -1, 0 };
    String      eTag_;
    tm          expires_ = { -1, -1, -1, -1, -1, -1, -1, -1, 0 };
    SPair       host_;
    URange      range_;
    String      referer_;
    String      userAgent_;
    String      userAgentForFirefox_;

// Request items
    Method  method_     = GET;
    Version version_    = HTTP_1_1;
    String  url_;
    SSMap   query_;

// Body
    BodyPointer body_;
public:
    ExpertHttpRequest() = default;
    ExpertHttpRequest(const ExpertHttpRequest&) = default;
    ExpertHttpRequest(ExpertHttpRequest&& other);
    ExpertHttpRequest& operator=(const ExpertHttpRequest&) = default;
// Virtual function
    BodyPointer getBody()       const { return body_; }
    Method      getMethod()     const { return method_; }
    Version     getVersion()    const { return version_; }
    String      getUrl()        const { return url_;}
    SSMap       getQuery()      const { return query_; }
    String      getHead()       const;

    void        setBody     (BodyPointer body)  { body_ = body; }
    void        setMethod   (Method method)     { method_ = method; }
    void        setUrl      (String url)        { url_ = std::move(url); }
    void        setQuery    (SSMap query)       { query_ = std::move(query); }
    void        setVersion  (Version version)   { version_ = version; }
    void        setHead     (String head);

    bool        available()  const { return !url_.empty(); }
    bool        operator==(const ExpertHttpRequest* request);

// Strengthen function
    /**
     * @param request A http request string
     * @return An ExpertHttpRequest instance
    */
    static ExpertHttpRequest parse(String request);
    /**
     * @param accept { {"text/html",double}, {"application/xml",double}, ... }
    */
    void setAccept              (const WSet accept)                     { accept_ = std::move(accept); }
    /**
     * @param charset { {"utf-8",double}, {"iso-8859-1",double}, ... }
    */
    void setAcceptCharset       (const WSet charset)                    { acceptCharset_ = std::move(charset); }
    /**
     * @param encoding { {"deflate",double}, {"gzip",double}, ... }
    */
    void setAceptEncoding       (const WSet encoding)                   { acceptEncoding_ = std::move(encoding); }
    /**
     * @param language { {"fr-CH",double}, {"fr",double}, ... }
    */
    void setAcceptLanguage      (const WSet language)                   { acceptLanguage_ = std::move(language); }
    /**
     * @param accept true | false
    */
    void setAcceptRanges        (bool accept)                           { acceptRanges_ = accept; }
    /**
     * @param allow { "GET", "POST", ... }
    */
    void setAllow               (const SSet allow)                      { allow_ = std::move(allow); }
    /**
     * @param authorization { {"Basic", "YWxhZGRpbjpvcGVuc2VzYW1l"} }
    */
    void setAuthorization       (const SPair authorization)             { authorization_ = authorization; }
    /**
     * @param type string: "Basic"
     * @param credentials string: "YWxhZGRpbjpvcGVuc2VzYW1l"
    */
    void setAuthorization       (String type, String credentials)       { authorization_ = SPair(type, credentials); }
    /**
     * @param cachecontrol {"max-age", "20"}
    */
    void setCacheControl        (const SPair cachecontrol)              { cacheControl_ = cachecontrol; }
    /**
     * @param model string: "max-age"
     * @param value string: "20"(s)
    */
    void setCacheControl        (String model, String value)            { cacheControl_ = SPair(model, value); }
    /**
     * @param connection string: "keep-alive"
    */
    void setConnection          (String connection)                     { connection_ = std::move(connection); }
    /**
     * @param encoding { "gzip", "identity", ... }
    */
    void setContentEncoding     (const SSet encoding)                   { contentEncoding_ = encoding; }
    /**
     * @param language { "de-DE", "en-US", ... }
    */
    void setContentLanguage     (const SSet language)                   { contentLanguage_ = language; }
    /**
     * @param length int
    */
    void setContentLength       (int length)                            { contentLength_ = length; }
    /**
     * @param cookies { {"key", "value"}, {"key", "value"}, ... }
    */
    void setCookie              (const SSMap cookies)                   { cookie_ = cookies; }
    /**
     * @param date string: "Tue, 02 Mar 2021 13:31:09 GMT"
    */
    void setDate                (tm date)                               { date_ = std::move(date); }
    /**
     * @param etag 
    */
    void setETag                (String etag)                           { eTag_ = std::move(etag); }
    /**
     * @param expires string: "Tue, 02 Mar 2021 13:31:09 GMT"
    */
    void setExpires             (tm expires)                            { expires_ = std::move(expires); }
    /**
     * @param host { "127.0.0.1", "12219" }
    */
    void setHost                (const SPair host)                      { host_ = host; }
    /**
     * @param range { "bytes", { {int, int}, {int, int}, ... } }
    */
    void setRange               (const URange range)                    { range_ = range; }
    /**
     * @param unit string: "bytes" | "none"
     * @param ranges { {int, int}, {int, int}, ... }
    */
    void setRange               (String unit, RangeSet ranges)          { range_ = { unit, ranges }; }
    /**
     * @param referer string: "http://www.non-page.none"
    */
    void setReferer             (String referer)                        { referer_ = std::move(referer); }
    /**
     * @param useragent string
    */
    void setUserAgent           (String useragent)                      { userAgent_ = std::move(useragent); }
    /**
     * @param useragent string
    */
    void setUserAgentForFirefox (String useragent)                      { userAgentForFirefox_ = std::move(useragent); }
    /**
     * @param contenttype { "text/html", {"charset", "utf-8"} }
    */
    void setContentType         (ContentType contenttype)               { contentType_ = contenttype; }
    /**
     * @param type string: text/html
     * @param property { "charset", "utf-8" }
    */
    void setContentType         (String type, const SPair property)     { contentType_ = { type, property }; }
    /**
     * @param type string: text/html
     * @param key string: charset
     * @param value string: utf-8
    */
    void setContentType         (String type, String key, String value) { contentType_ = { type, { key, value } }; }

    WSet        getAccept()                 const { return accept_; }
    WSet        getAcceptCharset()          const { return acceptCharset_; }
    WSet        getAceptEncoding()          const { return acceptEncoding_; }
    WSet        getAcceptLanguage()         const { return acceptLanguage_; }
    bool        getAcceptRanges()           const { return acceptRanges_; }
    SSet        getAllow()                  const { return allow_; }
    SPair       getAuthorization()          const { return authorization_; }
    SPair       getCacheControl()           const { return cacheControl_; }
    String      getConnection()             const { return connection_; }
    SSet        getContentEncoding()        const { return contentEncoding_; }
    SSet        getContentLanguage()        const { return contentLanguage_; }
    int         getContentLength()          const { return contentLength_; }
    SSMap       getCookie()                 const { return cookie_; }
    tm          getDate()                   const { return date_; }
    String      getETag()                   const { return eTag_; }
    tm          getExpires()                const { return expires_; }
    SPair       getHost()                   const { return host_; }
    URange      getRange()                  const { return range_; }
    String      getReferer()                const { return referer_; }
    String      getUserAgent()              const { return userAgent_; }
    String      getUserAgentForFirefox()    const { return userAgentForFirefox_; }
    ContentType getContentType()            const { return contentType_; }

};

