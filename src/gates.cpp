#include "../include/ket_bits/ket_init.hpp"
#include "../include/ket_bits/qubit_bit.hpp"

void ket::x(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("x", q(i).qubits);
}

void ket::y(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("y", q(i).qubits);
}

void ket::z(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("z", q(i).qubits);
}

void ket::h(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("h", q(i).qubits);
}

void ket::s(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("s", q(i).qubits);
}

void ket::sd(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("sd", q(i).qubits);
}

void ket::t(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("t", q(i).qubits);
}

void ket::td(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("td", q(i).qubits);
}

void ket::cnot(const Qubit& ctrl, const Qubit& target) {
    if (ctrl.size() == target.size()) {
        for (size_t i = 0; i < ctrl.size(); i++) {
            ket_handle->add_gate("cnot", {{ctrl(i).qubits , target(i).qubits}});
        }
    }
}

void ket::u1(double lambda, const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("u1", q(i).qubits, {lambda});
}

void ket::u2(double phi, double lambda, const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("u2", q(i).qubits, {phi, lambda});
}

void ket::u3(double theta, double phi, double lambda, const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) 
        ket_handle->add_gate("u3", q(i).qubits, {theta, phi, lambda});
}

ket::Bit ket::measure(const ket::Qubit& q) {
    std::vector<base::Handler::Bits> bits;
    for (size_t i = 0; i < q.size(); i++)
        bits.push_back(ket_handle->measure(q(i).qubits));
    return Bit{bits};
}

ket::Qubit ket::dirty(size_t size) {
    return Qubit{ket_handle->alloc(size, true)};
}

void ket::free(const ket::Qubit& q) {
    ket_handle->free(q.qubits);
}

void ket::freedirty(const ket::Qubit& q) {
    ket_handle->free_dirty(q.qubits);
}

void ket::__apply_oracle(const std::string& gate, const Qubit& q) {
    ket_handle->add_oracle(gate, q.qubits);
}
