#include <chrono>
#include "../src/expression/Expression.h"


namespace tests_rk {
    
    enum TestType {
        BASIC_PARSE_TEST = 0,
        BASIC_BENCHMARK = 1,
        LOOP_TEST = 2,
        LOOP_BENCHMARK = 3
    };

    using nanosec = std::chrono::nanoseconds;
    using microsec = std::chrono::microseconds;
    using millisec = std::chrono::milliseconds;
    using s_clock = std::chrono::steady_clock;

    template<typename measure = microsec, typename T = std::chrono::_V2::steady_clock::time_point>
    auto measure_cast(const T& val) { 
        return std::chrono::time_point_cast<measure>(val);
    }

    template<int P = 95, typename measure = microsec>
    class OverkillTimer {
    public:
        OverkillTimer(): name("Timer"), start_time(measure_cast<measure>(s_clock::now())), out(std::cout) {};
        explicit OverkillTimer(const std::string& _name, std::ostream& _out = std::cout): name(_name), start_time(measure_cast<measure>(s_clock::now())), out(_out) {};
        explicit OverkillTimer(const char * _name, std::ostream& _out = std::cout): name(_name), start_time(measure_cast<measure>(s_clock::now())), out(_out) {};
        ~OverkillTimer();
        std::time_t getTime();
        measure reset(int discardResult = 0);
        std::time_t get95Percentile();
        
    private:
        std::string name;
        std::chrono::time_point<s_clock, measure> start_time;
        std::ostream& out;
        measure sum_time{0};
        uint64_t iterations{0};
        measure q[5];
        constexpr static const double dn[5] = {0.0, (static_cast<double>(P))/200 , static_cast<double>(P), (1 + static_cast<double>(P))/200, 1.0};
        double desired_n[5] = {1, 1 + ((static_cast<double>(P)) / 50), 1 + ((static_cast<double>(P)) / 25), 3 + ((static_cast<double>(P)) / 50), 5};
        uint64_t n[5] ={1, 2, 3, 4, 5};
        void p_square(measure);
    };

    template <typename measure>
    std::string getSuffix();

    // run_test is not included in this commit (its not functional yet)
    // Will just open a text document with a bunch of functions
    // Pick functions randomly, or based on provided arguments and run tests with them
    //template<typename T>
    //void run_test(std::string&, std::vector<T>&, TestType);


    // TODO: random func generator
    /*
    template<typename T>
    std::string gen_string(std::vector<T>&);
    */

}

#include "Tests.cpp"