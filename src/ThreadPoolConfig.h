#pragma once

// Thread Pool Configuration
// These values can be adjusted based on your system's capabilities and workload

// T1 (Request Handler) - Thread pool for parallel request processing
#define REQUEST_THREAD_POOL_SIZE 4

// T2 (JSON to SBE Sender) - Thread pool for parallel SBE encoding
#define SBE_ENCODING_THREAD_POOL_SIZE 6

// T3 (Aeron Receiver) - Thread pool for parallel SBE decoding and response processing
#define SBE_DECODING_THREAD_POOL_SIZE 8

// Total threads in the system:
// - 1 main thread (T1 coordinator)
// - 1 main thread (T2 coordinator) 
// - 1 main thread (T3 coordinator)
// - REQUEST_THREAD_POOL_SIZE worker threads for T1
// - SBE_ENCODING_THREAD_POOL_SIZE worker threads for T2
// - SBE_DECODING_THREAD_POOL_SIZE worker threads for T3
// Total: 3 + REQUEST_THREAD_POOL_SIZE + SBE_ENCODING_THREAD_POOL_SIZE + SBE_DECODING_THREAD_POOL_SIZE
// = 3 + 4 + 6 + 8 = 21 threads total 