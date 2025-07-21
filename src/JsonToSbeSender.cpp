#include "QueueManager.h"
#include "Logger.h"
#include <thread>
#include "JsonToSbeSender.h"
#include <nlohmann/json.hpp>
#include "Aeron.h"
#include "Publication.h"
#include <memory>
#include <chrono>
// #include "external/sbe/my_app_messages/IdentityMessage.h"
// #include "external/sbe/my_app_messages/MessageHeader.h"

using json = nlohmann::json;

void jsonToSbeSenderThread(std::shared_ptr<aeron::Publication> publication) {
    GatewayTask task;
    while (true) {
        if (ReceiverQueue.try_dequeue(task)) {
            Logger::getInstance().log("[T2] Dequeued task for processing");
           
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                    try {
                        if (publication->isClosed()) {
                            Logger::getInstance().log("[TestThread] Publication closed");
                            break;
                        }
                        std::string testMsg= task.json;
                        aeron::concurrent::AtomicBuffer buffer(
                            const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(testMsg.c_str())),
                            testMsg.size()
                        );

                        int64_t result = publication->offer(buffer, 0, testMsg.size());
                        
                      
                            Logger::getInstance().log("[TestThread] Failed to send: " + std::to_string(result));
                        
                    } catch (const std::exception& e) {
                        Logger::getInstance().log(std::string("[TestThread] Error: ") + e.what());
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                
           
          
            try {
                auto parsed = json::parse(task.json);
                Logger::getInstance().log("[T2] JSON parsed successfully");

                // TODO: integrate SBE encoding of parsed JSON into buffer here in future
                // For now, create a simple echo response
                json response;
                response["echo"] = parsed;
                response["processed"] = true;
                response["status"] = "success";
                response["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();

                std::string responseJson = response.dump();
                Logger::getInstance().log("[T2] Created response: " + responseJson);

                // Create response task and push to ResponseQueue
                GatewayTask responseTask;
                responseTask.json = responseJson;
                responseTask.request = task.request;  // Keep original request for context
                responseTask.callback = task.callback;  // Keep the callback to send response
                
                ResponseQueue.enqueue(responseTask);
                Logger::getInstance().log("[T2] Response enqueued for dispatch");

            } catch (const std::exception &e) {
                Logger::getInstance().log(std::string("[T2] JSON parse error: ") + e.what());
                
                // Create error response
                json errorResponse;
                errorResponse["error"] = "Invalid JSON";
                errorResponse["message"] = e.what();
                errorResponse["status"] = "error";
                
                GatewayTask errorTask;
                errorTask.json = errorResponse.dump();
                errorTask.request = task.request;
                errorTask.callback = task.callback;
                
                ResponseQueue.enqueue(errorTask);
                Logger::getInstance().log("[T2] Error response enqueued for dispatch");
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}