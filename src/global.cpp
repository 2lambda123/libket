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
    process_stack.top()->add_inst("JUMP\t@" + label_name.name); 
}

void ket::branch(const future& cond, const label& label_true, const label& label_false) {
    if (not *(cond.process_on_top) or not *(label_true.process_on_top) or not *(label_false.process_on_top))
        throw std::runtime_error("process out of scope");
    
    process_stack.top()->add_inst("BR\ti" + std::to_string(cond.get_id()) + "\t@" + label_true.name + "\t@" + label_false.name); 
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

void ket::plugin(const std::string& name, const quant& q, const std::string& args) {
    if (not *(q.process_on_top))
        throw std::runtime_error("process out of scope");
    
    process_stack.top()->add_plugin(name, q.qubits, args);
}
