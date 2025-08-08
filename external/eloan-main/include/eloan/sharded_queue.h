//
// Created by muhammad-abdullah on 6/4/25.
//

#ifndef SHARDED_QUEUE_H
#define SHARDED_QUEUE_H
#include <queue>
#include <concurrent/ringbuffer/OneToOneRingBuffer.h>
#include <variant>
#include <optional>
#include <string>
#include <functional>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <mutex>
#include <condition_variable>
#include <array>
#include <utility>

#include "utils/params.h"
#include "data_types.h"

// Forward declaration of GatewayTask
struct GatewayTask {
    std::string json;
    drogon::HttpRequestPtr request;
    std::function<void(const drogon::HttpResponsePtr &)> callback;
};

#endif //SHARDED_QUEUE_H

class ShardedQueue {
    public:
    ShardedQueue();
    ~ShardedQueue();
    void enqueue(aeron::concurrent::AtomicBuffer, int32_t, int32_t);
    void enqueue(const GatewayTask& task);
    void enqueue(GatewayTask&& task);
    std::optional<std::variant<Order, TradeExecution, GatewayTask>> dequeue();
    int size();
    private:
    static constexpr size_t BUFFER_SIZE = MAX_RING_BUFFER_SIZE + aeron::concurrent::ringbuffer::RingBufferDescriptor::TRAILER_LENGTH;
    std::array<uint8_t, BUFFER_SIZE> buffer;
    aeron::concurrent::AtomicBuffer _buffer;
    aeron::concurrent::ringbuffer::OneToOneRingBuffer _ring_buffer;
    
    // Separate queue for GatewayTask objects
    std::queue<GatewayTask> _gateway_task_queue;
    mutable std::mutex _gateway_queue_mutex;
    std::condition_variable _gateway_queue_cv;
};