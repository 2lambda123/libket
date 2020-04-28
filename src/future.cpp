#include "../include/ket_bits/future.hpp"

using namespace ket;
using namespace ket::base;

_future::_future(const std::shared_ptr<i64>& bits, _process* ps) :
    ps{ps},
    bits{bits} 
    {}

const std::shared_ptr<i64>& _future::get_base_i64() const {
    return bits;
}

std::int64_t _future::get() {
    if (not bits->has_value()) { 
        ps->eval();
        new (ps) _process;
    }

    return bits->get_value();
}

_process* _future::get_ps() const {
    return ps;
}