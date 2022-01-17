/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/bitwise/plugins.hpp>
#include <sstream>
#include <cstdio>

using namespace ket::bitwise;
 
template<class T>
inline T _pown(T x, T a, T N) { 
    if (a == 0) return 1;

    size_t y = 1;
    for (; a > 1; a >>=  1) {
        if (a & 1) y = (y*x)%N;
        x = (x*x)%N;
    }
    
    return (x*y)%N;
}

template<class T>
int bit_length(T num) {
    int len = 0;
    while (num) {
      num >>= 1;
      len++;  
    } 
    return len;    
}

void ket::bitwise::pown(quantum_space_t* space, qubit_list_t qubit_list, char* args, ctrl_list_t ctrl_list, bool adj) {
    if (adj) throw std::runtime_error{"ket::bitwise::pown does not implement adj"};

    ket::dump::state_base_t x, N;
    std::stringstream args_ss{args};
    args_ss >> x >> N;

    auto L = bit_length(N);

    quantum_space_t &quantum_space = *space;

    quantum_space_t new_space;
    for (auto &state : quantum_space) {
        if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/kbw_t::qubits_number_base]&(1ul << (ctrl%kbw_t::qubits_number_base)); })) {
            auto new_state = state.first;
            auto a_b = state.first[0] & ((1ul << qubit_list.size())-1);
            new_state[0] ^= a_b;
            auto a = a_b >> L;
            auto b = a_b & ((1ul << L)-1);
            b *= _pown(x, a, N);
            a_b = (a << L)|b;
            new_state[0] |= a_b;     
            new_space[new_state] = state.second;
        } else {
            new_space[state.first] = state.second;
        }
    }
    quantum_space.swap(new_space);


}
