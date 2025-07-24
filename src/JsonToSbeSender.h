#pragma once
#include "Publication.h"
#include <memory>

void jsonToSbeSenderThread(std::shared_ptr<aeron::Publication> publication);
