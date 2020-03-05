#pragma once
#include "qubit_bit.hpp"
#include "ctrl_adj.hpp"

namespace ket {
    void swap(const Qubit& a, const Qubit& b);

    void qft(const Qubit& q);

    void invert(const Qubit& q);

    template <class T>
    T to(Bit bit) {
      T ret{};
      auto size = bit.size();
      for (size_t i = 0; i < size; i++) {
        ret |= bit[size-i-1] << (i);
      }
      return ret;
    }
}