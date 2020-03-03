#include "../include/ket_bits/ctrl_adj.hpp"

ket::Qubit_or_Bit::Qubit_or_Bit(const Qubit& qubit) : bit{qubit}, _quantum{true} {}

ket::Qubit_or_Bit::Qubit_or_Bit(const Bit& bit) : bit{bit}, _quantum{false} {}

bool ket::Qubit_or_Bit::quantum() const {
    return _quantum;
}

ket::base::Handler::Qubits ket::Qubit_or_Bit::get_qubit() const {
    return std::get<Qubit>(bit).qubits;
}

ket::base::Handler::Bits ket::Qubit_or_Bit::get_bit() const {
    return std::get<Bit>(bit).bits;
}
