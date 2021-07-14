#include <ket>

using namespace ket;


int main() {
  
    quant q{1};
    RX(-M_PI_2, q);
    measure(q).get();
    
    return 0;   
}