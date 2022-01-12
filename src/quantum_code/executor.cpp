/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/quantum_code/executor.hpp>
#include <ket/quantum_code/execution_exception.hpp>
#include <boost/container/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/complex.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <chrono>
#include <future>

using namespace ket::quantum_code;

executor_t::executor_t(quantum_code_ptr quantum_code, features_t features) :
    quantum_code{quantum_code}, features{features}
{
    check_features();
}

void executor_t::check_features() {
    quantum_code_t code{quantum_code};
    auto& needed_features = code.header->features;
    if (((features^features_t::all) ^ needed_features) != needed_features) {
        throw feature_not_available_error();
    }
}

ket::quantum_result_ptr executor_t::get_result() {
    std::stringstream dump_stream;
    boost::archive::binary_oarchive oarchive{dump_stream};
    for (auto &dump : dump_map) oarchive << dump.second;

    auto num_int = int_map.size();
    auto num_dump = dump_map.size();
    size_t dump_data_size = dump_stream.tellp();

    auto base_size = 
        sizeof(quantum_result::header_t) +
        sizeof(quantum_result::int_result_t)*num_int +
        sizeof(quantum_result::dump_result_t)*num_dump;

    auto result_size = base_size+dump_data_size;

    ket::quantum_result_ptr result{new char[result_size]};
    
    auto* header      = reinterpret_cast<quantum_result::header_t*>(result.get());
    auto* int_result  = reinterpret_cast<quantum_result::int_result_t*>(header+1);
    auto* dump_result = reinterpret_cast<quantum_result::dump_result_t*>(int_result+num_int);
    auto* dump_data   = reinterpret_cast<char*>(dump_result+num_dump);

    *header = {};
    header->size      = result_size;
    header->num_int   = num_int;
    header->num_dump  = num_dump;
    header->dump_size = dump_data_size;
    header->exec_time = exec_time;

    auto index = 0;
    for (auto int_result_ : int_map) {
        int_result[index].index = int_result_.first;
        int_result[index].value = int_result_.second;
        index++;
    }
    
    index = 0;
    for (auto &dump : dump_map) {
        dump_result[index].index = dump.first;
        index++;
    }

    dump_stream.read(dump_data, dump_data_size);

    return result;    
}
