/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/bitwise/kbw.hpp>
#include <algorithm>
#include <random>
#include <ctime>

using namespace ket::bitwise;
using namespace ket;
using namespace std::literals::complex_literals;

kbw_t::kbw_t(quantum_code_ptr quantum_code) :
    executor_t{quantum_code, features_t::all}
{   
    try {
        std::srand(load_var<unsigned>("KBW_SEED"));
    } catch (std::runtime_error) {
        std::srand(std::time(nullptr));
    }

    for (index_t qubit = 0; qubit < qubit_number; qubit++) {
        free_qubits.push(qubit);
    }
    dump::state_t ket0;
    ket0.resize(qubit_multiplier, 0);
    quantum_space[ket0] = 1;
}

void kbw_t::alloc(index_t qubit, bool dirty) {
    if (dirty and not dirty_qubits.empty()) {
        qubit_map[qubit] = dirty_qubits.top();
        dirty_qubits.pop();
    } else {
        qubit_map[qubit] = free_qubits.top();
        free_qubits.pop();
    }
}

void kbw_t::free(index_t qubit, bool dirty) {
    if (dirty) {
        dirty_qubits.push(qubit);
    } else {
        free_qubits.push(qubit);
    }
    qubit_map.erase(qubit);
}

void kbw_t::pauli_x(index_t qubit, ctrl_list_t ctrl_list) {
    ctrl_list = map_qubits(ctrl_list);
    qubit = qubit_map[qubit];

    quantum_space_t new_space;
    for (auto &state : quantum_space) {
        if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
            auto new_state = state.first;
            new_state[qubit/qubits_number_base] ^= 1ul << (qubit%qubits_number_base);
            new_space[new_state] = state.second;
        } else {
            new_space[state.first] = state.second;
        }
    }

    quantum_space.swap(new_space);
}

void kbw_t::pauli_y(index_t qubit, ctrl_list_t ctrl_list) {
    ctrl_list = map_qubits(ctrl_list);
    qubit = qubit_map[qubit];

    quantum_space_t new_space;
    for (auto &state : quantum_space) {
        if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
            auto new_state = state.first;
            new_state[qubit/qubits_number_base] ^= 1ul << (qubit%qubits_number_base);
            if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
                new_space[new_state] = -1i*state.second;
            } else {
                new_space[new_state] = 1i*state.second;
            }
        } else {
            new_space[state.first] = state.second;
        }
    }

    quantum_space.swap(new_space);
}

void kbw_t::pauli_z(index_t qubit, ctrl_list_t ctrl_list) {
    ctrl_list = map_qubits(ctrl_list);
    qubit = qubit_map[qubit];

    for (auto &state : quantum_space) if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
        if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
            quantum_space[state.first] *= -1;
        } 
    }
}

void kbw_t::phase(double lambda, index_t qubit, ctrl_list_t ctrl_list) {
    ctrl_list = map_qubits(ctrl_list);
    qubit = qubit_map[qubit];

    for (auto &state : quantum_space) if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
        if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
            quantum_space[state.first] *= std::exp(1i*lambda);
        } 
    }
}

void kbw_t::hadamard(index_t qubit, ctrl_list_t ctrl_list) {
    ctrl_list = map_qubits(ctrl_list);
    qubit = qubit_map[qubit];

    quantum_space_t new_space;
    for (auto &state : quantum_space) {
        if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
            auto new_state = state.first;
            new_state[qubit/qubits_number_base] ^= 1ul << (qubit%qubits_number_base);
            auto new_amp = state.second*M_SQRT1_2;

            new_space[new_state] += new_amp;
    
            if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
                new_space[state.first] -= new_amp;
            } else {
                new_space[state.first] += new_amp;
            }

            if (std::abs(new_space[new_state]) < 1e-10) {
                new_space.erase(new_state);
            }

            if (std::abs(new_space[state.first]) < 1e-10) {
                new_space.erase(state.first);
            }
        } else {
            new_space[state.first] = state.second;
        }
    }

    quantum_space.swap(new_space);
}

void kbw_t::rotation_x(double theta, index_t qubit, ctrl_list_t ctrl_list) {
    ctrl_list = map_qubits(ctrl_list);
    qubit = qubit_map[qubit];

    quantum_space_t new_space;
    for (auto &state : quantum_space) {
        if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
            auto new_state = state.first;
            new_state[qubit/qubits_number_base] ^= 1ul << (qubit%qubits_number_base);

            new_space[new_state] += state.second*-1i*std::sin(theta/2.0);
            new_space[state.first] += state.second*std::cos(theta/2.0);
    
            if (std::abs(new_space[new_state]) < 1e-10) {
                new_space.erase(new_state);
            }

            if (std::abs(new_space[state.first]) < 1e-10) {
                new_space.erase(state.first);
            }
        } else {
            new_space[state.first] = state.second;
        }
    }

    quantum_space.swap(new_space);
}


