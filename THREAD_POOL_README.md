# Thread Pool Implementation for API Gateway

## Overview

This implementation adds thread pools to each of the three main threads (T1, T2, T3) in the API Gateway to improve performance by parallelizing work within each thread.

## Architecture

### Original Architecture (Single-threaded per stage)
```
T1 (Request Handler) → T2 (JSON to SBE) → T3 (Aeron Receiver)
```

### New Architecture (Thread pools within each stage)
```
T1 (Request Handler + 4 worker threads)
    ↓
T2 (JSON to SBE + 6 worker threads)  
    ↓
T3 (Aeron Receiver + 8 worker threads)
```

## Thread Pool Details

### T1 - Request Handler Thread Pool
- **Size**: 4 worker threads
- **Purpose**: Parallel request processing and JSON validation
- **Benefits**: 
  - Multiple HTTP requests can be processed simultaneously
  - JSON validation happens in parallel
  - Reduced latency for high-concurrency scenarios

### T2 - JSON to SBE Encoding Thread Pool  
- **Size**: 6 worker threads
- **Purpose**: Parallel SBE message encoding
- **Benefits**:
  - Multiple JSON to SBE conversions happen simultaneously
  - Faster throughput for bulk message processing
  - Better CPU utilization during encoding

### T3 - Aeron Receiver Thread Pool
- **Size**: 8 worker threads  
- **Purpose**: Parallel SBE decoding and response processing
- **Benefits**:
  - Multiple SBE messages decoded simultaneously
  - Parallel HTTP response generation and sending
  - Improved throughput for high-volume message processing

## Configuration

Thread pool sizes can be configured in `src/ThreadPoolConfig.h`:

```cpp
#define REQUEST_THREAD_POOL_SIZE 4
#define SBE_ENCODING_THREAD_POOL_SIZE 6  
#define SBE_DECODING_THREAD_POOL_SIZE 8
```

## Performance Benefits

1. **Parallel Processing**: Each stage can now process multiple items simultaneously
2. **Better CPU Utilization**: Multiple cores are utilized effectively
3. **Reduced Latency**: Requests don't wait in line for processing
4. **Higher Throughput**: More messages can be processed per second
5. **Scalability**: Thread pool sizes can be tuned based on system capabilities

## Thread Safety

- All thread pools use proper synchronization with mutexes and condition variables
- Each worker thread has its own local buffers to avoid contention
- Queue operations are thread-safe using the existing concurrent queue implementations

## Monitoring

The implementation includes detailed logging to monitor thread pool performance:

- Thread pool initialization messages
- Worker thread activity logs (e.g., `[T1-Worker]`, `[T2-Worker]`, `[T3-Worker]`)
- Processing status and error messages

## Total Thread Count

The system now uses a total of 21 threads:
- 3 main coordinator threads (T1, T2, T3)
- 4 request processing worker threads
- 6 SBE encoding worker threads  
- 8 SBE decoding worker threads

## Usage

The thread pools are automatically initialized when the first request is processed. No additional configuration is required beyond adjusting the pool sizes in the config file.

## Future Enhancements

1. **Dynamic Thread Pool Sizing**: Adjust pool sizes based on load
2. **Work Stealing**: Allow threads to steal work from other pools when idle
3. **Priority Queues**: Handle high-priority requests first
4. **Metrics Collection**: Add performance metrics for monitoring 