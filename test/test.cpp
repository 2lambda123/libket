#include "../include/ket"
#include <iostream>

ket_main(
    ket::_quant a{1};
    ket::_quant aux{1};
    ket::_quant b{1};

    ket::h(aux);
    ket::ctrl(aux, ket::x, b);
    
    ket::ctrl(a, ket::x, aux);

    ket::h(a);

    auto m1 = ket::measure(a);
    auto m2 = ket::measure(aux);

    IF (m1) THEN (
        ket::x(b);
    ) END;

    IF (m2) THEN (
        ket::z(b);
    ) END; 

    auto c = ket::measure(b);

    std::cout << c.get();
)
