#include <cstdlib>
#include <cstdint>

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <queue>
#include <future>
#include <algorithm>
#include <chrono>
#include <string>
#include <list>


struct Config
{
    using DataRange = std::pair<std::int64_t, std::int64_t>;

    static constexpr DataRange firstProducer = { 0, 5'000'000 };
    static constexpr DataRange secondProducer = { firstProducer.second, 2 * firstProducer.second };
    static constexpr std::chrono::milliseconds progressInterval{ 100 };
    static constexpr std::chrono::milliseconds queueAwaitTimeout{ 1000 };
};

struct SharedData
{
    std::condition_variable dataCondition;
    std::mutex mutex;
    int pendingCount{ 0 };
    std::queue<std::int64_t> entities;
};


int main()
try
{
    using namespace std::chrono_literals;

    SharedData sharedData = {};

    const std::shared_future<void> producers[] = {
        std::async(std::launch::async,
            [&sharedData]()
            {
                const auto& range = Config::firstProducer;
                for (std::int64_t i = range.first; i < range.second; ++i)
                {
                    const std::lock_guard lock(sharedData.mutex);
                    sharedData.entities.push(i);
                    ++sharedData.pendingCount;
                    sharedData.dataCondition.notify_all();
                }
            }).share(),
        std::async(std::launch::async,
            [&sharedData]()
            {
                const auto& range = Config::secondProducer;
                for (std::int64_t i = range.first; i < range.second; ++i)
                {
                    const std::lock_guard lock(sharedData.mutex);
                    sharedData.entities.push(i);
                    ++sharedData.pendingCount;
                    sharedData.dataCondition.notify_all();
                }
            }).share()
    };

    auto consumer = std::async(std::launch::async,
        [&sharedData, &producers]()
        {
            const auto hasAnyRunningProducer = [&producers]()
            {
                return std::any_of(
                    std::cbegin(producers), std::cend(producers),
                    [](const std::shared_future<void>& value)
                    {
                        return value.wait_for(0ms) == std::future_status::timeout;
                    });
            };

            std::unique_lock lock(sharedData.mutex);

            do
            {
                const bool hasAnyPending = sharedData.dataCondition.wait_for(lock,
                    Config::queueAwaitTimeout,
                    [&]() noexcept
                    {
                        return !sharedData.entities.empty();
                    });

                if (!hasAnyPending)
                    continue;

                while (!sharedData.entities.empty())
                {
                    sharedData.entities.pop();
                    --sharedData.pendingCount;
                }
            }
            while (hasAnyRunningProducer());
        }).share();

    std::cout << "Started producer-consumer threads." << std::endl;

    while (std::future_status::timeout == consumer.wait_for(Config::progressInterval))
        std::cout << '.' << std::flush;

    std::cout << std::endl;

    std::cout << "Pending entities count: " << sharedData.pendingCount << std::endl;

    std::list<std::shared_future<void>> threads(std::cbegin(producers), std::cend(producers));
    threads.push_back(consumer);

    for (const auto& thread : threads)
    {
        try
        {
            thread.get();
        }
        catch (const std::logic_error& ex)
        {
            std::clog << "Logic error in some thread: " << ex.what() << "." << std::endl;
            throw;
        }
        catch (const std::exception& ex)
        {
            std::clog << "Runtime exception in some thread: " << ex.what() << "." << std::endl;
        }
        catch (...)
        {
            std::clog << "Undefined error in some thread. " << std::endl;
            throw;
        }
    }

    std::cout << "Finished successfully." << std::endl;

    std::string ignore;
    std::getline(std::cin, ignore);

    return EXIT_SUCCESS;
}
catch (...)
{
    std::clog << "Exiting with fatal error." << std::endl;
    return EXIT_FAILURE;
}
