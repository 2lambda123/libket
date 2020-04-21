#include "../include/ket_bits/macros.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>

using namespace ket;

void ket::begin(int argc, char* argv[]) {
    ket_hdl = new ket::process;
    ket_hdl->begin_block("entry");
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Show this information")
        ("seed,s", boost::program_options::value<size_t>(), "Pseudo random number generator seed")
        ("out,o", boost::program_options::value<std::string>()->default_value(""), "kqasm output file")
        ("kbw,b", boost::program_options::value<std::string>()->default_value("kbw"), "Path to the Ket Bitwise simulator")
        ("no-execute", "Do not execute the quantum code, measuments will return 0");

    boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    } 

    if (vm.count("seed")) ket_seed = vm["seed"].as<size_t>();
    else ket_seed =std::time(nullptr);
    
    std::string kbw_path = vm["kbw"].as<std::string>();
    ket_kbw_path = new char[kbw_path.size()+1];
    std::copy(kbw_path.begin(), kbw_path.end(), ket_kbw_path);
    ket_kbw_path[kbw_path.size()] = '\0';

    ket_no_execute = vm.count("no-execute");

    std::string out_path = vm["out"].as<std::string>();
    if (out_path != "") {
        ket_out = 1;   
        ket_kqasm_path = new char[out_path.size()+1];
        std::copy(out_path.begin(), out_path.end(), ket_kqasm_path);
        ket_kqasm_path[out_path.size()] = '\0';
        std::ofstream out{ket_kqasm_path};
        out.close();
    }
}

void ket::end() {
    if (ket_hdl) delete ket_hdl;
    if (ket_kbw_path) delete[] ket_kbw_path;
    if (ket_kqasm_path) delete[] ket_kqasm_path;
}
