#include "cket.h"
#include "../include/ket"

void ket_begin(int argc, char* argv[]) {
    ket::begin(argc, argv);
}

void ket_end() {
    ket::end();
}

ket_qubit* ket_qubit_new(size_t size) {
    return new ket::Qubit{size};;
}

void ket_qubit_delete(ket_qubit* q) {
    delete static_cast<ket::Qubit*>(q);
}

ket_qubit* ket_qubit_get(ket_qubit* q, size_t index) {
    return new ket::Qubit((*static_cast<ket::Qubit*>(q))(index));
}

ket_qubit* ket_qubit_get_rage(ket_qubit* q, size_t begin, size_t end) {
    return new ket::Qubit((*static_cast<ket::Qubit*>(q))(begin, end));
}

size_t ket_qubit_size(ket_qubit* q) {
    return static_cast<ket::Qubit*>(q)->size();
}

void       ket_bit_delete(ket_bit* b) {
    delete static_cast<ket::Bit*>(b);
}

ket_bit* ket_bit_get(ket_bit* b, size_t index) {
    return new ket::Bit((*static_cast<ket::Bit*>(b))(index));
}

ket_bit* ket_bit_get_range(ket_bit* b, size_t begin, size_t end) {
    return new ket::Bit((*static_cast<ket::Bit*>(b))(begin, end));
}

int  ket_bit_at(ket_bit* b, size_t index) {
    return (*static_cast<ket::Bit*>(b))[index];
}
size_t ket_bit_size(ket_bit* b) {
    return static_cast<ket::Bit*>(b)->size();
}

void ket_ctrl_begin() {
    ket_handle->ctrl_begin();
}

void ket_ctrl_add_qubit(ket_qubit* q) {
    ket_handle->add_ctrl(ket::Qubit_or_Bit{*static_cast<ket::Qubit*>(q)}.get_qubit());
}

void ket_ctrl_add_bit(ket_bit* b) {
    ket_handle->add_ctrl(ket::Qubit_or_Bit{*static_cast<ket::Bit*>(b)}.get_bit());
}

void ket_ctrl_end() {
    ket_handle->ctrl_end();
}

void       ket_adj_begin() {
    ket_handle->adj_begin();
}

void       ket_adj_end() {
    ket_handle->adj_end();
}

void ket_x(ket_qubit* q) {
    ket::x(*static_cast<ket::Qubit*>(q));
}

void ket_y(ket_qubit* q) {
    ket::y(*static_cast<ket::Qubit*>(q));
}

void ket_z(ket_qubit* q) {
    ket::z(*static_cast<ket::Qubit*>(q));
}

void ket_h(ket_qubit* q) {
    ket::h(*static_cast<ket::Qubit*>(q));
}

void ket_s(ket_qubit* q) {
    ket::s(*static_cast<ket::Qubit*>(q));
}

void ket_sd(ket_qubit* q) {
    ket::sd(*static_cast<ket::Qubit*>(q));
}

void ket_t(ket_qubit* q) {
    ket::t(*static_cast<ket::Qubit*>(q));
}

void ket_td(ket_qubit* q) {
    ket::td(*static_cast<ket::Qubit*>(q));
}

void ket_cnot(ket_qubit* ctrl, ket_qubit* target)  {
    ket::cnot(*static_cast<ket::Qubit*>(ctrl), *static_cast<ket::Qubit*>(target));
}

void ket_u1(double lambda, ket_qubit* q) {
    ket::u1(lambda, *static_cast<ket::Qubit*>(q));
}

void ket_u2(double phi, double lambda, ket_qubit* q) {
    ket::u2(phi, lambda, *static_cast<ket::Qubit*>(q));
}


void ket_u3(double theta, double phi, double lambda, ket_qubit* q) {
    ket::u3(theta, phi, lambda, *static_cast<ket::Qubit*>(q));
}

ket_qubit* ket_qubit_join(ket_qubit* a, ket_qubit* b) {
    return new ket::Qubit((*static_cast<ket::Qubit*>(a)) + (*static_cast<ket::Qubit*>(b)));
}

ket_bit*   ket_bit_join(ket_bit* a, ket_bit* b) {
    return new ket::Bit((*static_cast<ket::Bit*>(a)) + (*static_cast<ket::Bit*>(b)));
}

ket_bit* ket_measure(ket_qubit* q) {
    return new ket::Bit(ket::measure(*static_cast<ket::Qubit*>(q)));
}

ket_qubit* ket_dirty(size_t size) {
    return new ket::Qubit(ket::dirty(size));
}

void ket_free(ket_qubit* q) {
    ket::free(*static_cast<ket::Qubit*>(q));
}

void ket_freedirty(ket_qubit* q) {
    ket::freedirty(*static_cast<ket::Qubit*>(q));
}

void ket_gate_delete(ket_gate* g) {
    delete static_cast<ket::oracle::Gate*>(g);
}

void ket_gate_apply(ket_gate* g, ket_qubit* q) {
    (*static_cast<ket::oracle::Gate*>(g))(*static_cast<ket::Qubit*>(q));
}

size_t ket_gate_size(ket_gate* g) {
    return static_cast<ket::oracle::Gate*>(g)->size();
}

ket_gate* ket_gate_lambda(ket_func func, size_t size, size_t begin, size_t end) {
    return new ket::oracle::Gate(ket::oracle::lambda(func, size, begin, end));
}

void ket_swap(ket_qubit* a, ket_qubit* b) {
   ket::swap(*static_cast<ket::Qubit*>(a), *static_cast<ket::Qubit*>(b));
}

void ket_qft(ket_qubit* q) {
    ket::qft(*static_cast<ket::Qubit*>(q));
}

void ket_invert(ket_qubit* q) {
    ket::invert(*static_cast<ket::Qubit*>(q));
}

size_t ket_bit_to_size_t(ket_bit* b) {
    return ket::to<size_t>(*static_cast<ket::Bit*>(b));
}
