#include "../include/ket"
#include <iostream>

ket_main(

    ket::quant a{1};
    ket::quant aux{1};
    ket::quant b{1};

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
    ) ELSE (
        IF (m2) THEN (
            ket::s(b);
        ) END;
    )

    auto c = ket::measure(b);

    std::cout << c.get();
)
