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
{
    kqasm << "LABEL @entry" << std::endl;
}


void process::add_inst(const std::string& inst) {
    if (not ctrl_stack.empty() or not adj_stack.empty())
        throw std::runtime_error("The instruction \"" + inst + "\" cannot be used with adj or ctrl");

    kqasm << inst << std::endl;
}

inline void set_to_adj(process::Gate gate, std::vector<double> args) {
    double theta, phi, lambda;
    switch (gate) {
        case process::s:
            gate = process::sd;
            break;
        case process::t:
            gate = process::td;
            break;
        case process::u2:
            phi = -args[1]-M_PI;
            lambda = -args[0]+M_PI;
            args[0] = phi;
            args[1] = lambda;
            break;
        case process::u3:
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
        case process::x:        
            return "X";
        case process::y:        
            return "Y";
        case process::z:        
            return "Z";
        case process::h:        
            return "H";
        case process::s:        
            return "S";
        case process::sd:        
            return "SD";
        case process::t:        
            return "T";
        case process::td:        
            return "TD";
        case process::u1:        
            tmp << "U1(" << args[0] << ")";
            return tmp.str();
        case process::u2:        
            tmp << "U2(" << args[0] << ' ' << args[1] << ')';
            return tmp.str();
        case process::u3:        
            tmp << "U3(" << args[0] << ' ' << args[1] << ' ' << args[2] <<  ")";
            return tmp.str();
        case process::dump:
            return "DUMP";
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
    tmp << "\tINT\ti" << future_count << "\tZE\t";

    for (auto i : qubits) {
        if (qubits_free.find(i) != qubits_free.end()) 
            throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(i));

        add_inst("\tMEASURE\tq" + std::to_string(i));
        qubits_free.insert(i);

        tmp << 'c' << i << ' ';
    }
     
    add_inst(tmp.str());

    auto result = std::make_shared<std::int64_t>(0);
    auto available = std::make_shared<bool>(false);

    measure_map[future_count] = std::make_pair(result, available);

    return std::make_tuple(future_count++, result, available);
}

std::tuple<size_t, std::shared_ptr<std::int64_t>, std::shared_ptr<bool>>
process::new_int(std::int64_t value) {
    add_inst("\tINT\ti" + std::to_string(future_count) + "\t" + std::to_string(value));
    
    auto result = std::make_shared<std::int64_t>(0);
    auto available = std::make_shared<bool>(false);

    measure_map[future_count] = std::make_pair(result, available);

    return std::make_tuple(future_count++, result, available);
}

void process::adj_begin() {
    adj_stack.push({});
}

std::tuple<size_t, std::shared_ptr<std::int64_t>, std::shared_ptr<bool>>
process::op_int(size_t left, const std::string& op, size_t right) {
    add_inst("\tINT\ti" + std::to_string(future_count) + "\ti" + std::to_string(left) + "\t" + op + "\ti" + std::to_string(right));

    auto result = std::make_shared<std::int64_t>(0);
    auto available = std::make_shared<bool>(false);
    
    measure_map[future_count] = std::make_pair(result, available);
    
    return std::make_tuple(future_count++, result, available);
}

void process::adj_end() {
    if (adj_stack.empty()) 
        throw std::runtime_error("no adj to end");

    std::stringstream tmp;

    while (not adj_stack.top().empty()) {
        tmp << adj_stack.top().top();
        adj_stack.top().pop();
    }

    adj_stack.pop();

    if (not adj_stack.empty()) {
        adj_stack.top().push(tmp.str());
    } else {
        kqasm << tmp.str();
    }
}

void process::ctrl_begin(const std::vector<size_t>& control) {
    ctrl_stack.push_back(control);
}

void process::ctrl_end() {
    if (ctrl_stack.empty()) 
        throw std::runtime_error("no ctrl to end");
    
    ctrl_stack.pop_back();
}

void process::free(size_t qubit, bool dirty) {
    if (qubits_free.find(qubit) != qubits_free.end()) 
        throw std::runtime_error("Double free on qubit q" + std::to_string(qubit));

    add_inst("\tFREE" + std::string{dirty? " DIRTY\tq" : "\tq"} + std::to_string(qubit));

    qubits_free.insert(qubit);
}

size_t process::new_label_id() {
    return label_count++;
}