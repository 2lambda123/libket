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
