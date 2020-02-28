#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define cket_init void *ket_handle

typedef void ket_qubit;
typedef void ket_bit;
typedef void ket_gate;
typedef size_t (*ket_func)(size_t);

void       ket_begin(int argc, char* argv[]);
void       ket_end();

ket_qubit* ket_qubit_new(size_t size);
void       ket_qubit_delete(ket_qubit* q);
ket_qubit* ket_qubit_get(ket_qubit* q, size_t index);
ket_qubit* ket_qubit_get_rage(ket_qubit* q, size_t begin, size_t end);
size_t     ket_qubit_size(ket_qubit* q);

void       ket_bit_delete(ket_bit* b);
ket_bit*   ket_bit_get(ket_bit* b, size_t index);
ket_bit*   ket_bit_get_range(ket_bit* b, size_t begin, size_t end);
int        ket_bit_at(ket_bit* b, size_t index);
size_t     ket_bit_size(ket_bit* b);

void       ket_ctrl_begin();
void       ket_ctrl_add_qubit(ket_qubit* q);
void       ket_ctrl_add_bit(ket_bit* b);
void       ket_ctrl_end();

void       ket_adj_begin();
void       ket_adj_end();

void       ket_x(ket_qubit* q);
void       ket_y(ket_qubit* q);
void       ket_z(ket_qubit* q);
void       ket_h(ket_qubit* q);
void       ket_s(ket_qubit* q);
void       ket_sd(ket_qubit* q);
void       ket_t(ket_qubit* q);
void       ket_td(ket_qubit* q);
void       ket_cnot(ket_qubit* ctrl, ket_qubit* target);
void       ket_u1(double lambda, ket_qubit* q);
void       ket_u2(double phi, double lambda, ket_qubit* q);
void       ket_u3(double theta, double phi, double lambda, ket_qubit* q);
ket_qubit* ket_qubit_join(ket_qubit* a, ket_qubit* b);
ket_bit*   ket_bit_join(ket_bit* a, ket_bit* b);
ket_bit*   ket_measure(ket_qubit* q);
ket_qubit* ket_dirty(size_t size);
void       ket_free(ket_qubit* q);
void       ket_freedirty(ket_qubit* q);

void      ket_gate_delete(ket_gate* g);
void      ket_gate_apply(ket_gate* g, ket_qubit* q);
size_t    ket_gate_size(ket_gate* g);
ket_gate* ket_gate_lambda(ket_func func, size_t size, size_t begin, size_t end);

void      ket_swap(ket_qubit* a, ket_qubit* b);
void      ket_qft(ket_qubit* q);
void      ket_invert(ket_qubit* q);
size_t    ket_bit_to_size_t(ket_bit* b);

#ifdef __cplusplus
}
#endif