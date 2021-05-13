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
#include <cmath>
#include <fstream>

using namespace ket;
using tcp = boost::asio::ip::tcp;    
namespace http = boost::beast::http; 

void process::exec() {
    if (output_kqasm) {
        std::ofstream out{kqasm_path, std::ofstream::app};
        out << kqasm.str() 
            << "=========================" 
            << std::endl;
        out.close();
    }

    if (execute_kqasm) {
        
        auto kqasm_file = kqasm.str();
        boost::replace_all(kqasm_file, " ",  "%20");
        boost::replace_all(kqasm_file, "\t", "%09");
        boost::replace_all(kqasm_file, "\n", "%0A");

        boost::asio::io_context ioc;
        tcp::resolver resolver{ioc};
        tcp::socket socket{ioc};

        auto const results = resolver.resolve(kbw_addr, kbw_port);
        boost::asio::connect(socket, results.begin(), results.end());

        std::stringstream param;
        param << "/api/v1/run?kqasm=" << kqasm_file;
        if (dump_to_fs) param << "&dump2fs=1";

        http::request<http::string_body> req{http::verb::get, param.str(), 11};
        req.set(http::field::host, kbw_addr);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

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

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(json_file, pt);

        for (auto result : pt.get_child("int")) {
            auto i = std::stol(result.first);
            *(measure_map[i].first) = std::stol(result.second.get_value<std::string>());
            *(measure_map[i].second) = true;
        }

        auto dump2fs = pt.get<std::string>("dump2fs") == "1";

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