void kbw_t::rotation_y(double theta, index_t qubit, ctrl_list_t ctrl_list) {
    ctrl_list = map_qubits(ctrl_list);
    qubit = qubit_map[qubit];

    quantum_space_t new_space;
    for (auto &state : quantum_space) {
        if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
            auto new_state = state.first;
            new_state[qubit/qubits_number_base] ^= 1ul << (qubit%qubits_number_base);
            auto new_amp = state.second*std::sin(theta/2.0);

            if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
                new_space[new_state] -= new_amp;
            } else {
                new_space[new_state] += new_amp;
            }

            new_space[state.first] += state.second*std::cos(theta/2.0);

            if (std::abs(new_space[new_state]) < 1e-10) {
                new_space.erase(new_state);
            }

            if (std::abs(new_space[state.first]) < 1e-10) {
                new_space.erase(state.first);
            }
        } else {
            new_space[state.first] = state.second;
        }
    }

    quantum_space.swap(new_space);
}

void kbw_t::rotation_z(double theta, index_t qubit, ctrl_list_t ctrl_list) {
    ctrl_list = map_qubits(ctrl_list);
    qubit = qubit_map[qubit];

    for (auto &state : quantum_space) if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
        if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
            quantum_space[state.first] *= std::exp(1i*theta/2.0);
        } else {
            quantum_space[state.first] *= std::exp(-1i*theta/2.0);
        }
    }
}

int_t kbw_t::measure(qubit_list_t qubit_list) {
    auto measure_qubit = [&](auto qubit) {
        double p1 = 0.0;
        for (auto &state : quantum_space) {
            if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
                auto abs_amp = std::abs(state.second);
                p1 += abs_amp*abs_amp;
            }
        }
        
        auto result = (double)std::rand()/RAND_MAX <= p1 and p1 > 1e-10? 1u : 0u;
        auto p = std::sqrt(result? p1 : 1.0-p1);

        quantum_space_t new_space;

        for (auto &state : quantum_space) {
            if (not ((bool)(state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) xor (bool)result)) {
                new_space[state.first] = state.second/p;
            } 
        }

        quantum_space.swap(new_space);
        
        return result;
    };

    qubit_list = map_qubits(qubit_list);
    int_t result = 0;
    auto size = qubit_list.size();
    for (auto i = 0u; i < size; i++) {
        result |= measure_qubit(qubit_list[i]) << (size-i-1);
    }

    return result;
}


void kbw_t::plugin(char* name, qubit_list_t, char* args, ctrl_list_t) {
}

dump_t kbw_t::dump(qubit_list_t qubit_list) {
    qubit_list = map_qubits(qubit_list);

    dump::states_t states;
    dump::amplitudes_t amplitudes;

    states.resize(quantum_space.size());
    amplitudes.resize(quantum_space.size());

    boost::unordered_map<dump::state_t,index_t> index_map;

    auto num_qubits = qubit_list.size();
    auto num_bases = (num_qubits+qubits_number_base)/qubits_number_base;

    size_t states_index = 0;
    for (auto state : quantum_space) {
        dump::state_t new_state;
        new_state.resize(num_bases);
        for (auto i = 0u; i < num_qubits; i++) {
            if (state.first[qubit_list[i]/qubits_number_base] & (1ul << (qubit_list[i]%qubits_number_base))) {
                auto index = num_qubits-i-1;
                new_state[index/qubits_number_base] ^= (1ul << (index%qubits_number_base));
            }
        }
        if (index_map.find(new_state) == index_map.end()) {
            states[states_index] = new_state;
            amplitudes[states_index++].push_back(state.second);
        } else {
            auto state_index = index_map[new_state];
            states[state_index] = new_state;
            amplitudes[state_index].push_back(state.second);
        }
    }

    return dump_t{states, amplitudes}; 
}

qubit_list_t kbw_t::map_qubits(qubit_list_t qubit_list) {
    for (auto i = 0u; i < qubit_list.size(); i++) {
        qubit_list[i] = qubit_map[qubit_list[i]];
    }
    return qubit_list;
}

quantum_result_ptr bitwise::ket_quantum_executor(quantum_code_ptr quantum_code) {
    kbw_t kbw{quantum_code};
    kbw.run();
    return kbw.get_result();
}
