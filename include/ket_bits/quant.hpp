#pragma once
#include "base.hpp"

namespace ket::base {

    class qubit_iterator;

    class quant {
    public:
        quant(size_t size, bool dirty = false);

        quant operator()(size_t idx) const;

        quant operator|(const quant& other) const;

        quant invert() const;

        const std::vector<std::shared_ptr<qubit>>& get_base_qubits() const;

        size_t len() const;

        qubit_iterator begin();
        qubit_iterator end();

    private:
        quant(const std::vector<std::shared_ptr<qubit>> &qubits);

        std::vector<std::shared_ptr<qubit>> qubits;

        friend class qubit_iterator;
    };

    class qubit_iterator {
    public:
        quant operator*();

        const qubit_iterator& operator++();

        bool operator!=(const qubit_iterator& other) const;
        
    private:
        qubit_iterator(const std::vector<std::shared_ptr<qubit>> &qubits);
        qubit_iterator(size_t size);

        const std::vector<std::shared_ptr<qubit>> *qubits;
        size_t index;
        friend class quant;
    };
}