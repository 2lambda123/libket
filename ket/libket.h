/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KET_SUCCESS    0
#define KET_ERROR      1
#define KET_PAULI_X    2 
#define KET_PAULI_Y    3  
#define KET_PAULI_Z    4
#define KET_ROTATION_X 5
#define KET_ROTATION_Y 6
#define KET_ROTATION_Z 7
#define KET_HADAMARD   8
#define KET_PHASE      9
#define KET_INT_EQ     10
#define KET_INT_NEQ    11
#define KET_INT_GT     12
#define KET_INT_GEQ    13
#define KET_INT_LT     14
#define KET_INT_LEQ    15
#define KET_INT_ADD    16
#define KET_INT_SUB    17
#define KET_INT_MUL    18
#define KET_INT_DIV    19
#define KET_INT_SLL    20
#define KET_INT_SRL    21
#define KET_INT_AND    22
#define KET_INT_OR     23
#define KET_INT_XOR    24
#define KET_INT_FF     25
#define KET_INT_FI     26
#define KET_INT_IF     27

typedef void*          ket_process_t;
typedef void*          ket_qubit_t;
typedef void*          ket_future_t;
typedef void*          ket_label_t;
typedef void*          ket_dump_t;
typedef void*          ket_dump_states_t;
typedef void*          ket_dump_amplitides_t;
typedef unsigned long* ket_dump_state_t;
typedef int            ket_gate_t;
typedef int            ket_int_op_t;
typedef int            ket_int_op_tt_t;

const char* ket_error_message();

int ket_process_new(ket_process_t*, unsigned);
int ket_process_delete(ket_process_t);
int ket_process_alloc(ket_process_t, ket_qubit_t, _Bool);
int ket_process_free(ket_process_t, ket_qubit_t, _Bool);
int ket_process_gate(ket_process_t, ket_gate_t, ket_qubit_t, double);
int ket_process_measure(ket_process_t, ket_future_t, int, ...);
int ket_process_new_int(ket_process_t, ket_future_t, long);
int ket_process_plugin(ket_process_t, char*, char*, int, ...);
int ket_process_ctrl_push(ket_process_t, int, ...);
int ket_process_ctrl_pop(ket_process_t);
int ket_process_adj_begin(ket_process_t);
int ket_process_adj_end(ket_process_t);
int ket_process_get_label(ket_process_t, ket_label_t);
int ket_process_open_block(ket_process_t, ket_label_t);
int ket_process_jump(ket_process_t, ket_label_t);
int ket_process_breach(ket_process_t, ket_future_t, ket_label_t, ket_label_t);
int ket_process_dump(ket_process_t, ket_dump_t, int, ...);
int ket_process_run(ket_process_t);
int ket_process_exec_time(ket_process_t, double*);
int ket_process_id(ket_process_t, unsigned*);

int ket_qubit_new(ket_qubit_t*);
int ket_qubit_delete(ket_qubit_t);
int ket_qubit_index(ket_qubit_t, unsigned*);
int ket_qubit_measured(ket_qubit_t, _Bool*);
int ket_qubit_allocated(ket_qubit_t, _Bool*);
int ket_qubit_process_id(ket_qubit_t, unsigned*);

int ket_future_new(ket_future_t*);
int ket_future_delete(ket_future_t);
int ket_future_value(ket_future_t, long*);
int ket_future_set(ket_future_t, ket_future_t);
int ket_future_available(ket_future_t, _Bool*);
int ket_future_index(ket_future_t, unsigned*);
int ket_future_process_id(ket_future_t, unsigned*);

int ket_future_op(ket_future_t, ket_int_op_t, ket_int_op_tt_t, ...);

int ket_label_new(ket_label_t*);
int ket_label_delete(ket_label_t);
int ket_label_index(ket_label_t, unsigned*);
int ket_label_process_id(ket_label_t, unsigned*);

int ket_dump_new(ket_dump_t*);
int ket_dump_delete(ket_dump_t);
int ket_dump_states(ket_dump_t, ket_dump_states_t*, size_t*);
int ket_dump_amplitides(ket_dump_t, ket_dump_amplitides_t*, size_t*);
int ket_dump_available(ket_dump_t, _Bool*);
int ket_dump_index(ket_dump_t, unsigned*);
int ket_dump_process_id(ket_dump_t, unsigned*);

int ket_dump_state_at(ket_dump_states_t, ket_dump_state_t*, size_t*, unsigned long);

int ket_dump_amp_at(ket_dump_amplitides_t, double*, double*, unsigned long);

#ifdef __cplusplus
}
#endif