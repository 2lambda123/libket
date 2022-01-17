#include <ket/libket.hpp>
#include <cmath>
#include "print_dump.hpp"

using namespace ket::libket;

void cnot(process_t& ps, qubit_t c, qubit_t t) {
    ps.ctrl_push({c});
    ps.gate(gate_t::pauli_x, t);
    ps.ctrl_pop();
}

void swap(process_t& ps, qubit_t a, qubit_t b) {
    cnot(ps, a, b);
    cnot(ps, b, a);
    cnot(ps, a, b);
}

void qft(process_t& ps, qubit_list_t qubits, bool invert = true) {
    if (qubits.size() == 1) {
        ps.gate(gate_t::hadamard, qubits[0]);
    } else {
        auto head = qubits[0];
        auto tail = qubits;
        tail.erase(tail.begin());

        ps.gate(gate_t::hadamard, head);
        for (auto i = 0u; i < tail.size(); i++) {
            ps.ctrl_push({tail[i]});
            ps.gate(gate_t::phase, head, 2.0*M_PI/pow(2.0, i+2));
            ps.ctrl_pop();
        }
        qft(ps, tail, false);
    }
    if (invert) {
        for (auto i = 0u; i < qubits.size()/2; i++) {
            swap(ps, qubits[i], qubits[qubits.size()-i-1]);
        }
    }
}

int main() {
    process_t ps{0};

    auto N = 15;
    auto x = 11;

    std::stringstream args;
    args << x << ' ' << N;

    const auto L = 4;
    const auto t = L;

    qubit_list_t qubits;
    for (auto i = 0; i < t; i++) {
        qubits.push_back(ps.alloc());
    }

    for (auto qubit : qubits) {
        ps.gate(gate_t::hadamard, qubit);
    }

    qubit_list_t result;
    for (auto i = 0; i < t; i++) {
        result.push_back(ps.alloc());
    }

    ps.gate(gate_t::pauli_x, result.back());

    qubit_list_t all_qubits = qubits;
    all_qubits.insert(all_qubits.end(), result.begin(), result.end());
    
    ps.plugin("pown", all_qubits, args.str());
   
    ps.adj_begin();
    qft(ps, qubits);
    ps.adj_end();
    
    print_dump<t+L>(ps.dump(all_qubits));
}