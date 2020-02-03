#include "../include/ket.hpp"

using namespace ket::base;

Handler::Bits::Bits(const std::vector<Bits>& list)
    : handler{list[0].handler} 
{
    for (auto &i : list) {
        bits.insert(bits.end(), i.bits.begin(), i.bits.end());
        measurement.insert(measurement.end(), i.measurement.begin(), i.measurement.end());
    }
}

Handler::Bits Handler::Bits::operator[](size_t index) const {
    return Bits{handler, {bits[index]}, {measurement[index]}};
}

size_t Handler::Bits::size() const {
    return bits.size();
}

Handler::Result Handler::Bits::get(size_t index) {
    if (*measurement[index] == NONE) {
        handler.__run(*this);
    }
    return *measurement[index];
}

Handler::Bits::Bits(Handler& handler, const std::vector<size_t>& bits, const std::vector<std::shared_ptr<Result>>& measurement) 
    : handler{handler}, bits{bits}, measurement{measurement} {}

