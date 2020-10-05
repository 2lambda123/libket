#include <ket>
#include <cmath>
#include <algorithm>  

using namespace ket;

void qft(quant q) {
    auto lambda = [](double k) {
        return M_PI*k/2.0;
    };

    for (size_t i = 0; i < q.len(); i++) {
        for (size_t j = 0; j < i; j++) {
            ctrl(q(i), u1, lambda(i-j), q(j));
        }
        h(q(i));
    }
}

int main() {
    quant reg1{4};

    h(reg1);

    auto reg2 = plugin::pown(7, reg1, 15);
    
    dump after_pown{reg1|reg2};

    qft(reg1);

    dump result{reg1.inverted()};
    
    std::cout << "|x⟩|aˣ mod 15⟩:" << std::endl
              << "==============" << std::endl;
    std::cout << after_pown.show("i4:b4");

    std::cout << "Result:" << std::endl
              << "=======" << std::endl;
    std::cout << result.show();

    auto r = result.get_states()[0];

    for (auto i : result.get_states()) 
        r = std::__gcd(r, i);

    r = std::pow(2, 4)/r;

    std::cout << "measurements = ";
    for (auto i : result.get_states()) std::cout << i << " ";
    std::cout << std::endl << "r = " << r << std::endl;
    auto p = std::__gcd(int(std::pow(7, r/2))+1, 15);
    auto q = std::__gcd(int(std::pow(7, r/2))-1, 15);
    std::cout << 15 << " = " << p << " x " << q << std::endl;

    return 0;
}