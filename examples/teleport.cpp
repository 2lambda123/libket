#include <ket/libket.hpp>
#include "print_dump.hpp"

int main() {
    ket::libket::process_t ps{0};

    auto alice_q      = ps.alloc();
    auto alice_bell_q = ps.alloc();
    auto bob_bell_q   = ps.alloc();

    ps.gate(ket::libket::gate_t::hadamard , alice_q);
    ps.gate(ket::libket::gate_t::pauli_z, alice_q);
  
    auto bell = [&ps](auto a, auto b) {
        ps.gate(ket::libket::gate_t::hadamard, a);
        ps.ctrl_push({a});
        ps.gate(ket::libket::gate_t::pauli_x, b);
        ps.ctrl_pop();
    };

    bell(alice_bell_q, bob_bell_q);

    ps.adj_begin();
    bell(alice_q, alice_bell_q);
    ps.adj_end();
    
    auto m0 = ps.measure({alice_q});
    auto m1 = ps.measure({alice_bell_q});

    auto if_ = [&ps](auto test, auto then) {
        auto then_label = ps.get_label();
        auto end_label = ps.get_label();
        ps.branch(test, then_label, end_label);
        ps.open_block(then_label);
        then();
        ps.jump(end_label);
        ps.open_block(end_label); 
    };

    if_(m1, [&]{
        ps.gate(ket::libket::gate_t::pauli_x, bob_bell_q);
    });

    if_(m0, [&]{
        ps.gate(ket::libket::gate_t::pauli_z, bob_bell_q);
    });

    ps.gate(ket::libket::gate_t::hadamard, bob_bell_q);


    auto dump = ps.dump({alice_q, alice_bell_q, bob_bell_q});
    auto result = ps.measure({bob_bell_q});

    std::cout << "Teleport: result=" << result.value()  << "; expected=1" << std::endl;
    std::cout << "Quantum state:" << std::endl;
    print_dump<3>(dump);

    return 0;
}
