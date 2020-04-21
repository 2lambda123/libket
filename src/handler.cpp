
#include "../include/ket_bits/base.hpp"

using namespace ket::base;

handler::handler() : 
    qubit_count{0},
    bit_count{0},
    i64_count{0},
    label_count{}
    {}

std::shared_ptr<qubit> handler::alloc(bool dirty) {
    if (not (adj_call.empty() and ctrl_qubit.empty())) 
        throw std::runtime_error("alloc cannot be used with adj or ctrl");

    auto new_qubit = std::make_shared<qubit>(qubit_count);
    qubit_map[qubit_count] = new_qubit.get();

    block_qubits.insert(qubit_count);

    block_call.push([new_qubit, dirty]() {
        auto alloc_gate = std::make_shared<gate>(gate::ALLOC, new_qubit->idx(), dirty, new_qubit->last_gate());
        new_qubit->add_gate(alloc_gate);
    });

    qubit_count++;
    return new_qubit;
}

void handler::add_gate(gate::TAG gate_tag, const std::shared_ptr<qubit>& qbit, const std::vector<double>& args) {
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

    block_qubits.insert(ctrl_q.begin(), ctrl_q.end());
    block_qubits.insert(qbit->idx());
    if (not adj_call.empty()) {
        adj_call.top().push(add);
    } else {
        block_call.push(add);
    } 
}

void handler::wait(const std::vector<std::shared_ptr<qubit>>& qbits) {
    std::vector<size_t> qbits_idx;
    for (auto &i : qbits) 
        qbits_idx.push_back(i->idx());
        
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
    
    block_qubits.insert(qbits_idx.begin(), qbits_idx.end());
}

std::shared_ptr<bit> handler::measure(const std::shared_ptr<qubit>& qbit) {
    if (not (adj_call.empty() and ctrl_qubit.empty())) 
        throw std::runtime_error("measure cannot be used with adj or ctrl");

    auto m_gate = std::make_shared<gate>(gate::MEASURE,
                                         qbit->idx(), 
                                         false, 
                                         qbit->last_gate());
    
    auto new_bit = std::make_shared<bit>(bit_count, m_gate);

    block_qubits.insert(qbit->idx());
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

void handler::free(const std::shared_ptr<qubit>& qbit, bool dirty) {
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

    block_qubits.insert(qbit->idx());
    block_call.push(fgate);
}

std::shared_ptr<i64> handler::new_i64(const std::vector<std::shared_ptr<bit>>& bits) {
    auto i64_ptr = std::make_shared<i64>(bits, i64_count);
    measurement_map[i64_count] = i64_ptr.get();
    return i64_ptr;
}

std::shared_ptr<i64> handler::i64_op(const std::string& op, const std::vector<std::shared_ptr<i64>>& args, bool infix) {
    auto i64_ptr = std::make_shared<i64>(op, args, i64_count++, infix);
    measurement_map[i64_count] = i64_ptr.get();
    return i64_ptr;
}

void handler::adj_begin() {
    adj_call.push({});
}

void handler::adj_end() {
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


void handler::ctrl_begin(const std::vector<std::shared_ptr<qubit>>& ctrl) {
    std::vector<size_t> ctrl_q;
    for (auto &i: ctrl) ctrl_q.push_back(i->idx());
    ctrl_qubit.push_back(ctrl_q);
}

void handler::ctrl_end() {
    ctrl_qubit.pop_back();
}

void handler::begin_block(const std::string& label,
                          const boost::unordered_set<size_t>& block_qubits) {
    this->block_qubits = block_qubits;
    this->label = label;
} 

void handler::end_block(const std::string& label_true,
                        const std::string& label_false,
                        const std::shared_ptr<i64>& bri64) {
    std::vector<size_t> qubits;
    for (auto &i: block_qubits) 
        qubits.push_back(i);
    block_qubits.clear();
    
    std::vector<std::shared_ptr<gate>> qubits_back;
    for (auto i: qubits) 
        qubits_back.push_back(qubit_map[i]->last_gate());

    auto label_gate = std::make_shared<gate>(gate::TAG::LABEL, qubits, qubits_back, label);

    for (auto i: qubits) 
        this->qubit_map[i]->add_gate(label_gate);
    
    while (not block_call.empty()) {
        block_call.front()();
        block_call.pop();
    }

    qubits_back.clear();
    for (auto i: qubits) 
        qubits_back.push_back(qubit_map[i]->last_gate());

    auto end_gate = std::make_shared<gate>(bri64? gate::TAG::BR : gate::TAG::JUMP, qubits, qubits_back,label_true, label_false, bri64);

    for (auto i: qubits) 
        qubit_map[i]->add_gate(end_gate);

    block_free.clear();
}

boost::unordered_set<size_t> handler::block_qubits_backup() {
    return block_qubits;
}


void handler::if_then(const std::shared_ptr<i64>& cond, std::function<void()> then, std::function<void()> otherwise) {
    auto then_label = label+std::string{".if.then"}+std::to_string(label_count);
    auto else_label = label+std::string{".if.else"}+std::to_string(label_count);
    auto end_label = label+std::string{".if.end"}+std::to_string(label_count);

    auto backup = block_qubits_backup();
    if (otherwise) {
        end_block(then_label, else_label, cond);
    } else {
        end_block(then_label, end_label, cond);
    }
    
    begin_block(then_label, backup);
    then();
    end_block(end_label);

    if (otherwise) {
        begin_block(else_label, backup);
        otherwise();
        end_block(end_label);
    }
    
    begin_block(end_label, backup);

    label_count++;
}

void handler::set_value(size_t idx, std::int64_t value) {
    measurement_map[idx]->set_value(value);

}

size_t handler::get_label_count() {
    return label_count++;
}
