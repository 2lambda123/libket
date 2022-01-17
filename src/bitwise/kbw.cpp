/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/bitwise/kbw.hpp>
#include <algorithm>
#include <random>
#include <ctime>
#include <boost/dll/import.hpp>

using namespace ket::bitwise;
using namespace ket;
using namespace std::literals::complex_literals;

kbw_t::kbw_t(quantum_code_ptr _quantum_code) :
    quantum_code{_quantum_code},
    qubit_multiplier{(quantum_code.header->num_qubits+qubits_number_base)/qubits_number_base},
    executor_t{_quantum_code, features_t::all}
{    
    num_qubits = qubits_number_base*qubit_multiplier;
    init();

    try {
        std::srand(load_var<unsigned>("KQE_SEED"));
    } catch (std::runtime_error) {
        std::srand(std::time(nullptr));
    }

    dump::state_t ket0;
    ket0.resize(qubit_multiplier, 0);
    quantum_space[ket0] = 1;
}


void kbw_t::_pauli_x(index_t qubit, ctrl_list_t ctrl_list) {
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

void kbw_t::_pauli_y(index_t qubit, ctrl_list_t ctrl_list) {
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

void kbw_t::_pauli_z(index_t qubit, ctrl_list_t ctrl_list) {
    for (auto &state : quantum_space) if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
        if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
            quantum_space[state.first] *= -1;
        } 
    }
}

void kbw_t::_phase(double lambda, index_t qubit, ctrl_list_t ctrl_list) {
    for (auto &state : quantum_space) if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
        if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
            quantum_space[state.first] *= std::exp(1i*lambda);
        } 
    }
}

void kbw_t::_hadamard(index_t qubit, ctrl_list_t ctrl_list) {
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

void kbw_t::_rotation_x(double theta, index_t qubit, ctrl_list_t ctrl_list) {
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


void kbw_t::_rotation_y(double theta, index_t qubit, ctrl_list_t ctrl_list) {
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

void kbw_t::_rotation_z(double theta, index_t qubit, ctrl_list_t ctrl_list) {
    for (auto &state : quantum_space) if (std::all_of(ctrl_list.begin(), ctrl_list.end(), [&](auto ctrl) {return state.first[ctrl/qubits_number_base]&(1ul << (ctrl%qubits_number_base)); })) {
        if (state.first[qubit/qubits_number_base] & (1ul << (qubit%qubits_number_base))) {
            quantum_space[state.first] *= std::exp(1i*theta/2.0);
        } else {
            quantum_space[state.first] *= std::exp(-1i*theta/2.0);
        }
    }
}

int_t kbw_t::_measure(qubit_list_t qubit_list) {
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

    int_t result = 0;
    auto size = qubit_list.size();
    for (auto i = 0u; i < size; i++) {
        result |= measure_qubit(qubit_list[i]) << (size-i-1);
    }

    return result;
}

void kbw_t::_plugin(char* name, qubit_list_t qubit_list, char* args, ctrl_list_t ctrl_list, bool adj) {
    auto plugin_path = ket::load_var<std::string>("KQE_PLUGIN_PATH")+"/kbw_plugins.so";
    auto plugin_gate = boost::dll::import_symbol<void(quantum_space_t*, qubit_list_t, char*, ctrl_list_t, bool)>(plugin_path, name);
    plugin_gate(&quantum_space, qubit_list, args, ctrl_list, adj);
}

dump_t kbw_t::_dump(qubit_list_t qubit_list) {
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


void kbw_t::swap(index_t a, index_t b) {
    quantum_space_t new_space;
    for (auto &state : quantum_space) {
        if ((bool)(state.first[a/qubits_number_base] & (1ul << (a%qubits_number_base))) xor
            (bool)(state.first[b/qubits_number_base] & (1ul << (b%qubits_number_base)))) {
            auto new_state = state.first;
            new_state[a/qubits_number_base] ^= (1ul << (a%qubits_number_base));
            new_state[b/qubits_number_base] ^= (1ul << (b%qubits_number_base));
            new_space[new_state] = state.second;    
        } else {
            new_space[state.first] = state.second;
        }
    }
    quantum_space.swap(new_space);
}


quantum_result_ptr bitwise::ket_quantum_executor(quantum_code_ptr quantum_code) {
    kbw_t kbw{quantum_code};
    kbw.run();
    return kbw.get_result();
}
