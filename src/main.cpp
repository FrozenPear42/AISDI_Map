#include <cstddef>
#include <string>
#include <random>

#include "HashMap.h"
#include "Benchmark.h"


void randomInsert(int n) {
    aisdi::HashMap<int, int> map;
    std::mt19937 device;
    std::uniform_int_distribution<int> distribution();
    for(int i = 0; i < n; ++i) {
        map[i] = i;
    }
}


int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    Benchmark::run(randomInsert, "Random insert", {1000, 2000, 5000, 8000, 10000, 20000, 50000, 80000, 100000, 200000, 500000, 800000, 1000000});
}
