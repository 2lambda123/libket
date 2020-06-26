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

future::future(size_t id,
       const std::shared_ptr<std::int64_t>& result,
       const std::shared_ptr<bool>& available) :
    id{id},
    result{result},
    available{available},
    process_on_top{process_on_top_stack.top()}
    {} 
    
future::future(std::int64_t value) {
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

#define FUTURE_ROP(op, name) future future::op(std::int64_t other) const {\
    if (not *process_on_top)\
        throw std::runtime_error("process out of scope");\
    auto [other_id, other_result, other_available] = process_stack.top()->new_int(other);\
    auto [_id, _result, _available] = process_stack.top()->op_int(other_id, name, id);\
    return future{_id, _result, _available};\
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

std::int64_t future::get() {
    if (*available) return *result;
    if (not *process_on_top)
        throw std::runtime_error("process out of scope");
    
    process_stack.top()->exec();

    process_stack.pop();
    process_on_top_stack.pop();
    *(process_on_top) = false;

    process_stack.push(std::make_shared<process>());
    process_on_top_stack.push(std::make_shared<bool>());

    return *result;    
}

void future::set(const future& other) {
    if (not *process_on_top or not *(other.process_on_top))
        throw std::runtime_error("process out of scope");

    process_stack.top()->add_inst("\tSET\ti" + std::to_string(id) + "\ti" + std::to_string(other.id));
}

bool future::get_id() const {
    return id;
}

bool future::on_top() const {
    return *process_on_top;
}