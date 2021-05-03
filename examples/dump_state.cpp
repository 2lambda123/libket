#include <ket>

using namespace ket;


int main() {
    
    auto state0  = run([]{
        quant q{3};
        H(q);
        S(q(0));
        T(q(1));
        dump state{q};
        state.get();
        return state;
    });

    auto state1  = run([]{
        quant q{3};
        H(q);
        X(q);
        T(q(1));
        S(q(0));
        dump state{q};
        state.get();
        return state;
    });

    std::cout << (state0 == state1? "Equal" : "Diferrent") << " states" << std::endl;;
    
    return 0;   
}