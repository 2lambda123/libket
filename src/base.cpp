#include "../include/ket_bits/base.hpp"

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
    adj_dirty{adj},
    args{args},
    ctrl_idx{ctrl_idx},
    ctrl_back{ctrl_back},
    visit{false}
    {}

gate::gate(TAG tag, 
           const std::vector<size_t>& ctrl_idx,
           const std::vector<std::shared_ptr<gate>>& ctrl_back,
           const std::string& label1,
           const std::string& label2,
           const std::shared_ptr<i64>& bri64) :
    tag{tag},
    back{nullptr},
    ctrl_idx{ctrl_idx},
    ctrl_back{ctrl_back},
    label{label1},
    label_false{label2},
    bri64{bri64} 
    {}

void gate::eval(std::stringstream& circuit) {
    using std::endl;
    
    if (visit) return;
    else visit = true;

    for (auto& i : ctrl_back) if (i) i->eval(circuit);
    if (back) back->eval(circuit);

    if (tag <= U3 and not ctrl_back.empty()) {
        circuit << "\tCTRL\t";
        for (auto i : ctrl_idx) circuit << "q" << i << " ";
    } 

    switch (tag) {
    case X:
        circuit << "\tX\tq" << qubit_idx << endl;
        break;
    case Y:
        circuit << "\tY\tq" << qubit_idx << endl;
        break;
    case Z:
        circuit << "\tZ\tq" << qubit_idx << endl;
        break;
    case H:
        circuit << "\tH\tq" << qubit_idx << endl;
        break;
    case S:
        circuit << (adj_dirty? "\tSD" : "\tS") << "\tq" << qubit_idx << endl;
        break;
    case T:
        circuit << (adj_dirty? "\tTD" : "\tT") << "\tq" << qubit_idx << endl;
        break;
    case U1:
    // TODO adj
        circuit << "\tU1(" << args[0] << ")\tq" << qubit_idx << endl;
        break;
    case U2:
        circuit << "\tU2(" << args[0] << " " << args[1] << ")\tq" << qubit_idx << endl;
        break;
    case U3:
        circuit << "\tU3(" << args[0] << " " << args[1] << " " << args[2] << ")\t\tq" << qubit_idx << endl;
        break;
    case MEASURE: 
        circuit << "\tMEASURE\tq" << qubit_idx;
        break;
    case ALLOC:
        circuit << "\tALLOC " << (adj_dirty? "DIRTY " : "")  << "\tq" << qubit_idx << endl;
        break;
    case FREE:
        circuit << "\tFREE " << (adj_dirty? "DIRTY " : "")  << "\tq" << qubit_idx << endl;
        break;
    case JUMP:
        circuit << "\tJUMP\t@" << label << endl;
        break;
    case BR:
        bri64->eval(circuit);
        circuit << "\tBR\ti" << bri64->idx() << "\t@" << label << "\t@" << label_false << endl;
        break;    
    case LABEL:
        circuit << "LABEL @" << label << endl;
        break;
    default:
        break;
    }
}

qubit::qubit(size_t qubit_idx) :
    qubit_idx{qubit_idx},
    tail_gate{nullptr}
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
    circuit << "\tc" << bit_idx << std::endl;
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
        circuit << "\tINT\ti" << i64_idx << "\t" << (se? "SE" : "ZE") << "\t";
        for (auto &i: bits) circuit << "c" << i->idx() << " ";
        circuit << endl;
        break;
    case TMP:
        for (auto &i: args) i->eval(circuit);
        if (infix) {
            circuit << "\tINT\ti" << i64_idx << "\ti" 
                    << args[0]->idx() << op << "i" 
                    << args[1]->idx() << endl;
        } else {
            circuit << "\tINT i" << i64_idx << "\t@" << op << "(";
            for (auto &i: args) circuit << "i" << i->idx() << " ";
            circuit << ")" << ";" << endl;
        } 
        break;
    case VALUE:
        circuit << "\tINT i" << i64_idx << "\t" << value << endl;
        visit = false;
        break;
    default:
        break;
    }
}

size_t i64::idx() const {
    return i64_idx;
}
