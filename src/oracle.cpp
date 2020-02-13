#include "../include/ket.hpp"
#include <boost/serialization/boost_unordered_map.hpp>
#include <boost/serialization/boost_unordered_set.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/complex.hpp>

ket::oracle::Gate::Gate(gate_map& gate, size_t size) : gate{gate}, _size{size} {}

boost::unordered_set<std::pair<std::complex<double>, size_t>>& ket::oracle::Gate::operator[](size_t index) {
    return gate[index];
}

void ket::oracle::Gate::operator()(const ket::Qubit& q) {
    std::stringstream out;
    boost::archive::text_oarchive oarchive(out);
    oarchive << gate;
    ket::__apply_oracle(out.str(), q);
}

size_t ket::oracle::Gate::size() const {
    return _size;
}

ket::oracle::Gate ket::oracle::lambda(std::function<size_t(size_t)> func, size_t size, size_t begin, size_t end) {
    if (end == 0) end = 1ul << size;
    gate_map gate;
    for (size_t i = begin; i < end; i++) 
        gate[i].insert(std::make_pair(1, func(i)));
    return {gate, size};
}

