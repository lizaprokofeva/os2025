#include <iostream>
#include <thread>
#include <chrono>

#include <unistd.h>
#define GET_PID() getpid()

int main() {
    std::cout << "Dummy1 PID: " << GET_PID() << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}