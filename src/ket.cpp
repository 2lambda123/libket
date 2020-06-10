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
#include "../include/ket_bits/quant.hpp"
#include "../include/ket_bits/future.hpp"
#include "../include/ket_bits/base.hpp"
#include <iostream>

using namespace ket;

quant::quant(const std::shared_ptr<void> &quant_ptr) : quant_ptr{quant_ptr} {}

quant quant::operator()(size_t idx) const {
    auto *ptr = new ket::_quant{(*static_cast<ket::_quant*>(quant_ptr.get()))(idx)};
    return quant{{ptr, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}

quant quant::operator|(quant other) const {
    auto *ptr = new ket::_quant{(*static_cast<ket::_quant*>(quant_ptr.get()))|(*static_cast<ket::_quant*>(other.quant_ptr.get()))};
    return quant{{ptr, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}

quant quant::invert() const {
    auto *ptr = new ket::_quant{static_cast<ket::_quant*>(quant_ptr.get())->invert()};
    return quant{{ptr, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}

size_t quant::len() const {
    return static_cast<ket::_quant*>(quant_ptr.get())->len();
}

future::future(const std::shared_ptr<void> &future_ptr) : future_ptr{future_ptr} {}

int64_t future::get() {
    return static_cast<ket::_future*>(future_ptr.get())->get();
}

#define FUTURE_OP(op, name) future future::operator op(const future& other) const {\
    auto *fl = static_cast<_future*>(future_ptr.get());\
    auto *fr = static_cast<_future*>(other.future_ptr.get());\
    auto ps = fl->get_ps()? fl->get_ps() : fr->get_ps();\
    return future{{new _future{ps->i64_op(name, {fl->get_base_i64(), fr->get_base_i64()}), ps},\
                   [](auto ptr){ delete static_cast<_future*>(ptr); } }};\
}\
future future::operator op(std::int64_t other) const {\
    auto *fl = static_cast<_future*>(future_ptr.get());\
    auto ps = fl->get_ps();\
    auto fr = ps->const_i64(other);\
    return future{{new _future{ps->i64_op(name, {fl->get_base_i64(), fr}), ps},\
                   [](auto ptr){ delete static_cast<_future*>(ptr); } }};\
}

FUTURE_OP(==, "==")
FUTURE_OP(!=, "!=")
FUTURE_OP(<, "<")
FUTURE_OP(<=, "<=")
FUTURE_OP(>, ">")
FUTURE_OP(>=, ">=")

FUTURE_OP(+, "+")
FUTURE_OP(-, "-")
FUTURE_OP(*, "*")
FUTURE_OP(/, "/")
FUTURE_OP(<<, "<<")
FUTURE_OP(>>, ">>")
FUTURE_OP(&, "and")
FUTURE_OP(^, "xor")
FUTURE_OP(|, "or")

#define FUTURE_ROP(op, name) future future::op(std::int64_t other) const {\
    auto *fr = static_cast<_future*>(future_ptr.get());\
    auto ps = fr->get_ps();\
    auto fl = ps->const_i64(other);\
    return future{{new _future{ps->i64_op(name, {fl, fr->get_base_i64()}), ps},\
                   [](auto ptr){ delete static_cast<_future*>(ptr); } }};\
}

FUTURE_ROP(__radd__, "+")
FUTURE_ROP(__rsub__, "-")
FUTURE_ROP(__rmul__, "*")
FUTURE_ROP(__rtruediv__, "/")
FUTURE_ROP(__rlshift__, "<<")
FUTURE_ROP(__rrshift__, ">>")
FUTURE_ROP(__rand__, "and")
FUTURE_ROP(__rxor__, "xor")
FUTURE_ROP(__ror__, "or")

process::process() : ps{new base::_process, [](auto ptr){ delete static_cast<base::_process*>(ptr); }} {} 

label::label(const std::string& label_name, process& _ps) : ps{_ps}, name{label_name+std::to_string(count++)} {} 

label::operator std::string() const {
    return name;
}

void label::begin() {
    auto psb = static_cast<base::_process*>(ps.ps.get());
    psb->begin_block(name);
}

void ket::jump(label& label_name) {
    auto ps = static_cast<base::_process*>(label_name.ps.ps.get());
    ps->end_block(label_name);
}

void ket::branch(future _cond, label& label_true, label& label_false) {
    auto ps = static_cast<base::_process*>(label_true.ps.ps.get());
    auto cond = static_cast<ket::_future*>(_cond.future_ptr.get());

    ps->end_block(label_true, label_false, cond->get_base_i64());
}

void ket::ctrl_begin(quant _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->ctrl_begin(q->get_base_qubits());
}

void ket::ctrl_end(quant _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->ctrl_end();
}

void ket::adj_begin(process &_ps) {
    auto ps = static_cast<base::_process*>(_ps.ps.get());
    ps->adj_begin();
}

void ket::adj_end(process &_ps) {
    auto ps = static_cast<base::_process*>(_ps.ps.get());
    ps->adj_begin();
}

void ket::ctrl_adj_begin(quant _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->ctrl_begin(q->get_base_qubits());
    q->get_ps()->adj_begin();
}

void ket::ctrl_adj_end(quant _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->adj_end();
    q->get_ps()->ctrl_end();
}
