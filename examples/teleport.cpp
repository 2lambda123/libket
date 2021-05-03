#include <ket>

using namespace ket;

quant bell(bool aux0, bool aux1) {
    quant q{2};
    if (aux0) 
        X(q(0));
    if (aux1)
        X(q(1));
    H(q(0));
    ctrl(q(0), X, q(1));
    return q;
}

quant teleport(quant a) {
    quant b = bell(0,0);
    ctrl(a, X, b(0));
    H(a);
    auto m0 = measure(a);
    auto m1 = measure(b(0));

    qc_if(m1 == 1, [&]{
        X(b(1));
    });
    
    qc_if(m0 == 1, [&]{
        Z(b(1));
    });

    return b(1);
}

int main() {
    quant a{1};
    H(a);
    Z(a);
    auto y = teleport(a);
    H(y);
    std::cout << measure(y).get() << std::endl;

    return 0;
}

