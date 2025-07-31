#pragma once
#include "aeron_wrapper.h"
#include <memory>

void jsonToSbeSenderThread(
    std::shared_ptr<aeron_wrapper::Publication> publication);
