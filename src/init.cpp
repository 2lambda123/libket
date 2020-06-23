/* MIT License
 * 
 * Copyright (c) 2020 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright (c) 2020 Rafael de Santiago <r.santiago@ufsc.br>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../include/ket"
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>

using namespace ket;

inline char* str_copy(const std::string& str) {
    char* ptr = new char[str.size()+1];
    std::copy(str.begin(), str.end(), ptr);
    ptr[str.size()] = '\0';
    return ptr;
}

void ket_init_free() {
    if (ket_ps) delete ket_ps;
    if (ket_kbw_path) delete[] ket_kbw_path;
    if (ket_kqasm_path) delete[] ket_kqasm_path;
    if (ket_plugin_path) delete[] ket_plugin_path;
}

void ket_init_new(int argc, char* argv[]) {
    ket_ps = new process;

    boost::program_options::options_description desc{"Ket program options"};
    desc.add_options()
        ("help,h", "Show this information")
        ("seed,s", boost::program_options::value<size_t>(), "Pseudo random number generator seed")
        ("out,o", boost::program_options::value<std::string>(), "kqasm output file")
        ("kbw,b", boost::program_options::value<std::string>(), "Path to the Ket Bitwise simulator")
        ("plugin,p", boost::program_options::value<std::string>(), "Ket Bitwise plugin directory path")
        ("no-execute", "Do not execute the quantum code, measuments will return 0");

    boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    } 

    if (vm.count("seed")) ket_seed = vm["seed"].as<size_t>();
    else ket_seed = std::time(nullptr);

    if (vm.count("kbw")) {
        ket_kbw_path= str_copy(vm["kbw"].as<std::string>());
    } else {
#ifdef SNAP 
        ket_kbw_path = str_copy(std::string{std::getenv("SNAP")} + "/usr/bin/kbw");
#else
        ket_kbw_path = str_copy("kbw");
#endif
    }

    ket_no_execute = vm.count("no-execute");

    if (vm.count("out")) {
        ket_kqasm_path = str_copy(vm["out"].as<std::string>());
        std::ofstream out{ket_kqasm_path};
        out.close();
    } else {
        ket_kqasm_path = nullptr;
    }
    
    if (vm.count("plugin")) ket_plugin_path = str_copy(vm["plugin"].as<std::string>());
    else ket_plugin_path = nullptr;
}
