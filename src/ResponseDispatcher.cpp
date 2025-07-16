#include "ResponseDispatcher.h"
#include "QueueManager.h"
#include "Logger.h"
#include <drogon/HttpResponse.h>
#include <thread>
#include <chrono>

void responseDispatcherThread() {
    Logger::getInstance().log("[T4] Response dispatcher thread started");
    
    GatewayTask task;
    while (true) {
        if (ResponseQueue.try_dequeue(task)) {
            Logger::getInstance().log("[T4] Dequeued response task for dispatch");
            
            try {
                // Create HTTP response
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setBody(task.json);
                resp->setStatusCode(drogon::k200OK);
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                
                // Send response using stored callback
                if (task.callback) {
                    task.callback(resp);
                    Logger::getInstance().log("[T4] HTTP response sent successfully");
                } else {
                    Logger::getInstance().log("[T4] Warning: No callback available for response");
                }
            } catch (const std::exception &e) {
                Logger::getInstance().log(std::string("[T4] Error creating response: ") + e.what());
                
                // Send error response
                if (task.callback) {
                    auto errorResp = drogon::HttpResponse::newHttpResponse();
                    errorResp->setStatusCode(drogon::k500InternalServerError);
                    errorResp->setBody("{\"error\": \"Internal server error\"}");
                    errorResp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                    task.callback(errorResp);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
