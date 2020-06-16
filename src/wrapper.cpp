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

#include "../include/ket"
#include "../include/ket.h"
#include <ctime>
#include <iostream>

using namespace ket;

void ket_quant_delete(ket_quant* q) {
    delete static_cast<quant*>(q);
}

ket_quant* ket_quant_at(ket_quant* q, size_t idx) {
    return new quant{(*static_cast<quant*>(q))(idx)};
}

void ket_future_delete(ket_future* f) {
    delete static_cast<future*>(f);
}

int64_t ket_future_get(ket_future* f) {
    return static_cast<future*>(f)->get();
}

ket_process* ket_process_new() {
    return new process();
}

void ket_process_delete(ket_process* p) {
    delete static_cast<process*>(p);
}

ket_label* ket_label_new(const char* name, ket_process* ps) {
   return new label{name, *static_cast<process*>(ps)};
}

void ket_label_delete(ket_label* l) {
    delete static_cast<label*>(l);
}

void ket_label_begin(ket_label* l) {
    static_cast<label*>(l)->begin();
}

void ket_jump(ket_label* l) {
    jump(*static_cast<label*>(l));
}

void ket_branch(ket_future* c, ket_label* t, ket_label* f) {
    branch(*static_cast<future*>(c), *static_cast<label*>(t), *static_cast<label*>(f));
}

void ket_ctrl_begin(ket_quant* q) {
    ctrl_begin(*static_cast<quant*>(q));
}

void ket_ctrl_end(ket_quant* q) {
    ctrl_end(*static_cast<quant*>(q));
}

void ket_adj_begin(ket_process* p) {
    adj_begin(*static_cast<process*>(p));
}

void ket_adj_end(ket_process* p) {
    adj_end(*static_cast<process*>(p));
}

void ket_ctrl_adj_begin(ket_quant* q) {
    ctrl_adj_begin(*static_cast<quant*>(q));
}

void ket_ctrl_adj_end(ket_quant* q) {
    ctrl_adj_end(*static_cast<quant*>(q));
}

ket_quant* ket_qalloc(size_t size, ket_process* p) {
    return new quant{qalloc(size, *static_cast<process*>(p))};
}

ket_quant* ket_qalloc_dirty(size_t size, ket_process* p) {
    return new quant{qalloc_dirty(size, *static_cast<process*>(p))};
}

void ket_free(ket_quant* q) {
    ket::free(*static_cast<quant*>(q));
}

void ket_free_dirty(ket_quant* q) {
    free_dirty(*static_cast<quant*>(q));
}

ket_future* ket_measure(ket_quant* q, bool wait) {
    return new future{measure(*static_cast<quant*>(q), wait)};
}

void ket_wait(ket_process* p) {
    wait(*static_cast<process*>(p));
}

void ket_x(ket_quant* q) {
    x(*static_cast<quant*>(q));
}

void ket_y(ket_quant* q) {
    y(*static_cast<quant*>(q));
}

void ket_z(ket_quant*  q) {
    z(*static_cast<quant*>(q));
}

void ket_h(ket_quant* q) {
    h(*static_cast<quant*>(q));
}

void ket_s(ket_quant* q) {
    s(*static_cast<quant*>(q));
}

void ket_t(ket_quant* q) {
    t(*static_cast<quant*>(q));
}

void ket_u1(double lambda, ket_quant* q) {
    u1(lambda, *static_cast<quant*>(q));
}

void ket_u2(double phi, double lambda, ket_quant* q) {
    u2(phi, lambda, *static_cast<quant*>(q));
}

void ket_u3(double theta, double phi, double lambda, ket_quant* q) {
    u3(theta, phi, lambda, *static_cast<quant*>(q));
}

void ket_dump(ket_quant* q) {
    dump(*static_cast<quant*>(q));
}

void ket_if_then(ket_future* c, void (*t)(void), void (*e)(void), ket_process* p) {
    if_then(*static_cast<future*>(c), t, e, *static_cast<process*>(p));
}
