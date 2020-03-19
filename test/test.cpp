#include "../include/base.hpp"
#include <iostream>

int main() {
    auto h = ket::base::handler{};
    auto q = h.alloc();
    auto q1 = h.alloc();
    h.add_gate(ket::base::gate::H, q);
    h.adj_begin();
    h.adj_begin();
    h.ctrl_begin({q});
        h.add_gate(ket::base::gate::T, q1);
        h.add_gate(ket::base::gate::S, q1);
    h.ctrl_end();
    h.adj_end();
    h.adj_end();
    auto c = h.measure(q);
    std::stringstream ss;
    c->eval(ss);
    std::cout << ss.str();
}