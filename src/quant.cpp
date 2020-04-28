#include "../include/ket_bits/quant.hpp"

using namespace ket;
using namespace ket::base;

_quant::_quant(size_t size, bool dirty, _process* ps) :
    ps{ps}
{
    for (size_t i = 0; i < size; i++) 
        qubits.push_back(ps->alloc(dirty));
}

_quant::_quant(const std::vector<std::shared_ptr<qubit>>& qubits, _process *ps) :
    ps{ps},
    qubits{qubits}
    {}

_quant _quant::operator()(size_t idx) const {
    return _quant{{qubits[idx]}, ps};
}

_quant _quant::operator|(const _quant& other) const {
    auto qubits_ptr = qubits;
    qubits_ptr.insert(qubits_ptr.end(), other.qubits.begin(), other.qubits.end());
    return _quant{qubits_ptr, ps};
}

_quant _quant::invert() const {
    std::vector<std::shared_ptr<qubit>> invert_qubits;
    invert_qubits.insert(invert_qubits.end(), qubits.rbegin(), qubits.rend());
    return _quant{invert_qubits, ps};
}

const std::vector<std::shared_ptr<qubit>>& _quant::get_base_qubits() const {
    return qubits;
}

size_t _quant::len() const {
    return qubits.size();
}

_process* _quant::get_ps() const {
    return ps;
}

qubit_iterator _quant::begin() const {
    return qubit_iterator{qubits, ps};
}

qubit_iterator _quant::end() const {
    return qubit_iterator{qubits.size()};
}

qubit_iterator::qubit_iterator(const std::vector<std::shared_ptr<qubit>> &qubits, _process* ps) :
    ps{ps},
    qubits{&qubits},
    index{0}
    {}
    
qubit_iterator::qubit_iterator(size_t size) : 
    qubits{nullptr},
    index{size}
    {}

_quant qubit_iterator::operator*() {
    return _quant{{qubits->at(index)}, ps};
}

const qubit_iterator& qubit_iterator::operator++() {
    index++;
    return *this;
}

bool qubit_iterator::operator!=(const qubit_iterator& other) const {
    return index != other.index;
}
