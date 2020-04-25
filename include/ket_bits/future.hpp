#pragma once
#include "base.hpp"

namespace ket {
    class future {
    public:
        future(const std::shared_ptr<base::i64>& bits, process* ps);

        const std::shared_ptr<base::i64>& get_base_i64() const;

        std::int64_t get();

    private:
        process *ps;

        std::shared_ptr<base::i64> bits;
    };
}