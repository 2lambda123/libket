#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ket_init void *ket_handle

typedef void ket_qubit;
typedef void ket_bit;

void       ket_begin(int argc, char* argv[]);
void       ket_end();

ket_qubit* ket_qubit_new(size_t size);
void       ket_x(ket_qubit* q);
ket_bit*   ket_measure(ket_qubit* q);
int        ket_bit_get(ket_bit* b, size_t index);




#ifdef __cplusplus
}
#endif