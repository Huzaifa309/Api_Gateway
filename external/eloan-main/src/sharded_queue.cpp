//
// Created by muhammad-abdullah on 6/4/25.
//
#include<concurrent/BackOffIdleStrategy.h>
#include "sharded_queue.h"
#include "baseline/Order.h"
#include <iostream>
#include <vector>
#include <cstring>

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
    aeron::concurrent::BackoffIdleStrategy idleStrategy(100,1000);
    bool isWritten = false;
    auto start = std::chrono::high_resolution_clock::now();
    
    // Serialize GatewayTask to buffer
    // For simplicity, we'll just serialize the JSON string
    std::string jsonStr = task.json;
    int32_t length = jsonStr.length();
    
    // Create a temporary buffer for the serialized data
    std::vector<uint8_t> tempBuffer(length + sizeof(int32_t));
    
    // Write the length first
    *reinterpret_cast<int32_t*>(tempBuffer.data()) = length;
    
    // Copy the JSON string
    std::memcpy(tempBuffer.data() + sizeof(int32_t), jsonStr.data(), length);
    
    // Create AtomicBuffer from the temporary buffer
    aeron::concurrent::AtomicBuffer atomicBuffer(tempBuffer.data(), tempBuffer.size());
    
    while(!isWritten) {
        isWritten = _ring_buffer.write(3, atomicBuffer, 0, tempBuffer.size()); // Use msgType 3 for GatewayTask
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

std::optional<std::variant<Order, TradeExecution, GatewayTask>> ShardedQueue::dequeue() {
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
        else if (msgType == 3) {
            // Handle GatewayTask message
            if (length >= sizeof(int32_t)) {
                int32_t jsonLength = *reinterpret_cast<const int32_t*>(buffer.buffer() + offset);
                if (length >= sizeof(int32_t) + jsonLength) {
                    GatewayTask task;
                    task.json = std::string(reinterpret_cast<const char*>(buffer.buffer() + offset + sizeof(int32_t)), jsonLength);
                    // Note: We're not serializing request and callback as they're not easily serializable
                    // In a real implementation, you might want to handle these differently
                    result.emplace(task);
                }
            }
        }
        else {
            std::cerr << "Unexpected msgType: " << static_cast<int>(msgType) << std::endl;
        }
    });
    return result;
}

int ShardedQueue::size(){
    return _ring_buffer.size();
}

