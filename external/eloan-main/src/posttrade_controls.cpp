//
// Created by muhammad-abdullah on 5/27/25.
//

// File: src/posttrade_controls.cpp
#include "posttrade_controls.h"
#include "data_types.h"
#include <algorithm>

void eloan::PostTradeControls::onTrade(const TradeExecution &trade) {
    int shard = trade.account_id % NUM_SHARDS;
    //checks to execute in post trade
}