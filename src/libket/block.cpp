/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/block.hpp>
#include <cstring>

using namespace ket::libket;

void block_t::add_instruction(intruction_t intruction) {
    check_ended();

    if (not adj_instructions.empty()) {
        adj_instructions.top().push(intruction);
    } else {
        instructions.push_back(intruction);
    }    
}

char* block_t::add_string(std::string str) {
    auto pos = reinterpret_cast<char*>(args.size());
    args += str + '\0';
    return pos;
}

bool block_t::adj() const {
    return adj_instructions.size() & 1;
}

bool block_t::not_adj() const {
    return adj_instructions.empty();
}

void block_t::adj_begin() {
    adj_instructions.push({});
}

void block_t::adj_end() {
    if (adj_instructions.empty()) {
        throw not_in_adj();
    } else if (adj_instructions.size() == 1) {
        while (not adj_instructions.top().empty()) {
            instructions.push_back(adj_instructions.top().top());
            adj_instructions.top().pop();
        }
        adj_instructions.pop();
    } else {
        auto to_pop = adj_instructions.top();
        adj_instructions.pop();
        while (not to_pop.empty()) {
            adj_instructions.top().push(to_pop.top());
            to_pop.pop();
        }
    }    
}

void block_t::end() {
    ended_ = true;
}

bool block_t::ended() const {
    return ended_;
}

size_t block_t::size() const {
    return sizeof(quantum_code::intruction_t)*instructions.size() 
        + args.size();
}

size_t block_t::write(quantum_code::block_t* header, char* begin, char* pos) const {
    header->num_intructions = instructions.size();
    header->code_begin = reinterpret_cast<quantum_code::intruction_t*>(pos);
    header->args_begin = reinterpret_cast<char*>(header->code_begin+instructions.size());

    std::memcpy(begin+reinterpret_cast<size_t>(header->code_begin), instructions.data(), sizeof(quantum_code::intruction_t)*instructions.size());
    std::memcpy(begin+reinterpret_cast<size_t>(header->args_begin), args.c_str(), args.size());

    return size();
}

std::string block_t::str() const {
    std::stringstream stream;
    for (auto inst : instructions) {
        stream << "\t";
        switch (inst.op_code) {
        case quantum_code::op_code_t::alloc:
            stream << "ALLOC       q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::alloc_dirty:
            stream << "ALLOC DIRTY q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::free:
            stream << "FREE        q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::free_dirty:
            stream << "FREE DIRTY  q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_pauli_x:
            stream << "X           q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_pauli_y:
            stream << "Y           q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_pauli_z:
            stream << "Z           q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_hadamard:
            stream << "H           q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_phase:
            stream << "S           q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_rotation_x:
            stream << "RX          q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_rotation_y:
            stream << "RY          q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_rotation_z:
            stream << "RZ          q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::set_param:
            stream << "PARAM        " << std::get<double>(inst.arg);
            break;
        case quantum_code::op_code_t::push_ctrl:
            stream << "CTRL PUSH   q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::pop_ctrl:
            stream << "CTRL POP     " << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::push_qubit:
            stream << "PUSH        q" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::measure:
            stream << "MEASURE     i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::get_plugin_args:
            stream << "PLUGIN ARGS  " << args.c_str()[(size_t)std::get<char*>(inst.arg)];
            break;
        case quantum_code::op_code_t::plugin:
            stream << "PLUGIN       " << args.c_str()[(size_t)std::get<char*>(inst.arg)];
            break;
        case quantum_code::op_code_t::set_then:
            stream << "THEN        l" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::set_else:
            stream << "ELSE        l" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::breach:
            stream << "BR          i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::jump:
            stream << "JUMP        l" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::push_int:
            stream << "PUSH        i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_eq:
            stream << "EQ          i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_neq:
            stream << "NEQ         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_gt:
            stream << "GT          i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_geq:
            stream << "GEQ         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_lt:
            stream << "LT          i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_leq:
            stream << "LEQ         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_add:
            stream << "ADD         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_sub:
            stream << "SUB         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_mul:
            stream << "MUL         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_div:
            stream << "DIV         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_sll:
            stream << "SLL         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_srl:
            stream << "SRL         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_and:
            stream << "AND         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_or:
            stream << "OR          i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_xor:
            stream << "XOR         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_set:
            stream << "SET         i" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_const:
            stream << "SET          " << std::get<int_t>(inst.arg);
            break;
        case quantum_code::op_code_t::dump:
            stream << "DUMP        d" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::halt:
            stream << "HALT";
            break;
        }
        stream << std::endl;
    }
    return stream.str();
}

void block_t::check_ended() const {
    if (ended()) {
        throw ended_block();
    }   
}

const char* not_in_adj::what() const noexcept {
    return "trying to end an nonexistent adj scope";
}

const char* ended_block::what() const noexcept {
    return "trying to append instruction to an ended block";
}
