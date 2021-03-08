#include<content_types_mapping.h>
#include<regex>

using namespace std;

const std::unordered_map<std::string, std::string> HttpFileTypeMap::map =
{
    { ".ai",    "application/postscript" },
    { ".xml",   "application/atom+xml" },
    { ".js",    "application/javascript" },
    { ".edi",   "application/EDI-X12" },
    { ".json",  "application/json" },
    { ".ogg",   "application/ogg" },
    { ".rdf",   "application/rdf+xml" },
    { ".xml",   "application/xml" },
    { ".woff",  "application/font-woff" },
    { ".xhtml", "application/xhtml+xml" },
    { ".zip",   "application/zip" },
    { ".gzip",  "application/gzip" },
    { ".xls",   "application/x-xls" },
    { ".css",   "text/css" },
    { ".csv",   "text/csv" },
    { ".html",  "text/html" },
    { ".htm",   "text/html" },
    { ".jsp",   "text/html" },
    { ".math",  "text/xml" },
    { ".mml",   "text/xml" },
    { ".mtx",   "text/xml" },
    { ".plg",   "text/html" },
    { ".rdf",   "text/xml" },
    { ".txt",   "text/plain" },
    { ".sol",   "text/plain" },
    { ".xsl",   "text/xml" },
    { ".avi",   "video/avi" },
    { ".m4e",   "video/mpeg4" },
    { ".mp4",   "video/mp4" },
    { ".wav",   "audio/wav" },
    { ".wax",   "audio/x-ms-wax" },
    { ".tif",   "image/tiff" },
    { ".gif",   "image/gif" },
    { ".ico",   "image/x-icon" },
    { ".jfif",  "image/jpeg" },
    { ".jpe",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".jpg",   "image/jpeg" },
    { ".png",   "image/png" },
};

ContentType HttpFileTypeMap::parseFileType(std::string path)
{
    int status = 0;
    int offset = -1;
    for(int i = path.size() - 1; i >= 0; i--)
    {
        switch (status)
        {
        case 0:
            if(ISUPPER(path[i]) || ISLOWER(path[i]) || ISDIGIT(path[i]))
                status = 1;
            else
                return ContentType({"application/octet-stream", {"", ""}});
            break;
        case 1:
            if(path[i] == '.')
            {
                std::string suffix = path.substr(i, path.size() - (i - 2));
                transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
                auto iter = map.find(suffix);
                if(iter != map.end())
                {
                    regex major("text/");
                    if(regex_search(iter->second, major))
                        return ContentType({iter->second, {"charset", "utf-8"}});
                    else
                        return ContentType({iter->second, {"", ""}});
                }
                return ContentType({"application/octet-stream", {"", ""}});
            }
            else if(!(ISUPPER(path[i]) || ISLOWER(path[i]) || ISDIGIT(path[i])))
                return ContentType({"application/octet-stream", {"", ""}});
            break;
        default:
            break;
        }
    }
    return ContentType({"application/octet-stream", {"", ""}});
}