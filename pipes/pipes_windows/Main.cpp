#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

void SetNonInheritable(HANDLE h) {
    if (h == GetStdHandle(STD_INPUT_HANDLE) || h == GetStdHandle(STD_OUTPUT_HANDLE) || h == GetStdHandle(STD_ERROR_HANDLE)) return;

    if (!SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0)) {
        throw runtime_error("SetHandleInformation failed.");
    }
}

void SetInheritable(HANDLE h) {
    if (h == GetStdHandle(STD_INPUT_HANDLE) || h == GetStdHandle(STD_OUTPUT_HANDLE) || h == GetStdHandle(STD_ERROR_HANDLE)) return;

    if (!SetHandleInformation(h, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT)) {
        throw runtime_error("SetHandleInformation failed.");
    }
}

HANDLE StartProcess(string exeName, HANDLE hIn, HANDLE hOut) {
    try {
        SetInheritable(hIn);
        SetInheritable(hOut);
    }
    catch (const runtime_error& e) {
        cerr << "Error setting inheritable handles: " << e.what() << endl;
        exit(1);
    }

    PROCESS_INFORMATION pi;
    STARTUPINFOA si;

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = hIn;
    si.hStdOutput = hOut;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    string cmd = exeName;
    vector<char> cmdBuf(cmd.begin(), cmd.end());
    cmdBuf.push_back(0);

    if (!CreateProcessA(NULL, cmdBuf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        cerr << "Error: Could not launch " << exeName << ". Code: " << GetLastError() << endl;
        SetNonInheritable(hIn);
        SetNonInheritable(hOut);
        exit(1);
    }

    try {
        SetNonInheritable(hIn);
        SetNonInheritable(hOut);
    }
    catch (const runtime_error& e) {
        cerr << "Error resetting inheritable handles: " << e.what() << endl;
    }

    CloseHandle(pi.hThread);
    return pi.hProcess;
}

int main() {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hPipe1_R, hPipe1_W; // Main -> M
    HANDLE hPipe2_R, hPipe2_W; // M -> A
    HANDLE hPipe3_R, hPipe3_W; // A -> P
    HANDLE hPipe4_R, hPipe4_W; // P -> S

 
    if (!CreatePipe(&hPipe1_R, &hPipe1_W, &sa, 0) ||
        !CreatePipe(&hPipe2_R, &hPipe2_W, &sa, 0) ||
        !CreatePipe(&hPipe3_R, &hPipe3_W, &sa, 0) ||
        !CreatePipe(&hPipe4_R, &hPipe4_W, &sa, 0)) {
        cerr << "Pipe creation failed" << endl;
        return 1;
    }

    CloseHandle(StartProcess("M.exe", hPipe1_R, hPipe2_W));
    CloseHandle(StartProcess("A.exe", hPipe2_R, hPipe3_W));
    CloseHandle(StartProcess("P.exe", hPipe3_R, hPipe4_W));
    HANDLE hProcessS = StartProcess("S.exe", hPipe4_R, GetStdHandle(STD_OUTPUT_HANDLE));
    CloseHandle(hPipe1_R);
    CloseHandle(hPipe2_R); CloseHandle(hPipe2_W);
    CloseHandle(hPipe3_R); CloseHandle(hPipe3_W);
    CloseHandle(hPipe4_R); CloseHandle(hPipe4_W);

 
    cout << "Enter numbers for chain M->A->P->S: ";
    string inputData;
    getline(cin, inputData);
    inputData += "\n";

    DWORD written;
    WriteFile(hPipe1_W, inputData.c_str(), inputData.size(), &written, NULL);
    CloseHandle(hPipe1_W);

    
    WaitForSingleObject(hProcessS, INFINITE);

    CloseHandle(hProcessS);

    return 0;
}