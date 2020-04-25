#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/quant.hpp"

using namespace ket::base;

quant::quant(size_t size, bool dirty) {
    for (size_t i = 0; i < size; i++) 
        qubits.push_back(ket_hdl->alloc(dirty));
}

quant::quant(const std::vector<std::shared_ptr<qubit>>& qubits) :
    qubits{qubits}
    {}

quant quant::operator()(size_t idx) const {
    return quant{{qubits[idx]}};
}

quant quant::operator|(const quant& other) const {
    auto qubits_ptr = qubits;
    qubits_ptr.insert(qubits_ptr.end(), other.qubits.begin(), other.qubits.end());
    return quant{qubits_ptr};
}

quant quant::invert() const {
    std::vector<std::shared_ptr<qubit>> invert_qubits;
    invert_qubits.insert(invert_qubits.end(), qubits.rbegin(), qubits.rend());
    return quant{invert_qubits};
}

const std::vector<std::shared_ptr<qubit>>& quant::get_base_qubits() const {
    return qubits;
}

size_t quant::len() const {
    return qubits.size();
}

qubit_iterator quant::begin() {
    return qubit_iterator{qubits};
}

qubit_iterator quant::end() {
    return qubit_iterator{qubits.size()};
}

qubit_iterator::qubit_iterator(const std::vector<std::shared_ptr<qubit>> &qubits) :
    qubits{&qubits},
    index{0}
    {}
    
qubit_iterator::qubit_iterator(size_t size) : 
    qubits{nullptr},
    index{size}
    {}

quant qubit_iterator::operator*() {
    return quant{{qubits->at(index)}};
}

const qubit_iterator& qubit_iterator::operator++() {
    index++;
    return *this;
}

bool qubit_iterator::operator!=(const qubit_iterator& other) const {
    return index != other.index;
}