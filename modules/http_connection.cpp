#include<http_connection.h>
#include<boost/bind.hpp>
#include<fstream>

using namespace std;

HttpConnection::HttpConnection(IOService& service, Handler handler, size_t buffer_size, size_t http_buffer_size):
    socket_(service), handler_(handler), buffer_size_(buffer_size), buffer_(Buffer(new char[buffer_size])),
    http_buffer_(http_buffer_size), model_(BODYMODEL_UNKNOW)
{
}

HttpConnection::~HttpConnection()
{
}

HttpConnection::Socket& HttpConnection::socket()
{
    return socket_;
}


void HttpConnection::start()
{
    socket_.non_blocking(false);
    socket_.async_receive( boost::asio::buffer(buffer_.get(), buffer_size_),
                            boost::bind(&HttpConnection::run_after_read, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred) );
}

size_t HttpConnection::get_request_length(size_t buffer_bytes)
{
    http_buffer_.write(buffer_, buffer_bytes);
    auto data = http_buffer_.getBuffer();
    size_t i = 0;
    int status = 0;
    for(; i < http_buffer_.getSize(); i++)
    {
        if(data[i] == '\r')
        {
            if(status == 0)
            {
                status = 1;
            }
            else if(status == 2)
            {
                status = 3;
            }
            else
            {
                status = 0;
            }
        }
        else if(data[i] == '\n')
        {
            if(status == 1)
            {
                status = 2;
            }
            else if(status == 3)
            {
                status = 4;
                break;
            }
            else
            {
                status = 0;
            }
        }
        else
        {
            status = 0;
        }
    }
    if(status == 4)
    {
        return i + 1;
    }
    return 0;
}

void HttpConnection::run_after_read(const ErrorCode& e, std::size_t bytes_transferred)
{
    if(e)
    {
        return;
    }
    size_t count = get_request_length(bytes_transferred);
    if(count > 0)
    {
        std::string head;
        std::string requeststring(count, 0);
        http_buffer_.read((char*)requeststring.c_str(), count);

        request_ = Request::parse(requeststring);
        (*handler_)(request_, responses_, model_);
        while(!responses_.empty())
        {
            auto response = responses_.front();
            responses_.pop();
            switch (model_)
            {
            case BODYMODEL_UNKNOW:
                /* code */
                break;
            case BODYMODEL_CONTENT:
                reply_plain(response);
                break;
            case BODYMODEL_PATH:
                reply_file(response);
                break;
            default:
                break;
            }
        }
    }
}

void HttpConnection::reply_file(Response& response)
{
    string filepath = response.getBody()->getContent();
    ifstream is(filepath, ios::binary | ios::ate);
    if(!is.is_open())
    {
        try
        {
            socket_.wait(boost::asio::socket_base::wait_write);
            socket_.send(boost::asio::buffer(Response(404).getHead()));
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    else
    {
        char tempbuf[8192];
        response.setContentLength(is.tellg());

        if(!response.getContentRange().empty() && response.getContentRange().range.start >= 0)
        {
            int start = response.getContentRange().range.start;
            int end = response.getContentRange().range.end;
            if(end <= start)
            {
                end = is.tellg();
                response.setContentRange(start, end - 1);
            }
            int left = end - start;
            is.seekg(start);
            try
            {
                socket_.wait(boost::asio::socket_base::wait_write);
                socket_.send(boost::asio::buffer(response.getHead()));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            while(left > 0)
            {
                is.read(tempbuf, sizeof(tempbuf));
                try
                {
                    socket_.wait(boost::asio::socket_base::wait_write);
                    socket_.send(boost::asio::buffer(tempbuf, left < is.gcount() ? left:is.gcount()));
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    break;
                }
            }
        }
        else
        {
            is.seekg(0);
            try
            {
                socket_.wait(boost::asio::socket_base::wait_write);
                socket_.send(boost::asio::buffer(response.getHead()));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            while (!is.eof())
            {
                is.read(tempbuf, sizeof(tempbuf));
                try
                {
                    socket_.wait(boost::asio::socket_base::wait_write);
                    socket_.send(boost::asio::buffer(tempbuf, is.gcount()));
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    break;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
    is.close();
    try
    {
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void HttpConnection::reply_plain(Response& response)
{
    response.setContentLength(response.getBody()->getSize());
    try
    {
        socket_.wait(boost::asio::socket_base::wait_write);
        socket_.send(boost::asio::buffer(response.getHead()));
        socket_.wait(boost::asio::socket_base::wait_write);
        socket_.send(boost::asio::buffer(response.getBody()->getContent()));
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
