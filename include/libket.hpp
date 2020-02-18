#pragma once
#ifdef __LIBCKET
#include "ket.hpp"
#else
#include <ket>
#endif

namespace ket {

    void swap(const Qubit& a, const Qubit& b) {
        cnot(a, b);
        cnot(b, a);
        cnot(a, b);
    }

    void qft(const Qubit& q) {
        auto k = [](double m) { return (2*M_PI)/(std::pow(2.0, m)); };
        for (size_t i = 0; i < q.size(); i++) {
            h(q(i));
            double m = 2;
            for (size_t j = i+1; j < q.size(); j++) {
                ctrl({q(j)}, u1, k(m), q(i));
                m += 1;
            }
        }
    }

    void invert(const Qubit& q) {
        for (size_t i = 0; i < (q.size()/2); i++) 
            swap(q(i), q(q.size()-i-1));
    }

    template <class T>
    T to(Bit bit) {
      T ret{};
      auto size = bit.size();
      for (size_t i = 0; i < size; i++) {
        ret |= bit[size-i-1] << (i);
      }
      return ret;
    }

};