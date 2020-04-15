#pragma once
#include "base.hpp"

namespace ket {
    class quant {
    public:
        quant(size_t size, bool dirty = false);

        const std::vector<std::shared_ptr<base::qubit>>& get_base_qubits() const;

        size_t len() const;

    private:
        std::vector<std::shared_ptr<base::qubit>> qubits;
    };
}