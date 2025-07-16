#pragma once
#include <memory>
#include "Publication.h"

void jsonToSbeSenderThread(std::shared_ptr<aeron::Publication> publication);
