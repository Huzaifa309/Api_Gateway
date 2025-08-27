#include "IdentityMessage.h"
#include "Logger.h"
#include "MessageHeader.h"
#include "QueueManager.h"
#include "aeron_wrapper.h"
#include "ThreadPool.h"
#include "ThreadPoolConfig.h"
#include <chrono>
#include <nlohmann/json.hpp>
#include <thread>
#include <drogon/HttpResponse.h>
using json = nlohmann::json;

// Thread pool for parallel SBE decoding and response processing within T3
static std::unique_ptr<ThreadPool> sbeDecodingThreadPool;

// Function to decode a single SBE message and send response
void decodeSbeAndSendResponse(const aeron_wrapper::FragmentData& fragment) {
    using namespace my::app::messages;
    try {
        // 1. Wrap the header at the correct offset
        const uint8_t *data = fragment.buffer;
        size_t length = fragment.length;

        MessageHeader msgHeader;
        msgHeader.wrap(const_cast<char *>(reinterpret_cast<const char *>(data)),
                       0, 0, length);
        size_t headerOffset = msgHeader.encodedLength();

        // 2. Check template ID and decode the message
        if (msgHeader.templateId() == IdentityMessage::sbeTemplateId()) {
            IdentityMessage identity;
            identity.wrapForDecode(
                const_cast<char *>(reinterpret_cast<const char *>(data)),
                headerOffset, msgHeader.blockLength(), msgHeader.version(), length);
            
            // 3. Extract fields
            std::string msg = identity.msg().getCharValAsString();
            std::string type = identity.type().getCharValAsString();
            std::string id = identity.id().getCharValAsString();
            std::string name = identity.name().getCharValAsString();
            std::string dateOfIssue = identity.dateOfIssue().getCharValAsString();
            std::string dateOfExpiry = identity.dateOfExpiry().getCharValAsString();
            std::string address = identity.address().getCharValAsString();
            std::string verified = identity.verified().getCharValAsString();

            Logger::getInstance().log(
                "[T3-Worker] SBE Decoded: msg=" + msg + ", type=" + type + ", id=" + id +
                ", name=" + name + ", dateOfIssue=" + dateOfIssue +
                ", dateOfExpiry=" + dateOfExpiry + ", address=" + address +
                ", verified=" + verified);

            // 4. Convert to JSON
            json responseJson;
            responseJson["msg"] = msg;
            responseJson["type"] = type;
            responseJson["id"] = id;
            responseJson["name"] = name;
            responseJson["dateOfIssue"] = dateOfIssue;
            responseJson["dateOfExpiry"] = dateOfExpiry;
            responseJson["address"] = address;
            responseJson["verified"] = verified;
            responseJson["source"] = "aeron_sbe";
            responseJson["timestamp"] =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();

            std::string jsonString = responseJson.dump();
            Logger::getInstance().log("[T3-Worker] JSON created: " + jsonString);

            // Direct HTTP response dispatch (no ResponseQueue, no T4)
            GatewayTask callbackTask;
            auto callbackResult = CallBackQueue.dequeue();
            if (callbackResult.has_value()) {
                callbackTask = std::get<GatewayTask>(callbackResult.value());
                Logger::getInstance().log("[T3-Worker] Dequeued callback task for dispatch");

                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setBody(jsonString);
                resp->setStatusCode(drogon::k200OK);
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);

                callbackTask.callback(resp);
                Logger::getInstance().log("[T3-Worker] HTTP response sent successfully");
                Logger::getInstance().log("[T3-Worker] Aeron SBE response handled directly");
            } else {
                Logger::getInstance().log("[T3-Worker] No callback task available; dropping response");
            }

        } else {
            Logger::getInstance().log("[T3-Worker] Unexpected template ID: " +
                                      std::to_string(msgHeader.templateId()));
        }
    } catch (const std::exception &e) {
        Logger::getInstance().log(std::string("[T3-Worker] SBE decode error: ") +
                                  e.what());
    }
}

void aeronReceiverThread(
    std::shared_ptr<aeron_wrapper::Subscription> subscription) {
    
    // Initialize thread pool if not already done
    if (!sbeDecodingThreadPool) {
        sbeDecodingThreadPool = std::make_unique<ThreadPool>(SBE_DECODING_THREAD_POOL_SIZE);
        Logger::getInstance().log("[T3] SBE decoding thread pool initialized with " + 
                                 std::to_string(SBE_DECODING_THREAD_POOL_SIZE) + " workers");
    }

    Logger::getInstance().log("[T3] Aeron receiver thread started");

    aeron_wrapper::FragmentHandler handler = [&](const aeron_wrapper::FragmentData
                                                   &fragment) {
        // Submit to thread pool for parallel SBE decoding and response processing
        sbeDecodingThreadPool->enqueue_void([fragment]() {
            Logger::getInstance().log("[T3-Worker] Starting parallel SBE decoding");
            decodeSbeAndSendResponse(fragment);
        });
    };

    while (true) {
        try {
            int fragmentsRead = subscription->poll(handler, 1000);

            // High-performance: only yield if no work was done
            if (fragmentsRead == 0) {
                std::this_thread::yield();
            }
        } catch (const std::exception &e) {
            Logger::getInstance().log(std::string("[T3] Aeron poll error: ") +
                                      e.what());
        }
    }
}
