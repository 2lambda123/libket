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
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/complex.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/container/map.hpp>
#include <cmath>
#include <fstream>

using namespace ket;
using tcp = boost::asio::ip::tcp;    
namespace http = boost::beast::http; 

inline std::string urlencode(std::string str) {
    const boost::container::map<std::string, std::string> replace_map = {
	    {" ",  "%20"},
	    {"\t", "%09"},
	    {"\n", "%0A"},
	    {"=" , "%3D"},
	    {"!" , "%21"},
	    {">" , "%3E"},
	    {"<" , "%3C"},
	    {"+" , "%2B"},
	    {"*" , "%2A"},
	    {"/" , "%2F"},
	    {"@" , "%40"},
	    {"(" , "%28"},
	    {")" , "%29"},
    };

    for (auto pair : replace_map) boost::replace_all(str,  pair.first, pair.second);
    return str;
}

void process::exec() {
    if (output_kqasm) {
        std::ofstream out{kqasm_path, std::ofstream::app};
        out << kqasm 
            << "=========================" 
            << std::endl;
        out.close();
    }

    if (execute_kqasm) {
        
        auto kqasm_file = urlencode(kqasm);

        boost::asio::io_context ioc;
        tcp::resolver resolver{ioc};
        tcp::socket socket{ioc};

        auto const results = resolver.resolve(kbw_addr, kbw_port);
        boost::asio::connect(socket, results.begin(), results.end());

        std::string param{"/api/v1/run?"};
        if (dump_to_fs) param += "&dump2fs=1";
        if (send_seed) param += "&seed=" + std::to_string(std::rand());
        param += api_args;
        for (auto arg : api_args_map) param += "&" + arg.first + "=" + urlencode(arg.second);

        http::request<http::string_body> req{http::verb::get, param, 11};
        req.set(http::field::host, kbw_addr);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/x-www-form-urlencoded");

        std::string body{};

        body += "kqasm="             + kqasm_file
             +  "&n_blocks="         + std::to_string(n_blocks)
             +  "&n_qubits="         + std::to_string(used_qubits)
             +  "&max_alloc_qubits=" + std::to_string(max_allocated_qubits)
             +  "&has_plugins="      + std::to_string((plugins_sum == 0? 0 : 1))
             +  "&has_free="         + std::to_string((free_qubits == 0? 0 : 1))
             +  "&has_dump="         + std::to_string((n_dumps == 0? 0 : 1))
             +  "&has_set="          + std::to_string((n_set_inst == 0? 0 : 1));

        req.body() = body;
        req.prepare_payload();
        
        http::write(socket, req);
        
        boost::beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> res;
        res.body_limit(std::numeric_limits<std::uint64_t>::max());
        

        http::read(socket, buffer, res);

        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);
 
        if(ec && ec != boost::system::errc::not_connected)
            throw boost::system::system_error{ec};

        std::stringstream json_file;
        json_file << boost::beast::make_printable(res.get().body().data());
        result = json_file.str();

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(json_file, pt);

        for (auto result : pt.get_child("int")) {
            auto i = std::stol(result.first);
            *(measure_map[i].first) = std::stol(result.second.get_value<std::string>());
            *(measure_map[i].second) = true;
        }

        auto dump2fs = pt.count("dump2fs")? pt.get<std::string>("dump2fs") == "1" : false;

        if (dump2fs) for (auto result : pt.get_child("dump")) {
            auto i = std::stol(result.first);
            auto path = result.second.get_value<std::string>();

            std::ifstream stream_out{path};
            boost::archive::binary_iarchive iarchive{stream_out};

            iarchive >> *(dump_map[i].first); 

            *(dump_map[i].second) = true;

        } else for (auto result : pt.get_child("dump")) {
            auto i = std::stol(result.first);
            auto b64_bin = result.second.get_value<std::string>();

            using iter = boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6>;
            auto bin = std::string(iter(b64_bin.begin()), iter(b64_bin.end()));
            
            std::stringstream stream_out;
            stream_out.write(bin.c_str(), bin.size());
            
            boost::archive::binary_iarchive iarchive{stream_out};

            iarchive >> *(dump_map[i].first); 

            *(dump_map[i].second) = true;
        
        } 

    } else for (auto &i : measure_map) {
        *(i.second.first) =  0;
        *(i.second.second) = true;
    }

    executed = true;
}

void ket::exec_quantum() {
    process_stack.top()->exec();

    process_stack.pop();

    *process_on_top_stack.top() = false;
    process_on_top_stack.pop();

    process_stack.push(std::make_shared<process>());
    process_on_top_stack.push(std::make_shared<bool>(true));
}
