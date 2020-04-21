#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/gates.hpp"

using namespace ket;

future ket::measure(const quant& q, bool wait) {
    if (wait) ket_hdl->wait(q.get_base_qubits());

    std::vector<std::shared_ptr<base::bit>> bits;
    for (auto &i : q.get_base_qubits()) 
        bits.push_back(ket_hdl->measure(i));
    return future{ket_hdl->new_i64(bits)};
}

void ket::wait(const quant& q) {
    ket_hdl->wait(q.get_base_qubits());
}

void ket::x(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::X, i);
}

void ket::y(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::Y, i);
}

void ket::z(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::Z, i);
}

void ket::h(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::H, i);
}

void ket::s(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::S, i);
}

void ket::t(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::T, i);
}

void ket::u1(double lambda, const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::U1, i, {lambda});
}

void ket::u2(double phi, double lambda, const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::U2, i, {phi, lambda});
}

void ket::u3(double theta, double phi, double lambda, const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::U3, i, {theta, phi, lambda});
}

void ket::dump(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl->add_gate(base::gate::DUMP, i);
}