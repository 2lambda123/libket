#include <ket>
#include <iostream>

using namespace ket;

int main(int argc, char* argv[]) {

    if (argc == 1) {
        std::cout << "usage: " << argv[0] << " <number of bits>" << std::endl;
        exit(1);
    }

    ket::config("server", "192.168.0.32");
    ket::config("user", "ket");    
    ket::config("timeout", "1");    
 

    size_t n_bits = std::atoi(argv[1]);
    
    auto random_num = measure(H(H(H(H(H(H(quant{n_bits}))))))).get();
    std::cout << n_bits << " bits random number: " << random_num << std::endl;
}