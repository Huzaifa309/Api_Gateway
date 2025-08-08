//
// Created by muhammad-abdullah on 6/4/25.
//
#include<concurrent/BackOffIdleStrategy.h>
#include "sharded_queue.h"
#include "baseline/Order.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <utility>

ShardedQueue::ShardedQueue() : _buffer(buffer, BUFFER_SIZE), _ring_buffer(_buffer) {}

ShardedQueue::~ShardedQueue() = default;

void ShardedQueue::enqueue(aeron::concurrent::AtomicBuffer buffer, int32_t offset, int32_t length) {
    aeron::concurrent::BackoffIdleStrategy idleStrategy(100,1000);
    bool isWritten = false;
    auto start = std::chrono::high_resolution_clock::now();
    //int8_t msgType = buffer.getUInt8(offset);
    while(!isWritten) {
        isWritten = _ring_buffer.write(1, buffer, offset, length);
        if (isWritten) {
            return;
        }
        if (std::chrono::high_resolution_clock::now() - start >= std::chrono::microseconds(50)) {
            std::cerr << "retry timeout" << std::endl;
            return;
        }
        idleStrategy.idle();
    }
}

void ShardedQueue::enqueue(const GatewayTask& task) {
    std::lock_guard<std::mutex> lock(_gateway_queue_mutex);
    _gateway_task_queue.emplace(task);
    _gateway_queue_cv.notify_one();
}

void ShardedQueue::enqueue(GatewayTask&& task) {
    std::lock_guard<std::mutex> lock(_gateway_queue_mutex);
    _gateway_task_queue.emplace(std::move(task));
    _gateway_queue_cv.notify_one();
}

std::optional<std::variant<Order, TradeExecution, GatewayTask>> ShardedQueue::dequeue() {
    // First, try to dequeue from the GatewayTask queue
    {
        std::unique_lock<std::mutex> lock(_gateway_queue_mutex);
        if (!_gateway_task_queue.empty()) {
            GatewayTask task = std::move(_gateway_task_queue.front());
            _gateway_task_queue.pop();
            return std::make_optional<std::variant<Order, TradeExecution, GatewayTask>>(std::move(task));
        }
    }
    
    // If no GatewayTask, try the ring buffer for Order/TradeExecution
    std::optional<std::variant<Order, TradeExecution, GatewayTask>> result;
    _ring_buffer.read([&](int8_t msgType, aeron::concurrent::AtomicBuffer& buffer, int32_t offset, int32_t length)
    {
        if (msgType == 1) {
            // Handle Order message
            baseline::MessageHeader _message_header;
            baseline::Order _order_decoder;

            _message_header.wrap(reinterpret_cast<char*>(buffer.buffer()), offset, 0, buffer.capacity());
            offset += _message_header.encodedLength();  // usually 8 bytes

            if (_message_header.templateId() == 1) {
                _order_decoder.wrapForDecode(reinterpret_cast<char*>(buffer.buffer()), offset, _message_header.blockLength(), _message_header.version(), buffer.capacity());
                Order order;
                //getting fixed block from sbe
                order.order_id = _order_decoder.order_id();
                order.account_id = _order_decoder.account_id();
                order.instrument_id = _order_decoder.instrument_id();
                order.quantity = _order_decoder.quantity();
                order.price = _order_decoder.price();
                //getting variable length from sbe
                order.symbol = _order_decoder.getSymbolAsString();
                order.side = _order_decoder.getSideAsString();
                result.emplace(order);
            }
        }
        else if (msgType == 2) {
            // Handle TradeExecution message
            TradeExecution trade;
            if (length >= sizeof(trade)) {
                memcpy(&trade, buffer.buffer() + offset + 1, sizeof(trade));
            }
            result.emplace(trade);
        }
        else {
            std::cerr << "Unexpected msgType: " << static_cast<int>(msgType) << std::endl;
        }
    });
    return result;
}

int ShardedQueue::size(){
    std::lock_guard<std::mutex> lock(_gateway_queue_mutex);
    return _ring_buffer.size() + _gateway_task_queue.size();
}

