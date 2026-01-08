#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <string>
#include <limits.h>

using namespace std;

void close_all_pipes(int p_in[], int p1[], int p2[], int p3[]) {
    close(p_in[0]); close(p_in[1]);
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    close(p3[0]); close(p3[1]);
}

void launch_child(const char* exeName, int read_fd, int write_fd, int p_in[], int p1[], int p2[], int p3[]) {
    pid_t pid = fork();
    if (pid == 0) {
        dup2(read_fd, STDIN_FILENO);
        dup2(write_fd, STDOUT_FILENO);

        close_all_pipes(p_in, p1, p2, p3); 

        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            _exit(1);
        }

        string fullPath = cwd;
        fullPath += "/";
        fullPath += exeName;
        
        execl(fullPath.c_str(), exeName, NULL); 
        perror("execl"); 
        _exit(1);
    }
}
int main(int argc, char* argv[]) {
    int p_in[2]; // Main -> M
    int p1[2]; // M -> A
    int p2[2]; // A -> P
    int p3[2]; // P -> S

    if (pipe(p_in) < 0 || pipe(p1) < 0 || pipe(p2) < 0 || pipe(p3) < 0) {
        perror("pipe");
        return 1;
    }


    launch_child("M", p_in[0], p1[1], p_in, p1, p2, p3);
    launch_child("A", p1[0], p2[1], p_in, p1, p2, p3);
    launch_child("P", p2[0], p3[1], p_in, p1, p2, p3);
    launch_child("S", p3[0], STDOUT_FILENO, p_in, p1, p2, p3);
    close(p_in[0]);
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    close(p3[0]); close(p3[1]);

   
    cout << "Enter numbers for chain (M->A->P->S): ";
    string input;
    getline(cin, input);
    input += "\n";

    write(p_in[1], input.c_str(), input.size());

    close(p_in[1]);

    for (int i = 0; i < 4; i++) wait(NULL);

    return 0;
}
