#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>
#include <iomanip>

using namespace std;

const int N = 15;
struct Data { const vector<vector<int>>* A, * B; vector<vector<int>>* C; int rs, cs, bs; };

DWORD WINAPI Task(LPVOID p) {
    Data* d = (Data*)p;
    for (int i = d->rs; i < min(d->rs + d->bs, N); ++i)
        for (int j = d->cs; j < min(d->cs + d->bs, N); ++j) {
            (*d->C)[i][j] = 0;
            for (int k = 0; k < N; ++k) (*d->C)[i][j] += (*d->A)[i][k] * (*d->B)[k][j];
        }
    return 0;
}

int main() {
    vector<vector<int>> A(N, vector<int>(N, 1)), B(N, vector<int>(N, 1)), C(N, vector<int>(N, 0));
    auto s1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++) 
        for (int j = 0; j < N; j++) 
            for (int k = 0; k < N; k++) C[i][j] += A[i][k] * B[k][j];
    cout << "Multiply algotithm time: " << chrono::duration<double, milli>(chrono::high_resolution_clock::now() - s1).count() << " ms" << endl;
    cout << "N = " << N << endl;
    cout << "BlockSize\tThreads\tTime(ms)" << endl;
    for (int k = 1; k <= N; ++k) {
        vector<HANDLE> h; vector<Data*> args;
        auto s2 = chrono::high_resolution_clock::now();
        for (int i = 0; i < N; i += k) {
            for (int j = 0; j < N; j += k) {
                Data* d = new Data{ &A, &B, &C, i, j, k };
                args.push_back(d);
                h.push_back(CreateThread(NULL, 0, Task, d, 0, NULL));
            }
        }
        WaitForMultipleObjects(h.size(), h.data(), TRUE, INFINITE);
        auto e2 = chrono::high_resolution_clock::now();
        for (auto i : h) CloseHandle(i); for (auto i : args) delete i;
        cout << k <<  "\t\t" << h.size() << "\t" << chrono::duration<double, milli>(e2 - s2).count() << endl;
    }
    return 0;
}