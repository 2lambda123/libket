#pragma once
#include <stddef.h>
#include <stdint.h>

typedef void* ket_process_t;
typedef void* ket_qubit_t;
typedef void* ket_future_t;
typedef void* ket_dump_t;
typedef void* ket_label_t;
typedef void* ket_dump_states_t;

#define KET_PAULI_X 0
#define KET_PAULI_Y 1
#define KET_PAULI_Z 2
#define KET_HADAMARD 3
#define KET_PHASE 4
#define KET_RX 5
#define KET_RY 6
#define KET_RZ 7

#define KET_EQ 0
#define KET_NEQ 1
#define KET_GT 2
#define KET_GEQ 3
#define KET_LT 4
#define KET_LEQ 5
#define KET_ADD 6
#define KET_SUB 7
#define KET_MUL 8
#define KET_MOD 9
#define KET_DIV 10
#define KET_SLL 11
#define KET_SRL 12
#define KET_AND 13
#define KET_OR 14
#define KET_XOR 15

#define KET_SUCCESS 0
#define KET_CONTROL_TWICE 1
#define KET_DATA_NOT_AVAILABLE 2
#define KET_DEALLOCATED_QUBIT 3
#define KET_FAIL_TO_PARSE_RESULT 4
#define KET_NO_ADJ 5
#define KET_NO_CTRL 6
#define KET_NON_GATE_INSTRUCTION 7
#define KET_NOT_BIN 8
#define KET_NOT_JSON 9
#define KET_NOT_UNITARY 10
#define KET_PLUGIN_ON_CTRL 11
#define KET_TARGET_ON_CONTROL 12
#define KET_TERMINATED_BLOCK 13
#define KET_UNDEFINED_CLASSICAL_OP 14
#define KET_UNDEFINED_DATA_TYPE 15
#define KET_UNDEFINED_GATE 16
#define KET_UNEXPECTED_RESULT_DATA 17
#define KET_UNMATCHED_PID 18
#define KET_UNDEFINED_ERROR 19

const uint8_t ket_error_message(int32_t error_code, size_t* size);

int32_t ket_process_new(size_t pid, ket_process_t* process);

int32_t ket_process_delete(ket_process_t process);

int32_t ket_process_allocate_qubit(ket_process_t process, bool dirty,
                                   ket_qubit_t* qubit);

int32_t ket_process_free_qubit(ket_process_t process, ket_qubit_t qubit,
                               bool dirty);

int32_t ket_process_apply_gate(ket_process_t process, int32_t gate,
                               double param, ket_qubit_t target);

int32_t ket_process_apply_plugin(ket_process_t process, const char* name,
                                 ket_qubit_t* target, size_t target_size,
                                 const char* args);

int32_t ket_process_measure(ket_process_t process, ket_qubit_t* qubits,
                            size_t qubits_size, ket_future_t* future);

int32_t ket_process_ctrl_push(ket_process_t process, ket_qubit_t* qubits,
                              size_t qubits_size);

int32_t ket_process_ctrl_pop(ket_process_t process);

int32_t ket_process_adj_begin(ket_process_t process);

int32_t ket_process_adj_end(ket_process_t process);

int32_t ket_process_get_label(ket_process_t process, ket_label_t* label);

int32_t ket_process_open_block(ket_process_t process, ket_label_t label);

int32_t ket_process_jump(ket_process_t process, ket_label_t label);

int32_t ket_process_branch(ket_process_t process, ket_future_t test,
                           ket_label_t then, ket_label_t otherwise);

int32_t ket_process_dump(ket_process_t process, ket_qubit_t* qubits,
                         size_t qubits_size, ket_dump_t* dump);

int32_t ket_process_add_int_op(ket_process_t process, int32_t op,
                               ket_future_t lhs, ket_future_t rhs,
                               ket_future_t* result);

int32_t ket_process_int_new(ket_process_t process, int64_t value,
                            ket_future_t* future);

int32_t ket_process_int_set(ket_process_t process, ket_future_t result,
                            ket_future_t value);

int32_t ket_process_prepare_for_execution(ket_process_t process);

int32_t ket_process_exec_time(ket_process_t process, double* time);

int32_t ket_process_set_timeout(ket_process_t process, uint64_t timeout);

int32_t ket_process_serialize_metrics(ket_process_t process, int32_t data_type);

int32_t ket_process_serialize_quantum_code(ket_process_t process,
                                           int32_t data_type);

int32_t ket_process_set_serialized_result(ket_process_t process,
                                          uint8_t* result, size_t result_size,
                                          int32_t data_type);

int32_t ket_qubit_delete(ket_qubit_t qubit);

int32_t ket_qubit_index(ket_qubit_t qubit, size_t* index);

int32_t ket_qubit_pid(ket_qubit_t qubit, size_t* pid);

int32_t ket_qubit_allocated(ket_qubit_t qubit, bool* allocated);

int32_t ket_qubit_measured(ket_qubit_t qubit, bool* measured);

int32_t ket_dump_delete(ket_dump_t dump);

int32_t ket_dump_states(ket_dump_t dump, void* states, size_t* size);

int32_t ket_dump_get_state(void* states, size_t index, uint64_t** state,
                           size_t* size);

int32_t ket_dump_amplitudes_real(ket_dump_t dump, double** amp, size_t* size);

int32_t ket_dump_amplitudes_img(ket_dump_t dump, double** amp, size_t* size);

int32_t ket_dump_available(ket_dump_t dump, bool* available);

int32_t ket_future_delete(ket_future_t future);

int32_t ket_future_value(ket_future_t future, int64_t* value);

int32_t ket_future_index(ket_future_t future, size_t* index);

int32_t ket_future_pid(ket_future_t future, size_t* pid);

int32_t ket_label_delete(ket_label_t label);

int32_t ket_label_index(ket_label_t label, size_t* index);

int32_t ket_label_pid(ket_label_t label, size_t* pid);
