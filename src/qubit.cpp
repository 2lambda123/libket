#include "../include/ket_bits/ket_init.hpp"
#include "../include/ket_bits/qubit_gate.hpp"

ket::Qubit::Qubit(size_t size) : qubits{ket_handle->alloc(size)} {}

ket::Qubit::Qubit(const base::Handler::Qubits& qubits) : qubits{qubits} {}

ket::Qubit ket::Qubit::operator() (size_t index) const {
    return Qubit{qubits[index]};
}

ket::Qubit ket::Qubit::operator() (size_t begin, size_t end) const {
    std::vector<base::Handler::Qubits> qbits;

    for (auto i = begin; i < end; i++) {
        qbits.push_back(qubits[i]);
    }
    return Qubit{qbits};
}

size_t ket::Qubit::size() const {
    return qubits.size();
}

ket::Qubit ket::operator+(const Qubit& a, const Qubit& b) {
    return {{{a.qubits, b.qubits}}};
}
