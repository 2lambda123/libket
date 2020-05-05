#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void ket_quant;
typedef void ket_future;
typedef void ket_process;
typedef void ket_label;

#define ket_init ket_process* ket_ps;\
char* ket_kbw_path;\
int ket_no_execute;\
size_t ket_seed;\
char* ket_kqasm_path;\
char* ket_plugin_path;

void ket_quant_delete(ket_quant*);
ket_quant* ket_quant_at(ket_quant*, size_t);
void ket_future_delete(ket_future*);
int64_t ket_future_get(ket_future*);

ket_process* ket_process_new();
void ket_process_delete(ket_process*);

ket_label* ket_label_new(const char*, ket_process*);
void ket_label_delete(ket_label*);
void ket_label_begin(ket_label*);

void ket_args(int argc, char* argv[]);

void ket_jump(ket_label*);
void ket_branch(ket_future*, ket_label*, ket_label*);

void ket_ctrl_begin(ket_quant*);
void ket_ctrl_end(ket_quant*);

void ket_adj_begin(ket_process*);
void ket_adj_end(ket_process*);

void ket_ctrl_adj_begin(ket_quant*);
void ket_ctrl_adj_end(ket_quant*);

ket_quant* ket_alloc(size_t size, ket_process*);
ket_quant* ket_alloc_dirty(size_t size, ket_process*);

void ket_free(ket_quant*);
void ket_free_dirty(ket_quant*);

ket_future* ket_measure(ket_quant*, bool wait);
void ket_wait(ket_process*);

void ket_x(ket_quant*);
void ket_y(ket_quant*);
void ket_z(ket_quant*);
void ket_h(ket_quant*);
void ket_s(ket_quant*);
void ket_t(ket_quant*);
void ket_u1(double lambda, ket_quant*);
void ket_u2(double phi, double lambda, ket_quant*);
void ket_u3(double theta, double phi, double lambda, ket_quant*);

void ket_dump(ket_quant*);

void ket_if_then(ket_future*, void (*)(void), void (*)(void), ket_process*);

#ifdef __cplusplus
}
#endif