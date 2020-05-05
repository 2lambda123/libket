#include "../include/ket"
#include "../include/ket_bits/future.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>

using namespace ket;

char* str_copy(const std::string& str) {
    char* ptr = new char[str.size()+1];
    std::copy(str.begin(), str.end(), ptr);
    ptr[str.size()] = '\0';
    return ptr;
}

_init::_init() {
    ket_ps = new process;
    ket_kbw_path = str_copy("kbw");
    ket_no_execute = 0;
    ket_seed = std::time(nullptr);
    ket_kqasm_path = nullptr;
    ket_plugin_path = str_copy("/usr/lib/kbw/");
}

_init::~_init() {
    if (ket_ps) delete ket_ps;
    if (ket_kbw_path) delete[] ket_kbw_path;
    if (ket_kqasm_path) delete[] ket_kqasm_path;
    if (ket_plugin_path) delete[] ket_plugin_path;
}

void ket::args(int argc, char* argv[]) {
    if (ket_kbw_path) delete[] ket_kbw_path;
    if (ket_plugin_path) delete[] ket_plugin_path;
    
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Show this information")
        ("seed,s", boost::program_options::value<size_t>(), "Pseudo random number generator seed")
        ("out,o", boost::program_options::value<std::string>()->default_value(""), "kqasm output file")
        ("kbw,b", boost::program_options::value<std::string>()->default_value("kbw"), "Path to the Ket Bitwise simulator")
        ("plugin,p", boost::program_options::value<std::string>()->default_value("/usr/lib/kbw/"), "Ket Bitwise plugin directory path")
        ("no-execute", "Do not execute the quantum code, measuments will return 0");

    boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    } 

    if (vm.count("seed")) ket_seed = vm["seed"].as<size_t>();
    
    ket_kbw_path= str_copy(vm["kbw"].as<std::string>());

    ket_no_execute = vm.count("no-execute");

    std::string out_path = vm["out"].as<std::string>();
    if (out_path != "") {
        ket_kqasm_path = str_copy(out_path);
        
        std::ofstream out{ket_kqasm_path};
        out.close();
    }

    ket_plugin_path = str_copy(vm["plugin"].as<std::string>());
}
