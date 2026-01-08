#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#define GET_PID() GetCurrentProcessId()
#else
#include <unistd.h>
#define GET_PID() getpid()
#endif

int main() {
    std::cout << "Dummy1 PID: " << GET_PID() << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}