#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#define SET_ENV(name, val) SetEnvironmentVariableA(name, val)
#define UNSET_ENV(name) SetEnvironmentVariableA(name, NULL)
#define RUN_BG(cmd) system(("start /B " + std::string(cmd)).c_str())
#else
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#define SET_ENV(name, val) setenv(name, val, 1)
#define UNSET_ENV(name) unsetenv(name)
#define RUN_BG(cmd) system((std::string(cmd) + " &").c_str())
#endif

int getPidByName(const std::string& processName) {
    int pid = -1;
#ifdef _WIN32
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return -1;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            std::string currentProcess(pe.szExeFile);
            if (currentProcess == processName || currentProcess == processName + ".exe") {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
#else
    std::string command = "pgrep -f " + processName + " 2>/dev/null | head -n1";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return -1;
    char buffer[128];
    if (fgets(buffer, 128, pipe) != nullptr) {
        pid = std::stoi(buffer);
    }
    pclose(pipe);
#endif
    return pid;
}

int main() {
    std::string victim1, victim2;
#ifdef _WIN32
    victim1 = "dummy1.exe";
    victim2 = "dummy2.exe";
#else
    victim1 = "dummy1";
    victim2 = "dummy2";
#endif

    std::cout << "Test 1: Kill by ENV VAR" << std::endl;
    RUN_BG(victim1);
    RUN_BG(victim2);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    SET_ENV("PROC_TO_KILL", (victim1 + "," + victim2).c_str());
    system("killer.exe");

    UNSET_ENV("PROC_TO_KILL");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Test 2: Kill by --name" << std::endl;
    RUN_BG(victim1);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    system(("killer.exe --name " + victim1).c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Test 3: Kill by --id" << std::endl;
    RUN_BG(victim2);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    int targetPid = getPidByName(victim2);
    if (targetPid != -1) {
        system(("killer.exe --id " + std::to_string(targetPid)).c_str());
    }

    return 0;
}