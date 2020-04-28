#pragma once
#include "base.hpp"

namespace ket {
    class _future {
    public:
        _future(const std::shared_ptr<base::i64>& bits, base::_process* ps);

        const std::shared_ptr<base::i64>& get_base_i64() const;

        std::int64_t get();

        base::_process* get_ps() const;

    private:
        base::_process *ps;

        std::shared_ptr<base::i64> bits;
    };
}