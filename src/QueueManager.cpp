#include "QueueManager.h"
moodycamel::ConcurrentQueue<GatewayTask> ReceiverQueue;
moodycamel::ConcurrentQueue<GatewayTask> ResponseQueue;
