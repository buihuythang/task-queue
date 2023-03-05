/**
 * @file TaskQueue.h
 * @author ThangB <thangb@pm.me>
 * @brief Simple implementation of message passing queue
 * @details Support to post some tasks to the queue and run them on the thread that calls the
 * queue's loop function.
 * @version 0.1
 * @date 2022-05-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <chrono>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>


/**
 * @brief Simple Task Queue namespace.
 */
namespace stq {


using Task      = std::function<void()>;
using Delay     = std::chrono::milliseconds;
using Clock     = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

using namespace std::chrono_literals;


/**
 * @brief Singleton message passing queue.
 */
class TaskQueue {
public:
    static TaskQueue& instance();

    TaskQueue(const TaskQueue&)            = delete;
    TaskQueue(TaskQueue&&)                 = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;
    TaskQueue& operator=(TaskQueue&&)      = delete;

    ~TaskQueue();

    /**
     * @brief Poll internal queue and execute the tasks one by one.
     */
    void spin();

    /**
     * @brief Post a task into internal queue. It is suppose to run immediately, assuming that
     * there is no other one before it.
     *
     * @param[in] f    A callable object like function pointer, function object, lambda,...
     * @param[in] args Input parameters for callable object.
     */
    template<typename Callable, typename... Args>
    void post(Callable&& f, Args&&...  args);

    /**
     * @brief Post a task into internal queue. It is suppose to run after some period of time.
     *
     * @param[in] delay How long from now should the task be executed.
     * @param[in] f     A callable object like function pointer, function object, lambda,...
     * @param[in] args  Input parameters for callable object.
     */
    template<typename Callable, typename... Args>
    void postDelay(const Delay& delay,
                   Callable&&   f,
                   Args&&...    args);

private:
    TaskQueue();

    std::condition_variable cvReady;    /*!< Wait for tasks to be available before proceeding */
    std::mutex              mtxReady;   /*!< Protect task list from concurrency access */
    std::queue<Task>        tasks;      /*!< List of tasks ready to be executed */
    std::atomic_bool        started;    /*!< Main loop started? */
};


template <typename Callable, typename... Args>
void TaskQueue::post(Callable &&f, Args&&... args) {
    postDelay(0ms, f, args...);
}


template <typename Callable, typename... Args>
void TaskQueue::postDelay(const Delay& delay,
                          Callable&&   f,
                          Args&&...    args) {
    std::function<std::invoke_result_t<Callable, Args...>()>
        act((std::bind(std::forward<Callable>(f), std::forward<Args>(args)...)));

    auto task = [act]() { (void) act(); };

    std::thread([this, task, delay]() {
        auto ts = Clock::now();
        while (!started) std::this_thread::yield(); // Hopefully...
        auto te = Clock::now();

        Delay rd = 0ms;
        if (te-ts < delay) rd = delay - std::chrono::duration_cast<Delay>(te-ts);

        std::this_thread::sleep_for(rd);
        std::lock_guard<std::mutex> lock(mtxReady);
        tasks.push(task);
        cvReady.notify_one();
    }).detach();
}


} // End of namespace #stq
