#include<head_types.h>

const static std::unordered_map<std::string, int> WEEKDAYMAP = {
    {"Sun", 0}, {"Mon", 1}, {"Tue", 2}, {"Wed", 3}, {"Thu", 4}, {"Fri", 5}, {"Sat", 6}
};

const static std::unordered_map<std::string, int> MONTHMAP = {
    {"Jan", 0}, {"Feb", 1}, {"Mar", 2}, {"Apr", 3}, {"May", 4}, {"Jun", 5},
    {"Jul", 6}, {"Aug", 7}, {"Sep", 8}, {"Oct", 9}, {"Nov", 10}, {"Dec", 11}
};

WeightItem::WeightItem(std::string v, double w):value(v),weight(w){}

bool WeightItem::operator==(const WeightItem& other) const
{
    return value == other.value;
}

int WeightItemHashFunction::operator()(const WeightItem& item) const
{
    std::hash<std::string> dohash; return dohash(item.value);
}

bool WeightItemEqualFunction::operator()(const WeightItem& a, const WeightItem& b) const
{
    return a == b;
}

bool ContentType::operator==(ContentType &other) const
{
    return type == other.type && property == other.property;
}

Range::Range(int s, int e):start(s),end(e){}

int RangeHashFunction::operator()(const Range& range) const
{
    long long product = range.end * range.start;
    return product % range.end + range.end;
}

bool RangeEqualFunction::operator()(const Range& a, const Range& b) const
{
    return !(b.start >= a.end || a.start >= b.end);
}

URange::URange(std::string u, std::unordered_set<Range, RangeHashFunction, RangeEqualFunction> r):unit(u),ranges(r){}

bool URange::operator==(URange &other) const
{
    if(ranges.size() == other.ranges.size())
    {
        for(auto i : ranges)
        {
            if(other.ranges.find(i) == other.ranges.end())
            {
                return false;
            }
        }
    }
    else
        return false;
    return unit == other.unit;
}

bool URange::empty() const
{
    return unit.empty();
}

CRange::CRange(std::string u, Range r, int l):unit(u), range(r), length(l){}

bool CRange::operator==(CRange &other) const
{
    return  unit        == other.unit           &&
            range.start == other.range.start    &&
            range.end   == other.range.end      &&
            length      == other.length;
}

bool CRange::empty() const
{
    return unit.empty();
}

std::string timeToHttpString(tm t)
{
    std::string result;
    char temp[128]{0};
    int length = strftime(temp, sizeof(temp), "%a, %d %b %Y %H:%M:%S GMT", &t);
    result = temp;
    return result;
}

tm httpStringToTime(std::string str)
{
    int wdp = 0;    int wday;
    int mdp = 5;    int mday;
    int mp = 8;     int mon;
    int yp = 12;    int year;
    int hp = 17;    int hour;
    int mip = 20;   int min;
    int sp = 23;    int sec;
                    int yday;

    auto iter = WEEKDAYMAP.find(str.substr(wdp, 3));
    if(iter == WEEKDAYMAP.end())
        return tm{ -1, -1, -1, -1, -1, -1, -1, -1, 0 };
    else
        wday = iter->second;
    iter = MONTHMAP.find(str.substr(mp, 3));
    if(iter == MONTHMAP.end())
        return tm{ -1, -1, -1, -1, -1, -1, -1, -1, 0 };
    else
        mon = iter->second;
    sec = atoi(str.substr(sp, 2).c_str());
    min = atoi(str.substr(mip, 2).c_str());
    hour = atoi(str.substr(hp, 2).c_str());
    mday = atoi(str.substr(mdp, 2).c_str());
    year = atoi(str.substr(yp, 4).c_str()) - 1900;
    yday = year % 4 == 0 ?
            (mon <= 1 ?
                        mon*30 + mday + ((mon + 1) / 2):
                        mon*30 + mday + ((mon + 1) / 2) - 1):
            (mon <= 1 ?
                        mon*30 + mday + ((mon + 1) / 2):
                        mon*30 + mday + ((mon + 1) / 2) - 2);
    return tm{ sec, min, hour, mday, mon, year, wday, yday, 0};
}

tm getNowTime()
{
    time_t longsec;
    tm t;
    time(&longsec);
#ifdef _WIN32
    gmtime_s(&t, &longsec);
#elif __linux__
    gmtime_r(&longsec, &t);
#endif
    return t;
}

