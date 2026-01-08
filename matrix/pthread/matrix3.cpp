#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>

using namespace std;
const int N = 20;
struct ThreadArgs {
    vector<vector<int>>* A;
    vector<vector<int>>* B;
    vector<vector<int>>* C;
    int row_start;
    int col_start;
    int block_size;
};
void* multiply_block(void* arg) {
    ThreadArgs* data = (ThreadArgs*)arg;
    int row_end = min(data->row_start + data->block_size, N);
    int col_end = min(data->col_start + data->block_size, N);

    for (int i = data->row_start; i < row_end; ++i) {
        for (int j = data->col_start; j < col_end; ++j) {
            (*data->C)[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                (*data->C)[i][j] += (*data->A)[i][k] * (*data->B)[k][j];
            }
        }
    }
    pthread_exit(NULL);
    return NULL;
}
void simple_mult(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
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

    auto start_s = chrono::system_clock::now();
    simple_mult(A, B, C);
    auto end_s = chrono::system_clock::now();
    cout << "Multiply algorithm time: " << chrono::duration<double, milli>(end_s - start_s).count() << " ms" << endl;

    cout << "N = " << N << endl;
    cout << "BlockSize\tThreads\tTime(ms)" << endl;

    for (int k = 1; k <= N; ++k) {
        vector<pthread_t> threads;
        vector<ThreadArgs*> args_list;

        auto start_p = chrono::system_clock::now();

        for (int i = 0; i < N; i += k) {
            for (int j = 0; j < N; j += k) {
                ThreadArgs* args = new ThreadArgs{ &A, &B, &C, i, j, k };
                args_list.push_back(args);
                pthread_t thread;
                pthread_create(&thread, NULL, multiply_block, (void*)args);
                threads.push_back(thread);
            }
        }

        for (size_t i = 0; i < threads.size(); ++i) {
            pthread_join(threads[i], NULL);
        }

        auto end_p = chrono::system_clock::now();
        double dur = chrono::duration<double, milli>(end_p - start_p).count();
        for (auto ptr : args_list) delete ptr;
        cout << k << "\t\t" << threads.size() << "\t" << dur << endl;
    }

    return 0;
}