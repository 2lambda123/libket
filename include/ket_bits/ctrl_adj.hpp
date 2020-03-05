#pragma once
#include "ket_init.hpp"
#include "qubit_bit.hpp"

namespace ket {
  class Qubit_or_Bit {
     public:
        Qubit_or_Bit(const Qubit& qubit);
        Qubit_or_Bit(const Bit& bit);

        bool quantum() const;
        base::Handler::Qubits get_qubit() const;
        base::Handler::Bits get_bit() const;

     private:
        std::variant<Qubit, Bit> bit;
        bool _quantum;
    };

    template <class T, class F, class... Args> 
    T ctrl(const std::vector<Qubit_or_Bit>& c, F func, Args... args) {
        ket_handle->ctrl_begin();
        for (const auto &i: c) {
            if (i.quantum()) {
                ket_handle->add_ctrl(i.get_qubit());
            } else {
                ket_handle->add_ctrl(i.get_bit());
            }
        }
        T result = func(args...);
        ket_handle->ctrl_end();
        return result;
    }    

    template <class F, class... Args> 
    void ctrl(const std::vector<Qubit_or_Bit>& c, F func, Args... args) {
        ket_handle->ctrl_begin();
        for (const auto &i: c) {
            if (i.quantum()) {
                ket_handle->add_ctrl(i.get_qubit());
            } else {
                ket_handle->add_ctrl(i.get_bit());
            }
        }
        func(args...);
        ket_handle->ctrl_end();
    }   

    template <class F, class... Args>
    void adj(F func, Args...  args) {
        ket_handle->adj_begin();
        func(args...);
        ket_handle->adj_end();
    } 
}
