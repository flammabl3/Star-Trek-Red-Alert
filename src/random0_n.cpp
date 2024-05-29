#include "random0_n.hpp"

int random0_n(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);
    return dis(gen);
}

float randomfloat0_n(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n*10 - 1);  // Multiply n by 10
    return static_cast<float>(dis(gen))/1000.0f;  // Divide the result by 10
}

int randomNegPos() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    if (dis(gen) == 1) {
        return -1;
    } else {
        return 1;
    }
}