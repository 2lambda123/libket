#include "../include/ket"
#include <iostream>

ket_main(
    
)

int main() {
    auto h = ket::base::handler{};
    h.begin_block("entry");
        auto a = h.alloc();
        auto aux = h.alloc();
        auto b = h.alloc();

        // Bell
        h.add_gate(ket::base::gate::H, aux);
        h.ctrl_begin({aux});        
            h.add_gate(ket::base::gate::X, b);
        h.ctrl_end();

        h.ctrl_begin({a});
            h.add_gate(ket::base::gate::X, aux);
        h.ctrl_end();
        
        h.add_gate(ket::base::gate::H, a);

        auto c1 = h.measure(a);
        auto c2 = h.measure(aux);

        auto i1 = h.new_i64({c1});
        auto i2 = h.new_i64({c2});

        h.if_then(i2, [&]{ h.add_gate(ket::base::gate::X, b); });
        h.if_then(i1, [&]{ h.add_gate(ket::base::gate::Z, b); });

        auto c3 = h.measure(b);
    h.end_block("end");
    
    std::stringstream ss;
    c3->eval(ss);
        
    std::cout << ss.str();
}