/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <cstdint>
#include <ket/util/types.hpp>
#include <boost/shared_ptr.hpp>

namespace ket::quantum_result {

    struct alignas(32) header_t {
        char magic_number[4] = {'K', 'Q', 'u', 'R'};
        std::uint16_t major_version = 1;
        std::uint16_t minor_version = 0;

        std::uint64_t size;

        std::uint64_t num_int;
        std::uint64_t num_dump;
        std::uint64_t dump_size;

        double exec_time;
    };

    struct alignas(32) int_result_t {
        index_t index;
        int_t   value;
    };

    struct alignas(32) dump_result_t {
        index_t index;
    };

    template<class PTR>
    struct quantum_result_t {
        quantum_result_t(PTR quantum_result) :
            header{reinterpret_cast<header_t*>(quantum_result.get())},
            int_result{reinterpret_cast<int_result_t*>(header+1)},
            dump_result{reinterpret_cast<dump_result_t*>(int_result+header->num_int)},
            dump_data{reinterpret_cast<char*>(dump_result+header->num_dump)}
            {}
            
        header_t* header;
        int_result_t* int_result;
        dump_result_t* dump_result;
        char* dump_data;

    };
   
} // ket::quantum_code