#include "../include/ket.hpp"

using namespace ket::base;

Handler::Qubits::Qubits(const std::vector<Qubits>& bits) {
    for (auto &i : bits)
        qubits.insert(qubits.end(), i.qubits.begin(), i.qubits.end());
}

Handler::Qubits Handler::Qubits::operator[](size_t index) const {
    return Qubits{std::vector<size_t>{qubits[index]}};
}

size_t Handler::Qubits::size() const {
    return qubits.size();
}

const std::vector<size_t>::const_iterator Handler::Qubits::begin() const {
    return qubits.begin();
}

const std::vector<size_t>::const_iterator Handler::Qubits::end() const {
    return qubits.end();
}

Handler::Qubits::Qubits(const std::vector<size_t>& qubits) 
    : qubits{qubits} {}

