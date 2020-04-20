#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/future.hpp"

using namespace ket;

future::future(const std::shared_ptr<base::i64>& bits) : 
    bits{bits} 
    {}

const std::shared_ptr<base::i64>& future::get_base_i64() const {
    return bits;
}

std::string future::get() {
    auto next_label = "entry"+std::to_string(ket_hdl.get_label_count());
    ket_hdl.end_block(next_label);
    std::stringstream ss;
    bits->eval(ss);
    ket_hdl.begin_block(next_label);
    return ss.str();
}
