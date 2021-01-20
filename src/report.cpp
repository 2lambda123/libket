/* MIT License
 * 
 * Copyright (c) 2020 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright (c) 2020 Rafael de Santiago <r.santiago@ufsc.br>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../include/ket"

using namespace ket;

metrics ket::report() {
    return process_stack.top()->get_metrics();
}

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