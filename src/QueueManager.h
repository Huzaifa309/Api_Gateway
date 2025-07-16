#pragma once
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <concurrentqueue.h>
#include <functional>
#include <string>

struct GatewayTask {
    std::string json;
    drogon::HttpRequestPtr request;
    std::function<void(const drogon::HttpResponsePtr &)> callback;
};

extern moodycamel::ConcurrentQueue<GatewayTask> ReceiverQueue;
extern moodycamel::ConcurrentQueue<GatewayTask> ResponseQueue;
