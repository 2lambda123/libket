#include "../include/ket"
#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <limits>
#include <boost/program_options.hpp>

using namespace ket::base;

Handler::Qubit_alloc::Qubit_alloc(size_t qubit_index, bool dirty) 
    : qubit_index{{qubit_index}}
{
    circuit << "qubit" << (dirty? " dirty " : " " ) <<  "|" << qubit_index << ">" << std::endl;
}

Handler::Handler(const std::string& out_path, const std::string& kbw_path, const std::string& kqc_path, size_t seed, bool no_execute, bool no_optimise) 
    : out_to_file{out_path == ""? false : true}, kbw_path{kbw_path}, kqc_path{kqc_path},
      quantum_counter{0}, classical_counter{0}, seed{seed}, no_execute{no_execute}, no_optimise{no_optimise}
{
    if (out_to_file) out_file.open(out_path);        
}

Handler::~Handler() {
    if (out_to_file) out_file.close();
}

Handler::Qubits Handler::alloc(size_t size, bool dirty) {
    std::vector<size_t> qubits;

    for (size_t i = quantum_counter; i < quantum_counter+size; i++) {
        qubits.push_back(i);
        allocations[i] = std::make_shared<Qubit_alloc>(i, dirty);
    }
    
    quantum_counter += size;

    return Qubits{qubits};
}

void Handler::add_gate(const std::string& gate, const Qubits& qubits, const std::vector<double>& args) {
    if (adj_call.empty()) {
        __add_gate(gate, qubits, args, false, qctrl, cctrl);
    } else {
        bool adj = adj_call.size()%2;
        auto _qctrl = qctrl;
        auto _cctrl = cctrl;
        adj_call.back().push_back([this,gate,qubits,args,adj,_qctrl,_cctrl]{ this->__add_gate(gate, qubits, args, adj, _qctrl, _cctrl); });
    } 
}

void Handler::add_oracle(const std::string& gate, const Qubits& qubits) {
    add_gate("\""+gate+"\"", qubits);
}

void Handler::__add_gate(const std::string& gate, const Qubits& qubits, const std::vector<double>& args, bool adj, const std::vector<Qubits>& _qctrl, const std::vector<Bits>& _cctrl) {
    std::vector<size_t> qubits_cctrl;
    if (not _cctrl.empty()) for (auto &i: _cctrl) for (auto j: i.bits) 
        qubits_cctrl.push_back(measure_map[j]);

    if (not _qctrl.empty()) for (auto &i: _qctrl) for (auto j: i.qubits) 
        qubits_cctrl.push_back(j);

    auto& circuit = merge(qubits_cctrl.empty() ? qubits : Qubits{{{qubits_cctrl}, qubits}});

    if (not _cctrl.empty()) {
        circuit << "if ";
        for (auto &i : _cctrl) for (auto j: i.bits) 
            circuit << j << " ";
    }

    if (not _qctrl.empty()) {
        circuit << "ctrl ";
        for (auto &i : _qctrl) for (auto j: i.qubits) 
            circuit << "|" << j << "> ";
    }

    if (adj) circuit << "adj ";

    circuit << gate;

    if (not args.empty()) {
        circuit.precision(std::numeric_limits<double>::max_digits10);
        circuit << "(";
        for (auto i : args) circuit << std::fixed << i << " ";
        circuit << ")";
    }

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

void Handler::free(const Qubits& qubits) {
    auto& circuit = merge(qubits);
    for (auto i: qubits) {
        circuit << "free |" << i << ">" << std::endl;
        allocations.erase(i);    
    }
}

void Handler::free_dirty(const Qubits& qubits) {
    auto& circuit = merge(qubits);
    for (auto i: qubits) {
        circuit << "free dirty |" << i << ">" << std::endl;
        allocations.erase(i);    
    }
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
    adj_call.push_back({});
}

void Handler::adj_end() {
    if (adj_call.size() == 1) {
        for (auto i = adj_call.back().rbegin(); i != adj_call.back().rend(); ++i) 
            (*i)();
        adj_call.pop_back();
    } else {
        std::vector<std::function<void()>> inner = adj_call.back();
        adj_call.pop_back();
        adj_call.back().push_back([inner]{
            for (auto i = inner.rbegin(); i != inner.rend(); ++i) 
                (*i)();
        });
    }
}

std::string call(const std::string& command, const std::string& in) {
    boost::asio::io_service ios;

    std::future<std::string> outdata;

    boost::process::async_pipe qasm(ios);
    boost::process::child c(command+std::string{},
                            boost::process::std_out > outdata,
                            boost::process::std_in < qasm, ios);

    boost::asio::async_write(qasm, boost::process::buffer(in),
                            [&](boost::system::error_code, size_t) { qasm.close(); });

    ios.run();

    return outdata.get();
    return in;
}

void Handler::__run(const Bits& bits) {
    std::vector<size_t> qubits_bits;
    for (auto i :bits.bits) 
        qubits_bits.push_back(measure_map[i]);        

    std::vector<Qubits> qubits{{Qubits{qubits_bits}}};
    auto &circuit = merge(qubits);

    std::string qasm;
    if (no_optimise) 
        qasm = circuit.str();
    else 
        qasm = call(kqc_path, circuit.str());

    if (out_to_file) {
        out_file << qasm << ">>>" << std::endl;
    }

    auto &measuments = allocations[qubits[0].qubits[0]]->measurement_return;

    if (no_execute) {
        for (auto &i : measuments) {
            *i.second = ZERO;
        }
    } else {
        std::stringstream result{call(kbw_path+std::string(" -s ")+std::to_string(seed++), qasm)};

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
            qubit->qubit_index.insert(allocations[index]->qubit_index.begin(), allocations[index]->qubit_index.end());
            qubit->measurement_return.insert(allocations[index]->measurement_return.begin(),
                                                allocations[index]->measurement_return.end());
            std::vector<size_t> move_index;
            for (auto i : allocations[index]->qubit_index) move_index.push_back(i);
            for (auto i : move_index) allocations[i] = qubit;
        }
    }
    return qubit->circuit;
}
