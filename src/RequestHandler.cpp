#include "RequestHandler.h"
#include "QueueManager.h"
#include "Logger.h"

void RequestHandler::DataPass(const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    
    // Log detailed request information
    Logger::getInstance().log("[T1] ========== Incoming Request ==========");
    Logger::getInstance().log("[T1] Method: " + std::string(drogon::to_string(req->getMethod())));
    Logger::getInstance().log("[T1] Path: " + std::string(req->getPath()));
    Logger::getInstance().log("[T1] Content-Type: " + std::string(req->getHeader("content-type")));
    Logger::getInstance().log("[T1] Content-Length: " + std::string(req->getHeader("content-length")));
    Logger::getInstance().log("[T1] User-Agent: " + std::string(req->getHeader("user-agent")));
    
    // Log the actual request body/data
    std::string requestBody = std::string(req->body());
    Logger::getInstance().log("[T1] Request Body Data: " + requestBody);
    Logger::getInstance().log("[T1] Request Body Length: " + std::to_string(requestBody.length()) + " bytes");
    Logger::getInstance().log("[T1] =======================================");
    
    GatewayTask task{requestBody, req, std::move(callback)};
    std::cout<<task.json<<std::endl;
    ReceiverQueue.enqueue(task);
    Logger::getInstance().log("[T1] Request enqueued for processing");
}