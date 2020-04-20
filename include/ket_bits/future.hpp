#pragma once
#include "base.hpp"

namespace ket {
    class future {
    public:
        future(const std::shared_ptr<base::i64>& bits);

        const std::shared_ptr<base::i64>& get_base_i64() const;

        std::string get();

    private:
        std::shared_ptr<base::i64> bits;
    };
}