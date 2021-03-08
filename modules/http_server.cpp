#include<http_server.h>

#include<iostream>
using namespace std;

HttpServer::HttpServer(int port, Handler handler): serversocket_(io_service_), handler_(handler),
                    endpoint_(boost::asio::ip::address::from_string("0.0.0.0"), port)
{
    serversocket_.open(endpoint_.protocol());
    serversocket_.bind(endpoint_);
    serversocket_.listen();
    threadpool_.start();
    threadpool_.newThreads(6);
    for(int i=0; i<threadpool_.getThreadsCount(); i++)
        threadpool_.execute(std::bind([](IOService* service)->void
        {
            service->run();
        }, &io_service_));
    start_accept();
    threadpool_.wait();
}

void HttpServer::start_accept()
{
    connection_.reset(new HttpConnection(io_service_, handler_));
    serversocket_.async_accept(connection_->socket(), std::bind(&HttpServer::run_after_accept, this));
}

void HttpServer::run_after_accept()
{
    connection_->start();
    start_accept();
}