# Encoding : utf-8
# 一个简单的 Http 静态资源服务器

该项目使用了 Boost asio 库, 需要安装该库后再进行编译, 基于 1.73.0 版本

使用 CMake 编译
  Windows:
    $ cmake -D BOOST_DIR:PATH=<boost头文件路径> -D BOOST_LIBS_DIR:PATH=<boost库文件路径> -B build
    $ cmake --build ./build --config <Release | Debug>
  Linux:
    $ cmake -B build
    $ cmake --build ./build --config <Release | Debug>


将资源与编译出来的可执行文件放在同一目录下, 运行服务器, 即可通过 http 客户端访问资源

目前实现的功能较为简单, 仅实现了下列规范:
  method:
    GET
  http version:
    HTTP/1.1
  head:
    Accept-Ranges
    Connection
    Content-Length
    Content-Type
    Date

以后可能会实现更多规范, 这只是一项编程练习
