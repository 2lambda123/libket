/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/container/map.hpp>
#include <boost/unordered_map.hpp>
#include <ket/quantum_code/quantum_code.hpp>
#include <ket/quantum_code/quantum_result.hpp>
#include <ket/util/types.hpp>
#include <stack>

namespace ket::quantum_code {
    using dump::dump_t;
    using ctrl_list_t = std::vector<index_t>;
    using qubit_list_t = ctrl_list_t;

    using int_map_t = boost::unordered_map<index_t,int_t>;
    using dump_map_t = boost::container::map<index_t,dump_t>;

    using qubit_stack_t = std::stack<index_t>;
    using qubit_map_t = boost::unordered_map<index_t,index_t>;

    class executor_t {
    public:
        executor_t(quantum_code_ptr, features_t);

        void run();
        quantum_result_ptr get_result();

        virtual void alloc(index_t, bool dirty);
        virtual void free(index_t, bool dirty);

        void pauli_x(index_t, ctrl_list_t);
        void pauli_y(index_t, ctrl_list_t);
        void pauli_z(index_t, ctrl_list_t);
        void phase(double, index_t, ctrl_list_t) ;
        void hadamard(index_t, ctrl_list_t);
        void rotation_x(double, index_t, ctrl_list_t);
        void rotation_y(double, index_t, ctrl_list_t);
        void rotation_z(double, index_t, ctrl_list_t);
        int_t measure(qubit_list_t);
        void plugin(char* name, qubit_list_t, char* args, ctrl_list_t);
        dump_t dump(qubit_list_t);

    protected:
        void init();
        size_t num_qubits;

    private:
        virtual void _pauli_x(index_t, ctrl_list_t) = 0;
        virtual void _pauli_y(index_t, ctrl_list_t) = 0;
        virtual void _pauli_z(index_t, ctrl_list_t) = 0;
        virtual void _phase(double, index_t, ctrl_list_t) = 0;
        virtual void _hadamard(index_t, ctrl_list_t) = 0;
        virtual void _rotation_x(double, index_t, ctrl_list_t) = 0;
        virtual void _rotation_y(double, index_t, ctrl_list_t) = 0;
        virtual void _rotation_z(double, index_t, ctrl_list_t) = 0;
        virtual bool _measure(index_t) = 0;
        virtual void _plugin(char* name, qubit_list_t, char* args, ctrl_list_t, bool ajd) = 0;
        virtual dump_t _dump(qubit_list_t) = 0;

        virtual void swap(index_t, index_t) = 0;

        qubit_list_t map_qubits(qubit_list_t);

        quantum_code_ptr quantum_code;
        std::uint32_t    features;     

        int_map_t  int_map;
        dump_map_t dump_map;

        qubit_stack_t free_qubits;
        qubit_stack_t dirty_qubits;
        qubit_map_t qubit_map;

        double exec_time;
    };

} // ket::quantum_code