#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>
#include "Aeron.h"
#include "Context.h"
#include "Subscription.h"
#include "FragmentAssembler.h"
#include "IdentityMessage.h"
#include "MessageHeader.h"

void fragmentHandler(const aeron::AtomicBuffer& buffer,
    aeron::util::index_t offset,
    aeron::util::index_t length,
    const aeron::Header& aeronHeader)
{
    using namespace my::app::messages;
    
    std::cout << "\n[SBE] Received " << length << " bytes" << std::endl;
    
    // Get the base pointer for the message
    const char* basePtr = reinterpret_cast<const char*>(buffer.buffer()) + offset;
    
    // Wrap the message header
    MessageHeader header;
    header.wrap(
        const_cast<char*>(basePtr),           // Use basePtr instead of buffer.buffer()
        0,                                     // Start from beginning of message
        length,                                // Use actual message length
        MessageHeader::sbeSchemaVersion()      // Schema version
    );
    
    std::cout << "  Template ID: " << header.templateId()
              << ", Schema ID: " << header.schemaId()
              << ", Version: " << header.version()
              << ", Block Length: " << header.blockLength() << std::endl;
    
    if (header.templateId() == IdentityMessage::sbeTemplateId())
    {
        // Calculate the correct offset for the message body
        const size_t messageOffset = MessageHeader::encodedLength();
        
        IdentityMessage identity;
        identity.wrapForDecode(
            const_cast<char*>(basePtr),                    // Use basePtr
            messageOffset,                                 // Start after header
            header.blockLength(),                          // Use header's block length
            header.version(),                              // Use header's version
            length                                         // Use actual message length
        );
        
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S") << "] Decoded IdentityMessage:\n";
        
        auto printField = [](const char* label, const auto& field) {
            std::cout << "  " << label << ": " << field.getCharValAsString() << std::endl;
        };
        
        printField("msg", identity.msg());
        printField("type", identity.type());
        printField("id", identity.id());
        printField("name", identity.name());
        printField("dateOfIssue", identity.dateOfIssue());
        printField("dateOfExpiry", identity.dateOfExpiry());
        printField("address", identity.address());
        printField("verified", identity.verified());
    }
    else
    {
        std::cerr << "[SBE] Unknown template ID: " << header.templateId() << std::endl;
    }
}

// Poller class encapsulates polling logic for Aeron subscription
class Poller {
public:
    Poller(std::shared_ptr<aeron::Subscription> subscription,
           std::function<void(const aeron::AtomicBuffer&, aeron::util::index_t, aeron::util::index_t, const aeron::Header&)> handler,
           std::atomic<bool>& runningFlag)
        : subscription_(std::move(subscription)),
          handler_(std::move(handler)),
          running_(runningFlag) {}
    
    void start() {
        aeron::FragmentAssembler fragmentAssembler(handler_);
        std::cout << "Waiting for messages... (Press Ctrl+C to stop)" << std::endl;
        
        while (running_) {
            int fragmentsRead = subscription_->poll(fragmentAssembler.handler(), 10);
            if (fragmentsRead > 0) {
                std::cout << "Polled " << fragmentsRead << " fragments" << std::endl;
            }
            
            if (subscription_->isConnected()) {
                static int connectLogCounter = 0;
                if (connectLogCounter++ % 1000 == 0) {
                    std::cout << "Subscription is connected to publishers" << std::endl;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        std::cout << "Shutting down subscriber..." << std::endl;
    }
    
private:
    std::shared_ptr<aeron::Subscription> subscription_;
    std::function<void(const aeron::AtomicBuffer&, aeron::util::index_t, aeron::util::index_t, const aeron::Header&)> handler_;
    std::atomic<bool>& running_;
};

std::atomic<bool> running{true};

void signalHandler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    running = false;
}

int main()
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::cout << "Starting Aeron Subscriber on 172.17.10.58:50000" << std::endl;
    
    try {
        // Create Aeron context
        auto ctx = std::make_shared<aeron::Context>();
        ctx->aeronDir("/dev/shm/aeron-it-admin"); // Match your publisher's directory
        
        // Connect to Aeron
        auto aeronClient = aeron::Aeron::connect(*ctx);
        if (!aeronClient) {
            std::cerr << "Failed to connect to Aeron" << std::endl;
            return 1;
        }
        
        std::cout << "Connected to Aeron media driver" << std::endl;
        
        // Create subscription - this should match your publisher's channel
        std::string channel = "aeron:udp?endpoint=0.0.0.0:50000|reliable=true";
        std::int32_t streamId = 1001;
        
        std::cout << "Creating subscription on channel: " << channel << std::endl;
        std::cout << "Stream ID: " << streamId << std::endl;
        
        std::int64_t subscriptionId = aeronClient->addSubscription(channel, streamId);
        
        // Wait for subscription to be ready
        std::shared_ptr<aeron::Subscription> subscription;
        int attempts = 0;
        while (!subscription && attempts < 100) {
            attempts++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            subscription = aeronClient->findSubscription(subscriptionId);
        }
        
        if (!subscription) {
            std::cerr << "Failed to create subscription after " << attempts << " attempts" << std::endl;
            return 1;
        }
        
        std::cout << "Subscription created successfully after " << attempts << " attempts" << std::endl;
        std::cout << "Subscription channel: " << subscription->channel() << std::endl;
        std::cout << "Subscription stream ID: " << subscription->streamId() << std::endl;
        
        // Create Poller and start polling
        Poller poller(subscription, fragmentHandler, running);
        poller.start();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 