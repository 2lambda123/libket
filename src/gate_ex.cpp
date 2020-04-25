#include "../include/ket_bits/gates_ex.hpp"

using namespace ket::base;

void ket::pown(size_t n, size_t a, quant x, quant fx) {
    std::stringstream args;
    args << n << " " << a;
    x.get_ps()->add_plugin_gate("ket_pown", (x|fx).get_base_qubits(), args.str());
}