#include "ThreadPool.h"
#include "Logger.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <future>

// Simple test to verify thread pool functionality
void testThreadPool() {
    Logger::getInstance().log("[TEST] Starting thread pool test");
    
    // Create a thread pool with 4 workers
    ThreadPool pool(4);
    
    // Test basic functionality
    auto future1 = pool.enqueue([]() {
        Logger::getInstance().log("[TEST] Task 1 executed");
        return 42;
    });
    
    auto future2 = pool.enqueue([]() {
        Logger::getInstance().log("[TEST] Task 2 executed");
        return 100;
    });
    
    // Test void tasks
    pool.enqueue_void([]() {
        Logger::getInstance().log("[TEST] Void task executed");
    });
    
    // Wait for results
    int result1 = future1.get();
    int result2 = future2.get();
    
    Logger::getInstance().log("[TEST] Task 1 result: " + std::to_string(result1));
    Logger::getInstance().log("[TEST] Task 2 result: " + std::to_string(result2));
    
    // Test parallel execution
    std::vector<std::future<int>> futures;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.enqueue([i]() {
            // Simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            Logger::getInstance().log("[TEST] Parallel task " + std::to_string(i) + " completed");
            return i * i;
        }));
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.get();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    Logger::getInstance().log("[TEST] All parallel tasks completed in " + 
                             std::to_string(duration.count()) + "ms");
    Logger::getInstance().log("[TEST] Thread pool test completed successfully");
}

// Uncomment the following line to run the test
// int main() { testThreadPool(); return 0; } 