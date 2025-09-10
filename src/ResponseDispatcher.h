#pragma once
#include "QueueManager.h"
#include <thread>

// Thread T4 — dequeues GatewayTask from ResponseQueue and sends response
void responseDispatcherThread();
