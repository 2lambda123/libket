#include <ket>

using namespace ket;


int main() {
    
    auto state0  = run([]{
        quant q{3};
        h(q);
        s(q(0));
        t(q(1));
        dump state{q};
        state.get();
        return state;
    });

    auto state1  = run([]{
        quant q{3};
        h(q);
        x(q);
        t(q(1));
        s(q(0));
        dump state{q};
        state.get();
        return state;
    });

    std::cout << (state0 == state1? "Equal" : "Diferrent") << " states" << std::endl;;
    
    return 0;   
}