#include "../include/base.hpp"
#include <iostream>

int main() {
    auto h = ket::base::handler{};
    h.begin_block("entry");
        auto qq1 = h.alloc();
        auto qq3 = h.alloc();


        auto cc1 = h.measure(qq1);
        auto cc3 = h.measure(qq3);

        h.add_gate(ket::base::gate::X, qq1);

        auto ii1 = h.new_i64({cc1}); 
        auto ii2 = h.new_i64({cc3}); 

        auto ii3 = h.i64_op("+", {ii1, ii2});
    h.end_block("end");

    std::stringstream ss;
    ii3->eval(ss);
        
    std::cout << ss.str();
}