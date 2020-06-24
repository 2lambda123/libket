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
#include "../include/ket_bits/base.hpp"

using namespace ket;
using namespace ket::base;

_process::_process() : 
    qubit_count{0},
    bit_count{0},
    i64_count{0}
{
    begin_block("entry");
}

std::shared_ptr<qubit> _process::alloc(bool dirty) {
    if (not (adj_call.empty() and ctrl_qubit.empty())) 
        throw std::runtime_error("alloc cannot be used with adj or ctrl");

    auto new_qubit = std::make_shared<qubit>(qubit_count);
    qubit_map[qubit_count] = new_qubit;

    block_call.push([new_qubit, dirty]() {
        auto alloc_gate = std::make_shared<gate>(gate::ALLOC, new_qubit->idx(), dirty, new_qubit->last_gate());
        new_qubit->add_gate(alloc_gate);
    });

    qubit_count++;
    return new_qubit;
}

void _process::add_gate(gate::TAG gate_tag, 
                       const std::shared_ptr<qubit>& qbit,
                       const std::vector<double>& args) 
{
    std::vector<size_t> ctrl_q;
    for (auto &i: ctrl_qubit) for (auto &j : i) 
        ctrl_q.push_back(j);

    bool adj = adj_call.size()%2;

    auto add = [this, ctrl_q, adj, gate_tag, qbit, args]() {
        if (this->block_free.find(qbit->idx()) != this->block_free.end())
            throw std::runtime_error("operating with a freed qubit");

        std::vector<std::shared_ptr<gate>> ctrl_back;
        for (auto &i: ctrl_q) 
            ctrl_back.push_back(this->qubit_map[i]->last_gate());

        auto new_gate = std::make_shared<gate>(gate_tag, 
                                               qbit->idx(), 
                                               adj,
                                               qbit->last_gate(), 
                                               args,
                                               ctrl_q, 
                                               ctrl_back);
        qbit->add_gate(new_gate);
        for (auto &i: ctrl_q) 
            this->qubit_map[i]->add_gate(new_gate);
    };

    if (not adj_call.empty()) {
        adj_call.top().push(add);
    } else {
        block_call.push(add);
    } 
}

void _process::add_plugin_gate(const std::string &gate_name, 
                              const std::vector<std::shared_ptr<qubit>>& qbits,
                              const std::string& args) 
{
    if (not (adj_call.empty() and ctrl_qubit.empty())) 
        throw std::runtime_error("special gate "+gate_name+" cannot be used with adj or ctrl");

    std::vector<size_t> qbits_idx;
    for (auto &i : qbits) qbits_idx.push_back(i->idx());

    block_call.push([this, qbits_idx, gate_name, args] {
        std::vector<std::shared_ptr<gate>> qbits_back;
        for (auto &i: qbits_idx) 
            qbits_back.push_back(this->qubit_map[i]->last_gate());
        
        auto new_gate = std::make_shared<gate>(gate::PLUGIN, 
                                               qbits_idx, 
                                               qbits_back, 
                                               gate_name, 
                                               args);
     
        for (auto &i: qbits_idx) 
            this->qubit_map[i]->add_gate(new_gate);
    });

}

void _process::wait() {
    std::vector<size_t> qbits_idx;
    for (auto &i : qubit_map) 
        qbits_idx.push_back(i.second->idx());
        
    block_call.push([this, qbits_idx]{
        std::vector<std::shared_ptr<gate>> qbits_back;
        for (auto &i: qbits_idx) 
            qbits_back.push_back(this->qubit_map[i]->last_gate());

        auto wait_gate = std::make_shared<gate>(gate::WAIT,
                                                qbits_idx,
                                                qbits_back);
        for (auto &i : qbits_idx) 
            this->qubit_map[i]->add_gate(wait_gate);
    });
    
}

std::shared_ptr<bit> _process::measure(const std::shared_ptr<qubit>& qbit) {
    if (not (adj_call.empty() and ctrl_qubit.empty())) 
        throw std::runtime_error("measure cannot be used with adj or ctrl");

    auto m_gate = std::make_shared<gate>(gate::MEASURE,
                                         qbit->idx(), 
                                         false, 
                                         qbit->last_gate());
    
    auto new_bit = std::make_shared<bit>(bit_count, m_gate);

    block_call.push([this, m_gate, qbit]() {
        if (this->block_free.find(qbit->idx()) != this->block_free.end())
            throw std::runtime_error("measuring a freed qubit");
            
        new (m_gate.get()) gate{gate::MEASURE,
                                qbit->idx(), 
                                false, 
                                qbit->last_gate()};
                                
        this->block_free.insert(qbit->idx());
    });

    bit_count++;
    return new_bit;
}

