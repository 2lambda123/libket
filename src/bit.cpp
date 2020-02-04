#include "../include/ket.hpp"

ket::Bit ket::Bit::operator() (size_t index) const {
    return Bit{bits[index]};
}

ket::Bit ket::Bit::operator() (size_t begin, size_t end) const {
    std::vector<base::Handler::Bits> nbits;
    for (auto i = begin; i < end; i++) {
        nbits.push_back(bits[i]);
    }
    return Bit{nbits};
}

int ket::Bit::operator[] (size_t index) {
    return (int) bits.get(index);
}

size_t ket::Bit::size() const {
    return bits.size();
}

ket::Bit::Bit(const base::Handler::Bits& bits) : bits{bits} {}

ket::Bit ket::operator+(const Bit& a, const Bit& b) {
    return {{{a.bits, b.bits}}};
}

std::ostream& ket::operator<<(std::ostream& os, Bit bit) {
    for (auto i = bit.size(); i > 0; --i) 
        os << bit[i-1];
    return os;
}
