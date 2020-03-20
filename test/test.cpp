#include "../include/base.hpp"
#include <iostream>

int main() {
    auto h = ket::base::handler{};
    h.begin_block("entry");
        auto q = h.alloc();
        auto q1 = h.alloc();
        auto q2 = h.alloc();
        auto q3 = h.alloc();

        h.add_gate(ket::base::gate::H, q);
        h.add_gate(ket::base::gate::H, q2);
        h.add_gate(ket::base::gate::H, q3);

        auto c2 = h.measure(q2);
        auto i2 = h.new_i64({c2});

        h.if_then(i2, [&]() {
            h.adj_begin();
                h.ctrl_begin({q});
                    h.add_gate(ket::base::gate::T, q1);
                    h.add_gate(ket::base::gate::S, q1);
                h.ctrl_end();
            h.adj_end();
        });

        auto c = h.measure(q);
        auto c1 = h.measure(q1);
        auto i = h.new_i64({c, c1});
    h.end_block("and");
    std::stringstream ss;
    i->eval(ss);
    std::cout << ss.str();
}