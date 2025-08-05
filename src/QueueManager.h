#pragma once
#include "../external/eloan-main/include/eloan/sharded_queue.h"  // Include your custom queue
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <functional>
#include <string>
#include <variant>
#include <optional>

// GatewayTask is now defined in sharded_queue.h

// You can wrap GatewayTask as a variant
extern ShardedQueue ReceiverQueue;
extern ShardedQueue ResponseQueue;
extern ShardedQueue CallBackQueue;
