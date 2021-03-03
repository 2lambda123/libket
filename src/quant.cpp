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

quant::quant(const std::vector<size_t> &qubits, 
             const std::vector<bool> &indirect_ref, 
             const std::shared_ptr<bool>& ps_ot, 
             const std::shared_ptr<process>& ps) :
    qubits{qubits},
    indirect_ref{indirect_ref},
    process_on_top{ps_ot},
    ps{ps}
    {} 

quant::quant(size_t size) :
    qubits{process_stack.top()->quant(size, false)},
    indirect_ref{std::vector<bool>(size, false)},
    process_on_top{process_on_top_stack.top()},
    ps{process_stack.top()}
    {} 
    
quant quant::dirty(size_t size) {
    return quant{process_stack.top()->quant(size, true),
                 std::vector<bool>(size, false),
                 process_on_top_stack.top(),
                 process_stack.top()};
}

quant quant::operator()(int idx) const {
    if (idx < 0) idx = len() + idx;

    if (size_t(idx) >= len()) 
        throw std::out_of_range("qubit index out of bounds");

    return quant{{{qubits[idx]}}, {{indirect_ref[idx]}}, process_on_top, ps};
}

quant quant::operator()(int start, int end, int step) const {
    if (start < 0) start = len() + start;
    if (end < 0) end = len() + end;
    
    if (start < 0 or size_t(end) > len())
        throw std::out_of_range("qubits range out of bounds");
    if (start >= end)
        throw std::runtime_error("empty quant are not allowed");

    std::vector<size_t> ret_qubits;
    std::vector<bool> tmp_ref;
    for (int i = start; i < end; i += step) {
        ret_qubits.push_back(qubits[i]);
        tmp_ref.push_back(indirect_ref[i]);
    }
    return quant{ret_qubits, tmp_ref, process_on_top, ps};    
}

quant quant::operator()(const future &idx) const {
    if (process_on_top != idx.process_on_top)
        throw std::runtime_error("cannot index qubit with a future from a different process");
    
    for (bool i : indirect_ref) if (i) 
        throw std::runtime_error("cascading indirect references is not allowed");

    return quant{{idx.get_id()}, {{true}}, process_on_top, ps};
}

quant quant::operator|(const quant& other) const {
    if (ps != other.ps)
        throw std::runtime_error("cannot concatenate quant from different process");

    auto tmp_qubits = qubits;
    for (auto i : other.qubits)
        tmp_qubits.push_back(i);

    return quant{tmp_qubits, 
                 std::vector<bool>(tmp_qubits.size(), false),
                 process_on_top, ps};
}

quant quant::inverted() const {
    if (not *process_on_top) 
        throw std::runtime_error("process out of scope");
        
    return quant{std::vector<size_t>(qubits.rbegin(), qubits.rend()),
                 std::vector<bool>(indirect_ref.rbegin(), indirect_ref.rend()),
                 process_on_top, ps};
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

bool quant::is_free() const {
    for (auto i : qubits)
        if (not ps->is_free(i)) 
            return false;
    
    return true;
}