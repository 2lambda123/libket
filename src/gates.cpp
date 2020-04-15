#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/gates.hpp"

using namespace ket;

future ket::measure(const quant& q) {
    std::vector<std::shared_ptr<base::bit>> bits;
    for (auto &i : q.get_base_qubits()) 
        bits.push_back(ket_hdl.measure(i));
    return future{bits};
}

void ket::x(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl.add_gate(base::gate::X, i);
}

void ket::y(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl.add_gate(base::gate::Y, i);
}

void ket::z(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl.add_gate(base::gate::Z, i);
}

void ket::h(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl.add_gate(base::gate::H, i);
}

void ket::s(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl.add_gate(base::gate::S, i);
}

void ket::t(const quant& q) {
    for (auto &i : q.get_base_qubits()) 
        ket_hdl.add_gate(base::gate::T, i);
}
