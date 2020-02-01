#include "../include/ket.hpp"
#include <iostream>
#include <boost/program_options.hpp>

void ket::init(int argc, char* argv[]) {
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Show this")
        ("seed,s", boost::program_options::value<size_t>()->default_value(42), "Pseudo random number generator seed")
        ("qasm,o", boost::program_options::value<std::string>()->default_value(""), "qasm output file")
        ("kbw", boost::program_options::value<std::string>()->default_value("kbw"), "Path to the Ket Bitwise simulator");

    boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    } 

    size_t seed = vm["seed"].as<size_t>();
    std::string out_path = vm["qasm"].as<std::string>();
    std::string kbw_path = vm["kbw"].as<std::string>();


    hdr = std::make_unique<base::Handler>(out_path, kbw_path, seed);
}

ket::Qubit::Qubit(size_t size) : qubits{hdr->alloc(size)} {}

ket::Qubit::Qubit(const base::Handler::Qubits& qubits) : qubits{qubits} {}

ket::Qubit ket::Qubit::operator() (size_t index) const {
    return Qubit{qubits[index]};
}

ket::Qubit ket::Qubit::operator() (size_t begin, size_t end) const {
    std::vector<base::Handler::Qubits> qbits;

    for (auto i = begin; i < end; i++) {
        qbits.push_back(qubits[i]);
    }
    return Qubit{qbits};
}

size_t ket::Qubit::size() const {
    return qubits.size();
}

ket::Bit ket::Bit::operator() (size_t index) const {
    return Bit{bits[index]};
}

ket::Bit ket::Bit::operator() (size_t begin, size_t end) const {
    std::vector<base::Handler::Bits> nbits;
    for (auto i = begin; i < end; i++) {
        nbits.push_back(bits[i]);
    }
    return Bit{nbits};
}

int ket::Bit::operator[] (size_t index) {
    return (int) bits.get(index);
}

ket::Bit::Bit(const base::Handler::Bits& bits) : bits{bits} {}

void ket::x(const Qubit& q) {
    for (size_t i = 0; i < q.size(); i++) {
        hdr->add_gate("x", q(i).qubits);
    }
}

ket::Bit ket::measure(const ket::Qubit& q) {
    return Bit{hdr->measure(q.qubits)};
}

