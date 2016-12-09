#include <cstddef>
#include <string>
#include <random>

#include "HashMap.h"
#include "Benchmark.h"
#include "TreeMap.h"


template<class Collection>
void randomInsert(int n) {
    Collection map;
    std::mt19937 device;
    std::uniform_int_distribution<int> distribution(0, n);
    for (int i = 0; i < n; ++i) {
        map[distribution(device)] = i;
    }
}


template<int N>
void randomInsertBuckets(int n) {
    aisdi::HashMap<int, int> map(N);
    std::mt19937 device;
    std::uniform_int_distribution<int> distribution(0, n);
    for (int i = 0; i < n; ++i) {
        map[distribution(device)] = i;
    }
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    auto cases = {1000, 2000, 5000, 8000, 10000, 20000, 50000, 80000, 100000, 200000,
                  500000, 800000, 1000000};

    bm::BenchmarkSuite("Random Insert")
            .addBenchmark(bm::Benchmark("HashMap", randomInsert<aisdi::HashMap<int, int>>, cases))
            .addBenchmark(bm::Benchmark("TreeMap", randomInsert<aisdi::TreeMap<int, int>>, cases))
            .run([&](std::pair<const int, double> pPair, int percent) {
                std::cout << "Done " << percent <<"% -> " << pPair.first << " in " << pPair.second << "\n";
            })
            .exportCSVFile();

    bm::BenchmarkSuite("Buckets")
            .addBenchmark(bm::Benchmark("100 Buckets", randomInsertBuckets<100>, cases))
            .addBenchmark(bm::Benchmark("500 Buckets", randomInsertBuckets<500>, cases))
            .addBenchmark(bm::Benchmark("1000 Buckets", randomInsertBuckets<1000>, cases))
            .addBenchmark(bm::Benchmark("5000 Buckets", randomInsertBuckets<5000>, cases))
            .run([&](std::pair<const int, double> pPair, int percent) {
                std::cout << "Done " << percent <<"% -> " << pPair.first << " in " << pPair.second << "\n";
            })
            .exportCSVFile();
}
