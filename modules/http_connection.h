#pragma once

#include<boost/asio.hpp>
#include<memory>
#include<http_buffer.h>
#include<request.h>
#include<request_handler.h>

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
private:
    using IOService = boost::asio::io_service;
    using Socket    = boost::asio::ip::tcp::socket;
    using ErrorCode = boost::system::error_code;
    using Buffer    = std::shared_ptr<char[]>;
    using Request   = ExpertHttpRequest;
    using Response  = ExpertHttpResponse;
    using Responses = std::queue<ExpertHttpResponse>;
    using Handler   = std::shared_ptr<RequestHandler>;

    // using Endpoint = boost::asio::ip::tcp::endpoint;
    const size_t buffer_size_;
    HttpBuffer   http_buffer_;

    Socket    socket_;
    Buffer    buffer_;
    Handler   handler_;
    Request   request_;
    Responses responses_;
    BodyModel model_;

public:
    HttpConnection(IOService& service, Handler handler, size_t buffer_size = 20480, size_t http_buffer_size = 8192);
    ~HttpConnection();

    Socket& socket();
    void    start();

private:
    size_t get_request_length(size_t buffer_bytes);
    void   run_after_read(const ErrorCode& e, size_t bytes_transferred);
    void   reply_file(Response& response);
    void   reply_plain(Response& response);
};


