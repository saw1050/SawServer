#pragma once
#include<head_types.h>
#include<http_body.h>
#include<unordered_map>
#include<unordered_set>
#include<list>
#include<ctime>
#include<sstream>

#include<iostream>

class ExpertHttpResponse
{
private:
    using String        = std::string;
    using BodyPointer   = std::shared_ptr<HttpBody>;
    using SSMap         = std::unordered_map<std::string, std::string>;
    using SSMMap        = std::unordered_multimap<std::string, std::string>;
    using SSMapList     = std::list<std::unordered_map<std::string, std::string>>;
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
    CRange      contentRange_;
    ContentType contentType_;
    tm          date_ = { -1, -1, -1, -1, -1, -1, -1, -1, 0 };
    String      eTag_;
    tm          expires_ = { -1, -1, -1, -1, -1, -1, -1, -1, 0 };
    String      referer_;
    String      server_;
    SSMapList   setCookie_;
    SSMMap      otherItems_;

// Response items
    Version version_    = HTTP_1_1;
    int     status_     = -1;

// Body
    BodyPointer body_;
public:
    ExpertHttpResponse(int status = 400, Version version = HTTP_1_1) : status_(status), version_(version){}
    ExpertHttpResponse(const ExpertHttpResponse&) = default;
    ExpertHttpResponse(ExpertHttpResponse&& other);
    ExpertHttpResponse& operator=(const ExpertHttpResponse&) = default;
// Virtual function
    BodyPointer getBody()       const { return body_; }
    Version     getVersion()    const { return version_; }
    int         getStatus()     const { return status_; }
    String      getHead()       const;

    void        setBody     (BodyPointer body)                  { body_ = body; }
    void        setVersion  (Version version)                   { version_ = version; }
    void        setStatus   (int status)                        { status_ = status; }
    void        setHead     (String head);
    void        addHead     (std::string key, std::string value){ otherItems_.emplace(key, value); }
    void        delHead     (std::string key)                   { otherItems_.erase(key); }

    bool        available() const { return status_ >= 100; }
    bool        operator==(const ExpertHttpResponse* response);

// Strengthen function
    /**
     * @param response A http response string
     * @return An ExpertHttpResponse instance
    */
    static ExpertHttpResponse parse(String response);
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
    void setContentLength       (int length)                            { contentLength_ = length; contentRange_.length = length; }
    /**
     * @param range { "bytes", {int, int}, int }
    */
    void setContentRange        (const CRange range)                    { contentRange_ = range; }
    /**
     * @param unit string: "bytes"
     * @param range { int, int }
     * @param length int: the body size
    */
    void setContentRange        (String unit, Range range, int length)  { contentRange_ = { unit, range, length }; }
    /**
     * @param unit string: "bytes"
     * @param start int: range start position
     * @param end int: range end position
     * @param length int: data's all bytes count
    */
    void setContentRange (String unit, int start, int end, int length)  { contentRange_ = { unit, { start, end }, length }; }
    /**
     * @param unit string: "bytes"
     * @param start int: range start position
     * @param end int: range end position
     * @param length int: data's all bytes count
    */
    void setContentRange (int start, int end)                           { contentRange_.range.start = start; contentRange_.range.end = end; }
    /**
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
    /**
     * @param type string: text/html
    */
    void setContentType         (String type)                           { contentType_ = { type, { "", "" } }; }
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
     * @param referer string: "http://www.non-page.none"
    */
    void setReferer             (String referer)                        { referer_ = std::move(referer); }
    /**
     * @param server string: "saw"
    */
    void setServer              (String server)                         { server_ = std::move(server); }
    /**
     * @param cookies { {"key", "value"}, {"key", "value"}, ... }
    */
    void setSetCookie           (const SSMap cookies)                   { setCookie_.push_back(cookies); }

    WSet        getAccept()             const { return accept_; }
    WSet        getAcceptCharset()      const { return acceptCharset_; }
    WSet        getAceptEncoding()      const { return acceptEncoding_; }
    WSet        getAcceptLanguage()     const { return acceptLanguage_; }
    bool        getAcceptRanges()       const { return acceptRanges_; }
    SSet        getAllow()              const { return allow_; }
    SPair       getAuthorization()      const { return authorization_; }
    SPair       getCacheControl()       const { return cacheControl_; }
    String      getConnection()         const { return connection_; }
    SSet        getContentEncoding()    const { return contentEncoding_; }
    SSet        getContentLanguage()    const { return contentLanguage_; }
    int         getContentLength()      const { return contentLength_; }
    CRange      getContentRange()       const { return contentRange_; }
    ContentType getContentType()        const { return contentType_; }
    tm          getDate()               const { return date_; }
    String      getETag()               const { return eTag_; }
    tm          getExpires()            const { return expires_; }
    String      getReferer()            const { return referer_; }
    String      getServer()             const { return server_; }
    SSMapList   getSetCookie()          const { return setCookie_; }
};




