#pragma once
#include "base.hpp"

namespace ket {

    class iterator;

    class quant {
    public:
        quant(size_t size, bool dirty = false);

        quant operator()(size_t idx) const;

        const std::vector<std::shared_ptr<base::qubit>>& get_base_qubits() const;

        size_t len() const;

        iterator begin();
        iterator end();

    private:
        quant(const std::vector<std::shared_ptr<base::qubit>> &qubits);

        std::vector<std::shared_ptr<base::qubit>> qubits;

        friend class iterator;
    };

    class iterator {
    public:
        quant operator*();

        const iterator& operator++();

        bool operator!=(const iterator& other) const;
        
    private:
        iterator(const std::vector<std::shared_ptr<base::qubit>> &qubits);
        iterator();

        const std::vector<std::shared_ptr<base::qubit>> *qubits;
        size_t index;
        friend class quant;
    };
}