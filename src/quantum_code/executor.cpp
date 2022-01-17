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
#include <numeric>
#include <chrono>
#include <future>

using namespace ket::quantum_code;

executor_t::executor_t(quantum_code_ptr quantum_code, features_t features) :
    quantum_code{quantum_code}, features{features}
{}

void executor_t::init() {
    quantum_code_t code{quantum_code};
    auto& needed_features = code.header->features;
    if (((features^features_t::all) ^ needed_features) != needed_features) {
        throw feature_not_available_error();
    }
    if (code.header->num_qubits > num_qubits) {
        throw qubit_number_error{};
    }
    for (index_t qubit = 0; qubit < num_qubits; qubit++) {
        free_qubits.push(qubit);
    }
}

void executor_t::alloc(index_t qubit, bool dirty) {
    if (dirty and not dirty_qubits.empty()) {
        qubit_map[qubit] = dirty_qubits.top();
        dirty_qubits.pop();
    } else {
        qubit_map[qubit] = free_qubits.top();
        free_qubits.pop();
    }
}

void executor_t::pauli_x(index_t qubit, ctrl_list_t ctrl_list) {
    _pauli_x(qubit_map[qubit], map_qubits(ctrl_list));
}

void executor_t::pauli_y(index_t qubit, ctrl_list_t ctrl_list) {
    _pauli_y(qubit_map[qubit], map_qubits(ctrl_list));
}

void executor_t::pauli_z(index_t qubit, ctrl_list_t ctrl_list) {
    _pauli_z(qubit_map[qubit], map_qubits(ctrl_list));
}

void executor_t::phase(double lambda, index_t qubit, ctrl_list_t ctrl_list) {
    _phase(lambda, qubit_map[qubit], map_qubits(ctrl_list));
}

void executor_t::hadamard(index_t qubit, ctrl_list_t ctrl_list) {
    _hadamard(qubit_map[qubit], map_qubits(ctrl_list));
}

void executor_t::rotation_x(double theta, index_t qubit, ctrl_list_t ctrl_list) {
    _rotation_x(theta, qubit_map[qubit], map_qubits(ctrl_list));
}

void executor_t::rotation_y(double theta, index_t qubit, ctrl_list_t ctrl_list) {
    _rotation_y(theta, qubit_map[qubit], map_qubits(ctrl_list));
}

void executor_t::rotation_z(double theta, index_t qubit, ctrl_list_t ctrl_list) {
    _rotation_z(theta, qubit_map[qubit], map_qubits(ctrl_list));
}

ket::int_t executor_t::measure(qubit_list_t qubit_list) {
    return _measure(map_qubits(qubit_list));
}

void executor_t::plugin(char* name, qubit_list_t qubit_list, char* args, ctrl_list_t ctrl_list) {
    qubit_list = map_qubits(qubit_list);
    ctrl_list = map_qubits(ctrl_list);

    qubit_list_t qubits;
    qubits.insert(qubits.end(), qubit_list.rbegin(), qubit_list.rend());
    qubits.insert(qubits.end(), ctrl_list.rbegin(), ctrl_list.rend());
    
    qubit_list_t pos;
    pos.resize(num_qubits);
    std::iota(pos.begin(), pos.end(), 0);

    boost::container::vector<std::pair<ket::index_t, ket::index_t>> swap_list;

    for (auto i = 0u; i < qubits.size(); i++) {
        if (qubits[i] == pos[i]) continue;
        swap_list.push_back(std::make_pair(i, pos[qubits[i]]));
        auto tmp = pos[i];
        pos[i] = pos[qubits[i]];
        pos[qubits[i]] = tmp;
    }

    std::iota(qubit_list.rbegin(), qubit_list.rend(), 0);
    std::iota(ctrl_list.rbegin(), ctrl_list.rend(), qubit_list.size());

    for (auto i : swap_list) swap(i.first, i.second);

    _plugin(name[0] == '!'? name+1 : name, qubit_list, args, ctrl_list, name[0] == '!');    

    for (auto i : swap_list) swap(i.first, i.second);
}


dump_t executor_t::dump(qubit_list_t qubit_list) {
    return _dump(map_qubits(qubit_list));
}


void executor_t::free(index_t qubit, bool dirty) {
    if (dirty) {
        dirty_qubits.push(qubit);
    } else {
        free_qubits.push(qubit);
    }
    qubit_map.erase(qubit);
}

qubit_list_t executor_t::map_qubits(qubit_list_t qubit_list) {
    for (auto i = 0u; i < qubit_list.size(); i++) {
        qubit_list[i] = qubit_map[qubit_list[i]];
    }
    return qubit_list;
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
