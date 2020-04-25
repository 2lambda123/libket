#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/quant.hpp"

using namespace ket;
using namespace ket::base;

quant::quant(size_t size, bool dirty, process* ps) :
    ps{ps}
{
    for (size_t i = 0; i < size; i++) 
        qubits.push_back(ps->alloc(dirty));
}

quant::quant(const std::vector<std::shared_ptr<qubit>>& qubits, process *ps) :
    ps{ps},
    qubits{qubits}
    {}

quant quant::operator()(size_t idx) const {
    return quant{{qubits[idx]}, ps};
}

quant quant::operator|(const quant& other) const {
    auto qubits_ptr = qubits;
    qubits_ptr.insert(qubits_ptr.end(), other.qubits.begin(), other.qubits.end());
    return quant{qubits_ptr, ps};
}

quant quant::invert() const {
    std::vector<std::shared_ptr<qubit>> invert_qubits;
    invert_qubits.insert(invert_qubits.end(), qubits.rbegin(), qubits.rend());
    return quant{invert_qubits, ps};
}

const std::vector<std::shared_ptr<qubit>>& quant::get_base_qubits() const {
    return qubits;
}

size_t quant::len() const {
    return qubits.size();
}

process* quant::get_ps() const {
    return ps;
}

qubit_iterator quant::begin() const {
    return qubit_iterator{qubits, ps};
}

qubit_iterator quant::end() const {
    return qubit_iterator{qubits.size()};
}

qubit_iterator::qubit_iterator(const std::vector<std::shared_ptr<qubit>> &qubits, process* ps) :
    ps{ps},
    qubits{&qubits},
    index{0}
    {}
    
qubit_iterator::qubit_iterator(size_t size) : 
    qubits{nullptr},
    index{size}
    {}

quant qubit_iterator::operator*() {
    return quant{{qubits->at(index)}, ps};
}

const qubit_iterator& qubit_iterator::operator++() {
    index++;
    return *this;
}

bool qubit_iterator::operator!=(const qubit_iterator& other) const {
    return index != other.index;
}
