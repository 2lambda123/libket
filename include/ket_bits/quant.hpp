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