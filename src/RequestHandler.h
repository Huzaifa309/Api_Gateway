#pragma once
#include <drogon/HttpController.h>

class RequestHandler : public drogon::HttpController<RequestHandler> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RequestHandler::DataPass, "/api/data", drogon::Post);
    METHOD_LIST_END

    void DataPass(const drogon::HttpRequestPtr &req,
                  std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};