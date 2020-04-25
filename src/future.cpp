#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/future.hpp"

using namespace ket;
using namespace ket::base;

future::future(const std::shared_ptr<i64>& bits, process* ps) :
    ps{ps},
    bits{bits} 
    {}

const std::shared_ptr<i64>& future::get_base_i64() const {
    return bits;
}

std::int64_t future::get() {
    if (not bits->has_value()) { 
        ps->eval();
        new (ps) process;
    }

    return bits->get_value();
}

process* future::get_ps() const {
    return ps;
}