double getWeight(std::string str)
{
    int state = 0;
    for(auto word : str)
    {
        if(word == 'q' && state == 0)
        {
            state = 1;
        }
        else if(word == '=' && state == 1)
        {
            state = 2;
        }
        else if(word == '0')
        {
            if(state == 2)
                state = 3;
            else if(state == 5)
                state = 4;
        }
        else if(word == '1' && (state == 2 || state == 5))
        {
            state = 4;
        }
        else if(word == '.' && (state == 3 || state == 4))
        {
            state = 5;
        }
        else if(word >= 48 && word < 58 && state == 5)        // 整数
        {
            state = 4;
        }
        else
        {
            state = 6;
        }
    }
    if(state != 4)
    {
        return 1;
    }
    else
    {
        std::string weightstring((str.begin()+=2), str.end());
        return atof(weightstring.c_str());
    }
}

std::unordered_set<WeightItem, WeightItemHashFunction, WeightItemEqualFunction> stringToWSet(std::string str)
{
    int stage = 1;
    std::string type;
    std::string weight;
    std::unordered_set<WeightItem, WeightItemHashFunction, WeightItemEqualFunction> result;
    for(int i = 0; i <= str.size(); i++)
    {
        if(i == str.size())
        {
            result.emplace(std::move(type), getWeight(std::move(weight)));
            break;
        }
        if(str[i] == ' ')
            continue;
        if(stage == 1)
        {
            if(str[i] != ';' && str[i] != ',')
            {
                type += str[i];
            }
            else if(str[i] == ';')
            {
                stage = 2;
            }
            else
            {
                stage = 3;
            }
        }
        else if(stage == 2)
        {
            if(str[i] != ',')
            {
                weight += str[i];
            }
            else
            {
                stage = 3;
            }
        }
        else
        {
            result.emplace(std::move(type), getWeight(std::move(weight)));
            type += str[i];
            stage = 1;
        }
    }
    return result;
}

std::unordered_set<std::string> stringToSSet(std::string str)
{
    int stage = 1;
    std::string value;
    std::unordered_set<std::string> result;
    for(int i = 0; i <= str.size(); i++)
    {
        if(i == str.size())
        {
            result.emplace(std::move(value));
            break;
        }
        if(str[i] == ' ')
            continue;
        if(stage == 1)
        {
            if(str[i] != ',')
            {
                value += str[i];
            }
            else
            {
                stage = 2;
            }
        }
        else
        {
            result.emplace(std::move(value));
            value += str[i];
            stage = 1;
        }
    }
    return result;
}

std::unordered_map<std::string, std::string> stringToSSMap(std::string str, char separator)
{
    int stage = 1;
    // int serialnumber = 1;
    bool isbeginning = true;
    std::string key;
    std::string value;
    std::unordered_map<std::string, std::string> result;
    for(int i = 0; i <= str.size(); i++)
    {
        if(i == str.size())
        {
            result.emplace(std::move(key), std::move(value));
            break;
        }
        if(isbeginning && str[i] == ' ')
            continue;
        isbeginning = false;
        if(str[i] == '=')
        {
            if(stage == 1)
                stage = 2;
            else
                return std::unordered_map<std::string, std::string>();
        }
        else if(str[i] == separator)
        {
            stage = 3;
            isbeginning = true;
        }
        else
        {
            if(stage == 1)
            {
                key += str[i];
            }
            else if(stage == 2)
            {
                value += str[i];
            }
            else
            {
                result.emplace(std::move(key), std::move(value));
                key += str[i];
                stage = 1;
            }
        }
    }
    return result;
}

std::unordered_multimap<std::string, std::string> stringToSSMMap(std::string str)
{
    int stage = 1;
    // int serialnumber = 1;
    bool isbeginning = true;
    std::string key;
    std::string value;
    std::unordered_multimap<std::string, std::string> result;
    for(int i = 0; i <= str.size(); i++)
    {
        if(i == str.size())
        {
            result.emplace(std::move(key), std::move(value));
            break;
        }
        if(isbeginning && str[i] == ' ')
            continue;
        isbeginning = false;
        if(str[i] == '=')
        {
            if(stage == 1)
                stage = 2;
            else
                return std::unordered_multimap<std::string, std::string>();
        }
        else if(str[i] == ';')
        {
            stage = 3;
            isbeginning = true;
        }
        else
        {
            if(stage == 1)
            {
                key += str[i];
            }
            else if(stage == 2)
            {
                value += str[i];
            }
            else
            {
                result.emplace(std::move(key), std::move(value));
                key += str[i];
                stage = 1;
            }
        }
    }
    return result;
}

std::pair<std::string, std::string> stringToSPair(std::string str, char separator)
{
    int stage = 1;
    std::string value1;
    std::string value2;
    std::pair<std::string, std::string> result;
    for(int i = 0; i <= str.size(); i++)
    {
        if(i == str.size())
        {
            result.first = std::move(value1);
            result.second = std::move(value2);
            break;
        }
        if(str[i] == ' ' && separator != ' ')
            continue;
        if(stage == 1)
        {
            if(str[i] != separator)
            {
                value1 += str[i];
            }
            else
            {
                stage = 2;
            }
        }
        else
        {
            value2 += str[i];
        }
    }
    return result;
}

