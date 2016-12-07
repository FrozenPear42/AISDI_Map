#include <initializer_list>
#include <string>
#include <functional>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <map>
#include <list>


namespace bm {
    class Benchmark {
    public:

        static void runSingle(std::function<void(int)> pFunc, std::string pName, std::initializer_list<int> pCases,
                              std::ostream& pOut = std::cout) {
            using namespace std::chrono;
            time_point<system_clock> start, end;
            pOut << "Benchmark: " << pName << std::endl;
            for (auto&& e: pCases) {
                pOut << e << "\t\t\t";
                start = system_clock::now();
                pFunc(e);
                end = system_clock::now();

                duration<double> elapsed = end - start;
                pOut << std::setiosflags(std::ios::fixed) << std::setprecision(10) << elapsed.count() << std::endl;
            }
        }

        Benchmark(std::string pName,
                  std::function<void(int)> pFunc,
                  std::initializer_list<int> pCases) : mName(pName), mTestFunc(pFunc) {
            for (auto&& item : pCases)
                mResults.insert(std::make_pair(item, -1.0));
        };

        Benchmark& run() {
            return run([](std::pair<const int, double>) {});
        }

        template<typename Tt>
        Benchmark& run(Tt pCallback) {
            using namespace std::chrono;
            time_point<system_clock> start, end;
            duration<double> elapsed;
            for (auto&& item : mResults) {
                start = system_clock::now();
                mTestFunc(item.first);
                end = system_clock::now();
                elapsed = end - start;
                item.second = elapsed.count();
                pCallback(item);
            }
            return *this;
        }

        Benchmark& exportFancy(std::ostream& pOut) {
            pOut << "Benchmark: " << mName << "\n";
            for (auto&& result : mResults)
                pOut << result.first << "\t\t\t" << std::setprecision(10) << result.second << "\n";
            return *this;
        }

        Benchmark& exportCSV(std::ostream& pOut) {
            pOut << "N," << mName << "\n";
            for (auto&& result : mResults)
                pOut << result.first << "," << std::setprecision(10) << result.second << "\n";
            return *this;
        }

    private:
        std::string mName;
        std::function<void(int)> mTestFunc;
        std::map<int, double> mResults;

    };


    class BenchmarkSuite {
    public:

        BenchmarkSuite(std::string pName) : mName(pName), mReady(false) {};

        template<typename Tt>
        BenchmarkSuite(std::string pName, std::initializer_list<Tt>&& pBenchmarks) : BenchmarkSuite(pName) {
            for (auto&& item : pBenchmarks)
                mBenchmarks.emplace_back(std::forward<Tt>(item));
        }

        template<typename Tt>
        BenchmarkSuite& addBenchmark(Tt&& pBenchmark) {
            mBenchmarks.emplace_back(std::forward<Tt>(pBenchmark));
            mReady = false;
            return *this;
        }

        template<typename Tt>
        BenchmarkSuite& run(Tt pCallback) {
            for (auto&& item : mBenchmarks)
                item.run(std::forward<Tt>(pCallback));
            mReady = true;
            return *this;
        }

        BenchmarkSuite& run() {
            for (auto&& item : mBenchmarks)
                item.run();
            mReady = true;
            return *this;
        }


        BenchmarkSuite& exportFancy(std::ostream& pOut) {
            if (!mReady)
                throw std::logic_error("Trying to print unready benchmark results");
            pOut << "Benchmark Suite: " << mName << "\n\n";
            for (auto&& bench : mBenchmarks) {
                bench.exportFancy(pOut);
                pOut << "\n";
            }
            return *this;
        }

        BenchmarkSuite& exportCSV(std::ostream& pOut) {
            if (!mReady)
                throw std::logic_error("Trying to print unready benchmark results");
            pOut << mName << "\n\n";
            for (auto&& bench : mBenchmarks) {
                bench.exportCSV(pOut);
                pOut << "\n";
            }
            return *this;
        }

    private:
        std::string mName;
        std::list<Benchmark> mBenchmarks;
        bool mReady;
    };
}