#pragma once

#include<memory>
#include<string>

class HttpBody
{
private:
    std::string content;
    uint64_t    size;
public:
    HttpBody();
    HttpBody(uint64_t);
    HttpBody(std::string);

    std::string getContent() const { return content; }
    uint64_t    getSize() const { return size; }
};