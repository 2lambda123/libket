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
#include <limits>
#include <cmath>

using namespace ket;

process::process() : 
    qubit_count{0},
    future_count{0},
    label_count{0}
    {}


void process::add_inst(const std::string& inst) {
    if (not ctrl_stack.empty() or not adj_stack.empty())
        throw std::runtime_error("The instruction \"" + inst + "\" cannot be used with adj or ctrl");

    kqasm << inst << std::endl;
}

inline void set_to_adj(process::Gate gate, std::vector<double> args) {
    double theta, phi, lambda;
    switch (gate) {
        case process::Gate::S:
            gate = process::Gate::SD;
            break;
        case process::Gate::T:
            gate = process::Gate::TD;
            break;
        case process::Gate::U2:
            phi = -args[1]-M_PI;
            lambda = -args[0]+M_PI;
            args[0] = phi;
            args[1] = lambda;
            break;
        case process::Gate::U3:
            theta = -args[0];
            phi = -args[2];
            lambda = -args[1];
            args[0] = theta;
            args[1] = phi;
            args[2] = lambda;
            break;
        default:
            break;
    }
}

inline std::string gate_to_string(process::Gate gate, std::vector<double> args) {
    std::stringstream tmp;
    tmp.precision(std::numeric_limits<double>::max_digits10);
    switch (gate) {
        case process::Gate::X:        
            return "X";
        case process::Gate::Y:        
            return "Y";
        case process::Gate::Z:        
            return "z";
        case process::Gate::H:        
            return "H";
        case process::Gate::S:        
            return "S";
        case process::Gate::SD:        
            return "SD";
        case process::Gate::T:        
            return "T";
        case process::Gate::TD:        
            return "TD";
        case process::Gate::U1:        
            tmp << "U1(" << args[0] << ")";
            return tmp.str();
        case process::Gate::U2:        
            tmp << "U2(" << args[0] << ' ' << args[1] << ')';
            return tmp.str();
        case process::Gate::U3:        
            tmp << "U3(" << args[0] << ' ' << args[1] << ' ' << args[2] <<  ")";
            return tmp.str();
    }
    return "<GATE NOT DEFINED>";
}

void process::add_gate(Gate gate, size_t qubit, std::vector<double> args) {
    if (qubits_free.find(qubit) != qubits_free.end()) 
        throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(qubit));

    std::stringstream tmp;
    if (not ctrl_stack.empty()) {
        tmp << "\tCTRL\t";
        for (auto cc : ctrl_stack) for (auto c : cc)
            tmp << 'q' << c << ' ';
    }

    if (not adj_stack.empty() and adj_stack.size() % 2) 
        set_to_adj(gate, args); 
    
    tmp << '\t' << gate_to_string(gate, args) << "\tq" << qubit << std::endl;

    if (not adj_stack.empty()) {
        adj_stack.top().push(tmp.str());
    } else {
        kqasm << tmp.str();
    }
}

std::vector<size_t> process::quant(size_t size, bool dirty) {
    std::vector<size_t> qubits;
    for (auto i = qubit_count; i < qubit_count+size; i++) {
        qubits.push_back(i);
        auto alloc = dirty? "\tALLOC DIRTY\tq" : "\tALLOC\tq";
        add_inst(alloc + std::to_string(i));
    }
    qubit_count += size;
    return qubits;
}


std::tuple<size_t, std::shared_ptr<std::int64_t>, std::shared_ptr<bool>>
process::measure(const std::vector<size_t>& qubits) {
    std::stringstream tmp;
    tmp << "\tINT\t" << future_count << "ZE\t";

    for (auto i : qubits) {
        if (qubits_free.find(i) != qubits_free.end()) 
            throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(i));

        add_inst("\tMEASURE\t" + std::to_string(i));
        qubits_free.insert(i);

        tmp << 'c' << i << ' ';
    }
     
    add_inst(tmp.str());

    auto result = std::make_shared<std::int64_t>(0);
    auto available = std::make_shared<bool>(false);

    return std::make_tuple(future_count++, result, available);
}