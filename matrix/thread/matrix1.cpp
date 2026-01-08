#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <iomanip>

using namespace std;
const int N = 20;
mutex mtx;

void multiply_block(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int row_start, int col_start, int common_start, int block_size) {
    int row_end = min(row_start + block_size, N);
    int col_end = min(col_start + block_size, N);
    int common_end = min(common_start + block_size, N);

    vector<vector<int>> local_res(block_size, vector<int>(block_size, 0));

    for (int i = row_start; i < row_end; ++i) {
        for (int j = col_start; j < col_end; ++j) {
            int sum = 0;
            for (int k = common_start; k < common_end; ++k) {
                sum += A[i][k] * B[k][j];
            }
            local_res[i - row_start][j - col_start] = sum;
        }
    }

    lock_guard<mutex> lock(mtx);
    for (int i = row_start; i < row_end; ++i) {
        for (int j = col_start; j < col_end; ++j) {
            C[i][j] += local_res[i - row_start][j - col_start];
        }
    }
}

void single_thread_mult(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k)
                C[i][j] += A[i][k] * B[k][j];
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
    cout << "Single thread time: " << chrono::duration<double, milli>(end_seq - start_seq).count() << " ms" << endl;

    cout << "N = " << N << endl;
    cout << "BlockSize\tThreads\tTime(ms)" << endl;

    for (int k = 1; k <= N; ++k) {
        for (auto& row : C) fill(row.begin(), row.end(), 0);
        vector<thread> threads;
        auto start_par = chrono::high_resolution_clock::now();

        for (int i = 0; i < N; i += k) {
            for (int j = 0; j < N; j += k) {
                for (int l = 0; l < N; l += k) {
                    threads.emplace_back(multiply_block, ref(A), ref(B), ref(C), i, j, l, k);
                }
            }
        }

        for (auto& t : threads) t.join();

        auto end_par = chrono::high_resolution_clock::now();
        cout << k << "\t\t" << threads.size() << "\t" << chrono::duration<double, milli>(end_par - start_par).count() << endl;
    }
    return 0;
}