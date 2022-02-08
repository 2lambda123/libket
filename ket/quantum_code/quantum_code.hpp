/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <cstdint>
#include <variant>
#include <ket/util/types.hpp>
#include <boost/variant.hpp>

namespace ket {
namespace quantum_code {

    enum features_t : std::uint32_t {
        none_feature               = 0,
        gate_pauli_x               = 0b1,
        gate_ctrl_pauli_x          = 0b10,
        gate_mult_ctrl_pauli_x     = 0b100,
        
        gate_pauli_y               = 0b1000,
        gate_ctrl_pauli_y          = 0b10000,
        gate_mult_ctrl_pauli_y     = 0b100000,
        
        gate_pauli_z               = 0b1000000,
        gate_ctrl_pauli_z          = 0b10000000,
        gate_mult_ctrl_pauli_z     = 0b100000000,
        
        gate_phase                 = 0b1000000000,
        gate_ctrl_phase            = 0b10000000000,
        gate_mult_ctrl_phase       = 0b100000000000,
        
        gate_hadamard              = 0b1000000000000,
        gate_ctrl_hadamard         = 0b10000000000000,
        gate_mult_ctrl_hadamard    = 0b100000000000000,
        
        gate_rotation_x            = 0b1000000000000000,
        gate_ctrl_rotation_x       = 0b10000000000000000,
        gate_mult_ctrl_rotation_x  = 0b100000000000000000,
        
        gate_rotation_y            = 0b1000000000000000000,
        gate_ctrl_rotation_y       = 0b10000000000000000000,
        gate_mult_ctrl_rotation_y  = 0b100000000000000000000,
        
        gate_rotation_z            = 0b1000000000000000000000,
        gate_ctrl_rotation_z       = 0b10000000000000000000000,
        gate_mult_ctrl_rotation_z  = 0b100000000000000000000000,
        
        qubit_alloc_dirty          = 0b1000000000000000000000000,
        qubit_free                 = 0b10000000000000000000000000,
        qubit_free_dirty           = 0b100000000000000000000000000,
        
        int_operations             = 0b1000000000000000000000000000,
        int_set                    = 0b10000000000000000000000000000,

        plugin                     = 0b100000000000000000000000000000,
        
        quantum_dump               = 0b1000000000000000000000000000000,

        all                        = 0b1111111111111111111111111111111,
    };

    struct alignas(32) header_t {
        char magic_number[4] = {'K', 'Q', 'u', 'C'};
        std::uint16_t major_version = 1;
        std::uint16_t minor_version = 0;

        std::uint64_t size;
        std::uint32_t num_blocks;

        features_t    features;

        std::uint32_t num_qubits;

        std::uint64_t timeout;
    };

    enum class op_code_t : std::uint32_t {
        alloc,
        alloc_dirty,
    
        free,
        free_dirty,

        gate_pauli_x,
        gate_pauli_y,
        gate_pauli_z,

        gate_hadamard,
        gate_phase,

        gate_rotation_x,
        gate_rotation_y,
        gate_rotation_z,

        set_param,

        push_ctrl,
        pop_ctrl,

        push_qubit,

        measure,

        get_plugin_args,
        plugin,

        set_then,
        set_else,
        breach,

        jump, 

        push_int,

        int_eq,
        int_neq,
        int_gt,
        int_geq,
        int_lt,
        int_leq,
        int_add,
        int_sub,
        int_mul,
        int_div,
        int_sll,
        int_srl,
        int_and,
        int_or,
        int_xor,
        int_set,
        int_const,
        
        dump,

        halt,
    };

    struct alignas(32) intruction_t {
        op_code_t op_code; 
        boost::variant<double, index_t, char*, int_t> arg;
    };

    struct alignas(32) block_t {
        std::uint32_t num_intructions;
        intruction_t* code_begin;
        char*         args_begin;
    };

    template<class PTR>
    struct quantum_code_t {
        quantum_code_t(PTR quantum_code) :
            header{reinterpret_cast<header_t*>(quantum_code.get())},
            block{reinterpret_cast<block_t*>(header+1)}
            {}
            
        header_t* header;
        block_t* block;
    };   

}} // namespace ket::quantum_code
