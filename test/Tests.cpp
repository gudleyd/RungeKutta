namespace tests_rk {

    template <typename measure>
    std::string getSuffix() {   
        if (typeid(measure) == typeid(nanosec))
            return "ns";
        else if (typeid(measure) == typeid(microsec))
            return "us";
        else if (typeid(measure) == typeid(millisec))
            return "ms";
        else if (typeid(measure) == typeid(std::chrono::seconds))
            return "s";
        return "(user defined time measure)";
    }

    template<int P, typename measure>
    measure OverkillTimer<P, measure>::reset(int discardResult) {
        auto cur_time = measure_cast<measure>(std::chrono::steady_clock::now());
        measure x = cur_time - start_time;
        start_time = cur_time;
        if (discardResult)
            return x;
        sum_time += x;
        if (iterations >= 5) {
            p_square(x);
        } else {
            // Just setup
            q[iterations] = x;
            if (iterations == 4) {
                std::qsort(q, 5, sizeof(measure), [](const void* a, const void* b) {
                    auto a_val = *(measure *)a;
                    auto b_val = *(measure *)b;
                    return (a_val < b_val) ? -1 : (a_val != b_val);
                });
            }
        }
        ++iterations;
        start_time = measure_cast<measure>(std::chrono::steady_clock::now());
        return x;
    }
    template<int P, typename measure>
    OverkillTimer<P, measure>::~OverkillTimer() {
        auto diff = measure_cast<measure>(std::chrono::steady_clock::now()) - start_time;
        auto suffix = getSuffix<measure>();
        out << "[\t" << name << "\t]\n";
        out << std::string(15 + name.size(), '-') << '\n';
        out << "Iterations: " << iterations << '\n' << "Last run result:\t" << diff.count() << suffix << '\n';
        if (iterations > 1) {
            out << "Estimated avg. value:\t" << ((long double)sum_time.count()) / iterations << suffix << '\n';
            if (iterations > 5)
                out << "Estimated " << P << " quantile:\t" << q[3].count() << suffix << "\n\n";
        }
    }

    template<int P, typename measure>
    void OverkillTimer<P, measure>::p_square(measure x) {
        int k = 0;
        if (x < q[0])
            q[0] = x;
        for (size_t i = 1; i < 5; ++i)
            if (x < q[i]) {
                k = i - 1;
                break;
            }
        if (x > q[4]) {
            q[4] = x;
            k = 3;
        }
        // Adjust position of markers k + 1 to 5
        for (size_t i = k + 1; i < 5; ++i) {
            n[i] += 1;
        }
        // Now update all desired positions:
        for (size_t i =0; i < 5; ++i) {
            desired_n[i] += dn[i];
        }

        // If necessary adjust middle 3 marker values (e.g. q[i] where 1 <= i <= 3)
        for (size_t i = 1; i < 4; ++i) {
            double d = desired_n[i] - n[i];
            auto d1 =  n[i+1] - n[i];
            auto d2 = n[i] - n[i-1];
            if ((d <= -1 && d2 > 1) || (d >= 1 && d1 > 1)) {
                d = (d >= 0) ? 1 : -1;
                // Use P^2 formula (e.g. adjust q in a such a way, that all three points sit on a parabola)
                double new_q = (d / (n[i+1] - n[i-1])) * ( (d2 + d) * ((q[i+1] - q[i]).count()/ d1) + (d1 - d) * ((q[i] - q[i-1]).count()/ d2) );

                 if (q[i - 1].count() < new_q && new_q < q[i + 1].count()) {
                    q[i] += measure(static_cast<int64_t>(std::round(new_q)));
                } else {
                    // If the P^2 formula couldn't properly adjust q[i]
                    // Use this linear workaround
                    q[i] += measure(static_cast<int64_t>(std::round(d * static_cast<double>((q[i + static_cast<int>(d)] - q[i]).count()) / (n[i +  static_cast<int>(d)] - n[i]))));
                }
                n[i] += d;
            }
        }
    }

    template<typename ValueType>
    const void * get_conv_func() {
         if (typeid(ValueType) == typeid(float)) {
            return (const void *)utils_rk::stringToFloat;
        } else if (typeid(ValueType) == typeid(double)) {
            return  (const void *)utils_rk::stringToDouble;
        } else if (typeid(ValueType) == typeid(long double)) {
            return (const void *)utils_rk::stringToLongDouble;
        } else {
            throw std::invalid_argument("Incorrect ValueType");
        }
    }

    template<typename ValueType>
    template<class Container>
    int BasicTest<ValueType>::loadInputFile(const std::string& fileName, Container& data, bool readFlag) {
        std::ifstream inFile;
        inFile.open(fileName);
        if (!inFile.is_open()) {
            std::cerr << "UNABLE TO OPEN FILE [ " << fileName << " ]\n";
            return 1;
        }
        if (!readFlag) {
            for (auto it = data.begin(); it != data.end(); ++it) {
                inFile >> *it;
            }
        } else {
            std::getline(inFile, data.data);
        }
        return 0;
    }

    template<typename ValueType>
    BasicTest<ValueType>::BasicTest(const std::string& _func, const std::vector<std::string>& _vars, double _delta, 
                                    const std::vector<std::vector<ValueType>>& _points, const std::vector<ValueType>& _values): 
                                    expr(), func(_func), vars(_vars), delta(_delta), pos(_points), vals(_values) {
        this->force_parse();
    }

    template<typename ValueType>
    BasicTest<ValueType>::BasicTest(const std::string& _func, const std::vector<std::string>&& _vars, double _delta, 
                                    const std::vector<std::vector<ValueType>>&& _points, 
                                    const std::vector<ValueType>&& _values): 
                                    expr(), func(_func), vars(std::move(_vars)), 
                                    delta(std::move(_delta)), pos(std::move(_points)), vals(std::move(_values)) {
        this->force_parse();
    }

    template<typename ValueType>
    BasicTest<ValueType>::BasicTest(const std::string& funcFile, const std::vector<std::string>& _vars, double _delta, 
                                    const std::string& _pointFile, const std::string& _valFile): 
                                    expr(), vars(_vars), delta(_delta) {
        if (BasicTest<ValueType>::loadInputFile<std::string>(funcFile, &func, 1))
            throw std::runtime_error("Unable to load function from file");
        if (BasicTest<ValueType>::loadInputFile<std::vector<std::vector<ValueType>>>(_pointFile, &pos))
            throw std::runtime_error("Unable to load points from file");
        if (BasicTest<ValueType>::loadInputFile<std::vector<ValueType>>(_valFile, &vals))
            throw std::runtime_error("Unable to load values from file");
        this->force_parse();
    }

    template<typename ValueType>
    void  BasicTest<ValueType>::force_parse() {
        expr.parse(func, vars, (std::pair<ValueType, bool> (*)(const std::string&))get_conv_func<ValueType>());
    }

    template<typename ValueType>
    int BasicTest<ValueType>::run_parse_test(std::ostream& out) {
        out << "\nTesting parsing for function [" << func << "]\n";
        out << "expr addr (Debug) [" <<  &expr << "]\n";
        expr.compile();
        size_t i = 0, errCount = 0;
        for (auto it = pos.begin(); it != pos.end(); ++it) {
            auto tmpVal = expr.evaluate(*it);
            if (fabs(tmpVal - vals[i]) > delta) {
                out << "Solution at point [" << i << "] deviates more than delta " << delta << "\n";
                out << "Expected solution [" << vals[i] << "]\n";
                out << "Got [" << tmpVal << "]\n";
                ++errCount;
            }
            ++i;
        }
        out << "\nFinished test with [" << errCount << "] errors\n\n";
        return errCount;
    }
    
    template<typename ValueType>
    int BasicTest<ValueType>::run_solve_test(std::vector<ValueType> initValues, double gridSize, std::ostream& out) {
        size_t errCount = 0;
        for (auto it = pos.begin(); it != pos.end(); ++it) {
            auto res = rk::RKSolve<ValueType>(expr, initValues, *it[1], gridSize);
            size_t i = 0;
            for (auto coord = res.begin(); coord != res.end(); ++coord) {
                if (fabs(*coord - *it[i])) {
                    out << "Solution at point [" << i << "] deviates more than delta " << delta << "\n";
                    ++errCount;
                    break;
                }
            }
        }
        return errCount;
    }
}