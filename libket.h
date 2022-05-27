#pragma once
#include <stdint.h>
#include <stddef.h>

typedef void *ket_process_t;
typedef void *ket_qubit_t;
typedef void *ket_future_t;
typedef void *ket_dump_t;
typedef void *ket_label_t;
typedef void *ket_dump_states_t;

#define KET_SUCCESS 0
#define KET_ERROR 1

#define PAULI_X 0
#define PAULI_Y 1
#define PAULI_Z 2
#define HADAMARD 3
#define PHASE 4
#define RX 5
#define RY 6
#define RZ 7

#define EQ 0
#define NEQ 1
#define GT 2
#define GEQ 3
#define LT 4
#define LEQ 5
#define ADD 6
#define SUB 7
#define MUL 8
#define DIV 9
#define SLL 10
#define SRL 11
#define AND 12
#define OR 13
#define XOR 14

const uint8_t *ket_error_message(size_t *size);

ket_process_t ket_process_new(uint32_t pid);
void ket_process_delete(ket_process_t process);
ket_qubit_t ket_process_alloc(ket_process_t process, bool dirty);
int32_t ket_process_free(ket_process_t process, ket_qubit_t qubit, bool dirty);
int32_t ket_process_apply_gate(ket_process_t process, uint32_t gate, double param, ket_qubit_t target);
ket_future_t ket_process_measure(ket_process_t process, ket_qubit_t qubits, size_t num_qubits);
int32_t ket_process_ctrl_push(ket_process_t process, ket_qubit_t qubits, size_t num_qubits);
int32_t ket_process_ctrl_pop(ket_process_t process);
int32_t ket_process_adj_begin(ket_process_t process);
int32_t ket_process_adj_end(ket_process_t process);
ket_label_t ket_process_get_label(ket_process_t process);
int32_t ket_process_open_block(ket_process_t process, ket_label_t label);
int32_t ket_process_jump(ket_process_t process, ket_label_t label);
int32_t ket_process_branch(ket_process_t process, ket_future_t test, ket_label_t then, ket_label_t otherwise);
ket_dump_t ket_process_dump(ket_process_t process, ket_qubit_t qubits, size_t num_qubits);
ket_future_t ket_process_add_int_op(ket_process_t process, uint32_t op, ket_future_t lhs, ket_future_t rhs);
int32_t ket_process_int_set(ket_process_t process, ket_future_t result, ket_future_t value);
ket_future_t ket_process_int_new(ket_process_t process, int64_t value);
const uint8_t *ket_process_get_quantum_code_as_json(ket_process_t process, size_t *size);
const uint8_t *ket_process_get_quantum_code_as_bin(ket_process_t process, size_t *size);

void ket_qubit_delete(ket_qubit_t qubit);
uint32_t ket_qubit_index(ket_qubit_t qubit);
uint32_t ket_qubit_pid(ket_qubit_t qubit);
bool ket_qubit_allocated(ket_qubit_t qubit);
bool ket_qubit_measured(ket_qubit_t qubit);

void ket_dump_delete(ket_dump_t dump);
ket_dump_states_t ket_dump_states(ket_dump_t dump, size_t *size);
uint64_t *ket_dump_get_state(ket_dump_states_t state, size_t index, size_t *size);
double *ket_dump_amplitudes_real(ket_dump_t dump, size_t *size);
double *ket_dump_amplitudes_img(ket_dump_t dump, size_t *size);
bool ket_dump_available(ket_dump_t dump);

void ket_future_delete(ket_future_t future);
int64_t ket_future_value(ket_future_t future, bool *available);
uint32_t ket_future_index(ket_future_t future);
uint32_t ket_future_pid(ket_future_t future);
