#include "../include/base.hpp"
#include <iostream>


int main() {
    auto h = ket::base::handler{};

    h.begin_block("start");
        auto q0 = h.alloc();
        auto q1 = h.alloc();

        h.add_gate(ket::base::gate::H, q0);
        h.add_gate(ket::base::gate::H, q1);

        auto c0 = h.measure(q0);

        auto i0 = h.new_i64({c0});

        h.if_then_else(i0, 
                    [&]{ h.add_gate(ket::base::gate::S, q1); },
                    [&]{ h.add_gate(ket::base::gate::T, q1); });

        auto c1 = h.measure(q1);
    h.end_block("end");

    std::stringstream ss;

    c1->eval(ss);

    std::cout << ss.str();

}