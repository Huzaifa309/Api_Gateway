#include "JsonToSbeSender.h"
#include "RegistrationRequest.h"
#include "MessageHeader.h"
#include "Logger.h"
#include "QueueManager.h"
#include "aeron_wrapper.h"
#include "ThreadPool.h"
#include "ThreadPoolConfig.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

using json = nlohmann::json;

static std::unique_ptr<ThreadPool> sbeEncodingThreadPool;

void encodeJsonToSbe(const std::string& jsonPayload,
                     std::shared_ptr<aeron_wrapper::Publication> publication,
                     GatewayTask callbackTask) {
    try {
        auto parsed = json::parse(jsonPayload);
        Logger::getInstance().log("[T2-Worker] JSON parsed successfully");

        // Support both nested and flat payloads
        const json& rr = parsed.contains("RegistrationRequest") ? parsed["RegistrationRequest"] : parsed;
        const json& hdr = rr.contains("hdr")
                            ? rr["hdr"]
                            : (rr.contains("header") ? rr["header"] : json::object());

        // Allocate exact fixed size (header + block)
        const size_t bufferSize = SBE::RegistrationRequest::sbeBlockAndHeaderLength();
        std::vector<uint8_t> rawBuffer(bufferSize);

        // Wrap and write SBE transport header automatically
        SBE::RegistrationRequest req;
        req.wrapAndApplyHeader(reinterpret_cast<char*>(rawBuffer.data()), 0, rawBuffer.size());

        // Populate AppHeader
        SBE::AppHeader& h = req.header();
        if (hdr.contains("version"))      h.version(hdr["version"].get<uint16_t>());
        if (hdr.contains("messageType"))  h.putMessageType(hdr["messageType"].get<std::string>());
        if (hdr.contains("messageId"))    h.putMessageId(hdr["messageId"].get<std::string>());
        if (hdr.contains("messageCode"))  h.putMessageCode(hdr["messageCode"].get<std::string>());
        if (hdr.contains("sequence"))     h.sequence(hdr["sequence"].get<uint32_t>());
        if (hdr.contains("timestamp"))    h.timestamp(hdr["timestamp"].get<uint64_t>());
        if (hdr.contains("responseCode"))          h.responseCode(hdr["responseCode"].get<uint16_t>());
        if (hdr.contains("responseDescription"))   h.putResponseDescription(hdr["responseDescription"].get<std::string>());
        if (hdr.contains("deviceId"))     h.putDeviceId(hdr["deviceId"].get<std::string>());
        if (hdr.contains("deviceName"))   h.putDeviceName(hdr["deviceName"].get<std::string>());
        if (hdr.contains("deviceIp"))     h.putDeviceIp(hdr["deviceIp"].get<std::string>());
        if (hdr.contains("location"))     h.putLocation(hdr["location"].get<std::string>());

        // Populate message payload
        if (rr.contains("phoneNumber")) {
            req.phoneNumber().putV(rr["phoneNumber"].get<std::string>());
        }

        // Compute total length (transport header + message)
        const size_t totalLength =
            SBE::MessageHeader::encodedLength() + req.encodedLength();

        auto result = publication->offer(rawBuffer.data(), totalLength);
        if (result != aeron_wrapper::PublicationResult::SUCCESS) {
            Logger::getInstance().log("[T2-Worker] Failed to send message");
        } else {
            Logger::getInstance().log("[T2-Worker] SBE message sent, bytes: " +
                                      std::to_string(totalLength));
        }

        // Enqueue callback task for T3
        CallBackQueue.enqueue(std::move(callbackTask));

    } catch (const std::exception& e) {
        Logger::getInstance().log(std::string("[T2-Worker] Error: ") + e.what());
    }
}

void jsonToSbeSenderThread(
    std::shared_ptr<aeron_wrapper::Publication> publication) {

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

            std::string jsonPayload = std::move(task.json);

            sbeEncodingThreadPool->enqueue_void([jsonPayload = std::move(jsonPayload),
                                                 publication,
                                                 callbackTask = std::move(task)]() mutable {
                Logger::getInstance().log("[T2-Worker] Starting parallel SBE encoding");
                encodeJsonToSbe(jsonPayload, publication, std::move(callbackTask));
            });
        } else {
            std::this_thread::yield();
        }
    }
}