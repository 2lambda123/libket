/* MIT License
 * 
 * Copyright (c) 2020 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright (c) 2020 Rafael de Santiago <r.santiago@ufsc.br>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

ket_quant* ket_qalloc(size_t size, ket_process*);
ket_quant* ket_qalloc_dirty(size_t size, ket_process*);

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
