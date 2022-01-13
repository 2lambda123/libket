/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <boost/unordered_map.hpp>
#include <boost/container/map.hpp>
#include <ket/libket/block.hpp>
#include <ket/libket/dump.hpp>
#include <ket/libket/future.hpp>
#include <ket/libket/label.hpp>
#include <ket/libket/qubit.hpp>
#include <ket/util/types.hpp>
#include <stack>
#include <string>

namespace ket::libket {

    enum class gate_t {
        pauli_x,    pauli_y,    pauli_z,
        rotation_x, rotation_y, rotation_z,
        hadamard,   phase,
    };

    using qubit_list_t = std::vector<qubit_t>;
    using quantum_code::features_t;

    class process_t {
        using block_map_t = boost::container::map<int,block_t>;
        using ctrl_stack_t = std::vector<qubit_list_t>;
        using future_map_t = boost::unordered_map<unsigned, future_t>;
        using dump_map_t = boost::unordered_map<unsigned, dump_t>;
        using ctrl_holder_t = std::stack<qubit_list_t>;
        
    public:
        process_t(index_t);
        ~process_t();

        qubit_t  alloc(bool dirty = false);
        void     free(qubit_t, bool dirty = false);
        void     gate(gate_t, qubit_t, double = 0);
        future_t measure(qubit_list_t);
        future_t new_int(int_t);
        void     plugin(std::string name, qubit_list_t, std::string args);
        void     ctrl_push(qubit_list_t);
        void     ctrl_pop();
        void     adj_begin();
        void     adj_end();
        label_t  get_label();
        void     open_block(label_t);
        void     jump(label_t);
        void     breach(future_t, label_t then, label_t otherwise);
        dump_t   dump(qubit_list_t);

        double exec_time() const;
        
        void run();

        index_t process_id() const;
        future_t int_op(int_op_t, future_t, future_t);

    private:
        void int_set(future_t dest, future_t src);

        void check_not_adj_ctrl();
        void check_not_free(qubit_t);
        void check_process_id(qubit_t);
        void check_process_id(future_t);
        void check_process_id(label_t);
        void check_not_in_ctrl(qubit_t);

        void add_feature(features_t);

        int num_qubit = 0;
        int max_num_qubit = 0;

        index_t qubit_count = 0;
        index_t future_count = 0;
        index_t dump_count = 0;
        index_t label_count = 1;

        index_t pid;

        block_map_t block_map;
        int         current_block;

        features_t features = features_t::none_feature;

        ctrl_stack_t ctrl_stack;
        ctrl_holder_t ctrl_holder;

        future_map_t future_map;
        dump_map_t dump_map;

        double exec_time_ = 0;

        bool_ptr alive;

        friend class future_t;
    };

} // ket::libket