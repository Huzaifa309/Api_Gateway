// File: src/messaging.cpp
#include "messaging.h"
#include <iostream>
#include <cstring>
#include <FragmentAssembler.h>
#include <chrono>
#include <vector>
#include "baseline/Order.h"

using std::vector;
using namespace eloan;

static constexpr const char *CHANNEL_IN  = "aeron:udp?endpoint=localhost:40123"; // where orders arrive
static constexpr const char *CHANNEL_OUT = "aeron:udp?endpoint=localhost:40124"; // where trade confirmations go
static constexpr const char *CHANNEL_IPC  = "aeron:ipc"; // where orders arrive
static constexpr std::int32_t STREAM_ID_IN = 1001;
static constexpr std::int32_t STREAM_ID_OUT = 1002;
static constexpr std::chrono::duration<long, std::milli> SLEEP_IDLE_MS(1);

Messaging::Messaging(){
};

Messaging::~Messaging() {
    shutdown();
}

bool Messaging::initialize(std::unique_ptr<LoggerWrapper>& logWrapper_) {
    try {
        logWrapper = logWrapper_.get();
        aeron::Context context;
        aeron_ = aeron::Aeron::connect(context);

        // Create a subscription for incoming messages (orders/trades)
        std::int64_t id = aeron_->addSubscription(CHANNEL_IPC, STREAM_ID_IN);
        subscription_ = aeron_->findSubscription(id);
        // wait for the subscription to be valid
        while (!subscription_)
        {
            std::this_thread::yield();
            subscription_ = aeron_->findSubscription(id);
        }
        logWrapper->debug_fast(4, "[Messaging] Subscribed. Sub Id: {}", id);

        // Create a publication for outgoing messages (trade confirmations)
        id = aeron_->addPublication(CHANNEL_IPC, STREAM_ID_OUT);
        publication_ = aeron_->findPublication(id);
        // wait for the publication to be valid
        while (!publication_)
        {
            std::this_thread::yield();
            publication_ = aeron_->findPublication(id);
        }
        logWrapper->debug_fast(4, "[Messaging] Published. Pub Id: {}", id);
    }
    catch (const std::exception &ex) {
        logWrapper->error_fast(4, "[Messaging] Aeron initialization failed: {}", ex.what());
        return false;
    }

    running_ = true;
    listenerThread_ = std::thread(&Messaging::listenerLoop, this);
    logWrapper->debug(4, "[Messaging] Aeron initialized and listener thread started");
    return true;
}

aeron::fragment_handler_t Messaging::fragHandler() {
    return  [&](const aeron::AtomicBuffer &buffer,
                           std::int32_t offset,
                           std::int32_t length,
                           const aeron::Header &header) {
        if (length < sizeof(std::uint8_t)) {
            return; // too small to read any header
        }
        logWrapper->debug(4, "-----Got New Message-----");
        //dangerous, it will overflow after
        uint8_t shardId = (++_shard_counter) % (NUM_SHARDS);
        logWrapper->debug_fast(4, "enqueued, shardId: {}", shardId);
        sharded_queue[shardId].enqueue(buffer, offset, length);
    };
}


void Messaging::listenerLoop() {
    logWrapper->debug_fast(4, "[Messaging] listenerLoop started");
    aeron::FragmentAssembler fragmentAssembler(fragHandler());
    aeron::fragment_handler_t handler = fragmentAssembler.handler();
    aeron::SleepingIdleStrategy sleepStrategy(SLEEP_IDLE_MS);
    while (running_) {
        // Poll up to 10 fragments per iteration
        std::int32_t fragmentsRead = subscription_->poll(handler, MAX_FRAGMENT_BATCH_SIZE);
        sleepStrategy.idle(fragmentsRead);
    }
    logWrapper->debug_fast(4, "[Messaging] Listener thread exiting");
}

bool Messaging::sendTradeExecution(const TradeExecution &trade) {
    // Serialize: 1 byte for type, then struct bytes
    std::uint8_t msgType = 2;
    std::uint8_t bufferData[1 + sizeof(TradeExecution)];
    bufferData[0] = msgType;
    std::memcpy(bufferData + 1, &trade, sizeof(TradeExecution));

    aeron::AtomicBuffer srcBuffer(bufferData, sizeof(bufferData));
    std::int64_t result = publication_->offer(srcBuffer, 0, sizeof(bufferData));
    if (result < 0) {
        logWrapper->error_fast(4, "[Messaging] Failed to send trade execution; offer returned {}", result);
        return false;
    }
    return true;
}
bool Messaging::SendMessage(const Order& order) {
    //declaring a vector of uint8_t, representing plain byte buffer having size equal or greater than to the size of complete order message and SBE message order
    vector<uint8_t> raw_buffer(baseline::MessageHeader::encodedLength() + baseline::Order::sbeBlockLength() + order.side.size() + order.symbol.size() + sizeof(std::int64_t));
    aeron::concurrent::AtomicBuffer atomic_buffer(raw_buffer.data(), raw_buffer.size());

    baseline::MessageHeader header_encoder;
    header_encoder.wrap(reinterpret_cast<char*>(raw_buffer.data()), 0, 0, raw_buffer.size())
        .blockLength(baseline::Order::sbeBlockLength())
        .templateId(baseline::Order::sbeTemplateId())
        .schemaId(baseline::Order::sbeSchemaId())
        .version(baseline::Order::sbeSchemaVersion());

    baseline::Order order_encoder;
    order_encoder.wrapForEncode(reinterpret_cast<char*>(raw_buffer.data()), baseline::MessageHeader::encodedLength(), raw_buffer.size());
    order_encoder.order_id(order.order_id);
    order_encoder.account_id(order.account_id);
    order_encoder.instrument_id(order.instrument_id);
    order_encoder.quantity(order.quantity);
    order_encoder.price(order.price);
    order_encoder.putSymbol(order.symbol);
    order_encoder.putSide(order.side);

    std::int64_t result = publication_->offer(atomic_buffer, 0, header_encoder.encodedLength() + order_encoder.encodedLength());
    if (result > 0)
    {
        logWrapper->debug_fast(4, "yay! Message sent successfully!");
    }
    else if (BACK_PRESSURED == result)
    {
        logWrapper->error_fast(4, "Offer failed due to back pressure");
        return false;
    }
    else if (NOT_CONNECTED == result)
    {
        logWrapper->error_fast(4, "Offer failed because publisher is not connected to a subscriber");
        return false;
    }
    else if (ADMIN_ACTION == result)
    {
        logWrapper->error_fast(4, "Offer failed because of an administration action in the system");
        return false;
    }
    else if (PUBLICATION_CLOSED == result)
    {
        logWrapper->error_fast(4, "Offer failed because publication is closed");
        return false;
    }
    else
    {
        logWrapper->error_fast(4, "Offer failed due to unknown reason {}", result);
        return false;
    }
    return true;
}


std::array<ShardedQueue, NUM_SHARDS>& Messaging::getQueue() {
        return  sharded_queue;
}


void Messaging::shutdown() {
    if (!running_) return;
    running_ = false;
    if (listenerThread_.joinable()) {
        listenerThread_.join();
    }
    publication_.reset();
    subscription_.reset();
    aeron_.reset();
    logWrapper->debug_fast(4, "[Messaging] Shutdown complete");
}
