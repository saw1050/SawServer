#pragma once

#include<http_connection.h>
#include<threadpool.h>
#include<request_handler.h>

class HttpServer
{
private:
    using IOService   = boost::asio::io_service;
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using Endpoint    = boost::asio::ip::tcp::endpoint;
    using ConnectPtr  = std::shared_ptr<HttpConnection>;
    using Handler     = std::shared_ptr<RequestHandler>;

    IOService   io_service_;
    TcpAcceptor serversocket_;
    Endpoint    endpoint_;
    ConnectPtr  connection_;
    ThreadPool  threadpool_;
    Handler     handler_;
public:
    HttpServer(int port, Handler handler);

private:
    void start_accept();
    void run_after_accept();
};

