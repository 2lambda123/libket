#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/gates.hpp"

using namespace ket;
using namespace ket::base;

quant ket::alloc(size_t size, process* ps) {
    return quant{size, false, ps};
}

quant ket::alloc_dirty(size_t size, process* ps) {
    return quant{size, true, ps};
}

void free(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->free(i, false);
}

void free_dirty(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->free(i, true);
}

future ket::measure(const quant& q, bool wait) {
    if (wait) q.get_ps()->wait(q.get_base_qubits());

    std::vector<std::shared_ptr<base::bit>> bits;
    for (auto &i : q.get_base_qubits()) 
        bits.push_back(q.get_ps()->measure(i));
    return future{q.get_ps()->new_i64(bits), q.get_ps()};
}

void ket::wait(const quant& q) {
    q.get_ps()->wait(q.get_base_qubits());
}

void ket::x(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::X, i);
}

void ket::y(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::Y, i);
}

void ket::z(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::Z, i);
}

void ket::h(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::H, i);
}

void ket::s(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::S, i);
}

void ket::t(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::T, i);
}

void ket::u1(double lambda, const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::U1, i, {lambda});
}

void ket::u2(double phi, double lambda, const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::U2, i, {phi, lambda});
}

void ket::u3(double theta, double phi, double lambda, const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::U3, i, {theta, phi, lambda});
}

void ket::dump(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        q.get_ps()->add_gate(base::gate::DUMP, i);
}