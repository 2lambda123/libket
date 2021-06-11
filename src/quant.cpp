/* Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../include/ket"
#include <boost/unordered_set.hpp>

using namespace ket;

quant::quant(const std::vector<size_t> &qubits, const std::shared_ptr<bool>& ps_ot, const std::shared_ptr<process>& ps) :
    qubits{qubits},
    process_on_top{ps_ot},
    ps{ps}
    {} 

quant::quant(size_t size) :
    qubits{process_stack.top()->quant(size, false)},
    process_on_top{process_on_top_stack.top()},
    ps{process_stack.top()}
    {} 
    
quant quant::dirty(size_t size) {
    return quant{process_stack.top()->quant(size, true), process_on_top_stack.top(), process_stack.top()};
}

quant quant::operator()(int idx) const {
    if (idx < 0) idx = len() + idx;

    if (size_t(idx) >= len()) 
        throw std::out_of_range("qubit index out of bounds");

    return quant{{{qubits[idx]}}, process_on_top, ps};
}

quant quant::operator()(int start, int end, int step) const {
    if (start < 0) start = len() + start;
    if (end < 0) end = len() + end;
    
    if (start < 0 or size_t(end) > len())
        throw std::out_of_range("qubits range out of bounds");
    if (start >= end)
        throw std::runtime_error("empty quant are not allowed");

    std::vector<size_t> ret_qubits;
    for (int i = start; i < end; i += step) 
        ret_qubits.push_back(qubits[i]);

    return quant{ret_qubits, process_on_top, ps};    
}

quant quant::operator|(const quant& other) const {
    if (ps != other.ps)
        throw std::runtime_error("cannot concatenate quant of different process");

    boost::unordered_set<size_t> qubits_set{qubits.begin(), qubits.end()}; 

    auto tmp_qubits = qubits;
    for (auto i : other.qubits) {
        if (qubits_set.find(i) != qubits_set.end())
                throw std::runtime_error("quant with two references to the same qubit are not allowed");
        tmp_qubits.push_back(i);
    }
        
    return quant{tmp_qubits, process_on_top, ps};
}

quant quant::inverted() const {
    if (not *process_on_top) 
        throw std::runtime_error("process out of scope");

    std::vector<size_t> tmp_qubits;
    for (auto i = qubits.rbegin(); i != qubits.rend(); ++i) 
        tmp_qubits.push_back(*i);
    
    return quant{tmp_qubits, process_on_top, ps};
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
    if (ps->has_executed()) return true;
    for (auto i : qubits) if (not ps->is_free(i)) return false;
    return true;
}