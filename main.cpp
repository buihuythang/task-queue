#include "TaskQueue.h"

#include <iostream>
#include <functional>
#include <type_traits>

using namespace stq;
using namespace std;


template <typename T>
T add(T value) {
    return value;
}

template <typename T, typename... Args>
T add(T first, Args... args) {
    return first + add(args...);
}

void test() {
    TaskQueue::instance().post([]() {
        cout << "No delay task" << endl;
    });

    TaskQueue::instance().postDelay(1000ms, []() {
        cout << "Delayed task after 1000ms" << endl;
        cout << add(1, 2, 3, 4, 5) << endl;
    });
}

int main() {
    thread t(test);
    t.detach();
    TaskQueue::instance().spin();

    return 0;
}
