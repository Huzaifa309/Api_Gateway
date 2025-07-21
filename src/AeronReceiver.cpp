#include "QueueManager.h"
#include "Logger.h"
#include "Aeron.h"
#include "FragmentAssembler.h"
#include "Subscription.h"
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

using namespace aeron;
using json = nlohmann::json;

void aeronReceiverThread(std::shared_ptr<Subscription> subscription)
{
    Logger::getInstance().log("[T3] Aeron receiver thread started");

    fragment_handler_t handler = [&](AtomicBuffer &buffer, std::int32_t offset, std::int32_t length, const Header &header)
    {
        try {
            // Extract the raw data from the buffer
            std::string rawData(reinterpret_cast<const char *>(buffer.buffer() + offset), length);
            Logger::getInstance().log("[T3] Received raw data from backend, length: " + std::to_string(length));
            
            // Try to parse as JSON
            json receivedJson;
            try {
                receivedJson = json::parse(rawData);
                Logger::getInstance().log("[T3] Successfully parsed JSON: " + receivedJson.dump());
                
                // Create a processed response with additional metadata
                json processedResponse;
                processedResponse["received_data"] = receivedJson;
                processedResponse["processed"] = true;
                processedResponse["status"] = "received";
                processedResponse["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                processedResponse["source"] = "aeron_backend";
                
                std::string responseJson = processedResponse.dump();
                Logger::getInstance().log("[T3] Created processed response: " + responseJson);
                
                // Push to ResponseQueue
                GatewayTask responseTask;
                responseTask.json = responseJson;
                ResponseQueue.enqueue(responseTask);
                Logger::getInstance().log("[T3] Response enqueued for dispatch");
                
            } catch (const json::parse_error& e) {
                Logger::getInstance().log("[T3] JSON parse error: " + std::string(e.what()));
                Logger::getInstance().log("[T3] Raw data received: " + rawData);
                
                // Create error response for invalid JSON
                json errorResponse;
                errorResponse["error"] = "Invalid JSON received";
                errorResponse["message"] = e.what();
                errorResponse["raw_data"] = rawData;
                errorResponse["status"] = "error";
                errorResponse["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                
                GatewayTask errorTask;
                errorTask.json = errorResponse.dump();
                ResponseQueue.enqueue(errorTask);
                Logger::getInstance().log("[T3] Error response enqueued for dispatch");
            }
            
        } catch (const std::exception &e) {
            Logger::getInstance().log(std::string("[T3] Buffer processing error: ") + e.what());
            
            // Create error response for buffer processing issues
            json errorResponse;
            errorResponse["error"] = "Buffer processing error";
            errorResponse["message"] = e.what();
            errorResponse["status"] = "error";
            errorResponse["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            
            GatewayTask errorTask;
            errorTask.json = errorResponse.dump();
            ResponseQueue.enqueue(errorTask);
            Logger::getInstance().log("[T3] Buffer error response enqueued for dispatch");
        }
    };

    while (true)
    {
        try
        {
            subscription->poll(handler, 10);
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().log(std::string("[T3] Aeron poll error: ") + e.what());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
