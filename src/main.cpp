#include <drogon/drogon.h>
#include "Aeron.h"
#include "Context.h"
#include "Publication.h" 
#include "Subscription.h"
#include "Logger.h"
#include "RequestHandler.h"
#include "QueueManager.h"
#include "JsonToSbeSender.h"
#include "AeronReceiver.h"
#include "ResponseDispatcher.h"

#include <memory>
#include <thread>
#include <atomic>
#include <chrono>

// Global flag for graceful shutdown
std::atomic<bool> running{true};

int main() {
    Logger::getInstance().log("[Main] Starting API Gateway");

    try {
        // Start T4: Response dispatcher (doesn't depend on Aeron)
        std::thread t4(responseDispatcherThread);
        t4.detach();
        Logger::getInstance().log("[Main] Response dispatcher thread started");

        // Try to initialize Aeron safely
        try {
            Logger::getInstance().log("[Main] Initializing Aeron...");
            auto ctx = std::make_shared<aeron::Context>();
            auto aeronClient = aeron::Aeron::connect(*ctx);
            
            if (!aeronClient) {
                throw std::runtime_error("Failed to connect Aeron client");
            }

            // Create Aeron publication
            std::int64_t pubId = aeronClient->addPublication("aeron:ipc", 1001);
            auto publication = aeronClient->findPublication(pubId);
            
            // Wait for publication to be ready
            while (!publication) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                publication = aeronClient->findPublication(pubId);
            }
            Logger::getInstance().log("[Main] Aeron publication created");

            // Create Aeron subscription  
            std::int64_t subId = aeronClient->addSubscription("aeron:ipc", 1001);
            auto subscription = aeronClient->findSubscription(subId);
            
            // Wait for subscription to be ready
            while (!subscription) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                subscription = aeronClient->findSubscription(subId);
            }
            Logger::getInstance().log("[Main] Aeron subscription created");

            // Start T2: JSON to Aeron sender
            std::thread t2(jsonToSbeSenderThread, publication);
            t2.detach();
            Logger::getInstance().log("[Main] JSON sender thread started");

            // Start T3: Aeron receiver
            std::thread t3(aeronReceiverThread, subscription);
            t3.detach();
            Logger::getInstance().log("[Main] Aeron receiver thread started");
            
        } catch (const std::exception &e) {
            Logger::getInstance().log(std::string("[Main] Aeron initialization failed: ") + e.what());
            Logger::getInstance().log("[Main] Continuing without Aeron backend communication");
        }

        // Configure and start Drogon HTTP server (T1)
        Logger::getInstance().log("[Main] Starting Drogon server on port 8080");
        drogon::app().addListener("0.0.0.0", 8080);
        drogon::app().setThreadNum(1);
        drogon::app().run();

    } catch (const std::exception &e) {
        Logger::getInstance().log(std::string("[Main] Critical error: ") + e.what());
        return 1;
    }

    return 0;
}
