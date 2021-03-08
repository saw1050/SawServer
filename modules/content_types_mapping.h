#pragma once
#include<unordered_map>
#include<string>
#include<iostream>
#include<algorithm>
#include<head_types.h>
#define ISUPPER(word) (word > 64 && word <= 90)
#define ISLOWER(word) (word > 96 && word <= 122)
#define ISDIGIT(word) (word >= 48 && word < 58)

class HttpFileTypeMap
{
public:
    const static std::unordered_map<std::string, std::string> map;
    static ContentType parseFileType(std::string path);
};



