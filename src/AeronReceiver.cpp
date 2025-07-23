#include "QueueManager.h"
#include "Logger.h"
#include "Aeron.h"
#include "FragmentAssembler.h"
#include "Subscription.h"
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include "IdentityMessage.h"
#include "MessageHeader.h"
using namespace aeron;
using json = nlohmann::json;

void aeronReceiverThread(std::shared_ptr<Subscription> subscription)
{
    Logger::getInstance().log("[T3] Aeron receiver thread started");

    fragment_handler_t handler = [&](AtomicBuffer &buffer, std::int32_t offset, std::int32_t length, const Header &header)
    {
        using namespace my::app::messages;
        try {
            // 1. Wrap the header at the correct offset
            MessageHeader msgHeader;
            msgHeader.wrap(reinterpret_cast<char*>(buffer.buffer()), offset, 0, buffer.capacity());
            offset += msgHeader.encodedLength();
            // 2. Check template ID and decode the message
            if (msgHeader.templateId() == IdentityMessage::sbeTemplateId()) {
                IdentityMessage identity;
                identity.wrapForDecode(reinterpret_cast<char*>(buffer.buffer()), offset, msgHeader.blockLength(), msgHeader.version(), buffer.capacity());
                // 3. Extract fields
                std::string msg = identity.msg().getCharValAsString();
                std::string type = identity.type().getCharValAsString();
                std::string id = identity.id().getCharValAsString();
                std::string name = identity.name().getCharValAsString();
                std::string dateOfIssue = identity.dateOfIssue().getCharValAsString();
                std::string dateOfExpiry = identity.dateOfExpiry().getCharValAsString();
                std::string address = identity.address().getCharValAsString();
                std::string verified = identity.verified().getCharValAsString();
                
                Logger::getInstance().log("[T3] SBE Decoded: msg=" + msg + ", type=" + type + ", id=" + id + ", name=" + name + ", dateOfIssue=" + dateOfIssue + ", dateOfExpiry=" + dateOfExpiry + ", address=" + address + ", verified=" + verified);
                
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
                responseJson["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();
                
                std::string jsonString = responseJson.dump();
                Logger::getInstance().log("[T3] JSON created: " + jsonString);
                
                // Create GatewayTask and enqueue in ResponseQueue
                GatewayTask responseTask;
                responseTask.json = jsonString;
                // responseTask.request = nullptr; // No HTTP request for Aeron responses
                // responseTask.callback = nullptr; // No callback for Aeron responses
                
                ResponseQueue.enqueue(responseTask);
                Logger::getInstance().log("[T3] Response enqueued in ResponseQueue");
                
            } else {
                Logger::getInstance().log("[T3] Unexpected template ID: " + std::to_string(msgHeader.templateId()));
            }
        } catch (const std::exception &e) {
            Logger::getInstance().log(std::string("[T3] SBE decode error: ") + e.what());
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
