/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/quantum_code/executor.hpp>
#include <ket/quantum_code/execution_exception.hpp>
#include <boost/container/vector.hpp>
#include <chrono>
#include <future>

using namespace ket::quantum_code;

void executor_t::run() {
    std::uint64_t next_block = 0;

    ctrl_list_t ctrl_list;
    qubit_list_t qubit_list;   

    double param;
    char* plugin_args;
    std::uint64_t then, otherwise;

    boost::container::vector<std::uint32_t> int_stack;

    bool not_halt = true;

    auto int_op = [&](auto index, auto op) {
        int_map[index] = op(int_map[int_stack[0]], int_map[int_stack[1]]);
        int_stack.clear();
    };

    std::atomic_bool keep_running{true};

    quantum_code_t<decltype(quantum_code)> code{quantum_code};

    auto get_addr = [&](auto offset) {
        return reinterpret_cast<decltype(offset)>(
            reinterpret_cast<size_t>(quantum_code.get()) +
            reinterpret_cast<size_t>(offset)
        );
    };
    
    
    auto exec_time_future = std::async(std::launch::async, [&]() {
        auto start = std::chrono::high_resolution_clock::now();
        while (not_halt and keep_running) {
            auto current_block = code.block+next_block;
            auto pc = get_addr(current_block->code_begin);
            auto end = pc+current_block->num_intructions;

            auto get_str = [&](auto offset) {
                return reinterpret_cast<decltype(offset)>(
                    reinterpret_cast<size_t>(quantum_code.get()) +
                    reinterpret_cast<size_t>(current_block->args_begin) +
                    reinterpret_cast<size_t>(offset)
                );
            };
            
            for (; pc < end; pc++) if (keep_running) {
                switch (pc->op_code) {
                case op_code_t::alloc:
                    alloc(boost::get<index_t>(pc->arg), false);
                    break;
                case op_code_t::alloc_dirty:
                    alloc(boost::get<index_t>(pc->arg), true);
                    break;
                case op_code_t::free:
                    free(boost::get<index_t>(pc->arg), false);
                    break;
                case op_code_t::free_dirty:
                    free(boost::get<index_t>(pc->arg), true);
                    break;
                case op_code_t::gate_pauli_x:
                    pauli_x(boost::get<index_t>(pc->arg), ctrl_list);
                    break;
                case op_code_t::gate_pauli_y:
                    pauli_y(boost::get<index_t>(pc->arg), ctrl_list);
                    break;
                case op_code_t::gate_pauli_z:
                    pauli_z(boost::get<index_t>(pc->arg), ctrl_list);
                    break;
                case op_code_t::gate_hadamard:
                    hadamard(boost::get<index_t>(pc->arg), ctrl_list);
                    break;
                case op_code_t::gate_phase:
                    phase(param, boost::get<index_t>(pc->arg), ctrl_list);
                    break;
                case op_code_t::gate_rotation_x:
                    rotation_x(param, boost::get<index_t>(pc->arg), ctrl_list);
                    break;
                case op_code_t::gate_rotation_y:
                    rotation_y(param, boost::get<index_t>(pc->arg), ctrl_list);
                    break;
                case op_code_t::gate_rotation_z:
                    rotation_z(param, boost::get<index_t>(pc->arg), ctrl_list);
                    break;
                case op_code_t::set_param:
                    param = boost::get<double>(pc->arg);
                    break;
                case op_code_t::push_ctrl:
                    ctrl_list.push_back(boost::get<index_t>(pc->arg));
                    break;
                case op_code_t::pop_ctrl:
                    ctrl_list.resize(ctrl_list.size()-boost::get<index_t>(pc->arg));
                    break;
                case op_code_t::push_qubit:
                    qubit_list.push_back(boost::get<index_t>(pc->arg));
                    break;
                case op_code_t::measure:
                    int_map[boost::get<index_t>(pc->arg)] = measure(qubit_list);
                    qubit_list.clear();
                    break;
                case op_code_t::get_plugin_args:
                    plugin_args = get_str(boost::get<char*>(pc->arg));
                    break;
                case op_code_t::plugin:
                    plugin(get_str(boost::get<char*>(pc->arg)), qubit_list, plugin_args, ctrl_list);
                    qubit_list.clear();
                    break;
                case op_code_t::set_then:
                    then = boost::get<index_t>(pc->arg);
                    break;
                case op_code_t::set_else:
                    otherwise = boost::get<index_t>(pc->arg);
                    break;
                case op_code_t::breach:
                    next_block = int_map[boost::get<index_t>(pc->arg)]? then : otherwise;
                    break;
                case op_code_t::jump:
                    next_block = boost::get<index_t>(pc->arg);
                    break; 
                case op_code_t::push_int:
                    int_stack.push_back(boost::get<index_t>(pc->arg));
                    break;
                case op_code_t::int_eq:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a == b; });
                    break;
                case op_code_t::int_neq:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a != b; });
                    break;
                case op_code_t::int_gt:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a > b; });
                    break;
                case op_code_t::int_geq:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a >= b; });
                    break;
                case op_code_t::int_lt:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a < b; });
                    break;
                case op_code_t::int_leq:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a >= b; });
                    break;
                case op_code_t::int_add:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a + b; });
                    break;
                case op_code_t::int_sub:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a - b; });
                    break;
                case op_code_t::int_mul:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a * b; });
                    break;
                case op_code_t::int_div:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a / b; });
                    break;
                case op_code_t::int_sll:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a << b; });
                    break;
                case op_code_t::int_srl:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a >> b; });
                    break;
                case op_code_t::int_and:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a & b; });
                    break;
                case op_code_t::int_or:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a | b; });
                    break;
                case op_code_t::int_xor:
                    int_op(boost::get<index_t>(pc->arg), [](auto a, auto b) {return a ^ b; });
                    break;
                case op_code_t::int_set:
                    int_map[boost::get<index_t>(pc->arg)] = int_stack[0];
                    int_stack.clear();
                    break;
                case op_code_t::int_const:
                    int_map[int_stack[0]] = boost::get<int_t>(pc->arg);
                    int_stack.clear();
                    break;
                case op_code_t::dump:
                    dump_map[boost::get<index_t>(pc->arg)] = dump(qubit_list);
                    qubit_list.clear();
                    break;
                case op_code_t::halt:
                    not_halt = false;
                    break;  
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        double exec_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        return exec_time * 1e-9;
    });   

    auto &timeout = code.header->timeout;
    if (timeout > 0) {
        auto status = exec_time_future.wait_for(std::chrono::seconds(timeout));
        if (status == std::future_status::timeout) {
            keep_running = false;
            throw quantum_execution_timeout{};
        }
    }

    exec_time = exec_time_future.get(); 
}