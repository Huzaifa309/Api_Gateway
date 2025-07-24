#pragma once
#include "aeron_wrapper.h"
#include <memory>

// T3 thread — listens on Aeron and pushes to ResponseQueue
void aeronReceiverThread(
    std::shared_ptr<aeron_wrapper::Subscription> subscription);
