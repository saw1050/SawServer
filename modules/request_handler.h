#pragma once
#include<request.h>
#include<response.h>
#include<queue>

enum BodyModel
{
    BODYMODEL_UNKNOW,
    BODYMODEL_CONTENT,
    BODYMODEL_PATH
};

class RequestHandler
{
private:
    virtual void run(ExpertHttpRequest& request, std::queue<ExpertHttpResponse>& responses, BodyModel& model) = 0;
public:
    void operator()(ExpertHttpRequest& request, std::queue<ExpertHttpResponse>& responses, BodyModel& model)
    {
        run(request, responses, model);
    }
};

