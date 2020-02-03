#include "../include/ket.hpp"
#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <boost/program_options.hpp>

using namespace ket::base;

Handler::Qubit_alloc::Qubit_alloc(size_t qubit_index) 
    : qubit_index{{qubit_index}} {
        circuit << "qubit |" << qubit_index << ">" << std::endl;
    }

Handler::Handler(const std::string& out_path, const std::string& kbw_path, size_t seed, bool no_execute) 
    : out_to_file{out_path == ""? false : true}, kbw_path{kbw_path},
      quantum_counter{0}, classical_counter{0}, seed{seed}, no_execute{no_execute}
{
    if (out_to_file) out_file.open(out_path);        
}

Handler::~Handler() {
    if (out_to_file) out_file.close();
}

Handler::Qubits Handler::alloc(size_t size) {
    std::vector<size_t> qubits;

    for (size_t i = quantum_counter; i < quantum_counter+size; i++) {
        qubits.push_back(i);
        allocations[i] = std::make_shared<Qubit_alloc>(i);
    }
    
    quantum_counter += size;

    return Qubits{qubits};
}

void Handler::add_gate(std::string gate, const Qubits& qubits) {

    std::vector<size_t> qubits_cctrl;
    if (not cctrl.empty()) for (auto &i: cctrl) for (auto j: i.bits) 
        qubits_cctrl.push_back(measure_map[j]);

    if (not qctrl.empty()) for (auto &i: qctrl) for (auto j: i.qubits) 
        qubits_cctrl.push_back(j);

    auto& circuit = merge(qubits_cctrl.empty() ? qubits : Qubits{{{qubits_cctrl}, qubits}});

    if (not cctrl.empty()) {
        circuit << "if ";
        for (auto &i : cctrl) for (auto j: i.bits) 
            circuit << j << " ";
    }

    if (not qctrl.empty()) {
        circuit << "ctrl ";
        for (auto &i : qctrl) for (auto j: i.qubits) 
            circuit << "|" << j << "> ";
    }

    if (adj_counter) {
        circuit << "adj " << adj_counter << " ";
    }

    circuit << gate;
    for (auto& i : qubits) circuit << " |" << i << ">";
    circuit << std::endl;
}

Handler::Bits Handler::measure(const Qubits& qubits) {
    auto& circuit = merge(qubits);

    std::vector<size_t> bits;
    std::vector<std::shared_ptr<Result>> measurement;

    size_t counter = classical_counter;
    for (auto i: qubits) {
        bits.push_back(counter);
        measurement.push_back(std::make_shared<Result>(NONE));
        circuit << "bit " << counter << std::endl;
        circuit << counter << " = measure |" << i << ">" << std::endl; 
        allocations[i]->measurement_return[i] = measurement.back();
        measure_map[counter] = i;
        counter++;
    }

    classical_counter += qubits.size();

    return Bits{*this, bits, measurement};
}

void Handler::ctrl_begin() {
    qctrl_b.push_back(true);
    cctrl_b.push_back(true);
}

void Handler::add_ctrl(const Qubits& qubits) {
    if (qctrl_b.back()) {
        qctrl.push_back(qubits);
        qctrl_b.back() = false;
    } else {
        auto q = qctrl.back();
        qctrl.pop_back();
        qctrl.push_back({{q, qubits}});
    }
}

void Handler::add_ctrl(const Bits& bits) {
    if (cctrl_b.back()) {
        cctrl.push_back(bits);
        cctrl_b.back() = false;
    } else {
        auto b = cctrl.back();
        cctrl.pop_back();
        cctrl.push_back({{b, bits}});
    }
}

void Handler::ctrl_end() {
    if (not qctrl_b.back())
        qctrl.pop_back();
    if (not cctrl_b.back())
        cctrl.pop_back();
    qctrl_b.pop_back();
    cctrl_b.pop_back();
}

void Handler::adj_begin() {
    adj_counter++;
}

void Handler::adj_end() {
    adj_counter--;
}

void Handler::__run(const Bits& bits) {
    std::vector<size_t> qubits_bits;
    for (auto i :bits.bits) 
        qubits_bits.push_back(measure_map[i]);        

    std::vector<Qubits> qubits{{Qubits{qubits_bits}}};
    auto &circuit = merge(qubits);

    if (out_to_file) {
        out_file << circuit.str()
                 << ">>>" << std::endl;
    }

    auto &measuments = allocations[qubits[0].qubits[0]]->measurement_return;

    if (no_execute) {
        for (auto &i : measuments) {
            *i.second = ZERO;
        }
    } else {
        boost::asio::io_service ios;

        std::future<std::string> outdata;

        boost::process::async_pipe qasm(ios);
        boost::process::child c(kbw_path+std::string(" -s ")+std::to_string(seed++),
                                boost::process::std_out > outdata,
                                boost::process::std_in < qasm, ios);

        boost::asio::async_write(qasm, boost::process::buffer(circuit.str()),
                                [&](boost::system::error_code, size_t) { qasm.close(); });

        ios.run();

        std::stringstream result{outdata.get()};

        while (result) {
            size_t bit;
            int mea;
            result >> bit >> mea;
            *(measuments[measure_map[bit]]) = (Result) mea;
        }
    }

    auto qubits_free = allocations[qubits[0].qubits[0]]->qubit_index;

    for (auto i : qubits_free) {
        allocations.erase(i);
    }
}


std::stringstream& Handler::merge(const Qubits& qubits) {
    std::shared_ptr<Qubit_alloc> qubit;
    for (auto &index : qubits) {
        if (not qubit) {
            qubit = allocations[index];
        } else if (qubit->qubit_index.find(index) == qubit->qubit_index.end()){
            qubit->circuit << allocations[index]->circuit.str();
            qubit->qubit_index.insert(index);
            qubit->measurement_return.insert(allocations[index]->measurement_return.begin(),
                                                allocations[index]->measurement_return.end());
            allocations[index] = qubit;
        }
    }
    return qubit->circuit;
}
