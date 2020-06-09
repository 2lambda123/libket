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

#pragma once
#include "base.hpp"

namespace ket {

    class qubit_iterator;

    class _quant {
    public:
        _quant(size_t size, bool dirty, base::_process *ps);

        _quant operator()(size_t idx) const;

        _quant operator|(const _quant& other) const;

        _quant invert() const;

        const std::vector<std::shared_ptr<base::qubit>>& get_base_qubits() const;

        size_t len() const;

        qubit_iterator begin() const;
        qubit_iterator end() const;

        base::_process* get_ps() const;

    private:
        _quant(const std::vector<std::shared_ptr<base::qubit>> &qubits, base::_process *ps);

        base::_process *ps;
        
        std::vector<std::shared_ptr<base::qubit>> qubits;

        friend class qubit_iterator;
    };

    class qubit_iterator {
    public:
        _quant operator*();

        const qubit_iterator& operator++();

        bool operator!=(const qubit_iterator& other) const;
        
    private:
        qubit_iterator(const std::vector<std::shared_ptr<base::qubit>> &qubits, base::_process* ps);
        qubit_iterator(size_t size);

        base::_process *ps;
        const std::vector<std::shared_ptr<base::qubit>> *qubits;
        size_t index;
        friend class _quant;
    };
}
