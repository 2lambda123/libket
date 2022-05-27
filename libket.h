#pragma once
#include <stdint.h>
#include <stddef.h>

typedef void *process_t;
typedef void *qubit_t;
typedef void *future_t;
typedef void *dump_t;
typedef void *label_t;
typedef void *dump_states_t;

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

process_t ket_process_new(uint32_t pid);
void ket_process_delete(process_t process);
qubit_t ket_process_alloc(process_t process, bool dirty);
int32_t ket_process_free(process_t process, qubit_t qubit, bool dirty);
int32_t ket_process_apply_gate(process_t process, uint32_t gate, double param, qubit_t target);
future_t ket_process_measure(process_t process, qubit_t qubits, size_t num_qubits);
int32_t ket_process_ctrl_push(process_t process, qubit_t qubits, size_t num_qubits);
int32_t ket_process_ctrl_pop(process_t process);
int32_t ket_process_adj_begin(process_t process);
int32_t ket_process_adj_end(process_t process);
label_t ket_process_get_label(process_t process);
int32_t ket_process_open_block(process_t process, label_t label);
int32_t ket_process_jump(process_t process, label_t label);
int32_t ket_process_branch(process_t process, future_t test, label_t then, label_t otherwise);
dump_t ket_process_dump(process_t process, qubit_t qubits, size_t num_qubits);
future_t ket_process_add_int_op(process_t process, uint32_t op, future_t lhs, future_t rhs);
int32_t ket_process_int_set(process_t process, future_t result, future_t value);
future_t ket_process_int_new(process_t process_t, int64_t value);
const uint8_t *ket_process_get_quantum_code_as_json(process_t process, size_t *size);
const uint8_t *ket_process_get_quantum_code_as_bin(process_t process, size_t *size);

void ket_qubit_delete(qubit_t qubit);
uint32_t ket_qubit_index(qubit_t qubit);
uint32_t ket_qubit_pid(qubit_t qubit);
bool ket_qubit_allocated(qubit_t qubit);
bool ket_qubit_measured(qubit_t qubit);

void ket_dump_delete(dump_t dump);
dump_states_t ket_dump_states(dump_t dump, size_t *size);
uint64_t *ket_dump_get_state(dump_states_t state, size_t index, size_t *size);
double *ket_dump_amplitudes_real(dump_t dump, size_t *size);
double *ket_dump_amplitudes_img(dump_t dump, size_t *size);
bool ket_dump_available(dump_t dump);

void ket_future_delete(future_t future);
int64_t ket_future_value(future_t future, bool *available);
uint32_t ket_future_index(future_t future);
uint32_t ket_future_pid(future_t future);
