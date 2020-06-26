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

auto period() {
    quant reg1{4};

    h(reg1);

    auto reg2 = plugin::pown(7, reg1, 15);

    qft(reg1);

    return measure(reg1.invert()).get();
}

int main() {
    auto r = period();
    std::vector<std::int64_t> results = {r};

    for (int i = 0; i < 4; i++) {
        results.push_back(period());
        r = std::__gcd(r, results.back());
    }

    r = std::pow(2, 4)/r;

    std::cout << "measurements = ";
    for (auto i : results) std::cout << i << " ";
    std::cout << std::endl << "r = " << r << std::endl;
    auto p = std::__gcd(int(std::pow(7, r/2))+1, 15);
    auto q = std::__gcd(int(std::pow(7, r/2))-1, 15);
    std::cout << 15 << " = " << p << " x " << q << std::endl;

    return 0;
}