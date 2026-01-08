#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;
const int N = 15;

void multiply_block(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int row_start, int col_start, int block_size) {
    int row_end = min(row_start + block_size, N);
    int col_end = min(col_start + block_size, N);

    for (int i = row_start; i < row_end; ++i) {
        for (int j = col_start; j < col_end; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
void single_thread_mult(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    vector<vector<int>> A(N, vector<int>(N));
    vector<vector<int>> B(N, vector<int>(N));
    vector<vector<int>> C(N, vector<int>(N));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }

    auto start_seq = chrono::high_resolution_clock::now();
    single_thread_mult(A, B, C);
    auto end_seq = chrono::high_resolution_clock::now();
    double seq_time = chrono::duration<double, milli>(end_seq - start_seq).count();

    cout << "Multiply alhorithm time: " << seq_time << " ms" << endl;
    cout << "N = " << N << endl;
    cout << "BlockSize\tThreads\tTime(ms)" << endl;

    for (int k = 1; k <= N; ++k) {
        vector<thread> threads;
        auto start_par = chrono::high_resolution_clock::now();

        for (int i = 0; i < N; i += k) {
            for (int j = 0; j < N; j += k) {
                threads.emplace_back(multiply_block, ref(A), ref(B), ref(C), i, j, k);
            }
        }

        for (auto& t : threads) {
            t.join();
        }
        auto end_par = chrono::high_resolution_clock::now();
        double par_time = chrono::duration<double, milli>(end_par - start_par).count();
        cout << k << "\t\t" << threads.size() << "\t" << par_time << endl;
    }

    return 0;
}