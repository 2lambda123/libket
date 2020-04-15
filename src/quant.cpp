#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/quant.hpp"

using namespace ket;

quant::quant(size_t size, bool dirty) {
    for (size_t i = 0; i < size; i++) 
        qubits.push_back(ket_hdl.alloc(dirty));
}

const std::vector<std::shared_ptr<base::qubit>>& quant::get_base_qubits() const {
    return qubits;
}

size_t quant::len() const {
    return qubits.size();
}
