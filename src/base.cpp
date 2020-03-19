#include "../include/base.hpp"

using namespace ket::base;

gate::gate(TAG tag, 
           size_t qubit_idx, 
           bool adj,
           const std::shared_ptr<gate>& back,
           const std::vector<double>& args, 
           const std::vector<size_t>& ctrl_idx,
           const std::vector<std::shared_ptr<gate>>& ctrl_back) :
    tag{tag}, 
    qubit_idx{qubit_idx},
    back{back},
    adj{adj},
    args{args},
    ctrl_idx{ctrl_idx},
    ctrl_back{ctrl_back},
    visit{false}
    {}

gate::gate(const std::vector<size_t>& ctrl_idx,
           const std::vector<std::shared_ptr<gate>>& ctrl_back,
           const std::string& next_label,
           const std::string& label_goto1,
           const std::string& label_goto2,
           const std::shared_ptr<i64>& bri64) :
    tag{bri64 != nullptr? BR : JUMP},
    back{nullptr},
    ctrl_idx{ctrl_idx},
    ctrl_back{ctrl_back},
    label_next{next_label},
    label_true{label_goto1},
    label_false{label_goto2},
    bri64{bri64} 
    {}

void gate::eval(std::stringstream& circuit) {
    using std::endl;
    
    if (visit) return;
    else visit = true;

    for (auto& i : ctrl_back) i->eval(circuit);
    if (back) back->eval(circuit);

    if (tag <= U3 and not ctrl_back.empty()) {
        circuit << "CTRL ";
        for (auto i : ctrl_idx) circuit << "q" << i << " ";
    } 

    switch (tag) {
    case X:
        circuit << "X q" << qubit_idx << ";" << endl;
        break;
    case Y:
        circuit << "Y q" << qubit_idx << ";" << endl;
        break;
    case Z:
        circuit << "Z q" << qubit_idx << ";" << endl;
        break;
    case H:
        circuit << "H q" << qubit_idx << ";" << endl;
        break;
    case S:
        circuit << (adj? "SD" : "S") << " q" << qubit_idx << ";" << endl;
        break;
    case T:
        circuit << (adj? "TD" : "T") << " q" << qubit_idx << ";" << endl;
        break;
    case U1:
        circuit << "U1(" << args[0] << ") q" << qubit_idx << ";" << endl;
        break;
    case U2:
        circuit << "U2(" << args[0] << ", " << args[1] << ") q" << qubit_idx << ";" << endl;
        break;
    case U3:
        circuit << "U3(" << args[0] << ", " << args[1] << ", " << args[2] << ") q" << qubit_idx << ";" << endl;
        break;
    case MEASURE: 
        circuit << "MEASURE q" << qubit_idx << "->";
        break;
    case ALLOC:
        circuit << "ALLOC q" << qubit_idx << ";" << endl;
        break;
    case JUMP:
        circuit << "JUMP " << label_true << ";" << endl
                << label_next << ":" << endl;
        break;
    case BR:
        bri64->eval(circuit);
        circuit << "BR i" << bri64->idx() << ", " << label_true << ", " << label_false << ";" << endl
                << label_next << ":" << endl;
        break;    
    default:
        break;
    }
}

qubit::qubit(size_t qubit_idx) :
    qubit_idx{qubit_idx},
    tail_gate{std::make_shared<gate>(gate::ALLOC, qubit_idx)}
    {}

size_t qubit::idx() {
    return qubit_idx;
}

void qubit::add_gate(const std::shared_ptr<gate>& new_gate) {
    tail_gate = new_gate;
}


std::shared_ptr<gate> qubit::last_gate() {
    return tail_gate;
}


bit::bit(size_t bit_idx,
         std::shared_ptr<gate> measurement_gate,
         std::shared_ptr<result> measurement) :
    bit_idx{bit_idx},
    measurement_gate{measurement_gate},
    measurement{measurement},
    visit{false}
    {}

result bit::get() const {
    return *measurement;
}

size_t bit::idx() const {
    return bit_idx;
}

void bit::eval(std::stringstream& circuit) {
    if (visit) return;
    else visit = true;
    measurement_gate->eval(circuit);
    circuit << " c" << bit_idx << ";" << std::endl;
}

i64::i64(const std::vector<std::shared_ptr<bit>>& bits,
         size_t i64_idx, 
         bool se) :
    tag{BIT},
    bits{bits},
    se{se},
    i64_idx{i64_idx},
    value_available{false},
    visit{false}
    {}

i64::i64(const std::string& op, 
         const std::vector<std::shared_ptr<i64>>& args, 
         size_t i64_idx,
         bool infix) :
    tag{TMP},
    i64_idx{i64_idx},
    op{op},
    args{args},
    infix{infix},
    value_available{false},
    visit{false}
    {}

i64::i64(std::int64_t value) :
    tag{VALUE},
    value{value},
    value_available{true},
    visit{false}
    {}

bool i64::has_value() {
    if (value_available) return true;
    else if (tag & TMP) return false;

    for (auto &i : bits) 
        if (i->get() == result::NONE) 
            return false;
    
    value = 0;

    for (size_t i = 0; i < bits.size(); i++) 
        value |= (int) bits[i]->get() << i;
    
    if (se) for (size_t i = bits.size(); i < 64; i++) 
        value |= (int) bits.back()->get() << i;

    value_available = true;
    return true;
}

std::int64_t i64::get() {
    has_value();
    if (value_available) return value;
    else return -1;
} 

