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
#include "../include/ket_bits/future.hpp"

using namespace ket;
using namespace ket::base;

_future::_future(const std::shared_ptr<i64>& bits, _process* ps) :
    ps{ps},
    bits{bits} 
    {}

const std::shared_ptr<i64>& _future::get_base_i64() const {
    return bits;
}

std::int64_t _future::get() {
    if (not bits->has_value()) { 
        ps->eval();
        ps->~_process();
        new (ps) _process;
    }

    return bits->get_value();
}

_process* _future::get_ps() const {
    return ps;
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

future::future(const std::shared_ptr<void> &future_ptr) : future_ptr{future_ptr} {}

int64_t future::get() {
    return static_cast<ket::_future*>(future_ptr.get())->get();
}
