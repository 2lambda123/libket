#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/quant.hpp"

using namespace ket;

quant::quant(size_t size, bool dirty) {
    for (size_t i = 0; i < size; i++) 
        qubits.push_back(ket_hdl.alloc(dirty));
}

quant::quant(const std::vector<std::shared_ptr<base::qubit>>& qubits) :
    qubits{qubits}
    {}

quant quant::operator()(size_t idx) const {
    return quant{{qubits[idx]}};
}

const std::vector<std::shared_ptr<base::qubit>>& quant::get_base_qubits() const {
    return qubits;
}

size_t quant::len() const {
    return qubits.size();
}

iterator quant::begin() {
    return iterator{qubits};
}

iterator quant::end() {
    return iterator{};
}

iterator::iterator(const std::vector<std::shared_ptr<base::qubit>> &qubits) :
    qubits{&qubits},
    index{0}
    {}
    
iterator::iterator() : 
    qubits{nullptr},
    index{SIZE_MAX}
    {}

quant iterator::operator*() {
    return quant{{qubits[index]}};
}

const iterator& iterator::operator++() {
    index++;
    return *this;
}

bool iterator::operator!=(const iterator& other) const {
    return index != other.index or qubits != other.qubits;
}