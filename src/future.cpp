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

future::future(size_t id,
       const std::shared_ptr<std::int64_t>& result,
       const std::shared_ptr<bool>& available) :
    process_on_top{process_on_top_stack.top()},
    id{id},
    result{result},
    available{available}
    {} 
    
future::future(std::int64_t value) :
    process_on_top{process_on_top_stack.top()}
{
    auto [_id, _result, _available] = process_stack.top()->new_int(value);
    id = _id;
    result = _result;
    available = _available;
}

#define FUTURE_OP(op, name) future future::operator op(const future& other) const {\
    if (not *process_on_top or not *(other.process_on_top))\
        throw std::runtime_error("process out of scope");\
    auto [_id, _result, _available] = process_stack.top()->op_int(id, name, other.id);\
    return future{_id, _result, _available};\
}\
future future::operator op(std::int64_t other) const {\
    if (not *process_on_top)\
        throw std::runtime_error("process out of scope");\
    auto [other_id, other_result, other_available] = process_stack.top()->new_int(other);\
    auto [_id, _result, _available] = process_stack.top()->op_int(id, name, other_id);\
    return future{_id, _result, _available};\
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

std::int64_t future::get() {
    if (*available) return *result;
    if (not *process_on_top)
        throw std::runtime_error("process out of scope");

    exec_quantum();    

    return *result;    
}

void future::set(const future& other) {
    if (not *process_on_top or not *(other.process_on_top))
        throw std::runtime_error("process out of scope");

    process_stack.top()->add_inst("SET\ti" + std::to_string(id) + "\ti" + std::to_string(other.id));
}

size_t future::get_id() const {
    return id;
}
