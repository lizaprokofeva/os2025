#include <iostream>

const int STUDENT_N = 19;

int main() {
    int x;
    while (std::cin >> x) {
        std::cout << (x + STUDENT_N) << " ";
    }
    return 0;
}