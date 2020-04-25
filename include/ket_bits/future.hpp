#pragma once
#include "base.hpp"

namespace ket::base {
    class future {
    public:
        future(const std::shared_ptr<i64>& bits);

        const std::shared_ptr<i64>& get_base_i64() const;

        std::int64_t get();

    private:
        std::shared_ptr<i64> bits;
    };
}