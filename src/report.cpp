/* Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../include/ket"

using namespace ket;

metrics ket::report() {
    return process_stack.top()->get_metrics();
}

metrics::metrics() : used_qubits{0}, free_qubits{0},
    allocated_qubits{0}, max_allocated_qubits{0},
    measurements{0}, gates{{}}, gates_sum{0}, 
    ctrl_gates{{}}, ctrl_gates_sum{0}, 
    plugins{{}}, plugins_sum{0} 
    {}

metrics::metrics(size_t used_qubits, size_t free_qubits, size_t allocated_qubits,
                 size_t max_allocated_qubits, size_t measurements, 
                 std::unordered_map<std::string, size_t> gates, size_t gates_sum, 
                 std::unordered_map<size_t, size_t> ctrl_gates, size_t ctrl_gates_sum, 
                 std::unordered_map<std::string, size_t> plugins, size_t plugins_sum) :
    used_qubits{used_qubits}, free_qubits{free_qubits},
    allocated_qubits{allocated_qubits}, max_allocated_qubits{max_allocated_qubits},
    measurements{measurements}, gates{gates}, gates_sum{gates_sum}, 
    ctrl_gates{ctrl_gates}, ctrl_gates_sum{ctrl_gates_sum}, 
    plugins{plugins}, plugins_sum{plugins_sum} 
    {}         

std::string metrics::str() const {
    std::stringstream out;

    out << "Qubits used:         \t" << used_qubits << std::endl
        << "Free qubits:         \t" << free_qubits << std::endl
        << "Allocated Qubits:    \t" << allocated_qubits << std::endl
        << "  ↳ Max concurrently:\t" << max_allocated_qubits  << std::endl
        << "Measurements:        \t" << measurements << std::endl
        << "Quantum gates:       \t" << gates_sum << std::endl;

    for (auto u : gates) {
        out << "  ↳ " << u.first 
              << " gate:         \t" << u.second << std::endl;
    }

    out << "Quantum CTRL gates:  \t" << ctrl_gates_sum << std::endl;
    for (auto i : ctrl_gates) {
        out << "  ↳ " << i.first 
              << " control:      \t" << i.second << std::endl;
    }

    out << "Ket Bitwise plugins: \t" << plugins_sum << std::endl;
    for (auto i : plugins) {
        out << "  ↳ " << i.first << ":\t\t" << i.second << std::endl;
    }

    return out.str();
}

std::string metrics::__str__() const {
    return this->str();
}

size_t metrics::get_qubits_used() {
    return used_qubits;
}

size_t metrics::get_free_qubits() {
    return free_qubits;
}

size_t metrics::get_allocated_qubits() {
    return allocated_qubits;
}

size_t metrics::get_max_allocated_qubits() {
    return max_allocated_qubits;
}

size_t metrics::get_measurements() {
    return measurements;
}

size_t metrics::get_gates(std::string gate) {
    return gates[gate];
}

size_t metrics::get_gates_sum() {
    return gates_sum;
}

size_t metrics::get_ctrl_gates(size_t number) {
    return ctrl_gates[number];
}

size_t metrics::get_ctrl_gates_sum() {
    return ctrl_gates_sum;
}

size_t metrics::get_plugins(std::string name) {
    return plugins[name];
}

size_t metrics::get_plugins_sum() {
    return plugins_sum;
}
