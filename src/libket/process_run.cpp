/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/process.hpp>
#include <boost/dll/import.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/complex.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <ket/quantum_code/quantum_result.hpp>
#include <fstream>

using namespace ket::libket;

void process_t::run() {
    block_map[current_block].add_instruction({
        quantum_code::op_code_t::halt, 0.0
    });

    try {
        auto kqasm_path = load_var<std::string>("KQASM_OUTPUT");
        std::ofstream kqasm{kqasm_path, std::ios::app};
        for (auto &block : block_map) {
            kqasm << "l" << block.first << ":" << std::endl;
            kqasm << block.second.str() << std::endl;
        }
        kqasm << "===========================" << std::endl;
    } catch (...) {}

    auto block_num = block_map.size();
    auto header_code_size = sizeof(quantum_code::header_t)+
        sizeof(quantum_code::block_t)*block_num;
    size_t blocks_size = 0;
    for (auto &block : block_map) blocks_size += block.second.size();

    auto total_size = header_code_size+blocks_size;

    quantum_code_ptr quantum_code{new char[total_size]};

    auto header = reinterpret_cast<quantum_code::header_t*>(quantum_code.get());
    *header = quantum_code::header_t{};
    header->size = total_size;
    header->num_blocks = block_num;
    header->features = features;
    header->num_qubits = max_num_qubit;
    header->timeout = timeout;

    auto blocks = reinterpret_cast<quantum_code::block_t*>(header+1);   
    char* next_block = reinterpret_cast<char*>(header_code_size);

    for (auto &block : block_map) {
        next_block += block.second.write(blocks+block.first, quantum_code.get(), next_block);
    }

    auto quantum_executor = boost::dll::import_symbol<quantum_result_ptr(quantum_code_ptr)>(load_var<std::string>("KET_QUANTUM_EXECUTOR"), "ket_quantum_executor");
    auto result_ptr = quantum_executor(quantum_code);

    quantum_result::quantum_result_t<decltype(result_ptr)> result{result_ptr};

    for (auto i = 0u; i < result.header->num_int; i++) {
        auto &future = future_map[result.int_result[i].index];
        *future.value_ = result.int_result[i].value;
        *future.available_ = true;
    }

    std::stringstream dump_stream;
    dump_stream.write(result.dump_data, result.header->dump_size);   
    boost::archive::binary_iarchive iarchive{dump_stream};

    for (auto i = 0u; i < result.header->num_dump; i++) {
        auto &dump_obj = dump_map[result.dump_result[i].index];
        iarchive >> (*dump_obj.data);
        *dump_obj.available_ = true;
    }

    exec_time_ = result.header->exec_time;
}