#include "AeronReceiver.h"
#include "JsonToSbeSender.h"
#include "Logger.h"
#include "QueueManager.h"
#include "RequestHandler.h"
#include "ResponseDispatcher.h"
#include "aeron_wrapper.h"
#include <atomic>
#include <chrono>
#include <drogon/drogon.h>
#include <memory>
#include <thread>
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
    std::shared_ptr<aeron_wrapper::Aeron> aeronClient;
    std::shared_ptr<aeron_wrapper::Publication> publication;
    std::shared_ptr<aeron_wrapper::Subscription> subscription;

    // Try to initialize Aeron safely
    try {
      Logger::getInstance().log("[Main] Initializing Aeron...");
      aeronClient =
          std::make_shared<aeron_wrapper::Aeron>("/dev/shm/aeron-huzaifa");
      if (!aeronClient) {
        throw std::runtime_error("Failed to connect Aeron client");
      }

      // Create Aeron publication
      publication = aeronClient->create_publication("aeron:ipc", 1001);
      if (!publication) {
        throw std::runtime_error("Failed to create publication");
      }
      Logger::getInstance().log("[Main] Publication created successfully");

      // Create Aeron subscription
      subscription = aeronClient->create_subscription("aeron:ipc", 1001);
      if (!subscription) {
        throw std::runtime_error("Failed to create subscription");
      }
      Logger::getInstance().log("[Main] Aeron subscription created");

      // Start threads with proper resource capturing
      std::thread t2(
          [aeronClient, publication]() { jsonToSbeSenderThread(publication); });
      t2.detach();
      Logger::getInstance().log("[Main] JSON sender thread started");

      // Start receiver thread
      std::thread t3(
          [aeronClient, subscription]() { aeronReceiverThread(subscription); });
      t3.detach();
      Logger::getInstance().log("[Main] Aeron receiver thread started");

    } catch (const std::exception &e) {
      Logger::getInstance().log(
          std::string("[Main] Aeron initialization failed: ") + e.what());
      Logger::getInstance().log("[Main] Continuing without Aeron backend");
    }

    // Configure and start Drogon HTTP server
    Logger::getInstance().log("[Main] Starting Drogon server on port 8080");
    drogon::app()
        .addListener("0.0.0.0", 8080)
        .setThreadNum(1)
        .registerPostHandlingAdvice([](const drogon::HttpRequestPtr &,
                                       const drogon::HttpResponsePtr &) {
          // Keep alive check
          if (!running) {
            drogon::app().quit();
          }
        })
        .run();

  } catch (const std::exception &e) {
    Logger::getInstance().log(std::string("[Main] Critical error: ") +
                              e.what());
    return 1;
  }

  // Cleanup
  running = false;
  std::this_thread::sleep_for(
      std::chrono::seconds(1)); // Allow threads to finish

  Logger::getInstance().log("[Main] Shutdown complete");
  return 0;
}