ContentType stringToContentType(std::string str)
{
    int stage = 1;
    std::string type;
    std::string key;
    std::string value;
    ContentType result;
    for(int i = 0; i <= str.size(); i++)
    {
        if(i == str.size())
        {
            result.type = std::move(type);
            result.property.first = std::move(key);
            result.property.second = std::move(value);
            break;
        }
        if(str[i] == ' ')
            continue;
        if(stage == 1)
        {
            if(str[i] == ';')
            {
                stage = 2;
            }
            else
            {
                type += str[i];
            }
        }
        else if(stage == 2)
        {
            if(str[i] == '=')
            {
                stage = 3;
            }
            else
            {
                key += str[i];
            }
        }
        else if(stage == 3)
        {
            value += str[i];
        }
    }
    return result;
}

URange stringToURange(std::string str)
{
    int stage = 1;
    std::string unit;
    std::string rangestart;
    std::string rangeend;
    int end = 0;
    URange result;
    for(int i = 0; i <= str.size(); i++)
    {
        if(i == str.size())
        {
            if(rangeend.empty())
                    end = -1;
                else
                    end = atoi(rangeend.c_str());
            result.unit = std::move(unit);
            result.ranges.emplace(Range(atoi(rangestart.c_str()), end));
            break;
        }
        if(str[i] == ' ')
            continue;
        if(stage == 1)
        {
            if(str[i] == '=')
                stage = 2;
            else
                unit += str[i];
        }
        else if(str[i] == '-' && stage == 2)
        {
            stage = 3;
        }
        else if(str[i] == ',' && (stage == 2 || stage == 3))
        {
            stage = 4;
        }
        else if(str[i] >= 48 && str[i] < 58) // !!!!!!!!!!!!!!!!!!!
        {
            if(stage == 2)
            {
                rangestart += str[i];
            }
            else if(stage == 3)
            {
                rangeend += str[i];
            }
            else if(stage == 4)
            {
                if(rangeend.empty())
                    end = -1;
                else
                    end = atoi(rangeend.c_str());
                result.ranges.emplace(Range(atoi(rangestart.c_str()), end));
                rangeend.clear();
                rangestart.clear();
                stage = 2;
                rangestart += str[i];
            }
        }
        else
        {
            return URange();
        }
    }
    return result;
}

#define ISLETTER(word) ((word > 40 && word <= 90) || (word > 60 && word <= 122))
#define ISNUMBER(word) (word >= 48 && word < 58)

CRange stringToCRange(std::string str)
{
    int status = -1;
    bool isbeginning = true;
    std::string unit;
    std::string startstring;
    std::string endstring;
    std::string lengthstring;
    CRange result;
    for(auto word : str)
    {
        switch (status)
        {
        case -1:
            if(ISLETTER(word))
            {
                unit += word;
                status = 0;
            }
            else
                status = 9;
            break;
        case 0:
            if(ISLETTER(word))
            {
                unit += word;
            }
            else if(word == ' ')
            {
                status = 1;
            }
            else
                status = 9;
            break;
        case 1:
            if(word == ' ')
                continue;
            else if(ISNUMBER(word))
            {
                startstring += word;
                status = 2;
            }
            else if(word == '*')
            {
                startstring = word;
                endstring = word;
                status = 3;
            }
            else
                status = 9;
            break;
        case 2:
            if(ISNUMBER(word))
            {
                startstring += word;
            }
            else if(word == '-')
            {
                status = 4;
            }
            else
                status = 9;
            break;
        case 3:
            if(word == '/')
            {
                status = 7;
            }
            else
                status = 9;
            break;
        case 4:
            if(ISNUMBER(word))
            {
                endstring += word;
                status = 5;
            }
            else
                status = 9;
            break;
        case 5:
            if(ISNUMBER(word))
            {
                endstring += word;
            }
            else if(word == '/')
            {
                status = 6;
            }
            else
                status = 9;
            break;
        case 6:
            if(ISNUMBER(word))
            {
                lengthstring += word;
                status = 7;
            }
            else if(word == '*')
            {
                lengthstring = word;
                status = 8;
            }
            else
                status = 9;
            break;
        case 7:
            if(ISNUMBER(word))
            {
                lengthstring += word;
            }
            else
                status = 9;
            break;
        case 8:
            status = 9;
            break;
        default:
            return CRange();
            break;
        }
    }
    if(status == 7 || status == 8){
        int start = -1;
        int end = -1;
        int length = -1;
        if(startstring != "*")
            start = atoi(startstring.c_str());
        if(endstring != "*")
            end = atoi(endstring.c_str());
        if(lengthstring != "*")
            length = atoi(lengthstring.c_str());
        result.unit = unit;
        result.range = { start, end };
        result.length = length;
    }
    return result;
}