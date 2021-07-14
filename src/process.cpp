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
#include <limits>
#include <cmath>
#include <queue>
#include <stdarg.h>

using namespace ket;

process::process() : 
    qubit_count{0},
    future_count{0},
    label_count{0},
    dump_count{0},
    kqasm{"LABEL @entry\n"},
    used_qubits{0},
    free_qubits{0},
    allocated_qubits{0},
    max_allocated_qubits{0},
    measurements{0},
    n_dumps{0},
    n_set_inst{0},
    gates_sum{0},
    ctrl_gates_sum{0},
    plugins_sum{0},
    n_blocks{1},
    executed{false}
{}


inline std::string qubit_list_str(const std::vector<size_t>& qubits) {
    std::string tmp{"["};
    auto it = qubits.begin();
    auto end = qubits.end();
    if (it != end) tmp += "q" + std::to_string(*it++);
    while (it != end) tmp += ", q" + std::to_string(*it++);
    tmp += "]";
    return tmp;    
}

void process::add_inst(const std::string& inst) {
    if (not ctrl_stack.empty() or not adj_stack.empty())
        throw std::runtime_error("The instruction \"" + inst + "\" cannot be used with adj or ctrl");

    if (inst.substr(0, 3) == "SET") n_set_inst += 1;

    kqasm += '\t' + inst + "\n";
}

void process::add_label(const std::string& label) {
    if (not ctrl_stack.empty() or not adj_stack.empty())
        throw std::runtime_error("The instruction \"LABEL @" + label + "\" cannot be used with adj or ctrl");

    kqasm += "LABEL @" + label + "\n";
    
    n_blocks += 1;
}

inline std::string dtos(const char* format, ...) {
    va_list args;
    va_start(args, format);
    const auto n = std::numeric_limits<double>::max_exponent10+22;
    char buffer[n];
    auto len = std::vsnprintf(buffer, n, format, args);
    std::string ret{buffer, buffer+len};
    return ret;
}

inline std::string gate_arg_to_str(const std::string& gate, double args) {
    std::string tmp{gate};
    tmp += "(" + dtos("%.20f", args) + ")";
    return tmp;    
}


inline void set_to_adj(process::Gate &gate, double &arg) {
    switch (gate) {
        case process::S:
            gate = process::SD;
            break;
        case process::SD:
            gate = process::S;
            break;
        case process::T:
            gate = process::TD;
            break;
        case process::TD:
            gate = process::T;
            break;
        case process::phase:
        case process::RX:
        case process::RY:
        case process::RZ:
            arg = -arg;
            break;
        default:
            break;
    }
}

inline std::string gate_to_str(process::Gate gate, double arg = NAN) {
    switch (gate) {
        case process::X:        
            return "X";
        case process::Y:        
            return "Y";
        case process::Z:        
            return "Z";
        case process::H:        
            return "H";
        case process::S:        
            return "S";
        case process::SD:        
            return "SD";
        case process::T:        
            return "T";
        case process::TD:        
            return "TD";
        case process::phase:
            if (std::isnan(arg)) return "P";        
            else return gate_arg_to_str("P", arg);
        case process::RX:
            if (std::isnan(arg)) return "RX";
            else return gate_arg_to_str("RX", arg);
        case process::RY:
            if (std::isnan(arg)) return "RY";
            else return gate_arg_to_str("RY", arg);
        case process::RZ:
            if (std::isnan(arg)) return "RZ";
            else return gate_arg_to_str("RZ", arg);
    }
    return "<GATE NOT DEFINED>";
}

void process::add_gate(Gate gate, size_t qubit, double arg) {
    if (qubits_free.find(qubit) != qubits_free.end()) 
        throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(qubit));

    gates_sum += 1;
    gates[gate_to_str(gate)] += 1;

    std::string tmp{"\t"} ;

    if (not ctrl_stack.empty()) {
        auto n_ctrl_qubits = 0ul;
        std::vector<size_t> ctrl_qubits;

        for (auto cc : ctrl_stack) {
            for (auto c : cc) if (qubit == c)
                throw std::runtime_error("trying to operate with the control qubit q" + std::to_string(qubit));
            else if (qubits_free.find(c) != qubits_free.end())
                throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(c));
            
            n_ctrl_qubits += cc.size();
            ctrl_qubits.insert(ctrl_qubits.end(), cc.begin(), cc.end());
        }
        
        tmp += "CTRL " + qubit_list_str(ctrl_qubits) + ",\t";

        ctrl_gates_sum += 1;
        ctrl_gates[n_ctrl_qubits] += 1;
    }

    if (not adj_stack.empty() and adj_stack.size() % 2) 
        set_to_adj(gate, arg); 
    
    tmp += gate_to_str(gate, arg) + "\tq" + std::to_string(qubit) + "\n";

    if (not adj_stack.empty()) {
        adj_stack.top().push(tmp);
    } else {
        kqasm += tmp;
    }
}

