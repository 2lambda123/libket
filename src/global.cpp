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

void ket::process_begin() {
    process_stack.push(std::make_shared<process>());
    *(process_on_top_stack.top()) = false;
    process_on_top_stack.push(std::make_shared<bool>(true));
}

void ket::process_end() {
    process_stack.pop();
    *(process_on_top_stack.top()) = false;
    process_on_top_stack.pop();
    *(process_on_top_stack.top()) = true;
}

void ket::jump(const label& label_name) {
    if (not *(label_name.process_on_top))
        throw std::runtime_error("process out of scope");
    process_stack.top()->add_inst("\tJUMP\t@" + label_name.name); 
}

void ket::branch(const future& cond, const label& label_true, const label& label_false) {
    if (not cond.on_top() or not *(label_true.process_on_top) or not *(label_false.process_on_top))
        throw std::runtime_error("process out of scope");
    
    process_stack.top()->add_inst("\tBR\ti" + std::to_string(cond.get_id()) + "\t@" + label_true.name + "\t@" + label_false.name); 
}

void ket::ctrl_begin(const quant& q) {
    if (not *(q.process_on_top))
        throw std::runtime_error("process out of scope");
    process_stack.top()->ctrl_begin(q.qubits);
}

void ket::ctrl_end() {
    process_stack.top()->ctrl_end();
}

void ket::adj_begin() {
    process_stack.top()->adj_begin();
}

void ket::adj_end() {
    process_stack.top()->adj_end();
}

future ket::measure(const quant& q) {
    if (not *(q.process_on_top))
        throw std::runtime_error("process out of scope");

    auto [id, result, available] = process_stack.top()->measure(q.qubits); 

    return future{id, result, available};
}
