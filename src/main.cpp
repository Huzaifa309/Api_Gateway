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
using namespace std;
// Global flag for graceful shutdown
std::atomic<bool> running{true};

int main() {
    Logger::getInstance().log("[Main] Starting API Gateway");
    
    try {
        // Start T4: Response dispatcher (doesn't depend on Aeron)
        std::thread t4(responseDispatcherThread);
        t4.detach();
        Logger::getInstance().log("[Main] Response dispatcher thread started");

        // Aeron resources (declared outside try block for proper lifetime)
        std::shared_ptr<aeron::Aeron> aeronClient;
        std::shared_ptr<aeron::Publication> publication;
        std::shared_ptr<aeron::Subscription> subscription;

        // Try to initialize Aeron safely
        try {
            Logger::getInstance().log("[Main] Initializing Aeron...");
            auto ctx = std::make_shared<aeron::Context>();
            ctx->aeronDir("/dev/shm/aeron-it-admin");
            
            // Add error handler
            ctx->errorHandler([](const std::exception& e) {
                Logger::getInstance().log(std::string("[AeronError] ") + e.what());
            });

            aeronClient = aeron::Aeron::connect(*ctx);
            if (!aeronClient) {
                throw std::runtime_error("Failed to connect Aeron client");
            }

            // Create Aeron publication
            std::int64_t pubId = aeronClient->addPublication(
                "aeron:udp?endpoint=172.17.10.58:50000|reliable=true|mtu=1408|term-length=64k", 
                1001);
            
            Logger::getInstance().log("[Main] Publication ID created: " + std::to_string(pubId));
            publication = aeronClient->findPublication(pubId);

            // Wait for publication to be ready
            int attempts = 0;
            while (!publication && attempts++ < 100) {
                Logger::getInstance().log("[Main] Waiting for publication, attempt " + std::to_string(attempts));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                publication = aeronClient->findPublication(pubId);
            }

            if (!publication) {
                throw std::runtime_error("Timeout waiting for publication");
            }

            Logger::getInstance().log("[Main] Publication ready after " + std::to_string(attempts) + " attempts");
            
            // Create Aeron subscription
            std::int64_t subId = aeronClient->addSubscription("aeron:udp?endpoint=0.0.0.0:10001", 1001);
            subscription = aeronClient->findSubscription(subId);
            
            // Wait for subscription to be ready
            attempts = 0;
            while (!subscription && attempts++ < 100) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                subscription = aeronClient->findSubscription(subId);
            }

            if (!subscription) {
                throw std::runtime_error("Timeout waiting for subscription");
            }
            Logger::getInstance().log("[Main] Aeron subscription created");

            // Start threads with proper resource capturing
            std::thread t2([aeronClient, publication]() {
                jsonToSbeSenderThread(publication);
            });
            t2.detach();
            Logger::getInstance().log("[Main] JSON sender thread started");

            

            // Start receiver thread
            std::thread t3([aeronClient, subscription]() {
                aeronReceiverThread(subscription);
            });
            t3.detach();
            Logger::getInstance().log("[Main] Aeron receiver thread started");

        } catch (const std::exception &e) {
            Logger::getInstance().log(std::string("[Main] Aeron initialization failed: ") + e.what());
            Logger::getInstance().log("[Main] Continuing without Aeron backend");
        }

        // Configure and start Drogon HTTP server
        Logger::getInstance().log("[Main] Starting Drogon server on port 8080");
        drogon::app()
            .addListener("0.0.0.0", 8080)
            .setThreadNum(1)
            .registerHandler("/api/identity", 
                [](const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
                    RequestHandler handler;
                    handler.DataPass(req, std::move(callback));
                })
            .registerPostHandlingAdvice([](const drogon::HttpRequestPtr&, const drogon::HttpResponsePtr&) {
                // Keep alive check
                if (!running) {
                    drogon::app().quit();
                }
            })
            .run();

    } catch (const std::exception &e) {
        Logger::getInstance().log(std::string("[Main] Critical error: ") + e.what());
        return 1;
    }

    // Cleanup
    running = false;
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Allow threads to finish
    
    Logger::getInstance().log("[Main] Shutdown complete");
    return 0;
}