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
    std::uniform_int_distribution<int> distribution();
    for (int i = 0; i < n; ++i) {
        map[i] = i;
    }
}


int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    using Map = aisdi::HashMap<int, int>;
    using Tree = aisdi::TreeMap<int, int>;

    bm::BenchmarkSuite randomInsertSuite("Random Insert");

    auto cases = {1000, 2000, 5000, 8000, 10000, 20000, 50000, 80000, 100000, 200000,
                  500000, 800000, 1000000};

    randomInsertSuite.addBenchmark(bm::Benchmark("HashMap", randomInsert<Map>, cases))
                     .addBenchmark(bm::Benchmark("TreeMap", randomInsert<Tree>, cases));

    randomInsertSuite.run(
            /*        [&](std::pair<const int, double> pPair) {
                        std::cout << "Done: " << pPair.first << " in " << pPair.second << "\n";
                    }
            */
    ).exportCSVFile();
}
