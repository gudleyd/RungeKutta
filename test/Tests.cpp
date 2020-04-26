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

    /* run_test() is in development */


}

