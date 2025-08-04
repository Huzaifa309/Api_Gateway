// File: src/risk_engine.cpp
#include "loan_engine.h"
#include "config_loader.h"
#include <iostream>
#include <fmt/format.h>

using namespace eloan;

LoanEngine::LoanEngine() : blogger_("../log/eloan/loan_engine.log") {
};

LoanEngine::~LoanEngine() {
    stop();
}

bool LoanEngine::initialize(const std::string &config_path) {
    if (!ConfigLoader::loadConfig(config_path)) {
        return false;
    }
    std::cout << "initializing the logger: " << config_path << std::endl;
    //initializing logger
    logger_wrapper_ = std::make_unique<LoggerWrapper>(4, "../log/eloan/loan_engine");
    // Initialize Messaging with callbacks bound to this instance
    bool ok = messaging_.initialize(logger_wrapper_);
    if (!ok) {
        blogger_.error("[LoanEngine] Failed to initialize Messaging");
        return false;
    }
    return true;
        }

void LoanEngine::start() {
    running_ = true;

    // Launch shard threads
    for (int i = 0; i < NUM_SHARDS; ++i) {
        shard_threads_.emplace_back(&LoanEngine::runShard, this, i);
    }

    blogger_.debug("[LoanEngine] Shard threads started");
    blogger_.debug("[LoanEngine] Listening for messages via Aeron");
    // Messaging already spun up its listener thread in initialize()
}

void LoanEngine::stop() {
    running_ = false;

    // Join shard threads
    for (auto &t : shard_threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
    shard_threads_.clear();

    // Shut down messaging
    messaging_.shutdown();

    blogger_.debug("[LoanEngine] Stopped all threads and messaging");
}

void LoanEngine::runShard(int shard_id) {
    logger_wrapper_->debug(shard_id, "[LoanEngine] Running shard");
    aeron::concurrent::BackoffIdleStrategy idle_strategy(100, 1000);
    auto& queue = messaging_.getQueue()[shard_id];
    bool processed = false;
    while (running_) {
        for (int i = 0; i < MAX_FRAGMENT_BATCH_SIZE; ++i) {
            auto msg = queue.dequeue();
            if (msg.has_value()) {
                processed = true;
                logger_wrapper_->debug(shard_id, "LoanEngine dequeing completed on shard");
                auto variant = msg.value();
                if (std::holds_alternative<Order>(variant)) {
                    onOrderReceived(std::get<Order>(variant), shard_id);
                }
                else if (std::holds_alternative<TradeExecution>(variant)) {
                    onTradeReceived(std::get<TradeExecution>(variant), shard_id);
                }
                else {
                    logger_wrapper_->error(shard_id, "[LoanEngine] Unknown or malformed message received");
                }
            }
            else
                break;
        }
        if (!processed) {
            idle_strategy.idle();
        }
        processed = false;
    }
    logger_wrapper_->debug(shard_id, "[LoanEngine] runShard exiting");
}

void LoanEngine::onOrderReceived(const Order &order, int shard_id) {
    logger_wrapper_->debug(shard_id, "[LoanEngine] Received order");
    // Determine which shard this order belongs to
    int shard = static_cast<int>(order.account_id % NUM_SHARDS);


    // If passed, send the order to the matching engine (not implemented here).
    logger_wrapper_->debug_fast(shard_id, "[LoanEngine] Order accepted: account {}, qty {}", order.account_id, order.quantity);
    //sending message to ME
    messaging_.SendMessage(order);

    //insertion in database
    std::string query = "insert into orders(order_id, account_id, instrument_id, quantity, price, symbol, side) values ("+ std::to_string(order.order_id)+ ", " + std::to_string(order.account_id) +
        ", " + std::to_string(order.instrument_id) + " ," + std::to_string(order.quantity) +" ,"+ std::to_string(order.price) + " ,'" + order.symbol +"' , '" + order.side +"');";
    Database& conn = Database::get_connection(R"(dbname=eagri_db user=eagri password=12345 host=localhost)");
    conn.insert(query);
}

void LoanEngine::onTradeReceived(const TradeExecution &trade, int shard_id) {
    // Determine shard
    int shard = static_cast<int>(trade.account_id % NUM_SHARDS);

    // After updating positions, send a trade confirmation back if needed
    // (In this example, we simply log it)
    logger_wrapper_->debug_fast(shard_id, "[LoanEngine] Trade received: account {}, qty {}, price {}", trade.account_id, trade.quantity, trade.price);
}