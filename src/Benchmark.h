#include <initializer_list>
#include <string>
#include <functional>
#include <iostream>
#include <chrono>
#include <iomanip>

class Benchmark {
public:
    static void run(std::function<void(int)> pFunc, std::string pName, std::initializer_list<int> pCases, std::ostream& pOut = std::cout) {
        using namespace std::chrono;
        time_point <system_clock> start, end;
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
};
