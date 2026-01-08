#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>
#include <unistd.h>
#endif

std::string getEnvVar(const std::string& key) {
    const char* val = std::getenv(key.c_str());
    return val == nullptr ? "" : std::string(val);
}

void killById(int pid) {
    if (pid <= 0) return;
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        return;
    }
    TerminateProcess(hProcess, 1);
    CloseHandle(hProcess);
#else
    if (kill(pid, SIGKILL) == -1) {
        return;
    }
#endif
    std::cout << "Killed: " << pid << std::endl;
}

void killByName(const std::string& processName) {
    bool found = false;
#ifdef _WIN32
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            std::string currentProcess(pe.szExeFile);
            if (currentProcess == processName || currentProcess == processName + ".exe") {
                killById(pe.th32ProcessID);
                found = true;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
#else
    DIR* dir = opendir("/proc");
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type != DT_DIR) continue;

        char* end;
        long pid = strtol(entry->d_name, &end, 10);
        if (*end != '\0') continue;

        std::string path = "/proc/" + std::string(entry->d_name) + "/comm";
        std::ifstream cmdFile(path);
        if (!cmdFile) continue;

        std::string cmdName;
        if (std::getline(cmdFile, cmdName)) {
            if (cmdName == processName) {
                killById(static_cast<int>(pid));
                found = true;
            }
        }
    }
    closedir(dir);
#endif
}

void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  killer --id <PID>" << std::endl;
    std::cout << "  killer --name <processname>" << std::endl;
    std::cout << "  killer" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        printUsage();
        return 0;
    }

    bool processedArgs = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--id") {
            if (i + 1 < argc) {
                try {
                    killById(std::stoi(argv[++i]));
                    processedArgs = true;
                }
                catch (...) {
                    return 1;
                }
            }
        }
        else if (arg == "--name") {
            if (i + 1 < argc) {
                killByName(argv[++i]);
                processedArgs = true;
            }
        }
    }

    std::string envList = getEnvVar("PROC_TO_KILL");
    if (!envList.empty()) {
        std::stringstream ss(envList);
        std::string segment;
        while (std::getline(ss, segment, ',')) {
            segment.erase(remove(segment.begin(), segment.end(), ' '), segment.end());
            segment.erase(remove(segment.begin(), segment.end(), '\"'), segment.end());
            if (!segment.empty()) {
                killByName(segment);
            }
        }
    }
    else if (!processedArgs && argc == 1) {
        printUsage();
    }

    return 0;
}