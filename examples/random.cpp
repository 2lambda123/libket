#include <ket>

using namespace ket;

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "usage: " << argv[0] << " <number of bits>" << std::endl;
        exit(1);
    }

    size_t n_bits = std::atoi(argv[1]);
    
    quant q{n_bits};
    
    H(q);

    auto random_num = measure(q).get();
    std::cout << n_bits << " bits random number: " << random_num << std::endl;
}