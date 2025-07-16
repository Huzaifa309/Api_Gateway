#include "QueueManager.h"
#include "Logger.h"
#include "Aeron.h"
#include "FragmentAssembler.h"
#include "Subscription.h"
#include <thread>
#include <chrono>

using namespace aeron;

void aeronReceiverThread(std::shared_ptr<Subscription> subscription)
{
    Logger::getInstance().log("[T3] Aeron receiver thread started");

    fragment_handler_t handler = [&](AtomicBuffer &buffer, std::int32_t offset, std::int32_t length, const Header &header)
    {
        std::string data(reinterpret_cast<const char *>(buffer.buffer() + offset), length);
        Logger::getInstance().log("[T3] Received data from backend: " + data);

        // TODO: Integrate SBE decoding here
        // auto json = sbe_decode(data);

        // Push to ResponseQueue
        GatewayTask responseTask;
        responseTask.json = data; // for now we keep it as raw json
        ResponseQueue.enqueue(responseTask);
    };

    while (true)
    {
        try
        {
            subscription->poll(handler, 10);
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().log(std::string("[T3] Aeron poll error: ") + e.what());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
