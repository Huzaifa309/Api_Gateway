#include "QueueManager.h"
moodycamel::ConcurrentQueue<GatewayTask> ReceiverQueue;
moodycamel::ConcurrentQueue<GatewayTask> ResponseQueue;
moodycamel::ConcurrentQueue<GatewayTask> CallBackQueue;