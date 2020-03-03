#include "../include/ket_bits/ket_init.hpp"
#include <iostream>
#include <ctime>
#include <boost/program_options.hpp>

void ket::begin(int argc, char* argv[]) {
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Show this information")
        ("seed,s", boost::program_options::value<size_t>(), "Pseudo random number generator seed")
        ("out,o", boost::program_options::value<std::string>()->default_value(""), "qasm output file")
        ("kbw,b", boost::program_options::value<std::string>()->default_value("kbw"), "Path to the Ket Bitwise simulator")
        ("kqc,c", boost::program_options::value<std::string>()->default_value("kqc"), "Path to the Ket Quantum Compiler")
        ("no-execute", "Do not execute the quantum code, measuments will return 0")
        ("no-optimise", "Do not optimize the quantum code");

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
    std::string kqc_path = vm["kqc"].as<std::string>();
    bool no_execute = vm.count("no-execute");
    bool no_optimise = vm.count("no-optimise");

    ket_handle = new base::Handler(out_path, kbw_path, kqc_path, seed, no_execute, no_optimise);
}

void ket::end() {
    delete ket_handle;
}
