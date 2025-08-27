#include "JsonToSbeSender.h"
#include "IdentityMessage.h"
#include "Logger.h"
#include "MessageHeader.h"
#include "QueueManager.h"
#include "aeron_wrapper.h"
#include "ThreadPool.h"
#include "ThreadPoolConfig.h"
#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <thread>

using json = nlohmann::json;

// Thread pool for parallel SBE encoding within T2
static std::unique_ptr<ThreadPool> sbeEncodingThreadPool;

// Function to encode a single JSON message to SBE format
void encodeJsonToSbe(const std::string& jsonPayload, 
                     std::shared_ptr<aeron_wrapper::Publication> publication,
                     GatewayTask callbackTask) {
    try {
        // Parse JSON and convert to SBE
        auto parsed = json::parse(jsonPayload);
        Logger::getInstance().log("[T2-Worker] JSON parsed successfully");

        // Create SBE buffer with proper size
        const size_t bufferSize =
            my::app::messages::MessageHeader::encodedLength() +
            my::app::messages::IdentityMessage::sbeBlockLength();

        std::vector<uint8_t> rawBuffer(bufferSize);

        // Encode the message header
        my::app::messages::MessageHeader headerEncoder;
        headerEncoder
            .wrap(reinterpret_cast<char *>(rawBuffer.data()), 0, 0,
                  rawBuffer.size())
            .blockLength(my::app::messages::IdentityMessage::sbeBlockLength())
            .templateId(my::app::messages::IdentityMessage::sbeTemplateId())
            .schemaId(my::app::messages::IdentityMessage::sbeSchemaId())
            .version(my::app::messages::IdentityMessage::sbeSchemaVersion());

        // Encode the IdentityMessage
        my::app::messages::IdentityMessage identityEncoder;
        identityEncoder.wrapForEncode(
            reinterpret_cast<char *>(rawBuffer.data()),
            my::app::messages::MessageHeader::encodedLength(),
            rawBuffer.size());

        // Set the fields from JSON
        if (parsed.contains("msg")) {
            identityEncoder.msg().putCharVal(parsed["msg"].get<std::string>());
        }
        if (parsed.contains("type")) {
            identityEncoder.type().putCharVal(parsed["type"].get<std::string>());
        }
        if (parsed.contains("id")) {
            identityEncoder.id().putCharVal(parsed["id"].get<std::string>());
        }
        if (parsed.contains("name")) {
            identityEncoder.name().putCharVal(parsed["name"].get<std::string>());
        }
        if (parsed.contains("dateOfIssue")) {
            identityEncoder.dateOfIssue().putCharVal(
                parsed["dateOfIssue"].get<std::string>());
        }
        if (parsed.contains("dateOfExpiry")) {
            identityEncoder.dateOfExpiry().putCharVal(
                parsed["dateOfExpiry"].get<std::string>());
        }
        if (parsed.contains("address")) {
            identityEncoder.address().putCharVal(
                parsed["address"].get<std::string>());
        }
        if (parsed.contains("verified")) {
            identityEncoder.verified().putCharVal(
                parsed["verified"].get<std::string>());
        }

        // Send the encoded message
        const size_t totalLength =
            headerEncoder.encodedLength() + identityEncoder.encodedLength();
        aeron_wrapper::PublicationResult result =
            publication->offer(rawBuffer.data(), totalLength);

        if (result != aeron_wrapper::PublicationResult::SUCCESS) {
            Logger::getInstance().log("[T2-Worker] Failed to send message");
        } else {
            Logger::getInstance().log(
                "[T2-Worker] SBE message sent successfully, total length: " +
                std::to_string(totalLength));
        }

        // Enqueue callback task for T3
        CallBackQueue.enqueue(std::move(callbackTask));

    } catch (const std::exception &e) {
        Logger::getInstance().log(std::string("[T2-Worker] Error: ") + e.what());
    }
}

void jsonToSbeSenderThread(
    std::shared_ptr<aeron_wrapper::Publication> publication) {
    
    // Initialize thread pool if not already done
    if (!sbeEncodingThreadPool) {
        sbeEncodingThreadPool = std::make_unique<ThreadPool>(SBE_ENCODING_THREAD_POOL_SIZE);
        Logger::getInstance().log("[T2] SBE encoding thread pool initialized with " + 
                                 std::to_string(SBE_ENCODING_THREAD_POOL_SIZE) + " workers");
    }

    while (true) {
        auto result = ReceiverQueue.dequeue();
        if (result.has_value()) {
            auto task = std::get<GatewayTask>(result.value());
            Logger::getInstance().log("[T2] Dequeued task for processing");

            // Move JSON payload out to avoid extra copy while allowing task to be moved later
            std::string jsonPayload = std::move(task.json);

            // Submit to thread pool for parallel SBE encoding
            sbeEncodingThreadPool->enqueue_void([jsonPayload = std::move(jsonPayload), 
                                                publication, 
                                                callbackTask = std::move(task)]() mutable {
                Logger::getInstance().log("[T2-Worker] Starting parallel SBE encoding");
                encodeJsonToSbe(jsonPayload, publication, std::move(callbackTask));
            });

        } else {
            // High-performance: yield instead of sleep
            std::this_thread::yield();
        }
    }
}