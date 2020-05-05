#include "../include/ket"
#include "../include/ket_bits/future.hpp"
#include "../include/ket_bits/base.hpp"

using namespace ket;

void ket::if_then(future cond, std::function<void(void)> then, std::function<void(void)> otherwise, process& ps) {
    label then_l{"if.then.", ps}; 
    label else_l{"if.else.", ps}; 
    label end_l{"if.end.", ps}; 

    if (otherwise) {
        branch(cond, then_l, else_l);
    } else {
        branch(cond, then_l, end_l);
    }

    then_l.begin();
    then();
    jump(end_l);

    if (otherwise) {
        else_l.begin();
        otherwise();
        jump(end_l);
    }

    end_l.begin();

}