void i64::eval(std::stringstream& circuit) {
    using std::endl;
    if (visit) return;
    else visit = true;

    switch (tag) {
    case BIT:
        for (auto &i : bits) i->eval(circuit);
        circuit << "[";
        for (auto &i: bits) circuit << i->idx() << ", ";
        circuit << "] -> " << (se? "SE" : "ZE") <<" i" << i64_idx << ";" << endl;
        break;
    case TMP:
        for (auto &i: args) i->eval(circuit);
        if (infix) {
            circuit << "i" << i64_idx << " = i" 
                    << args[0]->idx() << op << "i" 
                    << args[1] << ";" << endl;
        } else {
            circuit << "i" << i64_idx << " = @" << op << "(";
            for (auto &i: args) circuit << "i" << i << ", ";
            circuit << ")" << ";" << endl;
        } 
        break;
    case VALUE:
        circuit << "i" << i64_idx << " = " << value << ";" << endl;
        visit = false;
        break;
    default:
        break;
    }
}

size_t i64::idx() const {
    return i64_idx;
}

handler::handler() : 
    qubit_count{0},
    bit_count{0},
    i64_count{0}
    {}

std::shared_ptr<qubit> handler::alloc() {
    if (not (adj_call.empty() and ctrl_qubit.empty())) 
        throw std::runtime_error("alloc cannot be used with adj or ctrl");

    auto new_qubit = std::make_shared<qubit>(qubit_count);
    qubit_map[qubit_count] = new_qubit;

    if (not block_call.empty())
        block_qubits.top().insert(qubit_count);

    qubit_count++;
    return new_qubit;
}

void handler::add_gate(gate::TAG gate_tag, const std::shared_ptr<qubit>& qbit, const std::vector<double>& args) {
    std::vector<size_t> ctrl_q;
    for (auto &i: ctrl_qubit) for (auto &j : i) 
        ctrl_q.push_back(j);

    bool adj = adj_call.size()%2;

    auto add = [this, ctrl_q, adj, gate_tag, qbit, args]() {
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

    auto add_adj = [this, add]() {
        if (not this->adj_call.empty()) {
            this->adj_call.top().push(add);
        } else {
            add();
        } 
    };

    if (not block_call.empty()) {
        block_qubits.top().insert(ctrl_q.begin(), ctrl_q.end());
        block_qubits.top().insert(qbit->idx());
        block_call.top().push(add_adj);
    } else {
        add_adj();
    }
}

std::shared_ptr<bit> handler::measure(const std::shared_ptr<qubit>& qbit) {
    if (not (adj_call.empty() and ctrl_qubit.empty())) 
        throw std::runtime_error("measure cannot be used with adj or ctrl");

    auto m_gate = std::make_shared<gate>(gate::MEASURE,
                                         qbit->idx(), 
                                         false, 
                                         qbit->last_gate());
    
    auto m_result = std::make_shared<result>(result::NONE);
    measurement_map[bit_count] = m_result;

    auto new_bit = std::make_shared<bit>(bit_count, m_gate, m_result);

    if (not block_call.empty()) {
        block_qubits.top().insert(qbit->idx());
        block_call.top().push([m_gate, qbit]() {
            new (m_gate.get()) gate{gate::MEASURE,
                                    qbit->idx(), 
                                    false, 
                                    qbit->last_gate()};
        });
    }

    bit_count++;
    return new_bit;
}

std::shared_ptr<i64> handler::new_i64(const std::vector<std::shared_ptr<bit>>& bits) {
    return std::make_shared<i64>(bits, i64_count++);
}

std::shared_ptr<i64> handler::i64_op(const std::string& op, const std::vector<std::shared_ptr<i64>>& args, bool infix) {
    return std::make_shared<i64>(op, args, i64_count++, infix);
}

void handler::adj_begin() {
    adj_call.push({});
}

void handler::adj_end() {
    auto inner = adj_call.top();
    adj_call.pop();
    
    auto call_and_adj = [this, inner]() mutable {
        if (not this->adj_call.empty()) {
            while (not inner.empty()) {
                this->adj_call.top().push(inner.top());
                inner.pop();
            }
        } else {
            while (not inner.empty()) {
                inner.top()();
                inner.pop();
            }
        }
    };

    if (not block_call.empty()) {
        block_call.top().push(call_and_adj);
    } else {
        call_and_adj();
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

void handler::begin_block() {
    block_call.push({});
    block_qubits.push({});
} 
void handler::end_block(const std::string& label_next,
                        const std::string& label_true,
                        const std::string& label_false,
                        const std::shared_ptr<i64>& bri64) {
    std::vector<size_t> qubits;
    for (auto &i: block_qubits.top()) 
        qubits.push_back(i);
    block_qubits.pop();
    
    auto calls = block_call.top();
    block_call.pop();


    auto end = [this, qubits, calls, label_next, label_true, label_false, bri64]() mutable {
        std::vector<std::shared_ptr<gate>> qubits_back;
        for (auto i: qubits) 
            qubits_back.push_back(this->qubit_map[i]->last_gate());

        auto new_gate = std::make_shared<gate>(qubits, qubits_back, label_next, label_true, label_false, bri64);

        for (auto i: qubits) 
            this->qubit_map[i]->add_gate(new_gate);
        
        while (not calls.empty()) {
            calls.front()();
            calls.pop();
        }
    };

    if (not block_call.empty()) {
        block_call.top().push(end);
    } else {
        end();
    }
}
