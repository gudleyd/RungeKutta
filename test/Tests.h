#include <chrono>
#include <stdexcept>
#include "../src/expression/Expression.h"

#pragma once

namespace tests_rk {
    
    enum TestType {
        BASIC_PARSE = 0,
        BASIC_SOLVE = 1,
        LOOP_PARSE = 2,
        LOOP_SOLVE = 3
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

    template<typename ValueType>
    const void * get_conv_func();
    
    template<typename ValueType>
    class BasicTest {
    public:
        BasicTest(const std::string&, const std::vector<std::string>&, double, const std::vector<std::vector<ValueType>>&, const std::vector<ValueType>&);
        BasicTest(const std::string&, const std::vector<std::string>&&, double, const std::vector<std::vector<ValueType>>&&, const std::vector<ValueType>&&);
        explicit BasicTest(const std::string&, const std::vector<std::string>&, double, const std::string&, const std::string&);
        int run_parse_test(std::ostream& out = std::cout);
        int run_solve_test(std::vector<ValueType> initValues, double gridSize = 0.0001, std::ostream& out = std::cout);
        void force_parse();
    private:
        std::string func;
        std::vector<std::string> vars;
        rk::Expression<ValueType> expr;
        double delta;
        std::vector<std::vector<ValueType>> pos;
        std::vector<ValueType> vals;

        template<class Container>
        static int loadInputFile(const std::string&, Container&, bool readFlag=0);
        
    };

    // TODO: random func generator
    /*
    template<typename T>
    std::string gen_string(std::vector<T>&);
    */

}

#include "Tests.cpp"