#include "../include/ket_bits/macros.hpp"

using namespace ket;

void ket::begin() {
    ket_hdl.begin_block("entry");
}

void ket::end() {
    ket_hdl.end_block("end");
}
