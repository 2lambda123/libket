#include "../include/ket_bits/macros.hpp"

using namespace ket;

void ket::begin() {
    ket_hdl = new ket::process;
    ket_hdl->begin_block("entry");
}

void ket::end() {
    delete ket_hdl;
}
