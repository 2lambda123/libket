#include <ket/libket/process.hpp>
#include "print_dump.hpp"

int main() {
    ket::libket::process_t ps{0};

    auto a = ps.alloc();
    auto b = ps.alloc();

    auto bell = [&ps](auto a, auto b) {
        ps.gate(ket::libket::gate_t::hadamard, a);
        ps.ctrl_push({a});
        ps.gate(ket::libket::gate_t::pauli_x, b);
        ps.ctrl_pop();
    };

    bell(a, b);

    auto bell_dump = ps.dump({a, b});
    auto result = ps.measure({a, b});

    std::cout << "Bell: result=" << result.value()  << "; expected=0or3" << std::endl;
    std::cout << "Bell state:" << std::endl;
    print_dump<2>(bell_dump);
    
    return 0;
}
