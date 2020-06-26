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

#define GATE(x) void ket::x(const quant& q) {\
    if (not *(q.process_on_top))\
        throw std::runtime_error("process out of scope");\
    for (auto i : q.qubits)\
        process_stack.top()->add_gate(process::x, i);\
}

GATE(x)
GATE(y)
GATE(z)
GATE(h)
GATE(s)
GATE(sd)
GATE(t)
GATE(td)

GATE(dump)

void ket::u1(double lambda, const quant& q) {
    if (not *(q.process_on_top))
        throw std::runtime_error("process out of scope");

    for (auto i : q.qubits)
        process_stack.top()->add_gate(process::u1, i, {lambda});
}

void ket::u2(double phi, double lambda, const quant& q) {
    if (not *(q.process_on_top))
        throw std::runtime_error("process out of scope");
        
    for (auto i : q.qubits)
        process_stack.top()->add_gate(process::u2, i, {phi, lambda});
}

void ket::u3(double theta, double phi, double lambda, const quant& q) {
    if (not *(q.process_on_top))
        throw std::runtime_error("process out of scope");
        
    for (auto i : q.qubits)
        process_stack.top()->add_gate(process::u3, i, {theta, phi, lambda});
}
