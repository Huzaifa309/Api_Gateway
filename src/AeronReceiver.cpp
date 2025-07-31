#include "IdentityMessage.h"
#include "Logger.h"
#include "MessageHeader.h"
#include "QueueManager.h"
#include "aeron_wrapper.h"
#include <chrono>
#include <nlohmann/json.hpp>
#include <thread>
using json = nlohmann::json;

void aeronReceiverThread(
    std::shared_ptr<aeron_wrapper::Subscription> subscription) {
  Logger::getInstance().log("[T3] Aeron receiver thread started");

  aeron_wrapper::FragmentHandler handler = [&](const aeron_wrapper::FragmentData
                                                   &fragment) {
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
            "[T3] SBE Decoded: msg=" + msg + ", type=" + type + ", id=" + id +
            ", name=" + name + ", dateOfIssue=" + dateOfIssue +
            ", dateOfExpiry=" + dateOfExpiry + ", address=" + address +
            ", verified=" + verified);

        // 4. Convert to JSON and enqueue in ResponseQueue
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
        Logger::getInstance().log("[T3] JSON created: " + jsonString);

        // Create GatewayTask and enqueue in ResponseQueue
        GatewayTask responseTask;
        responseTask.json = jsonString;
        // responseTask.request = nullptr; // No HTTP request for Aeron
        // responses responseTask.callback = nullptr; // No callback for Aeron
        // responses

        ResponseQueue.enqueue(responseTask);
        Logger::getInstance().log("[T3] Response enqueued in ResponseQueue");

      } else {
        Logger::getInstance().log("[T3] Unexpected template ID: " +
                                  std::to_string(msgHeader.templateId()));
      }
    } catch (const std::exception &e) {
      Logger::getInstance().log(std::string("[T3] SBE decode error: ") +
                                e.what());
    }
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
