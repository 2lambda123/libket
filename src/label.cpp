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

label::label(const std::string& name) :
    name{name + std::to_string(process_stack.top()->new_label_id())},
    process_on_top{process_on_top_stack.top()},
    placed{false}
    {}

void label::begin() {
    if (not *process_on_top)
        throw std::runtime_error("process out of scope");
    if (placed)
        throw std::runtime_error("cannot place a label twice");

    placed = true;

    process_stack.top()->add_label(name);
}
