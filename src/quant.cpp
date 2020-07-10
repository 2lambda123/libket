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

using namespace ket;

quant::quant(const std::vector<size_t> &qubits) :
    qubits{qubits},
    process_on_top{process_on_top_stack.top()}
    {} 

quant::quant(size_t size) :
    qubits{process_stack.top()->quant(size, false)},
    process_on_top{process_on_top_stack.top()}
    {} 
    
quant quant::dirty(size_t size) {
    return quant{process_stack.top()->quant(size, true)};
}

quant quant::operator()(int idx) const {
    if (not *process_on_top) 
        throw std::runtime_error("process out of scope");

    if (idx < 0) idx = len() + idx;

    return quant{{{qubits[idx]}}};
}

quant quant::__getitem__(int idx) const {
    return (*this)(idx);
}

quant quant::operator|(const quant& other) const {
    if (not *process_on_top) 
        throw std::runtime_error("process out of scope");

    auto tmp_qubits = qubits;
    for (auto i : other.qubits)
        tmp_qubits.push_back(i);

    return quant{tmp_qubits};
}

quant quant::invert() const {
    if (not *process_on_top) 
        throw std::runtime_error("process out of scope");

    std::vector<size_t> tmp_qubits;
    for (auto i = qubits.rbegin(); i != qubits.rend(); ++i) 
        tmp_qubits.push_back(*i);
    
    return quant{tmp_qubits};
}

quant quant::__reversed__() const {
    return invert();
}

size_t quant::len() const {
    return qubits.size();
}

size_t quant::__len__() const {
    return len();
}

void quant::free(bool dirty) const {
    if (not *process_on_top) 
        throw std::runtime_error("process out of scope");

    for (auto i : qubits)
        process_stack.top()->free(i, dirty);
}
