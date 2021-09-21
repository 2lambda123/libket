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
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <random>

using namespace ket;

void ket::config(std::string param, std::string value) {
    if (param == "server") {
        kbw_addr = value;
    } else if (param == "port") {
        kbw_port = value;
    } else if (param == "execute") {
        execute_kqasm = boost::lexical_cast<bool>(value);
    } else if (param == "dump2fs") {
        dump_to_fs = boost::lexical_cast<bool>(value);
    } else if (param == "seed") {
        std::srand(std::stoi(value));
        send_seed = true;
    } else if (param == "kqasm") {
        kqasm_path = value;
        output_kqasm = true;
    } else if (param == "api-args") {
        api_args = value;
    } else {
        api_args_map[param] = value;
    }
}
