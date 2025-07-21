#include "QueueManager.h"
#include "Logger.h"
#include <thread>
#include "JsonToSbeSender.h"
#include <nlohmann/json.hpp>
#include "Aeron.h"
#include "Publication.h"
#include <memory>
#include <chrono>
#include "IdentityMessage.h"
#include "MessageHeader.h"

using json = nlohmann::json;

void jsonToSbeSenderThread(std::shared_ptr<aeron::Publication> publication) {
    GatewayTask task;
    while (true) {
        if (ReceiverQueue.try_dequeue(task)) {
            Logger::getInstance().log("[T2] Dequeued task for processing");
           
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                    try {
                        if (publication->isClosed()) {
                            Logger::getInstance().log("[TestThread] Publication closed");
                            break;
                        }
                        
                        // Parse JSON and convert to SBE
                        auto parsed = json::parse(task.json);
                        Logger::getInstance().log("[T2] JSON parsed successfully");

                        // Create SBE buffer with proper size
                        const size_t bufferSize = my::app::messages::MessageHeader::encodedLength() + 
                                                my::app::messages::IdentityMessage::sbeBlockLength();
                        std::vector<uint8_t> rawBuffer(bufferSize);
                        
                        // Create Aeron buffer
                        aeron::concurrent::AtomicBuffer atomicBuffer(rawBuffer.data(), rawBuffer.size());
                        
                        // Encode the message header
                        my::app::messages::MessageHeader headerEncoder;
                        headerEncoder.wrap(reinterpret_cast<char*>(rawBuffer.data()), 0, 0, rawBuffer.size())
                            .blockLength(my::app::messages::IdentityMessage::sbeBlockLength())
                            .templateId(my::app::messages::IdentityMessage::sbeTemplateId())
                            .schemaId(my::app::messages::IdentityMessage::sbeSchemaId())
                            .version(my::app::messages::IdentityMessage::sbeSchemaVersion());
                        
                        // Encode the IdentityMessage
                        my::app::messages::IdentityMessage identityEncoder;
                        identityEncoder.wrapForEncode(
                            reinterpret_cast<char*>(rawBuffer.data()), 
                            my::app::messages::MessageHeader::encodedLength(), 
                            rawBuffer.size()
                        );
                        
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
                            identityEncoder.dateOfIssue().putCharVal(parsed["dateOfIssue"].get<std::string>());
                        }
                        if (parsed.contains("dateOfExpiry")) {
                            identityEncoder.dateOfExpiry().putCharVal(parsed["dateOfExpiry"].get<std::string>());
                        }
                        if (parsed.contains("address")) {
                            identityEncoder.address().putCharVal(parsed["address"].get<std::string>());
                        }
                        if (parsed.contains("verified")) {
                            identityEncoder.verified().putCharVal(parsed["verified"].get<std::string>());
                        }
                        
                        // Send the encoded message
                        const size_t totalLength = headerEncoder.encodedLength() + identityEncoder.encodedLength();
                        int64_t result = publication->offer(atomicBuffer, 0, totalLength);
                        
                        if (result < 0) {
                            Logger::getInstance().log("[TestThread] Failed to send: " + std::to_string(result));
                        } else {
                            Logger::getInstance().log("[TestThread] SBE message sent successfully, total length: " + std::to_string(totalLength));
                        }
                        
                    } catch (const std::exception& e) {
                        Logger::getInstance().log(std::string("[TestThread] Error: ") + e.what());
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                
           
          
            try {
                auto parsed = json::parse(task.json);
                Logger::getInstance().log("[T2] JSON parsed successfully");

                // TODO: integrate SBE encoding of parsed JSON into buffer here in future
                // For now, create a simple echo response
                json response;
                response["echo"] = parsed;
                response["processed"] = true;
                response["status"] = "success";
                response["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();

                std::string responseJson = response.dump();
                Logger::getInstance().log("[T2] Created response: " + responseJson);

                // Create response task and push to ResponseQueue
                GatewayTask responseTask;
                responseTask.json = responseJson;
                responseTask.request = task.request;  // Keep original request for context
                responseTask.callback = task.callback;  // Keep the callback to send response
                
                ResponseQueue.enqueue(responseTask);
                Logger::getInstance().log("[T2] Response enqueued for dispatch");

            } catch (const std::exception &e) {
                Logger::getInstance().log(std::string("[T2] JSON parse error: ") + e.what());
                
                // Create error response
                json errorResponse;
                errorResponse["error"] = "Invalid JSON";
                errorResponse["message"] = e.what();
                errorResponse["status"] = "error";
                
                GatewayTask errorTask;
                errorTask.json = errorResponse.dump();
                errorTask.request = task.request;
                errorTask.callback = task.callback;
                
                ResponseQueue.enqueue(errorTask);
                Logger::getInstance().log("[T2] Error response enqueued for dispatch");
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}