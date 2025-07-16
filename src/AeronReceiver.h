#pragma once
#include <memory>
#include "Subscription.h"

// T3 thread — listens on Aeron and pushes to ResponseQueue
void aeronReceiverThread(std::shared_ptr<aeron::Subscription> subscription);