void _process::free(const std::shared_ptr<qubit>& qbit, bool dirty) {
    if (not (adj_call.empty() and ctrl_qubit.empty())) 
        throw std::runtime_error("measure cannot be used with adj or ctrl");
    
    auto fgate = [this, qbit, dirty]() {
        if (this->block_free.find(qbit->idx()) != this->block_free.end())
            throw std::runtime_error("qubit double free");
        
        auto free_gate = std::make_shared<gate>(gate::FREE,
                                                qbit->idx(),
                                                dirty,
                                                qbit->last_gate());

        qbit->add_gate(free_gate);
        this->block_free.insert(qbit->idx());
    };

    block_call.push(fgate);
}

std::shared_ptr<i64> _process::new_i64(const std::vector<std::shared_ptr<bit>>& bits) {
    auto i64_ptr = std::make_shared<i64>(bits, i64_count);
    measurement_map[i64_count] = i64_ptr;
    i64_count++;
    block_i64.push_back(i64_ptr);
    return i64_ptr;
}

std::shared_ptr<i64> _process::i64_op(const std::string& op,
                                     const std::vector<std::shared_ptr<i64>>& args, 
                                     bool infix) 
 {
    auto i64_ptr = std::make_shared<i64>(op, args, i64_count, infix);
    measurement_map[i64_count] = i64_ptr;
    i64_count++;
    block_i64.push_back(i64_ptr);
    return i64_ptr;
}

std::shared_ptr<i64> _process::const_i64(std::int64_t value) {
    auto i64_ptr = std::make_shared<i64>(value, i64_count);
    measurement_map[i64_count] = i64_ptr;
    i64_count++;
    block_i64.push_back(i64_ptr);
    return i64_ptr;   
}

void _process::set_i64(const std::shared_ptr<i64>& target, const std::shared_ptr<i64>& value) {
    auto i64_ptr = std::make_shared<i64>(std::vector<std::shared_ptr<i64>>{target, value});
    block_i64.push_back(i64_ptr);
}

void _process::adj_begin() {
    adj_call.push({});
}

void _process::adj_end() {
    auto inner = adj_call.top();
    adj_call.pop();
    
    if (not adj_call.empty()) {
        while (not inner.empty()) {
            adj_call.top().push(inner.top());
            inner.pop();
        }
    } else {
        while (not inner.empty()) {
            block_call.push(inner.top());
            inner.pop();
        }
    }
}


void _process::ctrl_begin(const std::vector<std::shared_ptr<qubit>>& ctrl) {
    std::vector<size_t> ctrl_q;
    for (auto &i: ctrl) ctrl_q.push_back(i->idx());
    ctrl_qubit.push_back(ctrl_q);
}

void _process::ctrl_end() {
    ctrl_qubit.pop_back();
}

void _process::begin_block(const std::string& label) {
    this->label = label;
} 

std::shared_ptr<ket::base::gate> _process::end_block(const std::string& label_true,
                        const std::string& label_false,
                        const std::shared_ptr<i64>& bri64) {
    std::vector<size_t> qubits;
    for (auto &i: qubit_map) 
        qubits.push_back(i.first);
    
    std::vector<std::shared_ptr<gate>> qubits_back;
    for (auto i: qubits) 
        qubits_back.push_back(qubit_map[i]->last_gate());

    auto label_gate = std::make_shared<gate>(gate::TAG::LABEL, qubits, qubits_back, label);

    for (auto i: qubits) 
        this->qubit_map[i]->add_gate(label_gate);
    
    for (auto i: block_i64)
        i->set_label(label_gate);
    
    while (not block_call.empty()) {
        block_call.front()();
        block_call.pop();
    }

    qubits_back.clear();
    for (auto i: qubits) 
        qubits_back.push_back(qubit_map[i]->last_gate());

    auto end_gate = std::make_shared<gate>(bri64? gate::TAG::BR : gate::TAG::JUMP, qubits, qubits_back, label_true, label_false, bri64, block_i64);

    for (auto i: qubits) 
        qubit_map[i]->add_gate(end_gate);

    block_i64.clear();

    return end_gate;
}

process::process() : ps{new base::_process, [](auto ptr){ delete static_cast<base::_process*>(ptr); }} {} 