#pragma once
#include "base.hpp"

namespace ket {

    class qubit_iterator;

    class quant {
    public:
        quant(size_t size, bool dirty, process *ps);

        quant operator()(size_t idx) const;

        quant operator|(const quant& other) const;

        quant invert() const;

        const std::vector<std::shared_ptr<base::qubit>>& get_base_qubits() const;

        size_t len() const;

        qubit_iterator begin() const;
        qubit_iterator end() const;

        process* get_ps() const;

    private:
        quant(const std::vector<std::shared_ptr<base::qubit>> &qubits, process *ps);

        process *ps;
        
        std::vector<std::shared_ptr<base::qubit>> qubits;

        friend class qubit_iterator;
    };

    class qubit_iterator {
    public:
        quant operator*();

        const qubit_iterator& operator++();

        bool operator!=(const qubit_iterator& other) const;
        
    private:
        qubit_iterator(const std::vector<std::shared_ptr<base::qubit>> &qubits, process* ps);
        qubit_iterator(size_t size);

        process *ps;
        const std::vector<std::shared_ptr<base::qubit>> *qubits;
        size_t index;
        friend class quant;
    };
}