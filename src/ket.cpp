#include "../include/ket.hpp"
#include <iostream>
#include <ctime>
#include <boost/program_options.hpp>

void ket::init(int argc, char* argv[]) {
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Show this information")
        ("seed,s", boost::program_options::value<size_t>(), "Pseudo random number generator seed")
        ("out,o", boost::program_options::value<std::string>()->default_value(""), "qasm output file")
        ("kbw,k", boost::program_options::value<std::string>()->default_value("kbw"), "Path to the Ket Bitwise simulator")
        ("no-execute", "Do not execute quantum code, measuments will return 0");

    boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    } 

    size_t seed;
    if (vm.count("seed")) 
        seed = vm["seed"].as<size_t>();
    else
        seed =std::time(nullptr);
    std::string out_path = vm["out"].as<std::string>();
    std::string kbw_path = vm["kbw"].as<std::string>();
    bool no_execute = vm.count("no-execute");

    handle = std::make_unique<base::Handler>(out_path, kbw_path, seed, no_execute);
}

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

void ket::x(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        handle->add_gate("x", q(i).qubits);
    }
}

void ket::y(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        handle->add_gate("y", q(i).qubits);
    }
}

void ket::z(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        handle->add_gate("z", q(i).qubits);
    }
}

void ket::h(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        handle->add_gate("h", q(i).qubits);
    }
}

void ket::s(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        handle->add_gate("s", q(i).qubits);
    }
}

void ket::sd(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        handle->add_gate("sd", q(i).qubits);
    }
}

void ket::t(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        handle->add_gate("t", q(i).qubits);
    }
}

void ket::td(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        handle->add_gate("td", q(i).qubits);
    }
}

void ket::cnot(const Qubit& ctrl, const Qubit& target) {
    if (ctrl.size() == target.size()) {
        for (size_t i = 0; i < ctrl.size(); i++) {
            handle->add_gate("cnot", {{ctrl(i).qubits , target(i).qubits}});
        }
    }
}

ket::Bit ket::measure(const ket::Qubit& q) {
    return Bit{handle->measure(q.qubits)};
}

