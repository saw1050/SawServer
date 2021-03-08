#pragma once

#include<request_handler.h>


class HandlerV1: public RequestHandler
{
private:
    void run(ExpertHttpRequest& request, std::queue<ExpertHttpResponse>& responses, BodyModel& model);
};