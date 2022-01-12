/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <ket/quantum_code/executor.hpp>
#include <array>
#include <stack>
#include <boost/unordered_map.hpp>

namespace ket::bitwise {
    using namespace quantum_code;

    class kbw_t : public executor_t {
        using qubit_stack_t = std::stack<index_t>;
        using qubit_map_t = boost::unordered_map<index_t,index_t>;
        using quantum_space_t = boost::unordered_map<dump::state_t,dump::complex_t>;

    public:
        kbw_t(quantum_code_ptr);
        
        virtual void alloc(index_t, bool dirty);
        virtual void free(index_t, bool dirty);

        virtual void pauli_x(index_t, ctrl_list_t);
        virtual void pauli_y(index_t, ctrl_list_t);
        virtual void pauli_z(index_t, ctrl_list_t);

        virtual void phase(double, index_t, ctrl_list_t);

        virtual void hadamard(index_t, ctrl_list_t);

        virtual void rotation_x(double, index_t, ctrl_list_t);
        virtual void rotation_y(double, index_t, ctrl_list_t);
        virtual void rotation_z(double, index_t, ctrl_list_t);

        virtual int_t measure(qubit_list_t);

        virtual void plugin(char* name, qubit_list_t, char* args, ctrl_list_t);

        virtual dump_t dump(qubit_list_t);
    private:
        qubit_list_t map_qubits(qubit_list_t);

        static const int qubit_multiplier = 2;
        static const int qubits_number_base = sizeof(dump::state_base_t)*8;
        static const int qubit_number = qubits_number_base*qubit_multiplier;

        qubit_stack_t free_qubits;
        qubit_stack_t dirty_qubits;
        qubit_map_t qubit_map;

        quantum_space_t quantum_space;
    };

    extern "C" quantum_result_ptr ket_quantum_executor(quantum_code_ptr);
} // ket::bitwise