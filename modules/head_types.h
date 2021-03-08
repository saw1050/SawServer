#pragma once

#include<string>
#include<unordered_map>
#include<unordered_set>
#include<regex>

enum Method { GET, POST, PUT , DEL, TRACE, OPTIONS, HEAD, PATCH, CONNECT };
enum Version { HTTP_0_9, HTTP_1_0, HTTP_1_1, HTTP_2_0 };

struct WeightItem
{
    WeightItem() = default;
    WeightItem(std::string v, double w);
    std::string value; double weight = 1.0;
    bool operator==(const WeightItem&) const;
};

struct WeightItemHashFunction
{
    int operator()(const WeightItem& item) const;
};

struct WeightItemEqualFunction
{
    bool operator()(const WeightItem& a, const WeightItem& b) const;
};

struct ContentType
{
    std::string type;
    std::pair<std::string, std::string> property;
    bool operator==(ContentType &other) const;
};

struct Range
{
    Range() = default;
    Range(int s, int e);
    int start = -1;
    int end = -1;
};

struct RangeHashFunction
{
    int operator()(const Range& range) const;
};

struct RangeEqualFunction
{
    bool operator()(const Range& a, const Range& b) const;
};

struct URange
{
    URange() = default;
    URange(std::string u, std::unordered_set<Range, RangeHashFunction, RangeEqualFunction> r);
    std::string unit; std::unordered_set<Range, RangeHashFunction, RangeEqualFunction> ranges;
    bool operator==(URange &other) const;
    bool empty() const;
};      // <unit> - { (<start> - <end>), ... }

struct CRange
{
    CRange() = default;
    CRange(std::string u, Range r, int l);
    std::string unit; Range range; int length;
    bool operator==(CRange &other) const;
    bool empty() const;
};

std::string timeToHttpString(tm t);

tm httpStringToTime(std::string str);

tm getNowTime();

double getWeight(std::string str);

std::unordered_set<WeightItem, WeightItemHashFunction, WeightItemEqualFunction> stringToWSet(std::string str);

std::unordered_set<std::string> stringToSSet(std::string str);

std::unordered_map<std::string, std::string> stringToSSMap(std::string str, char separator);

std::unordered_multimap<std::string, std::string> stringToSSMMap(std::string str);

std::pair<std::string, std::string> stringToSPair(std::string str, char separator);

ContentType stringToContentType(std::string str);

URange stringToURange(std::string str);

#define ISLETTER(word) ((word > 40 && word <= 90) || (word > 60 && word <= 122))
#define ISNUMBER(word) (word >= 48 && word < 58)

CRange stringToCRange(std::string str);