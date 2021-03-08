#include<http_buffer.h>
#include<string.h>

using namespace std;

HttpBuffer::HttpBuffer() : HttpBuffer(4096) {}

HttpBuffer::HttpBuffer(int capacity) : buffer_(shared_ptr<char[]>(new char[capacity])), readCursor_(0), writeCursor_(0), capacity_(capacity), size_(0) {}

// HttpBuffer::~HttpBuffer()
// {
//     delete [] buffer_;
// }

std::shared_ptr<char[]> HttpBuffer::getBuffer() const
{
    return buffer_;
}

int HttpBuffer::getCapacity() const
{
    return capacity_;
}

int HttpBuffer::getSize() const
{
    return size_;
}

int HttpBuffer::read(char* dst, int length)
{
    char* start = buffer_.get();
    // char* start = buffer_;
    int end = capacity_ - 1;
    int result = (length > size_) ? size_:length;

    if(readCursor_ + result > end)      // 越界, 返回起点
    {
        int a = end - readCursor_;
        int b = result - a;
        memcpy(dst, start + readCursor_, a);
        memcpy(dst + a, start, b);
        readCursor_ = b - 1;
    }
    else
    {
        memcpy(dst, start + readCursor_, result);
        readCursor_ += result;
    }
    size_ -= result;
    return result;
}

int HttpBuffer::write(const char* src, int length)
{
    char* start = buffer_.get();
    // char* start = buffer_;
    int end = capacity_ - 1;
    int writeable = capacity_ - size_;
    int result = (writeable > length) ? length:writeable;

    if(writeCursor_ + result > end)
    {
        int a = end - writeCursor_;
        int b = result - a;
        memcpy(start + writeCursor_, src, a);
        memcpy(start, src + a, b);
        writeCursor_ = b - 1;
    }
    else
    {
        memcpy(start + writeCursor_, src, result);
        writeCursor_ += result;
    }
    size_ += result;
    return result;
}

int HttpBuffer::write(std::shared_ptr<char[]> src, int length)
{
    return write(src.get(), length);
}
