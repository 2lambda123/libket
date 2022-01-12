/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <boost/container/vector.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/container/map.hpp>
#include <boost/unordered_map.hpp>
#include <ket/quantum_code/quantum_code.hpp>
#include <ket/quantum_code/quantum_result.hpp>
#include <ket/util/types.hpp>


namespace ket::quantum_code {
    using dump::dump_t;
    using ctrl_list_t = boost::container::vector<index_t>;
    using qubit_list_t = ctrl_list_t;

    using int_map_t = boost::unordered_map<index_t,int_t>;
    using dump_map_t = boost::container::map<index_t,dump_t>;

    class executor_t {
    public:
        executor_t(quantum_code_ptr, features_t);

        void run();
        quantum_result_ptr get_result();

        virtual void alloc(index_t, bool dirty) = 0;
        virtual void free(index_t, bool dirty) = 0;

        virtual void pauli_x(index_t, ctrl_list_t) = 0;
        virtual void pauli_y(index_t, ctrl_list_t) = 0;
        virtual void pauli_z(index_t, ctrl_list_t) = 0;

        virtual void phase(double, index_t, ctrl_list_t) = 0;

        virtual void hadamard(index_t, ctrl_list_t) = 0;

        virtual void rotation_x(double, index_t, ctrl_list_t) = 0;
        virtual void rotation_y(double, index_t, ctrl_list_t) = 0;
        virtual void rotation_z(double, index_t, ctrl_list_t) = 0;

        virtual int_t measure(qubit_list_t) = 0;

        virtual void plugin(char* name, qubit_list_t, char* args, ctrl_list_t) = 0;

        virtual dump_t dump(qubit_list_t) = 0;

    private:
        void check_features();

        quantum_code_ptr quantum_code;
        std::uint32_t    features;     

        int_map_t  int_map;
        dump_map_t dump_map;

        double exec_time;
    };

} // ket::quantum_code