#include "../include/ket"
#include "../include/ket_bits/quant.hpp"

using namespace ket;

quant ket::pown(size_t a, quant _x, size_t n) {
    auto x = *static_cast<_quant*>(_x.quant_ptr.get());
    _quant fx{x.len(), false, x.get_ps()};
    std::stringstream args;
    args << n << " " << a;
    x.get_ps()->add_plugin_gate("ket_pown", (x|fx).get_base_qubits(), args.str());
    return quant{{new _quant{fx}, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}