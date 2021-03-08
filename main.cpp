#include<http_server.h>
#include<handler_v1.h>
#include<thread>

using namespace std;

int main(int argc, char const *argv[])
{
    shared_ptr<HandlerV1> handler(make_shared<HandlerV1>());
    HttpServer server(12219, handler);

    return 0;
}
