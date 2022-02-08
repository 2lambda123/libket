/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <ket/quantum_code/executor.hpp>
#include <boost/unordered_map.hpp>

namespace ket {
namespace bitwise {

    using namespace quantum_code;

    using quantum_space_t = boost::unordered_map<dump::state_t,dump::complex_t>;
    
    class kbw_t : public executor_t {
    public:
        kbw_t(quantum_code_ptr);
        static const size_t qubits_number_base = sizeof(dump::state_base_t)*8;

    private:
        virtual void _pauli_x(index_t, ctrl_list_t);
        virtual void _pauli_y(index_t, ctrl_list_t);
        virtual void _pauli_z(index_t, ctrl_list_t);
        virtual void _phase(double, index_t, ctrl_list_t);
        virtual void _hadamard(index_t, ctrl_list_t);
        virtual void _rotation_x(double, index_t, ctrl_list_t);
        virtual void _rotation_y(double, index_t, ctrl_list_t);
        virtual void _rotation_z(double, index_t, ctrl_list_t);
        virtual bool _measure(index_t);
        virtual void _plugin(char* name, qubit_list_t, char* args, ctrl_list_t, bool adj);
        virtual dump_t _dump(qubit_list_t);

        virtual void swap(index_t, index_t);

        quantum_code_t<quantum_code_ptr> quantum_code;

        size_t qubit_multiplier; 

        quantum_space_t quantum_space;
    };

    extern "C" quantum_result_ptr ket_quantum_executor(quantum_code_ptr);
}} // ket::bitwise