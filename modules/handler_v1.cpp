#include<handler_v1.h>
#include<content_types_mapping.h>
#include<regex>

using namespace std;

constexpr char servname[] = "SAW-SERV";
constexpr char welcome[] = "<HTML>"\
                                "<head>"\
                                    "<title>main page</title>"\
                                "</head>"\
                                "<body>"\
                                    "<h1 width='100%'; text-align='center'; margin='0'>Welcome</h1>"\
                                "</body>"\
                            "</HTML>";

void HandlerV1::run(ExpertHttpRequest& request, std::queue<ExpertHttpResponse>& responses, BodyModel& model)
{
    string path = request.getUrl();
    if(path == "/")
    {
        model = BODYMODEL_CONTENT;
        ExpertHttpResponse response;
        response.setServer(servname);
        response.setStatus(200);
        response.setContentType("text/html", "charset", "utf-8");
        response.setConnection("close");
        response.setBody(make_shared<HttpBody>(welcome));
        response.setDate(getNowTime());
        responses.push(response);
    }
    else if(path[0] != '/')
    {
        responses.push(ExpertHttpResponse(400));
    }
    else
    {
        model = BODYMODEL_PATH;
        path.erase(0, 1);
        if(request.getRange().empty())  // 无 Range
        {
            ExpertHttpResponse response;
            ContentType type = HttpFileTypeMap::parseFileType(path);
            regex major("text/");
            if(!regex_search(type.type, major))
                response.setAcceptRanges(true);
            response.setServer(servname);
            response.setStatus(200);
            response.setContentType(type);
            response.setDate(getNowTime());
            response.setConnection("keep-alive");
            response.setBody(make_shared<HttpBody>(path));
            responses.push(response);
        }
        else    // 有 Range
        {
            for(auto range:request.getRange().ranges)
            {
                ExpertHttpResponse response;
                response.setServer(servname);
                response.setStatus(206);
                response.setAcceptRanges(true);
                response.setContentRange(request.getRange().unit, range, 0);
                response.setContentType(HttpFileTypeMap::parseFileType(path));
                response.setDate(getNowTime());
                response.setConnection("keep-alive");
                // response.addHead("last-modified", "Sun, 24 Jan 2021 11:59:05 GMT");
                response.setBody(make_shared<HttpBody>(path));
                responses.push(response);
            }
        }
    }
}