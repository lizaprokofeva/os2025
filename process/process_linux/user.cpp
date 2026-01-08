#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define SET_ENV(name, val) setenv(name, val, 1)
#define UNSET_ENV(name) unsetenv(name)
#define RUN_BG(cmd) system(("./" + std::string(cmd) + " &").c_str())

int getPidByName(const std::string& processName) {
    int pid = -1;
    std::string command = "pgrep -x " + processName + " 2>/dev/null | head -n1";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return -1;
    char buffer[128];
    if (fgets(buffer, 128, pipe) != nullptr) {
        pid = std::stoi(buffer);
    }
    pclose(pipe);
    return pid;
}

int main() {
    std::string victim1 = "dummy1";
    std::string victim2 = "dummy2";

    std::cout << "Test 1: Kill by ENV VAR" << std::endl;
    RUN_BG(victim1);
    RUN_BG(victim2);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    SET_ENV("PROC_TO_KILL", (victim1 + "," + victim2).c_str());
    system("./killer");

    UNSET_ENV("PROC_TO_KILL");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Test 2: Kill by --name" << std::endl;
    RUN_BG(victim1);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    system(("./killer --name " + victim1).c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Test 3: Kill by --id" << std::endl;
    RUN_BG(victim2);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    int targetPid = getPidByName(victim2);
    if (targetPid != -1) {
        system(("./killer --id " + std::to_string(targetPid)).c_str());
    }

    return 0;
}

