#include <ket>

using namespace ket;

quant bell(bool aux0, bool aux1) {
    quant q{2};
    if (aux0) 
        x(q(0));
    if (aux1)
        x(q(1));
    h(q(0));
    ctrl(q(0), x, q(1));
    return q;
}

quant teleport(quant a) {
    quant b = bell(0,0);
    ctrl(a, x, b(0));
    h(a);
    auto m0 = measure(a);
    auto m1 = measure(b(0));

    qc_if(m1 == 1, [&]{
        x(b(1));
    });
    
    qc_if(m0 == 1, [&]{
        z(b(1));
    });

    return b(1);
}

int main() {
    quant a{1};
    h(a);
    z(a);
    auto y = teleport(a);
    h(y);
    std::cout << measure(y).get() << std::endl;

    return 0;
}

