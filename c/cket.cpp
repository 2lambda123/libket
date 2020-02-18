#include "cket.h"
#include <ket>


void ket_begin(int argc, char* argv[]) {
    ket::begin(argc, argv);
}

void ket_end() {
    ket::end();
}

ket_qubit* ket_qubit_new(size_t size) {
    ket_qubit* q;
    q = new ket::Qubit(size);
    return q;
}

void ket_x(ket_qubit* q) {
    ket::x(*static_cast<ket::Qubit*>(q));
}

ket_bit* ket_measure(ket_qubit* q) {
    ket_bit* m;
    m = new ket::Bit(ket::measure(*static_cast<ket::Qubit*>(q)));
    return m;
}

int ket_bit_get(ket_bit* b, size_t index) {
    return (*static_cast<ket::Bit*>(b))[index];
}