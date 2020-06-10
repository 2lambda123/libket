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

using namespace ket;
using namespace ket::base;

_quant::_quant(size_t size, bool dirty, _process* ps) :
    ps{ps}
{
    for (size_t i = 0; i < size; i++) 
        qubits.push_back(ps->alloc(dirty));
}

_quant::_quant(const std::vector<std::shared_ptr<qubit>>& qubits, _process *ps) :
    ps{ps},
    qubits{qubits}
    {}

_quant _quant::operator()(size_t idx) const {
    return _quant{{qubits[idx]}, ps};
}

_quant _quant::operator|(const _quant& other) const {
    auto qubits_ptr = qubits;
    qubits_ptr.insert(qubits_ptr.end(), other.qubits.begin(), other.qubits.end());
    return _quant{qubits_ptr, ps};
}

_quant _quant::invert() const {
    std::vector<std::shared_ptr<qubit>> invert_qubits;
    invert_qubits.insert(invert_qubits.end(), qubits.rbegin(), qubits.rend());
    return _quant{invert_qubits, ps};
}

const std::vector<std::shared_ptr<qubit>>& _quant::get_base_qubits() const {
    return qubits;
}

size_t _quant::len() const {
    return qubits.size();
}

_process* _quant::get_ps() const {
    return ps;
}

qubit_iterator _quant::begin() const {
    return qubit_iterator{qubits, ps};
}

qubit_iterator _quant::end() const {
    return qubit_iterator{qubits.size()};
}

qubit_iterator::qubit_iterator(const std::vector<std::shared_ptr<qubit>> &qubits, _process* ps) :
    ps{ps},
    qubits{&qubits},
    index{0}
    {}
    
qubit_iterator::qubit_iterator(size_t size) : 
    qubits{nullptr},
    index{size}
    {}

_quant qubit_iterator::operator*() {
    return _quant{{qubits->at(index)}, ps};
}

const qubit_iterator& qubit_iterator::operator++() {
    index++;
    return *this;
}

bool qubit_iterator::operator!=(const qubit_iterator& other) const {
    return index != other.index;
}

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

size_t quant::__len__() const {
    return static_cast<ket::_quant*>(quant_ptr.get())->len();
}
