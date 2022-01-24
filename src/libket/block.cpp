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
            stream << "ALLOC" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::alloc_dirty:
            stream << "ALLOC DIRTY" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::free:
            stream << "FREE" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::free_dirty:
            stream << "FREE DIRTY" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_pauli_x:
            stream << "X" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_pauli_y:
            stream << "Y" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_pauli_z:
            stream << "Z" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_hadamard:
            stream << "H" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_phase:
            stream << "S" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_rotation_x:
            stream << "RX" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_rotation_y:
            stream << "RY" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::gate_rotation_z:
            stream << "RZ" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::set_param:
            stream << "PARAM" << "\t" << std::get<double>(inst.arg);
            break;
        case quantum_code::op_code_t::push_ctrl:
            stream << "CTRL PUSH" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::pop_ctrl:
            stream << "CTRL POP" << "\t" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::push_qubit:
            stream << "PUSH" << "\tq" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::measure:
            stream << "MEASURE" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::get_plugin_args:
            stream << "PLUGIN ARGS" << "\t" << args.c_str()[(size_t)std::get<char*>(inst.arg)];
            break;
        case quantum_code::op_code_t::plugin:
            stream << "PLUGIN" << "\t" << args.c_str()[(size_t)std::get<char*>(inst.arg)];
            break;
        case quantum_code::op_code_t::set_then:
            stream << "THEN" << "\tl" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::set_else:
            stream << "ELSE" << "\tl" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::breach:
            stream << "BR" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::jump:
            stream << "JUMP" << "\tl" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::push_int:
            stream << "PUSH" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_eq:
            stream << "EQ" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_neq:
            stream << "NEQ" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_gt:
            stream << "GT" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_geq:
            stream << "GEQ" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_lt:
            stream << "LT" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_leq:
            stream << "LEQ" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_add:
            stream << "ADD" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_sub:
            stream << "SUB" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_mul:
            stream << "MUL" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_div:
            stream << "DIV" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_sll:
            stream << "SLL" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_srl:
            stream << "SRL" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_and:
            stream << "AND" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_or:
            stream << "OR" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_xor:
            stream << "XOR" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_set:
            stream << "SET" << "\ti" << std::get<index_t>(inst.arg);
            break;
        case quantum_code::op_code_t::int_const:
            stream << "SET" << "\t" << std::get<int_t>(inst.arg);
            break;
        case quantum_code::op_code_t::dump:
            stream << "DUMP" << "\td" << std::get<index_t>(inst.arg);
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
