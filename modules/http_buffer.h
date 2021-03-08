#pragma once

#include<string>
#include<memory>

class HttpBuffer
{
private:
    std::shared_ptr<char[]> buffer_;
    int capacity_;
    int size_;
    int readCursor_;
    int writeCursor_;
public:
    HttpBuffer();
    HttpBuffer(int capacity);
    // ~HttpBuffer();
    std::shared_ptr<char[]> getBuffer() const;
    int getCapacity() const;
    int getSize() const;
    int read(char* dst, int length);
    int write(const char* src, int length);
    int write(std::shared_ptr<char[]> src, int length);

};

