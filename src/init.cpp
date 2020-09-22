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

void ket::ket_init_new(int argc, char* argv[]) {
    process_stack.push(std::make_shared<process>());
    process_on_top_stack.push(std::make_shared<bool>(true));

    boost::program_options::options_description desc{"Ket program options"};
    desc.add_options()
        ("help,h", "Show this information")
        ("out,o", boost::program_options::value<std::string>(), "KQASM output file")
        ("kbw,s", boost::program_options::value<std::string>()->default_value("127.0.1.1"), "Quantum execution (KBW) address")
        ("port,p", boost::program_options::value<int>()->default_value(4242), "Quantum execution (KBW) port address")
        ("no-execute", "Does not execute the quantum code, measuments return 0");

    boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed, vm);

    if (vm.count("help")) {
        desc.print(std::cout);
        exit(0);
    } 

    kbw_addr = vm["kbw"].as<std::string>();
    kbw_port = vm["port"].as<int>();

    execute_kqasm = vm.count("no-execute")? false : true;

    if (vm.count("out")) {
        output_kqasm = true;
        kqasm_path = vm["out"].as<std::string>();
        std::ofstream out{kqasm_path};
        out.close();
    } else {
        output_kqasm = false;
    }
    
}
