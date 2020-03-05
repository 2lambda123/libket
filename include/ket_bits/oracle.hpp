#pragma once
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "qubit_bit.hpp"

namespace ket::oracle {
    using gate_map = boost::unordered_map<size_t, boost::unordered_set<std::pair<std::complex<double>, size_t>>>;
    class Gate {
     public:
        Gate(gate_map& gate, size_t size); 
        boost::unordered_set<std::pair<std::complex<double>, size_t>>& operator[](size_t index);
        size_t size() const;

        void operator()(const ket::Qubit& q);

     private:
        gate_map gate;
        size_t _size;
    };

    Gate lambda(std::function<size_t(size_t)> func, size_t size, size_t begin=0, size_t end=0);
}