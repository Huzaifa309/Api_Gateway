#include "ResponseDispatcher.h"
#include "Logger.h"
#include "QueueManager.h"
#include <chrono>
#include <drogon/HttpResponse.h>
#include <thread>
using namespace std;
void responseDispatcherThread() {
  Logger::getInstance().log("[T4] Response dispatcher thread started");

  GatewayTask task;
  while (true) {
    if (ResponseQueue.try_dequeue(task)) {
      Logger::getInstance().log("[T4] Dequeued response task for dispatch");

      try {
        // Check if this is an Aeron response (no callback) or HTTP response
        // (with callback)
        GatewayTask callbackTask;
        if (CallBackQueue.try_dequeue(callbackTask)) {
          Logger::getInstance().log("[T4] Dequeued callback task for dispatch");
        } else {
          Logger::getInstance().log("[T4] No callback task dequeued");
        }

        // HTTP response - send via callback
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody(task.json);
        cout << "ResponseDispatcher data: " << task.json << endl;
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);

        callbackTask.callback(resp);
        Logger::getInstance().log("[T4] HTTP response sent successfully");

        // Aeron response - log and potentially store for future use
        Logger::getInstance().log("[T4] Aeron SBE response received: " +
                                  task.json);

        // TODO: Here you could implement additional processing for Aeron
        // responses such as storing in a database, forwarding to other
        // services, etc.

      } catch (const std::exception &e) {
        Logger::getInstance().log(
            std::string("[T4] Error processing response: ") + e.what());

        // Send error response only if there's a callback (HTTP request)
        if (task.callback) {
          auto errorResp = drogon::HttpResponse::newHttpResponse();
          errorResp->setStatusCode(drogon::k500InternalServerError);
          errorResp->setBody("{\"error\": \"Internal server error\"}");
          errorResp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
          task.callback(errorResp);
        }
      }
    } else {
      // High-performance: yield instead of sleep
      std::this_thread::yield();
    }
  }
}
