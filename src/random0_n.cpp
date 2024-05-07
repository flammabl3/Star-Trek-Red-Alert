#include "random0_n.hpp"

int random0_n(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);
    return dis(gen);
}