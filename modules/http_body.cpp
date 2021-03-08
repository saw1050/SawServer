#include<http_body.h>


HttpBody::HttpBody() : HttpBody(10240){}

HttpBody::HttpBody(uint64_t capacity) : size(0), content(capacity, 0) {}

HttpBody::HttpBody(std::string data) : content(data), size(data.size()) {}