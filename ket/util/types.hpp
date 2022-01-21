/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <complex>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>

namespace ket {
    namespace dump {
        using complex_t    = std::complex<double>;
        using state_base_t = std::uint64_t;
        using state_t      = std::vector<state_base_t>;
        using states_t     = std::vector<state_t>;
        using amplitudes_t = std::vector<complex_t>;
        using dump_t       = std::pair<states_t,amplitudes_t>;
        using dump_ptr     = boost::shared_ptr<dump_t>;
    }
    
    using index_t = std::uint32_t;
    using bool_ptr = boost::shared_ptr<bool>;
    using int_t = std::int64_t;
    using int_ptr = boost::shared_ptr<int_t>;

    using quantum_code_ptr = boost::shared_ptr<char[]>;
    using quantum_result_ptr = boost::shared_ptr<char[]>;

    template <class T>
    T load_var(std::string name) {
        auto var = std::getenv(name.c_str());
        if (not var) throw std::runtime_error{std::string{"undefined environment varaiable "}+name};
        return boost::lexical_cast<T>(var);
    }

} // ket