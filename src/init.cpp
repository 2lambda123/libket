/* Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
        ("kbw,s", boost::program_options::value<std::string>()->default_value("::1"), "Quantum execution (KBW) address")
        ("port,p", boost::program_options::value<int>()->default_value(4242), "Quantum execution (KBW) port address")
        ("no-execute", "Does not execute quantum code, measurements return 0");

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
