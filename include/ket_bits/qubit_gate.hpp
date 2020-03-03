#pragma once
#include "handler.hpp"

namespace ket{
 class Bit;
    class Qubit {
     public:
        Qubit(size_t size=1);
        Qubit operator() (size_t index) const;
        Qubit operator() (size_t begin, size_t end) const;
        size_t size() const;

     private:
        Qubit(const base::Handler::Qubits& qubits);

        base::Handler::Qubits qubits;

        friend void x(const Qubit& q);
        friend void y(const Qubit& q);
        friend void z(const Qubit& q);
        friend void h(const Qubit& q);
        friend void s(const Qubit& q);
        friend void sd(const Qubit& q);
        friend void t(const Qubit& q);
        friend void td(const Qubit& q);
        friend void cnot(const Qubit& ctrl, const Qubit& target);
        friend void u1(double lambda, const Qubit& q);
        friend void u2(double phi, double lambda, const Qubit& q);
        friend void u3(double theta, double phi, double lambda, const Qubit& q);
        friend Qubit operator+(const Qubit& a, const Qubit& b);
        friend Bit measure(const Qubit& q);
        friend Qubit dirty(size_t size);
        friend void free(const Qubit& q);
        friend void freedirty(const Qubit& q);
        friend void __apply_oracle(const std::string& gate, const Qubit& q);
        friend class Qubit_or_Bit;
    };

    class Bit {
     public:
        Bit operator() (size_t index) const;
        Bit operator() (size_t begin, size_t end) const;

        int operator[] (size_t index);

        size_t size() const;

     private:
        Bit(const base::Handler::Bits& bits);
        
        base::Handler::Bits bits;

        friend Bit measure(const Qubit& q);
        friend Bit operator+(const Bit& a, const Bit& b);
        friend class Qubit_or_Bit;
    };

    void x(const Qubit& q);
    void y(const Qubit& q);
    void z(const Qubit& q);
    void h(const Qubit& q);
    void s(const Qubit& q);
    void sd(const Qubit& q);
    void t(const Qubit& q);
    void td(const Qubit& q);
    void cnot(const Qubit& ctrl, const Qubit& target);
    void u1(double lambda, const Qubit& q);
    void u2(double phi, double lambda, const Qubit& q);
    void u3(double theta, double phi, double lambda, const Qubit& q);
    Qubit operator+(const Qubit& a, const Qubit& b);
    Bit operator+(const Bit& a, const Bit& b);
    std::ostream& operator<<(std::ostream& os, Bit bit); 
    Bit measure(const Qubit& q);
    Qubit dirty(size_t size);
    void free(const Qubit& q);
    void freedirty(const Qubit& q);
    void __apply_oracle(const std::string& gate, const Qubit& q);

}
