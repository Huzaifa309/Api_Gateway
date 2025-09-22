#include "RequestHandler.h"
#include "QueueManager.h"
#include "Logger.h"
#include "ThreadPool.h"
#include "ThreadPoolConfig.h"
#include <memory>
#include <nlohmann/json.hpp>

// Thread pool for parallel request processing within T1
static std::unique_ptr<ThreadPool> requestThreadPool;

void RequestHandler::DataPass(const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    
    // Initialize thread pool if not already done
    if (!requestThreadPool) {
        requestThreadPool = std::make_unique<ThreadPool>(REQUEST_THREAD_POOL_SIZE);
        Logger::getInstance().log("[T1] Request processing thread pool initialized with " + 
                                 std::to_string(REQUEST_THREAD_POOL_SIZE) + " workers");
    }
    
    // Log detailed request information
    Logger::getInstance().log("[T1] ========== Incoming Request ==========");
   Logger::getInstance().log(std::string("[T1] Method: ") + req->getMethodString());
    Logger::getInstance().log("[T1] Path: " + std::string(req->getPath()));
    Logger::getInstance().log("[T1] Content-Type: " + std::string(req->getHeader("content-type")));
    Logger::getInstance().log("[T1] Content-Length: " + std::string(req->getHeader("content-length")));
    Logger::getInstance().log("[T1] User-Agent: " + std::string(req->getHeader("user-agent")));
    
    // Log the actual request body/data
    std::string requestBody = std::string(req->body());
    Logger::getInstance().log("[T1] Request Body Data: " + requestBody);
    Logger::getInstance().log("[T1] Request Body Length: " + std::to_string(requestBody.length()) + " bytes");
    Logger::getInstance().log("[T1] =======================================");
    
    // Create task for parallel processing
    GatewayTask task{std::move(requestBody), req, std::move(callback)};
    
    // Enqueue the task to thread pool for parallel processing
    requestThreadPool->enqueue_void([task = std::move(task)]() mutable {
        // Process the request in parallel
        Logger::getInstance().log("[T1-Worker] Processing request in parallel thread");
        
        // Validate JSON format in parallel
        try {
            auto json = nlohmann::json::parse(task.json);
            Logger::getInstance().log("[T1-Worker] JSON validation successful");
        } catch (const std::exception& e) {
            Logger::getInstance().log("[T1-Worker] JSON validation failed: " + std::string(e.what()));
            // Handle invalid JSON - could send error response here
        }
        
        // Enqueue to ReceiverQueue for further processing
        ReceiverQueue.enqueue(std::move(task));
        Logger::getInstance().log("[T1-Worker] Request enqueued for processing");
    });
    
    Logger::getInstance().log("[T1] Request submitted to thread pool for parallel processing");
}
