#include "TaskQueue.h"

using std::mutex;
using std::lock_guard;
using std::unique_lock;
using namespace std::chrono_literals;


namespace stq {


TaskQueue& TaskQueue::instance() {
    static TaskQueue tq;

    return tq;
}


TaskQueue::TaskQueue()
    : started(false) {}


TaskQueue::~TaskQueue() {}


void TaskQueue::spin() {
    started = true;

    while (true) {
        unique_lock<mutex> lck(mtxReady);
        cvReady.wait(lck, [this]() { return !tasks.empty(); });
        auto act = tasks.front();
        tasks.pop();
        lck.unlock();

        act();
        std::this_thread::yield();
    }
}


} // End of namespace #stq
