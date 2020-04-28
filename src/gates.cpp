#include "../include/ket"
#include "../include/ket_bits/quant.hpp"
#include "../include/ket_bits/future.hpp"

using namespace ket;
using namespace ket::base;

quant ket::alloc(size_t size, process& ps) {
    return quant{{new _quant{size, false, static_cast<_process*>(ps.ps.get())}, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}

quant ket::alloc_dirty(size_t size, process& ps) {
    return quant{{new _quant{size, true, static_cast<_process*>(ps.ps.get())}, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}

void free(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->free(i, false);
}

void free_dirty(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->free(i, true);
}

future ket::measure(const quant& _q, bool wait) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    if (wait) q->get_ps()->wait(q->get_base_qubits());

    std::vector<std::shared_ptr<base::bit>> bits;
    for (auto &i : q->get_base_qubits()) 
        bits.push_back(q->get_ps()->measure(i));
    return future{{new _future{q->get_ps()->new_i64(bits), q->get_ps()},
                   [](auto ptr){ delete static_cast<_future*>(ptr); } }};
}

void ket::wait(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->wait(q->get_base_qubits());
}

void ket::x(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::X, i);
}

void ket::y(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::Y, i);
}

void ket::z(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::Z, i);
}

void ket::h(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::H, i);
}

void ket::s(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::S, i);
}

void ket::t(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::T, i);
}

void ket::u1(double lambda, const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::U1, i, {lambda});
}

void ket::u2(double phi, double lambda, const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::U2, i, {phi, lambda});
}

void ket::u3(double theta, double phi, double lambda, const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::U3, i, {theta, phi, lambda});
}

void ket::dump(const quant& _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    for (auto &i : q->get_base_qubits()) 
        q->get_ps()->add_gate(base::gate::DUMP, i);
}