void process::add_plugin(const std::string& name, const std::vector<size_t>& qubits, const std::string& args) {
    for (auto qubit : qubits) 
        if (qubits_free.find(qubit) != qubits_free.end()) 
            throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(qubit));

    plugins[name] += 1;
    plugins_sum += 1;

    std::string tmp{"\t"};

    if (not ctrl_stack.empty()) {
        std::vector<size_t> ctrl_qubits;
        for (auto cc : ctrl_stack) {
            for (auto c : cc) if (qubits_free.find(c) != qubits_free.end())
                throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(c));
            else for (auto q : qubits) if (q == c)
                throw std::runtime_error("trying to operate with the control qubit q" + std::to_string(q));

            ctrl_qubits.insert(ctrl_qubits.end(), cc.begin(), cc.end());
        }
        tmp += "CTRL " + qubit_list_str(ctrl_qubits) + ",\t";
    }

    tmp += "PLUGIN";

    if (not adj_stack.empty() and adj_stack.size() % 2) 
        tmp += "!";

    tmp += '\t' + name + '\t' + qubit_list_str(qubits);

    tmp += "\t\"" + args + "\"\n";
    
    if (not adj_stack.empty()) {
        adj_stack.top().push(tmp);
    } else {
        kqasm += tmp;
    }
}

std::vector<size_t> process::quant(size_t size, bool dirty) {
    used_qubits += size;
    allocated_qubits += size;
    if (allocated_qubits > max_allocated_qubits)
        max_allocated_qubits = allocated_qubits;

    std::vector<size_t> qubits;
    for (auto i = qubit_count; i < qubit_count+size; i++) {
        qubits.push_back(i);
        auto alloc = dirty? "ALLOC DIRTY\tq" : "ALLOC\tq";
        add_inst(alloc + std::to_string(i));
    }
    qubit_count += size;
    return qubits;
}

std::tuple<size_t, std::shared_ptr<std::int64_t>, std::shared_ptr<bool>>
process::measure(const std::vector<size_t>& qubits) {
    for (auto i : qubits) if (qubits_free.find(i) != qubits_free.end()) 
        throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(i));

    measurements += qubits.size();

    std::string tmp{"MEASURE\ti"};
    tmp += std::to_string(future_count) + "\t" + qubit_list_str(qubits);

    add_inst(tmp);

    auto result = std::make_shared<std::int64_t>(0);
    auto available = std::make_shared<bool>(false);

    measure_map[future_count] = std::make_pair(result, available);

    return std::make_tuple(future_count++, result, available);
}

std::tuple<size_t, std::shared_ptr<std::int64_t>, std::shared_ptr<bool>>
process::new_int(std::int64_t value) {
    add_inst("INT\ti" + std::to_string(future_count) + "\t" + std::to_string(value));
    
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
    add_inst("INT\ti" + std::to_string(future_count) + "\ti" + std::to_string(left) + "\t" + op + "\ti" + std::to_string(right));

    auto result = std::make_shared<std::int64_t>(0);
    auto available = std::make_shared<bool>(false);
    
    measure_map[future_count] = std::make_pair(result, available);
    
    return std::make_tuple(future_count++, result, available);
}

void process::adj_end() {
    if (adj_stack.empty()) 
        throw std::runtime_error("no adj to end");


    std::queue<std::string> tmp;
    while (not adj_stack.top().empty()) {
        tmp.push(adj_stack.top().top());
        adj_stack.top().pop();
    }

    adj_stack.pop();

    if (not adj_stack.empty()) while (not tmp.empty()) {
        adj_stack.top().push(tmp.front());
        tmp.pop();
    } else while (not tmp.empty()) {
        kqasm += tmp.front();
        tmp.pop();
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

    free_qubits += 1;
    allocated_qubits -= 1;


    add_inst("FREE" + std::string{dirty? " DIRTY\tq" : "\tq"} + std::to_string(qubit));

    qubits_free.insert(qubit);
}

size_t process::new_label_id() {
    return label_count++;
}

std::tuple<size_t, std::shared_ptr<std::map<std::vector<std::uint64_t>, std::vector<std::complex<double>>>>, std::shared_ptr<bool>>
process::dump(const std::vector<size_t>& qubits) {

    for (auto i : qubits) 
        if (qubits_free.find(i) != qubits_free.end()) 
            throw std::runtime_error("trying to operate with the freed qubit q" + std::to_string(i));

    n_dumps += 1;

    auto states = std::make_shared<std::map<std::vector<std::uint64_t>, std::vector<std::complex<double>>>>();    
    auto available = std::make_shared<bool>(false);
    
    dump_map[dump_count] = std::make_pair(states, available);
    
    std::string inst{"DUMP\t"};
    inst += qubit_list_str(qubits);
    add_inst(inst);

    return std::make_tuple(dump_count++, states, available);
}
       
bool process::is_free(size_t qubit) const {
    return qubits_free.find(qubit) != qubits_free.end();
}

metrics process::get_metrics() const {
    return metrics{used_qubits, 
                   free_qubits, 
                   allocated_qubits, 
                   max_allocated_qubits, 
                   measurements,
                   gates, 
                   gates_sum, 
                   ctrl_gates, 
                   ctrl_gates_sum,
                   plugins,
                   plugins_sum};
}

bool process::has_executed() const {
    return executed;
}

std::string process::get_result_str() const {
    return result;
}
