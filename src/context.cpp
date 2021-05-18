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
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace ket;

context::context() : 
    process_on_top{process_on_top_stack.top()}, 
    ps{process_stack.top()}
    {}

bool context::has_executed() const {
    return ps->has_executed();
}

bool context::in_scope() const {
    return *process_on_top;
}

std::string context::get_return(const std::string& arg) const {
    if (not ps->has_executed()) return "NA";

    std::stringstream json_file;
    json_file << ps->get_result_str();

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(json_file, pt);

    return pt.get<std::string>(arg);
}

std::string context::get_json() const {
    if (not ps->has_executed()) return "NA";
    return ps->get_result_str();
}