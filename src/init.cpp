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

void ket_init_new(int argc, char* argv[]) {
    process_stack.push(std::make_shared<process>());
    process_on_top_stack.push(std::make_shared<bool>());

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

    if (vm.count("seed")) {
        exec_seed = vm["seed"].as<size_t>();
    } else {
        exec_seed = std::time(nullptr);
    }

    if (vm.count("kbw")) {
        kbw_path = vm["kbw"].as<std::string>();
    } else {
#ifdef SNAP 
        kbw_path = std::string{std::getenv("SNAP")} + "/usr/bin/kbw";
#else
        kbw_path = "kbw";
#endif
    }

    execute_kqasm = vm.count("no-execute")? false : true;

    if (vm.count("out")) {
        output_kqasm = true;
        kqasm_path = vm["out"].as<std::string>();
        std::ofstream out{kqasm_path};
        out.close();
    } else {
        output_kqasm = false;
    }
    
    if (vm.count("plugin")) {
        use_plugin = true;
        plugin_path = vm["plugin"].as<std::string>();
    } else {
        use_plugin = false;
    } 
}
